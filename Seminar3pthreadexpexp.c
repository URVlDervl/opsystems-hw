#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>


#define MStock 20
#define StartingAmount 10

#define PDEME 1
#define CEAT 12


static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t condc = PTHREAD_COND_INITIALIZER;
static pthread_cond_t condp = PTHREAD_COND_INITIALIZER;


static int CID = 0;
static int PID = 0;
static int myClock = 1;
static int ProducerClock = 4;
static int ConsumerClock = 4;
static int stock = StartingAmount;
static int maxstock = MStock;
static int randomCap = 3;

static int offset = 0;
static int poffset = 0;
static int completeconsumer = 0;
static int completeproducer = 0;
static int totalProducer = 0;
static int totalConsumer = 0;

void PrintScreen(){
    offset = 0;
    for(int i = 0; i < (totalConsumer - completeconsumer);++i){
        ++offset;
        printf("C");
    }
    for(int i = 0; i < stock;++i){
        printf("*");
    }
    for(int i = 0; i < (maxstock - stock);++i){
        printf("-");
    }
    for(int i = 0; i < (totalProducer - completeproducer);++i){
        ++offset;
        printf("P");
    }
    for(int i = 0; i < (poffset-offset);++i){
        printf("%c",32);
    }
    for(int i = 0; i < (poffset-offset);++i){
        printf("%c",8);
    }
    fflush(stdout);
    poffset = offset;
    for(int i = 0; i < (maxstock + offset);++i){
        printf("%c",8);
    }
}

static void* threadProducer(void *args){

//    int product = 1 + (rand() % randomCap);
    int product = PDEME;
    int ret;

    while(myClock < 100 && product > 0){

        pthread_mutex_lock(&mtx);
        
        while(stock>maxstock-1){
            if(myClock > 99){
                pthread_mutex_unlock(&mtx);
                break;
            }
            pthread_cond_broadcast(&condc);
            ret = pthread_cond_wait(&condp,&mtx);
            if (ret != 0){
                printf("pthread wait error");
                return NULL;
            }
        }

        ++stock;
        --product;
        
        if(product == 0){
            ++completeproducer;
        }

        pthread_mutex_unlock(&mtx);
    }

    pthread_cond_broadcast(&condc);
    return NULL;
}


static void* threadConsumer(void *args){

    //int demand = 1 + (rand() % randomCap);
    int demand = CEAT;
    int ret;

    while(myClock < 100 && demand > 0){

        pthread_mutex_lock(&mtx);
        
        while(stock<1){
            if(myClock > 100){
                pthread_mutex_unlock(&mtx);
                break;
            }
            pthread_cond_broadcast(&condp);
            ret = pthread_cond_wait(&condc,&mtx);
            if (ret != 0){
                printf("pthread wait error");
                return NULL;
            }
        }

        --stock;
        --demand;

        if (demand == 0){
            ++completeconsumer;
        }

        pthread_mutex_unlock(&mtx);
    }

    pthread_cond_broadcast(&condp);
    return NULL;
}


int main(int argc, char*argv[]){

    printf("How often should producers arrive? Please respond with an integer.\n");
    scanf("%d", &ProducerClock);
    printf("How often should consumers arrive? Please respond with an integer.\n");
    scanf("%d", &ConsumerClock);
    printf("\033[1;35m\n==Begining Simulation== \n\033[0m");
    printf("Total Product\n");

    for(; myClock < 100; ++myClock){

        PrintScreen();

        if((myClock % ConsumerClock) == 0){
            pthread_t Cthread;
            pthread_create(&Cthread,NULL,threadConsumer,&CID);
            pthread_detach(Cthread);
            ++CID;
            ++totalConsumer;
        }


        if((myClock % ProducerClock) == 0){
            pthread_t Pthread;
            pthread_create(&Pthread,NULL,threadProducer,&PID);
            pthread_detach(Pthread);
            ++PID;
            ++totalProducer;
        }

        
        usleep(1000 * 750);
    }
    usleep(1000 * 750);
    PrintScreen();
}
