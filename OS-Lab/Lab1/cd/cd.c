#include <unistd.h>
#include <stdio.h>
#include<unistd.h>
#include<stdlib.h>

int _cd(int argc, char *argv[])
{
    char path[20] = "";
    char *s = getenv("HOME");
    strcpy(path,s);
    if (argc > 1)
    {
        strcpy(path, argv[1]);
    }
    chdir(path);
    // getcwd(path,sizeof(path));
    // printf("%s",path);
    return 1;
}