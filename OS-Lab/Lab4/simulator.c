#include<stdio.h>
#include<assert.h>
#include<stdlib.h>
#include<stdbool.h>

#define KB 1024
#define MB (1024 * KB)
#define MEMORY (512 * MB)
#define MIN_UNIT_SIZE KB
#define UNIT_NUMBER (MEMORY/MIN_UNIT_SIZE)
#define MAX_HANG_NUMBER 10
#define MAX_PROCESS_NUMBER 60

typedef struct Node{
    int size;
    int start_pos;
    int end_pos;
    struct Node* next;
}Node;

typedef struct{
    int available;
    Node* first_node;  
} AvailableList;

void ConcatAvailableList(AvailableList* list){
    for(Node* n = list->first_node;n != NULL;n = n->next){
        while(n->next!=NULL && n->end_pos == n->next->start_pos){
            n->size += n->next->size;
            n->end_pos = n->next->end_pos;
            Node* t = n->next->next;
            free(n->next);
            n->next = t;
        }
        if(n->next == NULL){
            break;
        }
    }
}

void ReportAvailableList(AvailableList* list){
    for(Node* n = list->first_node;n != NULL;n = n->next){
        printf("Avaliable from %dKB to %dKB\n",(n->start_pos/KB),(n->end_pos/KB));
    }
}

void FreeAvailableList(AvailableList* list){
    Node* lastnode = NULL;
    for(Node* n = list->first_node;n!=NULL;n = n->next){
        if(lastnode != NULL){
            free(lastnode);
        }
        lastnode = n;
    }
    free(lastnode);
    free(list);
}

int InitAvailableList(AvailableList * List){
    assert(List != NULL);

    // First, Init ALL memory
    List->available = MEMORY;
    Node* node = (Node*)malloc(sizeof(Node));
    node->size = 512 * MB;
    node->start_pos = 0;
    node->end_pos = MEMORY;
    node->next = NULL;
    List->first_node = node;    
    return 0;
}

Node* FindFrontNode(AvailableList* list,Node* node){
    for(Node* n = list->first_node;n != NULL;n = n->next){
        if(n->next == node){
            return n;
        }
    }
    return NULL;
}


typedef struct{
    int pid;
    int need;
    int start_pos;
    int end_pos;

    int create_time;
    int exec_time;

    int status; // create,hang,alllocate,delete
}Process;

int pid_p = 1; // 0 for the OS
int CreateProcess(Process* p,int timeclamp){
    p->pid = pid_p++;
    p->need = (rand() % 20) * KB;
    p->status = 1;
    p->create_time = timeclamp;
    p->exec_time = (rand() % 8);
    printf("Process %d create in %d\n",p->pid,timeclamp);
    return 0;
}

int AllocateNodeToProcess(Node* node,Process *p){
    p->start_pos = node->start_pos;
    p->end_pos = node->start_pos + p->need;
    p->status = 1;

    node->size -= p->need;
    node->start_pos += p->need;

    return 1;
}

typedef struct{
    int start;
    int num;
    Process* list[MAX_PROCESS_NUMBER]; // for real,it's hard to manage
}ProcessList;

void ReportProcessList(ProcessList* list){
    for(int i = list->start;i<list->num;i++){
        if(list->list[i] != NULL){
            printf("Process %d from %dKB to %dKB\n",list->list[i]->pid,list->list[i]->start_pos/KB,list->list[i]->end_pos/KB);
        }
        
    }
}

int InitProcessList(ProcessList * list){
    assert(list != NULL);
    // No Process
    list->num = 0;
    list->start = 0;
    for(int i = 0;i < MAX_PROCESS_NUMBER;i++){
        list->list[i] = NULL;
    }
    return 0;
}

void InsertProcess(ProcessList* list,Process* p){
    assert(list!=NULL);
    assert(p!=NULL);
    list->list[list->start + list->num++] = p;
    return 0;
}

