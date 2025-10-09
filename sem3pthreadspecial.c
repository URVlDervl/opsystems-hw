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
    //int product = 12;
    int ret;
    printf("\n\033[1;32mDelivering %d stock\033[0m\n", product);
    while(myClock < 100 && product > 0){
        //printf("\n\t\t\033[1;36mLocking mtx\033[0m\n");
        pthread_mutex_lock(&mtx);
        
        while(stock>maxstock-1){
            if(myClock > 100){
                //printf("\n\t\t\033[1;36mUnlocking mtx\033[0m\n");
                pthread_mutex_unlock(&mtx);
                break;
            }
            printf("\n\033[1;32m\tMax stock reached\033[0m\n");
            printf("\n\033[1;32m\tExcess supply %d\033[0m \n", product);
            //printf("\nWaiting for additional room\n");
            pthread_cond_broadcast(&condc);
            ret = pthread_cond_wait(&condp,&mtx);
            if (ret != 0){
                printf("\n\n\n\t\t\tpthread wait error\n\n\n");
                return NULL;
            }
        }

        ++stock;
        //printf("\n\t\t\033[1;36mUnlocking mtx\033[0m\n");
        pthread_mutex_unlock(&mtx);

        printf("+");

        --product;
    }
    printf("\n\033[1;36mProducer calling for other threads now \033[0m \n");
    if(stock<maxstock){
        printf("\n\033[1;32mMore room for other producers, signaling producers\033[0m \n");
        pthread_cond_signal(&condp);
    }
    pthread_cond_broadcast(&condc);
    return NULL;
}


static void* threadConsumer(void *args){

    int demand = baseD + (rand() % randomCap);
    //int demand = 1;
    int ret;
    int i;

    while(myClock < 100 && demand > 0){
        //printf("\n\t\t\033[1;36mLocking mtx\033[0m\n");
        pthread_mutex_lock(&mtx);
        
        while(stock<1){
            if(myClock > 100){
                //printf("\n\t\t\033[1;36mUnlocking mtx\033[0m\n");
                pthread_mutex_unlock(&mtx);
                break;
            }
            printf("\n\033[1;31mno stock to take\033[0m\n");
            i = demand - stock;
             printf("\n\033[1;31mExcess demand of %d\033[0m \n", i);
            pthread_cond_broadcast(&condp);
            ret = pthread_cond_wait(&condc,&mtx);
            if (ret != 0){
                printf("\n\n\n\t\t\tpthread wait error\n\n\n");
                return NULL;
            }
        }

        --stock;
        //printf("\n\t\t\033[1;36mUnlocking mtx\033[0m\n");
        pthread_mutex_unlock(&mtx);

        printf("-");

        --demand;
    }
    printf("\n\033[1;36mConsumer calling for other threads now \033[0m \n");
    if(stock>0)
        pthread_cond_signal(&condp); 
    if(stock==0)
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
   
    for(; myClock < 101; ++myClock){
    printf("\n\033[1;33mclock cycle: %d\033[0m\n\n\033[1;34mCurrent stock %d \n\033[0m", myClock,stock);

        if((myClock % ConsumerClock) == 0){
            pthread_t Cthread;
            printf("\nCurrent consumer number, %d\n", CID);
            pthread_create(&Cthread,NULL,threadConsumer,&CID);
            pthread_detach(Cthread);
            ++CID;
            
        }


        if((myClock % ProducerClock) == 0){
            pthread_t Pthread;
            printf("\nCurrent Producer number, %d\n", PID);
            pthread_create(&Pthread,NULL,threadProducer,&PID);
            pthread_detach(Pthread);
            ++PID;
            
        }

        
        usleep(1000 * 850);
    }

}
