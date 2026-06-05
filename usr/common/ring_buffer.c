#include "ring_buffer.h"


void ring_buffer_init(struct RingBuffer* rb)
{
    rb->count=0;
    rb->head=0;
    rb->tail=0;

    pthread_mutex_init(&rb->mutex,NULL);
    pthread_cond_init(&rb->cond,NULL);
}

void ring_buffer_destroy(struct RingBuffer* rb)
{
    pthread_mutex_destroy(&rb->mutex);
    pthread_cond_destroy(&rb->cond);
}

int ring_buffer_write(struct RingBuffer* rb,const unsigned char* data,int len)
{
    pthread_mutex_lock(&rb->mutex);

    int writter=0;

    while (writter<len&&rb->count<RING_BUFFER_SIZE)
    {
        rb->buffer[rb->head]=data[writter];
        rb->head=(rb->head+1)%RING_BUFFER_SIZE;
        writter++;
        rb->count++;
    }
    
    if(writter>0)
    {
        pthread_cond_signal(&rb->cond);
    }
    pthread_mutex_unlock(&rb->mutex);


    return writter;
}

int ring_buffer_read(struct RingBuffer* rb,unsigned char* data,int max_len,int times_out_ms)
{
    pthread_mutex_lock(&rb->mutex);

    while(rb->count==0)
    {
        struct timespec times;
        clock_gettime(CLOCK_REALTIME,&times);

        times.tv_sec+=times_out_ms/1000;
        times.tv_nsec+=(times_out_ms%1000)*1000*1000;
        if(times.tv_nsec>=1000*1000*1000)
        {
            times.tv_sec+=times.tv_nsec/(1000*1000*1000);
            times.tv_nsec%=1000*1000*1000;  
        }

        int ret=pthread_cond_timedwait(&rb->cond,&rb->mutex,&times);
        if(ret==ETIMEDOUT)
        {
            pthread_mutex_unlock(&rb->mutex);
            return 0;
        }

    }

    int len=0;
    while(len<max_len&&rb->count>0)
    {
        data[len]=rb->buffer[rb->tail];
        rb->tail=(rb->tail+1)%RING_BUFFER_SIZE;
        rb->count--;
        len++;
    }
    pthread_mutex_unlock(&rb->mutex);

    return len;
}

int ring_buffer_available(struct RingBuffer* rb)
{
    int count=0;
    pthread_mutex_lock(&rb->mutex);
    count=rb->count;
    pthread_mutex_unlock(&rb->mutex);
    return count;
}