#include <stdio.h>
#include <unistd.h>
#include <string.h>

int _echo(int argc, char *argv[])
{
    int newline = 1; // 默认在输出结束后添加换行符

    // 如果第一个参数是 "-n"，则不添加换行符
    int i = 1;
    if (argc > 1 && strcmp(argv[1], "-n") == 0)
    {
        newline = 0; // 禁止换行
        i = 2;       // 从第二个参数开始处理
    }

    // 输出所有剩余参数
    for (; i < argc; i++)
    {
        printf("%s", argv[i]); // 输出参数
        if (i < argc - 1)
        {
            printf(" ");
        }
    }
    // 如果没有 "-n" 选项，输出换行符
    if (newline)
    {
        printf("\n");
    }
    return 0;
}