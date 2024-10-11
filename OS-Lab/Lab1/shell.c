#define _XOPEN_SOURCE 700  // 启用 POSIX.1-2008 和 X/Open 7 标准，使用setenv
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUF_SIZE 1024
#define ARG_DELIM " \n\t\a\r"
#define MAX_ARGS 5

char __HOME[128];

extern int _cd(int argc,char* argv[]);
extern int _echo(int argc,char* argv[]);
extern int _mkdir(int argc,char* argv[]);
extern int _pstree(int argc,char* argv[]);
extern int _ls(int argc,char* argv[]);


char* read_command_line(void)
{
    char* command = NULL;
    ssize_t buf_size = 0;

    if (getline(&command, &buf_size, stdin) == -1){
    if (feof(stdin)) {
      exit(EXIT_SUCCESS);  // We recieved an EOF
    } else  {
      perror("readline");
      exit(EXIT_FAILURE);
    }
  }

  return command;
}

// 这个函数将命令字符串按空格分割并存储到 argv 数组中
char **parse_command_line(char* command,int* argc)
{
    int bufsize = BUF_SIZE;
    char** tokens;
    char* token;
    tokens = (char**)malloc(bufsize * sizeof(char*));

    if(!tokens)
    {
        perror("MALLOC ERROR");
        exit(EXIT_FAILURE);
    }
    int position = 0;
    token = strtok(command,ARG_DELIM);
    while(token != NULL)
    {
        // create safe token
        char* s_token = (char*)malloc(256 * sizeof(char));
        strcpy(s_token,token);
        if(s_token[0] == '~')
        {
            char s[128];
            sprintf(s,"%s%s\0",__HOME,s_token + 1);
            strcpy(s_token,s);
            tokens[position] = s_token;
        }
        else if(s_token[0] == '$')
        {
            char s[128];
            strcpy(s,s_token + 1);
            
            char* _env = getenv(s);
            strcpy(s_token,_env);
            
            tokens[position] = s_token;
        }
        else
        {
            tokens[position] = s_token;
        }
        
        position ++;
        if(position >= BUF_SIZE)
        {
            bufsize += BUF_SIZE;
            tokens = realloc(tokens,bufsize * sizeof(char*));
            if(!tokens)
            {
                perror("REALLOC ERROR");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL,ARG_DELIM);
    }
    tokens[position] = NULL;
    *argc = position;
    return tokens;
}


int main()
{
    char _PWD[128];
    //获取HOME环境变量
    char *s_s = getenv("HOME");
    strcpy(__HOME,s_s);
    while(1)
    {
        char* s = getenv("PWD");
        int argc = 0;
        char** argv;
        strcpy(_PWD,s);
        printf("kagu@kagu:%s$ ",_PWD);
        //printf("1");
        char* _COMMAND = read_command_line();
        
        //fputs(_COMMAND,stdout); //Debug Need it 
        argv = parse_command_line(_COMMAND,&argc);

        // for(int i = 0;i < argc;i++)
        // {
        //     printf("%s\n",argv[i]);
        // }
        
        _COMMAND[strcspn(_COMMAND, "\n")] = 0;
        
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
                do
                {
                    waitpid(pid, &status, 0);
                }while(!WIFSIGNALED(status) && !WIFEXITED(status));
            }
        }
        // 释放所有动态内存
        for(int i = 0;i < argc;i++)
        {
            free(argv[i]);
        }
        free(_COMMAND);
    }
    
}