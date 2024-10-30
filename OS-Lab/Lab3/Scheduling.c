#include<stdio.h>
#include<string.h>
#include<assert.h>
#include<stdlib.h>
#include<ctype.h>

#define MAX_JOBS 100
#define DEFAULT_JOBS 4
#define DEFAULT_

#define DEBUG 1

typedef enum{
    FIFO = 1,
    RR,
    STCF,
}Mode;

typedef struct{
    int id; // A job has one unique id
    int time; // How long does this job need
    int come_time; // when this job come (i wonder if this is necessary to write this in Job struct)
}Job_;


typedef struct{
    Job_ joblist[MAX_JOBS];
    int job_num; // How many job in this cpu
}FIFO_ARGS;


typedef struct {
    Job_ joblist[MAX_JOBS];
    int job_num;
    int len_timeslice; // how long a time slice is
}RR_ARGS;


typedef struct{
    Job_ joblist[MAX_JOBS];
    int job_num;
}STCF_ARGS;

int Init_ARGS(FIFO_ARGS* fifo,RR_ARGS* rr,STCF_ARGS* stcf){
    fifo->job_num = DEFAULT_JOBS;
    rr->job_num = DEFAULT_JOBS;
    rr->len_timeslice = 1;
    stcf->job_num = DEFAULT_JOBS;

    for(int i = 0 ;i < DEFAULT_JOBS;i ++){
        Job_ job;
        job.come_time = rand() % 5; // let come_time in 0-9
        job.id = i;
        job.time = rand() % 10; // let time of a job in 0-19
        fifo->joblist[i] = job;
        rr->joblist[i] = job;
        stcf->joblist[i] = job;
        if(DEBUG && i == 0){fprintf(stdout,"Adress of three pointer : fifo : %p, rr : %p, stcf : %p\n",&fifo->joblist[i],&rr->joblist[i],&stcf->joblist[i]);}
    }
}

int fifo_cmp(const void *a, const void *b){
    Job_* joba = (Job_*)a;
    Job_* jobb = (Job_*)b;
    if(joba->come_time == jobb->come_time){return 0;}
    if(joba->come_time < jobb->come_time){return -1;}
    return 1;
}

int simulate_fifo(FIFO_ARGS* fifo){
    int Timer = 0;
    Job_* job_list = fifo->joblist;
    int job_num = fifo->job_num;
    //sort by the come time
    qsort(job_list,job_num,sizeof(Job_),fifo_cmp);
    for(int i = 0;i < job_num;i++){
        Job_ now_job = job_list[i];
        if(DEBUG){fprintf(stdout,"Job %d, Come time %d, Excute time : %d\n",now_job.id,now_job.come_time,now_job.time);}
    }
    //And just simulate
    printf("Time\t\tJob\t\tState\n");
    int job_p = 0;
    Job_ current_job; // the job will do next or now
    while(job_p < job_num){
        current_job = job_list[job_p];
        while(Timer < current_job.come_time){
            printf("%d\t\t\t\n",Timer);
            Timer++;
        }
        assert(Timer >= current_job.come_time);
        for(int t =  0;t < current_job.time;t++){
            printf("%d\t\t\t%d\t\t",Timer,current_job.id);
            //print waiting job
            printf("Waiting Jobs : ");
            for(int i = job_p + 1;i < job_num;i++){
            if(Timer >= job_list[i].come_time){
                printf("%d\t",job_list[i].id);
                }
            }
            Timer++;
            printf("\n");
        }
        
        job_p++;
    }
    return 0;
}

