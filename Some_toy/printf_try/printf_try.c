#include<stdio.h>
#include<unistd.h>

int main(int argc,char* argv[],char* const envp[])
{
    fork();
    printf("Hello\n");
    // fflush(stdout);
    fork();
    printf("Hello\n");
}