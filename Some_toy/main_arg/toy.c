#include<stdio.h>
int main(int argc,char* argv[],char* const envp[])
{
    int i = 0;
    while(envp[i])
    {
        printf("%s\n",envp[i]);
        i++;
    }
}