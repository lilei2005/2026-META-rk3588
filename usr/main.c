#include "stdio.h"
#include "gpiod.h"
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>    // close()函数在这里声明
#include <fcntl.h>     // open()函数和O_*常量在这里声明
#include "serial.h"
#include "serial_manager.h"
#include "ring_buffer.h"
#define gpiolin 100
const char* gpiodev="/dev/gpiochip0";
struct gpiod_line *line;
struct gpiod_chip* gpio1;
unsigned char rx_data[64],tx_data[64]="";

int main(int agrc,char* agrv)
{
    int fd;
    Serialconfig config;
    Serialmanager manger;
    gpio1=gpiod_chip_open(gpiodev);
    line=gpiod_chip_get_line(gpio1,gpiolin);


    fd=serial_init(&config,&manger);

    usleep(100*1000);
    if(serial_pthread_start(&manger)!=0)
    {
        printf("pthread create fail\n");
        return 0;
    }
    while(strcmp(tx_data,"stop\n")!=0)
    {
        
        if(fgets(tx_data,sizeof(tx_data),stdin)==NULL)
        {
            printf("scanf error\n");
            return 0;
        }
        ring_buffer_write(&manger.tx_buff,tx_data,64);
        usleep(100*1000);
        ring_buffer_read(&manger.rx_buff,rx_data,64,100);
        printf("%s\n",rx_data);
    }
    usleep(100*1000);
    serial_close(&manger);
    return 0;
}