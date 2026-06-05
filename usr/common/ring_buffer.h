#ifndef SERIAL_BUFFER__H
#define SERIAL_BUFFER__H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>
#include <errno.h>
#include <sys/select.h>
#define RING_BUFFER_SIZE 4096
struct RingBuffer
{
    unsigned char buffer[RING_BUFFER_SIZE];
    int count;
    int head;
    int tail;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};
extern void ring_buffer_init(struct RingBuffer* rb);
extern void ring_buffer_destroy(struct RingBuffer* rb);
extern int ring_buffer_write(struct RingBuffer* rb,const unsigned char* data,int len);
extern int ring_buffer_read(struct RingBuffer* rb,unsigned char* data,int max_len,int times_out_ms);
extern int ring_buffer_available(struct RingBuffer* rb);
#endif
