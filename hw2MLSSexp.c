#include <stdio.h>
#include <stdlib.h>
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
    int TimeStart;
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

typedef struct _list{
    Process_t *first;
}Time_List_t;

Process_t* createProcess(char NewName, int NewSize, int NewTime);
Queue_t* createQueue(int mp, char n, int ts, int pg);
Time_List_t* createList();
int addToList(Time_List_t* L, Process_t* p);
Process_t* popFromList(Time_List_t* L);
int checkList(Time_List_t* L, Queue_t* q, int T);
int addToQueue(Queue_t* q, Process_t* p);
Process_t* PopFromQueue(Queue_t* q);
int ProcessQueue(Queue_t* qCurrent, Queue_t* qNext);
int MLFQ(Time_List_t* L, Queue_t* q0, Queue_t* q1, Queue_t* q2);

int pidSequence = 0;

Process_t* createProcess(char NewName, int NewSize, int NewTime){
    Process_t * result = (Process_t *)malloc(sizeof(Process_t));
    result->name = NewName;
    result->next = NULL;
    result->pid = ++pidSequence;
    result->size = NewSize;
    result->pen = 0;
    result->TimeStart = NewTime;
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

Time_List_t* createList(){
    Time_List_t* result = (Time_List_t*)malloc(sizeof(Time_List_t));
    result->first = NULL;
    return result;
}

int addToList(Time_List_t* L, Process_t* p){
    if(L == NULL)
        return -1;

    if (L->first == NULL){
        L->first = p;
        return 0;
    }

    if (L->first->TimeStart >= p->TimeStart){
        p->next = L->first;
        L->first = p;
        return 0;
    }

    Process_t* temp = L->first;

    for(;;){
        if(temp->next == NULL){
            temp->next = p;
            return 0;
        }
        if(temp->next->TimeStart >= p->TimeStart){
            p->next = temp->next;
            temp->next = p;
            return 0;
        }
        temp = temp->next;
    }
}

Process_t* popFromList(Time_List_t* L){
    if(L == NULL)
        return NULL;

    Process_t* result = L->first;

    if(L->first->next == NULL){
        L->first = NULL;
        return result;
    }

    L->first = L->first->next;

    return result;
}

int checkList(Time_List_t* L, Queue_t* q, int T){
    if(L == NULL||q == NULL)
        return -1;

    if(L->first!=NULL){
        while(L->first->TimeStart<= T){
            printf("Process %c PID:%d in Time List was added to Queue 0 at time %d with timestart %d\n",L->first->name, L->first->pid, T, L->first->TimeStart);
            addToQueue(q,popFromList(L));

            if(L->first == NULL){
                break;
            }
        }
    }

    return 0;
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

    printf("Process %c PID:%d in queue %c is running, Time Before = %d, Time After = %d, Penalty Before = %d, Penalty After = %d\n", runningProcess->name, runningProcess->pid, qCurrent->name, runningProcess->size, runningProcess->size - qCurrent->TS, runningProcess->pen, runningProcess->pen + qCurrent->PenGain);

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

int MLFQ(Time_List_t* L, Queue_t* q0, Queue_t* q1, Queue_t* q2){

    int CurrentTime = 0;
    printf("Time = %d\n", CurrentTime);

    for(;;){

        checkList(L,q0,CurrentTime);

        while (q0->first != NULL){
            ProcessQueue(q0,q1);
            CurrentTime += q0->TS;
            printf("Time = %d\n", CurrentTime);
            checkList(L,q0,CurrentTime);
        }
            
        if (q1->first != NULL){
            ProcessQueue(q1,q2);
            CurrentTime += q1->TS;
            printf("Time = %d\n", CurrentTime);
            checkList(L,q0,CurrentTime);
        }

        if (q0->first != NULL||q1->first != NULL)
            continue;

        if (q2->first != NULL){
            ProcessQueue(q2,q0);
            CurrentTime += q2->TS;
            printf("Time = %d\n", CurrentTime);
            checkList(L,q0,CurrentTime);
        }

        if (q0->first == NULL&&q1->first == NULL&&q2->first == NULL&&L->first == NULL)
            break;

        if(q0->first == NULL&&q1->first == NULL&&q2->first == NULL){
            printf("Nothing in queues, incrementing time\n\n");
            ++CurrentTime;
            printf("Time = %d\n", CurrentTime);
        }

    }

    printf("\nMLFQ ended at Time = %d\n", CurrentTime);

    return 0;
}


int main(){
    Time_List_t* TimeList = createList();

    Queue_t* readyQueue0 = createQueue(MaxPen0,'0', Q0TS, Q0PenGain);
    Queue_t* readyQueue1 = createQueue(MaxPen1,'1', Q1TS, Q1PenGain);
    Queue_t* readyQueue2 = createQueue(MaxPen2,'2', Q2TS, Q2PenGain);

    int count;
    int size;
    int time;
    char name = 't';

    printf("How many Processes would you like?\n");

    scanf("%d",&count);

    for(int i = 0; i<count; ++i){
        printf("What is the Process Name(1 char)\n");
        scanf(" %c",&name);
        printf("What is the Size(int)\n");
        scanf("%d",&size);
        printf("What is the time entered(int)\n");
        scanf("%d", &time);
        addToList(TimeList, createProcess(name,size,time));
    }

    MLFQ(TimeList, readyQueue0, readyQueue1, readyQueue2);

    return 0;
}
