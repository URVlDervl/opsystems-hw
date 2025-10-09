#include <stdio.h>
#include <stdlib.h>

typedef struct _process/* or Process; here*/{
    int pid;
    char name;
    int size;
    struct _process *next;
}Process_t;

typedef struct _queue{
    Process_t *first;
    Process_t *last;
}Queue_t;

int pidSequence = 0;
int timeslice = 2;

Process_t* createProcess(char NewName, int NewSize){
    Process_t * result = (Process_t *)malloc(sizeof(Process_t));
    result->name = NewName;
    result->next = NULL;
    result->pid = ++pidSequence;
    result->size = NewSize;
    return result;
}

Queue_t* creatQueue(){
    Queue_t* result = (Queue_t*)malloc(sizeof(Queue_t));
    result->first = NULL;
    result->last = NULL;
    return result;
}

int addToQueue(Queue_t* q, Process_t * p){
    if(q == NULL)
        return -1;
    
    if(q->first == NULL){
        q->first = p;
        q->last = p;
        return 0;
    }

    q->last->next = p;
    q->last = p;

    return 0;
}

Process_t* PopFromQueue(Queue_t* q){
    if(q == NULL){
        return NULL;
    }
    
    if(q->first == NULL){
        return q->first;
    }

    Process_t * result = q->first;

    if(q->first == q->last){
        q->first = NULL;
        q->last = NULL;
        return result;
    }

    q->first = q->first->next;
    result->next = NULL;
    return result;
}

int rr(Queue_t* q){

    while (q->first != NULL)
    {
        Process_t* runningProcess = PopFromQueue(q);

        printf("Process %c PID:%d is running time start = %d, timeleft = %d\n", runningProcess->name, runningProcess->pid, runningProcess->size, runningProcess->size - timeslice);

        runningProcess->size -= timeslice;

        if(runningProcess->size > 0)
            addToQueue(q, runningProcess);
        else{
            printf(" ########## Process %c is finished\n", runningProcess->name);
            free(runningProcess);
        }

    }
    
}

int main(){
    Queue_t* readyQueue = creatQueue();

    addToQueue(readyQueue, createProcess('A',8));
    addToQueue(readyQueue, createProcess('B',3));
    addToQueue(readyQueue, createProcess('C',1));

    rr(readyQueue);

    return 0;
}
