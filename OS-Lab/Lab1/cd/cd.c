#include <unistd.h>
#include <stdio.h>

int cd(int argc, char *argv[])
{
    char path[20] = '~';
    if (argc > 1)
    {
        strcpy(path, argv[1]);
    }
    chdir(path);
    return 1;
}