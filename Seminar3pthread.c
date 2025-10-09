#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>


#define MStock 20
#define StartingAmount 10

/*
#define ProducerClock 4
#define ConsumerClock 4
*/

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
static int baseD = 1;
static int baseP = 1;

static int totalProducer = 0;
static int totalConsumer = 0;


static void* threadProducer(void *args){

    int product = baseP + (rand() % randomCap);
//    int product = 12;
    int ret;

    while(myClock < 100 && product > 0){
        //printf("\n\t\t\033[1;36mLocking mtx\033[0m\n");
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
        
        for(int i = 0; i < stock;){
        printf("*");
        ++i;
    }
    for(int i = 0; i < (maxstock - stock);){
        printf("-");
        ++i;
    }
    fflush(stdout);
    for(int i = 0; i < maxstock;){
        printf("%c",8);
        ++i;
    }
        pthread_mutex_unlock(&mtx);;

        --product;
    }
    if(stock<maxstock){
        pthread_cond_signal(&condp);
    }
    pthread_cond_broadcast(&condc);
    return NULL;
}


static void* threadConsumer(void *args){

    int demand = baseD + (rand() % randomCap);
    //int demand = 1;
    int ret;
    int flag;

    while(myClock < 100 && demand > 0){
        //printf("\n\t\t\033[1;36mLocking mtx\033[0m\n");
        pthread_mutex_lock(&mtx);
        
        while(stock<1){
            if(myClock > 99){
                //printf("\n\t\t\033[1;36mUnlocking mtx\033[0m\n");
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

        for(int i = 0; i < stock;){
        printf("*");
        ++i;
    }
    for(int i = 0; i < (maxstock - stock);){
        printf("-");
        ++i;
    }
    fflush(stdout);
    for(int i = 0; i < maxstock;){
        printf("%c",8);
        ++i;
    }
        //printf("\n\t\t\033[1;36mUnlocking mtx\033[0m\n");
        pthread_mutex_unlock(&mtx);

        --demand;
    }

    pthread_cond_broadcast(&condp);
    return NULL;
}


int main(int argc, char*argv[]){

    printf("How often should producers arrive? Please respond with an integer.\n");
    scanf("%d", &ProducerClock);
    printf("How often should consumers arrive? Please respond with an integer.\n");
    scanf("%d", &ConsumerClock);
    printf("What should the base production be? Please respond with an integer.\n");
    scanf("%d", &baseP);
    printf("What should the base demand be? Please respond with an integer.\n");
    scanf("%d", &baseD);
    printf("\033[1;35m\n==Begining Simulation== \n\033[0m");
    printf("Total Product\n");

    for(int i = 0; i < stock;){
        printf("*");
        ++i;
    }
    for(int i = 0; i < (maxstock - stock);){
        printf("-");
        ++i;
    }
    fflush(stdout);
    for(int i = 0; i < maxstock;){
        printf("%c",8);
        ++i;
    }

    for(; myClock < 101; ++myClock){

        if((myClock % ConsumerClock) == 0){
            pthread_t Cthread;
            pthread_create(&Cthread,NULL,threadConsumer,&CID);
            pthread_detach(Cthread);
            ++CID;
        }


        if((myClock % ProducerClock) == 0){
            pthread_t Pthread;
            pthread_create(&Pthread,NULL,threadProducer,&PID);
            pthread_detach(Pthread);
            ++PID;
        }

        
        usleep(1000 * 750);
    }

}
