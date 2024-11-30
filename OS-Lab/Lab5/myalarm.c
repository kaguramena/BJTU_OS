#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/sysfs.h>
#include <linux/timer.h>

#define DEVICE_NAME "myalarm"
#define IOCTL_SET_ALARM _IOW('a', 1, unsigned long)

static int major;
static struct class *my_class;

static struct my_alarm_device {
    struct timer_list timer; // 内核定时器
    unsigned long alarm_time; // 定时器时间
} my_alarm_dev;

// 定时器回调函数
static void alarm_callback(struct timer_list *t) {
    struct my_alarm_device *dev = from_timer(dev, t, timer);
    printk(KERN_INFO "Alarm triggered! Alarm time: %lus\n", dev->alarm_time);
}

static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    switch (cmd) {
        case IOCTL_SET_ALARM:
            del_timer_sync(&my_alarm_dev.timer); // 删除旧定时器
            my_alarm_dev.alarm_time = arg;
            mod_timer(&my_alarm_dev.timer, jiffies + msecs_to_jiffies(arg * 1000));
            printk(KERN_INFO "Alarm set for %lus\n", arg);
            break;
        default:
            return -EINVAL;
    }
    return 0;
}

static int dev_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "myalarm: Device opened\n");
    return 0;
}

static int dev_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "myalarm: Device closed\n");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = dev_ioctl,
    .open = dev_open,
    .release = dev_release,
};

static ssize_t alarm_info_show(const struct class *class, const struct class_attribute *attr, char *buf){
    return sprintf(buf, "Device Name: %s\nCurrent Alarm: %lus\n", DEVICE_NAME, my_alarm_dev.alarm_time);
}
static CLASS_ATTR_RO(alarm_info);

static int __init myalarm_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "Failed to register device\n");
        return major;
    }

    // 创建设备类别
    my_class = class_create("myalarm_class");
    if (IS_ERR(my_class)) {
        unregister_chrdev(major, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create class\n");
        return PTR_ERR(my_class);
    }

    // 创建设备节点
    device_create(my_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    class_create_file(my_class, &class_attr_alarm_info);

    // 初始化定时器
    timer_setup(&my_alarm_dev.timer, alarm_callback, 0);

    printk(KERN_INFO "myalarm: Device registered with major number %d\n", major);
    printk(KERN_INFO "myalarm: Use IOCTL to set alarm in seconds\n");
    return 0;
}

static void __exit myalarm_exit(void) {
    del_timer_sync(&my_alarm_dev.timer); // 删除定时器
    device_destroy(my_class, MKDEV(major, 0));
    class_remove_file(my_class, &class_attr_alarm_info);
    class_destroy(my_class);
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "myalarm: Device unregistered\n");
}

module_init(myalarm_init);
module_exit(myalarm_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A virtual alarm device using Linux timer");