#ifndef SERIAL__H
#define SERIAL__H
#include "ring_buffer.h"
#include <pthread.h>

typedef  struct Serialdev{
    const char* dev;
    int speed;
    int bits;
    int event;
    int stop;
}Serialconfig;


typedef struct Serialmanager
{
    /* data */
    struct RingBuffer tx_buff;
    struct RingBuffer rx_buff;

    pthread_t tx_pthread;
    pthread_t rx_pthread;

    pthread_mutex_t serial_lock;

    int flag;
    int fd;
}Serialmanager;
extern void serial_close(Serialmanager* manager);
extern int serial_init(Serialconfig* config,Serialmanager* manager);
#endif