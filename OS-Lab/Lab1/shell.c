#define _XOPEN_SOURCE 700  // 启用 POSIX.1-2008 和 X/Open 7 标准，使用setenv
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_ARGS 5

extern int _cd(int argc,char* argv[]);
extern int _echo(int argc,char* argv[]);
extern int _mkdir(int argc,char* argv[]);
extern int _pstree(int argc,char* argv[]);
extern int _ls(int argc,char* argv[]);



// 这个函数将命令字符串按空格分割并存储到 argv 数组中
int parse_command(char *command, char *argv[]) {
    int argc = 0;
    char *token = strtok(command, " \t\n");  // 用空格、Tab 或换行符分割命令

    while (token != NULL && argc < MAX_ARGS) {
        if(strncmp(token,"$",1) == 0)
        {
            char s[64];
            strcpy(s,token + 1);
            argv[argc++] = getenv(s);
        }
        else
        {
            argv[argc++] = token;  // 将分割后的部分存储到 argv 数组中
        }
        token = strtok(NULL, " \t\n");  // 继续分割剩余部分
    }

    argv[argc] = NULL;  // 以 NULL 结束参数列表（方便 exec 系列函数调用）
    return argc;
}


int main()
{
    char _PWD[128];
    while(1)
    {
        char* s = getenv("PWD");
        int argc = 0;
        char* argv[4];
        strcpy(_PWD,s);
        printf("kagu@kagu:%s$ ",_PWD);
        char _COMMAND[128];
        fgets(_COMMAND,sizeof(_COMMAND),stdin); // get user input
        //fputs(_COMMAND,stdout); Debug Need it 

        _COMMAND[strcspn(_COMMAND, "\n")] = 0;
        argc = parse_command(_COMMAND,argv);
        if(!strncmp(_COMMAND,"cd",2)) // so far,cd only support 2 argue
        {
            _cd(argc,argv);
            char _CWD[128];
            getcwd(_CWD,sizeof(_CWD)); 
            setenv("PWD",_CWD,1); // reset the env_variable of PWD
        }
        else if(!strncmp(_COMMAND,"ls",2))
        {
            _ls(argc,argv);
        }
        else if(!strncmp(_COMMAND,"pstree",6))
        {
            _pstree(argc,argv);
        }
        else if(!strncmp(_COMMAND,"echo",4))
        {
            _echo(argc,argv);
        }
        else if(!strncmp(_COMMAND,"mkdir",5))
        {
            _mkdir(argc,argv);
        }
        else
        {
            //调用外部命令
            pid_t pid = fork();
                if (pid < 0) {
                // fork 失败
                perror("fork failed");
                exit(1);
            } 
            else if (pid == 0) 
            {
                // 子进程，调用 execvp 来执行外部命令
                if (execvp(argv[0], argv) == -1) {
                    // execvp 执行失败，输出错误信息
                    perror("exec failed");
                }
                exit(1);  // 如果 execvp 失败，子进程退出
            } 
            else {
                // 父进程，等待子进程结束
                int status;
                waitpid(pid, &status, 0);
            }
        }
    }
    
}