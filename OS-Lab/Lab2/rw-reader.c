#include<pthread.h>
#include<semaphore.h>
#include<stdio.h>

#define READERS 10
#define WRITERS 10

typedef struct __rwlock_t{
    sem_t lock;
    sem_t writelock;
    int readers;
} rwlock_t;


char buffer[10] = {'0'}; // 初始化缓冲区
int buffer_pointer = 0;

const char* sentence = "Kagu >_<";

rwlock_t* rwlock;

/*
    在这份代码中，读者优先，利用信号量来当互斥锁用，从而在第一个读者要读时就阻塞写者进入缓冲区
    写者负责循环往缓冲区添加
    读者负责重复读入缓冲区
    WARNING: 如果想看到理想的实验结果，请务必保证 WRITERS >= READERS，否则 WRITERS 真的会永远写不进去
             除非 READERS 刻意等待
*/

void rwlock_init(rwlock_t* rw)
{
    rw->readers = 0; // 可以有任意多的读者
    sem_init(&rw->lock,0,1);// 一把大锁
    sem_init(&rw->writelock,0,1);// 写者锁，保证不会出现 critical conflicts 
}

void rwlock_acquire_readlock(rwlock_t* rw)
{
    sem_wait(&rw->lock);
    rw->readers ++;
    if (rw->readers == 1){
        sem_wait(&rw->writelock); // 第一个读者要读时，必须保证这时没有人在写
    }
    sem_post(&rw->lock);
}

void rwlock_release_readlock(rwlock_t* rw)
{
    sem_wait(&rw->lock);
    rw->readers--;
    if(rw->readers == 0){
        sem_post(&rw->writelock); // 最后一个读者离开，现在可以让写者进入来写了
    }
    sem_post(&rw->lock);
}

void rwlock_acquire_writelock(rwlock_t* rw)
{
    sem_wait(&rw->writelock);
}

void rwlock_release_writelock(rwlock_t* rw)
{
    sem_post(&rw->writelock);
}

void* reader_thread(void* args)
{
    for(int i = 0;i < 100;i ++){
        rwlock_acquire_readlock(rwlock);
        printf("The %d reader get %s in buffer\n",rwlock->readers,buffer);
        rwlock_release_readlock(rwlock);
    }
    
}

void* writer_thread(void* args)
{
    for(int i = 0;i < 100;i ++){
        rwlock_acquire_writelock(rwlock);
        buffer[buffer_pointer % 10] = sentence[buffer_pointer % 7];
        buffer_pointer ++;
        printf("Writer put a %c in buffer\n",sentence[(buffer_pointer - 1) % 7]);
        rwlock_release_writelock(rwlock);
    }
}

int main(int argc,char ** argv)
{
    rwlock_t lock;
    rwlock = &lock;
    rwlock_init(rwlock);
    pthread_t p_reader[READERS],p_writer[WRITERS];
    for(int i = 0; i < READERS; i++){
        pthread_create(&p_reader[i],NULL,reader_thread,NULL);
    }
    for(int i = 0;i < WRITERS; i++){
        pthread_create(&p_writer[i],NULL,writer_thread,NULL);
    }
    for(int i = 0;i <WRITERS;i++){
        pthread_join(p_writer[i],NULL);
    }
    for(int i = 0;i < READERS;i++){
        pthread_join(p_reader[i],NULL);
    }
    
}