void FreeProcess(AvailableList* ava_list,ProcessList* proc_list,Process* p,int time){
    int pid = p->pid;
    for(int i = proc_list->start;i < proc_list->num;i++){
        if(proc_list->list[i] == p){
            proc_list->list[i] = NULL;
            break;
        }
    }
    FreeMemory(ava_list,p);
    printf("Process %d free in %d\n",pid,time);
}

void FreeProcessList(ProcessList* list){
    for(int i = list->start;i<list->num;i++){
        free(list->list[i]);
    }
    free(list);
}

typedef struct{
    int start;
    int num;
    Process* list[MAX_HANG_NUMBER]; // ...
}HangList;

int InitHangList(HangList * list){
    assert(list != NULL);
    list->start = 0;
    list->num = 0;
    for(int i = 0;i < MAX_HANG_NUMBER;i++){
        list->list[i] = NULL;
    }
    return 0;
}

int HangProcess(HangList* list,Process* p){
    assert(list != NULL);
    assert(p != NULL);
    list->list[list->start + list->num++] = p;
    p->status = 2;
    return 0;
}

int InitOS(AvailableList* ava_list,ProcessList* p_list){
    // Second, Allocate Low 128MB to OS
    Process* OS = (Process*)malloc(sizeof(Process));
    OS->need = 128*MB;
    OS->pid = 0;
    OS->start_pos = 0;
    OS->end_pos = 128 * MB;
    OS->status = 3;
    InsertProcess(p_list,OS);
    Node* fir_node = ava_list->first_node;
    fir_node->start_pos = OS->end_pos;
    fir_node->size -= OS->need;
    return 0;
}

int AllocateMemory(AvailableList * ava_list,HangList* hang_list,ProcessList* proc_list,Process * p){
    assert(ava_list != NULL);
    assert(p != NULL);
    // First ,check if there's enough memory
    if(ava_list->available < p->need){
        HangProcess(hang_list,p);
        printf("Process %d has been hang up",p->pid);
        return 1; // 1 rep hangout
    }
    // Second ,find if there's enough node to allocate
    Node* node = ava_list->first_node;
    while(node != NULL){
        if(node->size >= p->need){ // Find Node
            // if it is the first node
            if(node == ava_list->first_node){
                //Allocate the memory from first node
                //If need memory equal the last memory
                if(node->size == p->need){
                    //If there are other node
                    if(node->next != NULL){
                        ava_list->first_node = node->next;
                        ava_list->available -= p->need;

                        p->start_pos = node->start_pos;
                        p->end_pos = node->end_pos;
                        p->status = 1;
                        InsertProcess(proc_list,p);
                        printf("Process %d has been allocated memory from %dKB to %dKB\n",p->pid,p->start_pos/KB,p->end_pos/KB);
                        free(node);
                        return 0; // 0 rep allocate
                    }else{ // If there is no other node
                        ava_list->first_node = NULL;
                        ava_list->available -= p->need;

                        p->start_pos = node->start_pos;
                        p->end_pos = node->end_pos;
                        p->status = 1;
                        InsertProcess(proc_list,p);
                        printf("Process %d has been allocate memory from %dKB to %dKB\n",p->pid,p->start_pos/KB,p->end_pos/KB);
                        free(node);
                        return 0;
                    }
                }else{ // If need memory less than node last memory
                    ava_list->available -= p->need;
                    AllocateNodeToProcess(node,p);
                    InsertProcess(proc_list,p);
                    printf("Process %d has been allocate memory from %dKB to %dKB\n",p->pid,p->start_pos/KB,p->end_pos/KB);
                    return 0;
                }
            }else{ //Not the first Node
                if(node->size == p->need){
                    //Find the front node
                    Node* front_node = FindFrontNode(ava_list,node);
                    assert(front_node != NULL);
                    front_node ->next = node->next;
                    ava_list->available -= p->need;

                    p->start_pos = node->start_pos;
                    p->end_pos = node->end_pos;
                    p->status = 1;
                    InsertProcess(proc_list,p);
                    printf("Process %d has been allocate memory from %dKB to %dKB\n",p->pid,p->start_pos/KB,p->end_pos/KB);
                    free(node);
                    return 0;
                }else{
                    ava_list->available -= p->need;
                    AllocateNodeToProcess(node,p);
                    InsertProcess(proc_list,p);
                    printf("Process %d has been allocate memory from %dKB to %dKB\n",p->pid,p->start_pos/KB,p->end_pos/KB);
                    return 0;
                }
            }
        }else{
            node = node->next;
            if(node == NULL){
                HangProcess(hang_list,p);
                printf("Process %d has been hang up",p->pid);
                return 1;
            }
        }
    }
}

