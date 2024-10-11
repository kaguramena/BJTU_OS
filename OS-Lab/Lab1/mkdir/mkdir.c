#include <unistd.h>

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

int _mkdir(int argc, char *argv[])
{
    if (argc < 2)
    {
        // 如果没有提供目录名称，输出错误信息
        fprintf(stderr, "Usage: %s <directory> [mode]\n", argv[0]);
        return 1;
    }

    // 默认权限为 0755
    mode_t mode = 0755;

    // 如果提供了权限参数，将其转换为 mode
    if (argc == 3)
    {
        mode = strtol(argv[2], NULL, 8); // 将字符串转换为八进制整数
    }

    // 创建目录
    if (mkdir(argv[1], mode) == -1)
    {
        // 创建失败，输出错误信息
        fprintf(stderr, "mkdir: cannot create directory '%s': %s\n", argv[1], strerror(errno));
        return 1;
    }

    return 0;
}
