#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static int ready = 0;
static int done = 0;

static void* thread_produce(void *args) {
    int ret;
    for(;done < 100;){
        ret = pthread_mutex_lock(&mtx);
        if(ret != 0)
            return NULL;
        ++ready;
        printf("+");
        ret = pthread_mutex_unlock(&mtx);
        if(ret != 0)
            return NULL;
        pthread_cond_signal(&cond);
        if(ret != 0)
            return NULL;
        usleep(1000*100);
    }
    return NULL;
}

static void* thread_consume(void *args){
    int ret;
    for(;done < 10;){
        ret = pthread_mutex_lock(&mtx);
        if(ret!=0)
            return NULL;
        while(ready == 0){
            ret = pthread_cond_wait(&cond, &mtx);
            if(ret != 0)
                return NULL;
        }
        while (ready > 0){
            --ready;
            ++done;
            printf("-");
        }
        ret = pthread_mutex_unlock(&mtx);
        if(ret != 0)
            return NULL;
    }
    return NULL;
 }

int main(int argc, char*argv[]){
    pthread_t t1, t2;
    int loops, ret;
    loops = (argc > 1) ? atoi(argv[1]) : 1000000;

    printf("\n");

    ret = pthread_create(&t1, NULL, thread_consume, &loops);
    ret = pthread_create(&t2, NULL, thread_produce, &loops);

    pthread_join(t1, NULL);

    done = 110;
    pthread_join(t2, NULL);

    return 0;
}