int FreeMemory(AvailableList * ava_list,Process* p){
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->start_pos = p->start_pos;
    new_node->end_pos = p->end_pos;
    new_node->size = p->need;
    //If ava_list has no node
    if(ava_list->available == 0){
        assert(ava_list->first_node==NULL);
        ava_list->first_node = new_node;
        new_node->next = NULL;
        return 0;
    }
    if(ava_list->first_node->start_pos >= new_node->end_pos){
        new_node->next = ava_list->first_node;
        ava_list->first_node = new_node;
        return 0;
    }
    for(Node* n = ava_list->first_node;n!=NULL;n = n->next){
        if(n->end_pos <= new_node->start_pos){
            if(n->next == NULL){
                n->next = new_node;
                return 0;
            }else if(n->next->start_pos >= new_node->end_pos){
                new_node->next = n->next;
                n->next = new_node;
                return 0;
            }else{
                continue;
            }
        }
    }
}

void report(AvailableList* ava_list,HangList* hang_list,ProcessList* proc_list){
    printf("Report:\n");
    ReportProcessList(proc_list);
    ReportAvailableList(ava_list);
}

void simulate(AvailableList* ava_list,HangList* hang_list,ProcessList* proc_list){
    report(ava_list,hang_list,proc_list);
    int timeclamp = 1; // time 
    int proc_sum = 0;
    bool flag = true;
    int proc_alive = 0;
    while(flag){
        printf("--------------------\n");
        printf("Time : %d\n",timeclamp);
        printf("Create Process : \n");
        int proc_num = proc_sum > 10 ? 0 : rand()%2;// new proc
        proc_sum += proc_num;
        proc_alive += proc_num;
        for(int i = 0;i < proc_num;i++){
            Process* proc = (Process*)malloc(sizeof(Process));
            CreateProcess(proc,timeclamp);
            AllocateMemory(ava_list,hang_list,proc_list,proc);
        }
        printf("Free Process : \n");
        // Clean dead proc
        for(int i = proc_list->start;i < proc_list->num;i++){
            if(proc_list->list[i] != NULL){
                if(proc_list->list[i]->create_time + proc_list->list[i]->exec_time == timeclamp){
                    proc_alive-=1;
                    FreeProcess(ava_list,proc_list,proc_list->list[i],timeclamp);
                }
            }
        }
        //concat
        ConcatAvailableList(ava_list);
        timeclamp++;
        report(ava_list,hang_list,proc_list);
        printf("--------------------\n");
        if(proc_alive == 0){
            flag = false;
        }
    }
    return;
}

int main(int argc,char* argv[]){
    AvailableList * ava_list = (AvailableList*)malloc(sizeof(AvailableList));
    HangList * hang_list = (HangList*)malloc(sizeof(HangList));
    ProcessList * proc_list = (ProcessList*)malloc(sizeof(ProcessList));

    InitAvailableList(ava_list);
    InitHangList(hang_list);
    InitProcessList(proc_list);
    InitOS(ava_list,proc_list);

    simulate(ava_list,hang_list,proc_list);

    FreeAvailableList(ava_list);
    free(hang_list);
    FreeProcessList(proc_list);
    return 0;
}