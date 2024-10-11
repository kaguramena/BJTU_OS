#include<pthread.h>
#include<semaphore.h>
#include<stdio.h>

#define MAX 30
#define NLOOPS 10
#define PROD_NUM 10
#define CONS_NUM 10


// !!!WARNING: PROD_NUM CAN NOT GREATER THAN MAX. OTHERWISE THIS PROGRAM WILL CRASH



pthread_mutex_t Pmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Cmutex = PTHREAD_MUTEX_INITIALIZER;
char buffer[MAX];
const char* sentence= "I am kagu";
int ptr;
sem_t full; // when full == MAX ,the queue is full
sem_t empty;// when empty == MAX, the queue is empty
// thus, when init these two variable, full should be 0 and empty should be MAX
int fill; // used by producer
int use; // used by consumer


void put(int ptr)
{
    buffer[fill] = sentence[ptr];
    fill = (fill + 1)% MAX;
}

int get()
{
    char temp = buffer[use];
    use = (use + 1) % MAX;
    return temp;
}

void* producer(void* arg)
{
    // let's assume one producer only put one value into queue
    while(1)
    {
        
        sem_wait(&empty);
        pthread_mutex_lock(&Pmutex);
        put(ptr);
        printf("Put %c in buffer\n",sentence[ptr]);
        ptr+=1;
        ptr%=(sizeof(sentence) + 1);
        pthread_mutex_unlock(&Pmutex);
        sem_post(&full);
        
    }
    return;
}

void* consumer(void* arg)
{
    while(1)
    {
        sem_wait(&full);
        pthread_mutex_lock(&Cmutex);
        char temp = get();
        printf("Get %c from buffer\n",temp);
        pthread_mutex_unlock(&Cmutex);
        sem_post(&empty);
    }
    return;
}

int main()
{
    // sem_init()'s second arg: if it is not zero, it will be shared by all procs
    //                          and if it is zero,it is only used by this proc
    sem_init(&full,0,0);
    sem_init(&empty,0,MAX);
    pthread_t prod_[PROD_NUM];
    pthread_t cons_[CONS_NUM];

    // Init all thread
    for(int i = 0;i < PROD_NUM;i++){
        pthread_create(&prod_[i],NULL,producer,NULL);
    }
    for(int i = 0;i < CONS_NUM;i++){
        pthread_create(&cons_[i],NULL,consumer,NULL);

    }
    
    // Join all thread
    for(int i = 0;i < PROD_NUM;i++){
        pthread_join(prod_[i],NULL);
    }
        
    for(int i = 0;i < CONS_NUM;i++){
        pthread_join(cons_[i],NULL);
    }
        
}