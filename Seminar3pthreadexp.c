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
static int clockCycles = 100;
static float timerChoice = 0.975;

static int stock = StartingAmount;
static int maxstock = MStock;
static int randomCap = 10;
static int baseD = 1;
static int baseP = 1;



static int totalProducer = 0;
static int totalConsumer = 0;


static void* threadProducer(void *args){

    int product = baseP + (rand() % randomCap);
    //int product = 12;
    int ret;
    printf("\n\033[1;32mDelivering %d stock\033[0m\n", product);
    while(myClock < clockCycles && product > 0){
        //printf("\n\t\t\033[1;36mLocking mtx\033[0m\n");
        pthread_mutex_lock(&mtx);
        
        while(stock>maxstock-1){
            if(myClock > clockCycles){
                //printf("\n\t\t\033[1;36mUnlocking mtx\033[0m\n");
                pthread_mutex_unlock(&mtx);
                break;
            }
            printf("\n\033[1;32mMax stock reached\033[0m\n");
            printf("\n\033[1;32mExcess supply %d\033[0m \n", product);
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
    printf("\n\033[1;31mConsuming %d stock\033[0m\n", demand);
    while(myClock < clockCycles && demand > 0){
        //printf("\n\t\t\033[1;36mLocking mtx\033[0m\n");
        pthread_mutex_lock(&mtx);
        
        while(stock<1){
            if(myClock > clockCycles){
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
    printf("What should the max stock be? Please respond with an integer.\n");
    scanf("%d", &maxstock);
    printf("What should the base production be? Please respond with an integer.\n");
    scanf("%d", &baseP);
    printf("What should the base demand be? Please respond with an integer.\n");
    scanf("%d", &baseD);
    randomCap = baseD + baseP;
    printf("How many clock cycles should the program run for? \n");
    scanf("%d", &clockCycles);
    printf("How long should each clock cycle be? \n0.5 would be half a second, 2 would be two seconds\nPlease note a lot may be displayed per cycle \n");
    scanf("%f", &timerChoice);
    printf("\033[1;35m\n==Begining Simulation== \n\033[0m");
    printf("\nTotal Product\n");
   
    for(; myClock < (clockCycles+1); ++myClock){
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

        printf("\nTotal Product\n");
        for(int i = 0; i < stock;){
        printf("*");
        ++i;
        }
        for(int i = 0; i < (maxstock - stock);){
            printf("-");
            ++i;
        }
        fflush(stdout);
        printf("\n\n");
        usleep((1000000 * timerChoice));
    }

}
