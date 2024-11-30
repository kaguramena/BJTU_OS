#include <fcntl.h>
#include <stdio.h>
#include<unistd.h>
#include <sys/ioctl.h>

#define IOCTL_SET_ALARM _IOW('a', 1, unsigned long)

int main() {
    int fd = open("/dev/myalarm", O_RDWR);
    if (fd < 0) {
        perror("Failed to open /dev/myalarm");
        return 1;
    }

    unsigned long alarm_time = 5; // 设置5秒后触发
    if (ioctl(fd, IOCTL_SET_ALARM, alarm_time) < 0) {
        perror("Failed to set alarm");
        close(fd);
        return 1;
    }

    printf("Alarm set for %lu seconds\n", alarm_time);
    sleep(alarm_time + 1);
    close(fd);
    return 0;
}