#include<pthread.h>
#include<stdio.h>
#include<semaphore.h>

sem_t forks[5];

int left(int p){
    return p;
}

int right(int p){
    return (p + 1) % 5;
}

void get_fork(int p){
    if(p !=4 )
    {
        sem_wait(&forks[left(p)]);
        sem_wait(&forks[right(p)]);
    }else{
        sem_wait(&forks[right(p)]);
        sem_wait(&forks[left(p)]);
    }
    
}

void put_fork(int p){
    sem_post(&forks[left(p)]);
    sem_post(&forks[right(p)]);
}

void* Dining(void* arg){
    while(1){
        int p = (int)arg;
        get_fork(p);
        // eating
        printf("The %d Phi is dining\n",p);
        put_fork(p);
    }
    
}

int main()
{
    for(int i = 0;i < 5;i ++)
    {
        sem_init(&forks[i],0,1);
    }
    pthread_t Phi[5];
    for(int i = 0;i < 5;i ++)
    {
        pthread_create(&Phi[i],NULL,Dining,(void*)i);
    }
    for(int i = 0;i < 5;i++)
    {
        pthread_join(Phi[i],NULL);
    }
}