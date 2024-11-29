#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h> // 必须包含
#include<linux/sysfs.h>

#define DEVICE_NAME "mydevice"
#define IOCTL_GET_INFO _IOR('d', 1, char *)

static int major; // 主设备号

static struct class *my_class; // 用于设备分类

struct my_device{
    char buffer[1024]; // 模拟设备存储区
    loff_t read_offset;
    loff_t write_offset;
};

static struct my_device my_dev;


static ssize_t device_info_show(const struct class *class,const struct class_attribute *attr, char *buf){
    return sprintf(buf, "Device Name: %s\nBuffer Size: %ld\n", DEVICE_NAME, sizeof(my_dev.buffer));
}
static CLASS_ATTR_RO(device_info); // 只读属性

static int dev_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device opened\n");
    return 0;
}

static ssize_t dev_read(struct file *file, char __user *user_buffer, size_t len, loff_t *offset) {
    ssize_t ret = simple_read_from_buffer(user_buffer, len, &my_dev.read_offset, my_dev.buffer, sizeof(my_dev.buffer));
    printk(KERN_INFO "Read %zd bytes from buffer\n", ret);
    return ret;
}

static ssize_t dev_write(struct file *file, const char __user *user_buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "Custom write operation: %zu bytes\n", len);
    size_t ret = simple_write_to_buffer(my_dev.buffer, sizeof(my_dev.buffer), &my_dev.write_offset, user_buffer, len);
    my_dev.buffer[ret] = '\0';
    if(ret > 0){
        int i,j;
        for(i = 0, j = ret - 2;i < j;i ++,j--){
            char tmp = my_dev.buffer[i];
            my_dev.buffer[i] = my_dev.buffer[j];
            my_dev.buffer[j] = tmp;
        }
    }
    printk(KERN_INFO "Buffer content: %s\n", my_dev.buffer);
    return ret;
}

static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    char info[128];
    switch (cmd) {
        case IOCTL_GET_INFO:
            snprintf(info, sizeof(info), "Device Name: %s\nBuffer Size: %ld\n", DEVICE_NAME, sizeof(my_dev.buffer));
            if (copy_to_user((char __user *)arg, info, strlen(info) + 1))
                return -EFAULT;
            break;
        default:
            return -EINVAL;
    }
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
    .unlocked_ioctl = dev_ioctl,
};

static int __init mydevice_init(void) {
    memset(my_dev.buffer, 0, sizeof(my_dev.buffer));
    // 注册字符设备
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "Failed to register device\n");
        return major;
    }

    // 创建设备类别
    my_class = class_create("mydevice_class");
    if (IS_ERR(my_class)) {
        unregister_chrdev(major, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create class\n");
        return PTR_ERR(my_class);
    }

    // 创建设备节点
    device_create(my_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    class_create_file(my_class,&class_attr_device_info);

    printk(KERN_INFO "Device registered with major number %d\n", major);
    printk(KERN_INFO "Device info: Name=%s, Buffer Size=%ld\nUsage: Reverse the input\n", DEVICE_NAME, sizeof(my_dev.buffer));
    return 0;
}

static void __exit mydevice_exit(void) {
    device_destroy(my_class, MKDEV(major, 0)); // 删除设备节点
    class_remove_file(my_class, &class_attr_device_info);
    class_destroy(my_class); // 删除设备类别
    unregister_chrdev(major, DEVICE_NAME); // 注销字符设备
    printk(KERN_INFO "Device unregistered\n");
}

module_init(mydevice_init);
module_exit(mydevice_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple Linux character device with custom operations");