int simulate_rr(RR_ARGS* rr){
    int Timer = 0;
    Job_* joblist = rr->joblist;
    int job_num = rr->job_num;
    int time_slice = rr->len_timeslice;
    qsort(joblist,job_num,sizeof(Job_),fifo_cmp);
    // And I will try a ugly but clear method
    int is_end = 0;
    int job_p = 0;
    int status[DEFAULT_JOBS] = {0};
    int ex_time[DEFAULT_JOBS] = {0};
    for(int i = 0;i < job_num;i++){
        Job_ now_job = joblist[i];
        if(DEBUG){fprintf(stdout,"Job %d, Come time %d, Excute time : %d\n",now_job.id,now_job.come_time,now_job.time);}
    }
    printf("Time\t\tJob\t\tState\n");
    
    while(!is_end){
        for(int i = 0;i < job_num;i++){
            if(status[i] == 0 && joblist[i].come_time <= Timer && ex_time[i] == 0){
                status[i] = 1;
                job_p = i;
            }
        }
        for(int i = 0;i < job_num;i++){
            if(status[i] == 1 && ex_time[i] < joblist[i].time){
                printf("%d\t\t\t%d\t\t",Timer,joblist[i].id);
                ex_time[i] += 1;
                if(ex_time[i] == joblist[i].time){
                    printf("Job %d has done",joblist[i].id);
                }
                printf("\n");
                Timer+=1;
            }
        }
        is_end = 1;
        for(int i = 0;i < job_num;i++){
            if(status[i] == 1 && ex_time[i] == joblist[i].time){
                status[i] = 2;
            }
            if(status[i] == 1 && ex_time[i] < joblist[i].time){
                is_end = 0;
            }
        }
        if(is_end && Timer < joblist[job_p + 1].come_time){
            while(Timer < joblist[job_p + 1].come_time){
                printf("%d\n",Timer);
                Timer+=1;
            }
            is_end = 0;
        }
        
    }
}

int stcf_cmp(const void *a, const void *b){
    Job_* joba = (Job_*)a;
    Job_* jobb = (Job_*)b;
    if(joba->time == jobb->time){return 0;}
    if(joba->time < jobb->time){return -1;}
    return 1;
}



int simulate_stcf(STCF_ARGS* stcf){
    int Timer = 0;
    Job_* joblist = stcf->joblist;
    int job_num = stcf->job_num;
    qsort(joblist,job_num,sizeof(Job_),fifo_cmp);
    for(int i = 0;i < job_num;i++){
        Job_ now_job = joblist[i];
        if(DEBUG){fprintf(stdout,"Job %d, Come time %d, Excute time : %d\n",now_job.id,now_job.come_time,now_job.time);}
    }
    printf("Time\t\tJob\t\tState\t\t\n");
    int status[DEFAULT_JOBS] = {0};
    int ex_time[DEFAULT_JOBS] = {0};
    Job_ job_queue[DEFAULT_JOBS];
    int last_job = job_num;
    while(last_job > 0){
        //Adjust current job queue
        int job_q_p = 0;
        for(int i = 0;i < job_num;i++){
            if(Timer >= joblist[i].come_time){
                Job_ job = joblist[i];
                if(status[i] == 0){
                    // job time dont adjust
                    job_queue[job_q_p ++] = job;
                    status[i] = 1;
                }else if(status[i] == 1){
                    job.time = job.time - ex_time[i];
                    job_queue[job_q_p ++] = job;
                }
            }
        }
        // sort by last time
        if(job_q_p > 0){
            qsort(job_queue,job_q_p,sizeof(Job_),stcf_cmp);
            // excute job
            printf("%d\t\t\t%d\t\t",Timer,job_queue[0].id);
            for(int i = 0;i < job_num;i++){
                if(joblist[i].id == job_queue[0].id){
                    ex_time[i] += 1;
                }
            }
        }
        
        //
        for(int i = 0;i < job_num;i++){
            if(joblist[i].time == ex_time[i] && status[i] == 1){
                status[i] = 2;
                printf("%d has done",joblist[i].id);
                last_job --;
            }
        }
        printf("\n");
        Timer+=1;
    }
}

int simulate_(FIFO_ARGS* fifo,RR_ARGS* rr,STCF_ARGS* stcf){
    // First simulate fifo
    simulate_fifo(fifo);
    printf("\nRR Schedule \n");
    simulate_rr(rr);
    printf("\nSTCF Schedule \n");
    simulate_stcf(stcf);
}


int main(int argc,char * argv[]){
    FIFO_ARGS * fifo_args = (FIFO_ARGS*)malloc(sizeof(FIFO_ARGS));
    RR_ARGS* rr_args = (RR_ARGS*)malloc(sizeof(RR_ARGS));
    STCF_ARGS* stcf_args = (STCF_ARGS*)malloc(sizeof(RR_ARGS));
    Init_ARGS(fifo_args,rr_args,stcf_args);
    simulate_(fifo_args,rr_args,stcf_args);
    
    free(fifo_args);
    free(rr_args);
    free(stcf_args);
}

