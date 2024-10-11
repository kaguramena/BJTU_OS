#include<pthread.h>
#include<semaphore.h>
#include<stdio.h>

#define READERS 5
#define WRITERS 10

typedef struct __rwlock_t{
    sem_t lock1;
    sem_t lock2;
    sem_t writelock;
    sem_t Bufferlock;
    int readers;
    int waiting_writers;
} rwlock_t;


char buffer[10] = {'0'}; // 初始化缓冲区
int buffer_pointer = 0;

const char* sentence = "Kagu >_<";

rwlock_t* rwlock;

/*
    在这份代码中，写者优先
    写者负责循环往缓冲区添加
    读者负责重复读入缓冲区
*/

void rwlock_init(rwlock_t* rw)
{
    rw->readers = 0; // 可以有任意多的读者
    rw->waiting_writers = 0; // 有多少写者在等
    sem_init(&rw->lock1,0,1);// 保护变量锁
    sem_init(&rw->lock2,0,1);// 保护变量锁
    sem_init(&rw->writelock,0,1);// 写者锁，保证不会出现 critical conflicts 
    sem_init(&rw->Bufferlock,0,1);// 众生平等锁，这个锁的存在保证谁先来谁先上，而不是读者一直饿着写者，可以看到当有这把锁以后，永远最多只有一个读者在读
}

void rwlock_acquire_readlock(rwlock_t* rw) // 这个锁的作用是防止写者进入
{
    sem_wait(&rw->Bufferlock);
    sem_wait(&rw->lock1);
    rw->readers ++;
    
    if (rw->readers == 1){
        sem_wait(&rw->writelock); // 第一个读者要读时，必须保证这时没有人在写
    }
    sem_post(&rw->lock1);
    sem_post(&rw->Bufferlock);
}

void rwlock_release_readlock(rwlock_t* rw)
{
    sem_wait(&rw->lock1);
    rw->readers--;
    if(rw->readers == 0){
        sem_post(&rw->writelock); // 最后一个读者离开，现在可以让写者进入来写了
    }
    sem_post(&rw->lock1);
}

void rwlock_acquire_writelock(rwlock_t* rw)// 这个锁的作用是防止在写的过程中被打断
{
    sem_wait(&rw->lock2);
    rw->waiting_writers ++;
    if(rw->waiting_writers == 1){
        sem_wait(&rw->Bufferlock);
    }
    sem_post(&rw->lock2);
    sem_wait(&rw->writelock);
}

void rwlock_release_writelock(rwlock_t* rw)
{
    sem_post(&rw->writelock);
    sem_wait(&rw->lock2);
    rw->waiting_writers --;
    
    if(rw->waiting_writers == 0){
        sem_post(&rw->Bufferlock);
    }
    sem_post(&rw->lock2);
    sem_post(&rw->Bufferlock);
}

void* reader_thread(void* args)
{
    for(int i = 0;i < 100;i ++){
        rwlock_acquire_readlock(rwlock);
        printf("The %d reader get %s in buffer,And %d Writers are waiting\n",rwlock->readers,buffer,rwlock->waiting_writers);
        rwlock_release_readlock(rwlock);
    }
    
}

void* writer_thread(void* args)
{
    for(int i = 0;i < 100;i ++){
        rwlock_acquire_writelock(rwlock);
        buffer[buffer_pointer % 10] = sentence[buffer_pointer % 7];
        buffer_pointer ++;
        printf("Writer put a %c in buffer,And %d readers are waiting\n",sentence[(buffer_pointer - 1) % 7],rwlock->readers);
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