#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define IOCTL_GET_INFO _IOR('d', 1, char *)

int test() {
    int fd = open("/dev/mydevice", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    // IOCTL 获取设备信息
    char info[128];
    if (ioctl(fd, IOCTL_GET_INFO, info) == 0) {
        printf("Device Info:\n%s\n", info);
    } else {
        perror("IOCTL failed");
        close(fd);
        return 1;
    }

    // 写入设备
    char data_to_write[] = "Hello, my device!";
    if (write(fd, data_to_write, strlen(data_to_write)) < 0) {
        perror("Write failed");
        close(fd);
        return 1;
    }
    printf("Write success: %s\n", data_to_write);

    // 从设备读取
    char read_buffer[128];
    if (read(fd, read_buffer, sizeof(read_buffer)) < 0) {
        perror("Read failed");
        close(fd);
        return 1;
    }
    printf("Read success: %s\n", read_buffer);

    close(fd);
    return 0;
}

int main() {
    return test();
}