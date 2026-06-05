#include "stdio.h"
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>    // close()函数在这里声明
#include <fcntl.h>     // open()函数和O_*常量在这里声明
#include "serial.h"



int serial_init(Serialconfig* config,Serialmanager* manager)
{
    int fd;
    struct termios options;
    fd=open(config->dev,O_RDWR|O_NOCTTY|O_NDELAY);
    if(fd<0)
    {
        perror("open serial dev fail");
        return -1;
    }

    if(tcgetattr(fd,&options)!=0)
    {
        perror("fail to get serial attributes");
        return -1;
    }

    speed_t baud;
    switch(config->speed)
    {
        case 9600: baud = B9600; break;
        case 19200: baud = B19200; break;
        case 38400: baud = B38400; break;
        case 57600: baud = B57600; break;
        case 115200: baud = B115200; break;
        case 230400: baud = B230400; break;
        case 460800: baud = B460800; break;
        case 921600: baud = B921600; break;
        default:
            fprintf(stderr, "Unsupported baud rate: %d\n", config->speed);
            close(fd);
            return -1;
    }

    cfsetispeed(&options,baud);
    cfsetospeed(&options,baud);

    options.c_cflag&=~CSIZE;
    switch(config->bits)
    {
        case 5:options.c_cflag|=CS5;break;
        case 6:options.c_cflag|=CS6;break;
        case 7:options.c_cflag|=CS7;break;
        case 8:options.c_cflag|=CS8;break;
        default:
            fprintf(stderr,"Unsupported data bits: %d\n",config->bits);
            close(fd);
            return -1;   
    }


    switch(config->event)
    {
        case 'N':options.c_cflag&= ~PARENB;break;
        case 'o':options.c_cflag|= PARENB;
                 options.c_cflag|= PARODD;break;
        case 'E':options.c_cflag |= PARENB;
                 options.c_cflag &=~PARODD;break;
        default:fprintf(stderr, "Unsupported parity: %c\n", config->event);
                close(fd);
                return -1;          
    }

    switch (config->stop) {
        case 1:
            options.c_cflag &= ~CSTOPB;
            break;
        case 2:
            options.c_cflag |= CSTOPB;
            break;
        default:
            fprintf(stderr, "Unsupported stop bits: %d\n", config->stop);
            close(fd);
            return -1;
    }

    options.c_cflag&=~CRTSCTS;

    options.c_cflag |= (CLOCAL | CREAD);

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    options.c_oflag &= ~OPOST;


    options.c_cc[VTIME] = 10; // 超时时间：1秒（单位：0.1秒）
    options.c_cc[VMIN] = 0;   // 最少读取0个字节

    tcflush(fd,TCIOFLUSH);


    if(tcsetattr(fd,TCSANOW,&options)!=0)
    {
         perror("com set error");
         return -1;
    }
    // manager->tx_buff=malloc(sizeof(manager->tx_buff));
    // manager->rx_buff=malloc(sizeof(manager->rx_buff));
    // manager->rx_pthread=malloc(sizeof(manager->rx_pthread));
    // manager->tx_pthread=malloc(sizeof(manager->tx_pthread));


    ring_buffer_init(&manager->tx_buff);
    ring_buffer_init(&manager->rx_buff);
    pthread_mutex_init(&manager->serial_lock,NULL);
    manager->flag=0;
    manager->fd=fd;
    printf("set done!\n");


    return fd;

}

void serial_close(Serialmanager* manager)
{
    if(manager->flag)
    {
        manager->flag=0;
        pthread_join(manager->tx_pthread,NULL);
        pthread_join(manager->rx_pthread,NULL);
    }

    if(manager->fd>0)
    {
        close(manager->fd);
        manager->fd = -1;
    }

    ring_buffer_destroy(&manager->rx_buff);
    ring_buffer_destroy(&manager->tx_buff);
    // free(manager->rx_buff);
    // free(manager->tx_buff);
    // free(manager->rx_pthread);
    // free(manager->tx_pthread);
    pthread_mutex_destroy(&manager->serial_lock);

}

