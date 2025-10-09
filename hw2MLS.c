#include <stdio.h>
#include <stdlib.h>
//The amount of time slices each process needs to complete
#define PATime 8
#define PBTime 3
#define PCTime 1
//The time slice amount each process will run in a given queue
#define Q0TS 1
#define Q1TS 1
#define Q2TS 1
//The amount of penalty a process will gain for each run in a given queue
#define Q0PenGain 1
#define Q1PenGain 1
#define Q2PenGain 1
//The max amount of penalty a process will need in order to promote or demote in a given queue
#define MaxPen0 2
#define MaxPen1 2
#define MaxPen2 4

typedef struct _process{
    int pid;
    char name;
    int size;
    int pen;
    struct _process *next;
}Process_t;

typedef struct _queue{
    Process_t *first;
    Process_t *last;
    char name;
    int TS;
    int PenGain;
    int MaxPen;
}Queue_t;

int pidSequence = 0;

Process_t* createProcess(char NewName, int NewSize){
    Process_t * result = (Process_t *)malloc(sizeof(Process_t));
    result->name = NewName;
    result->next = NULL;
    result->pid = ++pidSequence;
    result->size = NewSize;
    result->pen = 0;
    return result;
}

Queue_t* createQueue(int mp, char n, int ts, int pg){
    Queue_t* result = (Queue_t*)malloc(sizeof(Queue_t));
    result->first = NULL;
    result->last = NULL;
    result->MaxPen = mp;
    result->name = n;
    result->TS = ts;
    result->PenGain = pg;
    return result;
}

int addToQueue(Queue_t* q, Process_t* p){
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

int ProcessQueue(Queue_t* qCurrent, Queue_t* qNext){

    if (qCurrent->first == NULL)
        return -1;

    Process_t* runningProcess = PopFromQueue(qCurrent);

    printf("Process %c PID:%d in queue %c is running time start = %d, timeleft = %d, Penalty Before = %d, Penalty After = %d\n", runningProcess->name, runningProcess->pid, qCurrent->name, runningProcess->size, runningProcess->size - qCurrent->TS, runningProcess->pen, runningProcess->pen + qCurrent->PenGain);

    runningProcess->size -= qCurrent->TS;
    runningProcess->pen += qCurrent->PenGain;

    if(runningProcess->size > 0){

        if(runningProcess->pen >= qCurrent->MaxPen){
            printf("Process %c PID:%d Penalty exceeded queue %c max penalty:%d moving to queue %c.\n", runningProcess->name, runningProcess->pid, qCurrent->name, qCurrent->MaxPen, qNext->name);
            runningProcess->pen = 0;
            addToQueue(qNext, runningProcess);
        }
        else{
            addToQueue(qCurrent, runningProcess);
        }
    }
    else{
        printf(" ########### Process %c finished in queue %c\n", runningProcess->name, qCurrent->name);
        free(runningProcess);
    }

    printf("\n");

    return 0;
}

int MLFQ(Queue_t* q0, Queue_t* q1, Queue_t* q2){

    for(;;){

        while (q0->first != NULL)
            ProcessQueue(q0,q1);

        if (q1->first != NULL)
            ProcessQueue(q1,q2);

        if (q0->first != NULL||q1->first != NULL)
            continue;

        if (q2->first != NULL)
            ProcessQueue(q2,q0);

        if (q0->first == NULL&&q1->first == NULL&&q2->first == NULL)
            break;

    }
    return 0;
}


int main(){
    Queue_t* readyQueue0 = createQueue(MaxPen0,'0', Q0TS, Q0PenGain);
    Queue_t* readyQueue1 = createQueue(MaxPen1,'1', Q1TS, Q1PenGain);
    Queue_t* readyQueue2 = createQueue(MaxPen2,'2', Q2TS, Q2PenGain);

    addToQueue(readyQueue0, createProcess('A',PATime));
    addToQueue(readyQueue0, createProcess('B',PBTime));
    addToQueue(readyQueue0, createProcess('C',PCTime));

    MLFQ(readyQueue0, readyQueue1, readyQueue2);

    return 0;
}
