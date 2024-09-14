#include <unistd.h>
#include <stdio.h>

int _cd(int argc, char *argv[])
{
    char path[20] = "";
    if (argc > 1)
    {
        strcpy(path, argv[1]);
    }
    chdir(path);
    // getcwd(path,sizeof(path));
    // printf("%s",path);
    return 1;
}