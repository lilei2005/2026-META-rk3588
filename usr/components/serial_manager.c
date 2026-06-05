#include "stdio.h"
#include "serial.h"
#include "ring_buffer.h"
void* serial_rx_pthread(void* arg)
{
    Serialmanager* manager=(Serialmanager*)arg;
    unsigned char buffer[256];

    printf("Serial RX thread started\n");

    while(manager->flag)
    {
        fd_set fd;
        FD_ZERO(&fd);
        FD_SET(manager->fd,&fd);

        struct timeval times;
        times.tv_sec=0;
        times.tv_usec=100*1000;
        
        int ret=select(manager->fd+1,&fd,NULL,NULL,&times);
        if(ret<0)
        {
            printf("select fail");
            break;
        }
        else if(ret==0)
        {
            continue;
        }

        pthread_mutex_lock(&manager->serial_lock);
        int len=read(manager->fd,buffer,sizeof(buffer));
        pthread_mutex_unlock(&manager->serial_lock);

        if(len<0)
        {
            perror("Failed to read from serial ");
            break;
        }
        else if(len>0)
        {
            ring_buffer_write(&manager->rx_buff,buffer,len);
        }
    }
    printf("Serial RX thread exited\n");
    return NULL;

}


void* serial_tx_pthread(void* arg)
{
    Serialmanager* manager=(Serialmanager*)arg;
    unsigned char buffer[256];
    int len=0;
    while(manager->flag)
    {
        len=ring_buffer_read(&manager->tx_buff,buffer,sizeof(buffer),100);

        if(len==0)
        {
            continue;
        }

        pthread_mutex_lock(&manager->serial_lock);
        int set=write(manager->fd,buffer,len);
        pthread_mutex_unlock(&manager->serial_lock);

        if(set<0)
        {
            perror("Failed to write to serial");
            break;
        }

    }

    printf("Serial TX thread exited\n");
    return NULL;
}

int serial_pthread_start(Serialmanager *manager)
{
     if(manager->flag)
     {
        return 0;
     }

     manager->flag=1;

     if(pthread_create(&manager->rx_pthread,NULL,serial_rx_pthread,manager)!=0)
     {
        perror("Failed to create RX thread");
        manager->flag=0;
        return -1;
     }

     if(pthread_create(&manager->tx_pthread,NULL,serial_tx_pthread,manager)!=0)
     {
        perror("Failed to create TX thread");
        manager->flag=0;
        pthread_cancel(manager->rx_pthread);
        pthread_join(manager->rx_pthread,NULL);
        return -1;
     }
     return 0;
}