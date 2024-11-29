#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h> // 必须包含

#define DEVICE_NAME "mydevice"

static int major; // 主设备号
static char buffer[1024]; // 模拟设备存储区
static struct class *my_class; // 用于设备分类

static int dev_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "Device opened\n");
    return 0;
}

static ssize_t dev_read(struct file *file, char __user *user_buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "Custom read operation triggered!\n");
    return simple_read_from_buffer(user_buffer, len, offset, buffer, sizeof(buffer));
}

static ssize_t dev_write(struct file *file, const char __user *user_buffer, size_t len, loff_t *offset) {
    printk(KERN_INFO "Custom write operation: %zu bytes\n", len);
    size_t ret = simple_write_to_buffer(buffer, sizeof(buffer), offset, user_buffer, len);
    if(ret > 0){
        int i,j;
        for(int i = 0, j = ret - 2;i < j;i ++,j--){
            char tmp = buffer[i];
            buffer[i] = buffer[j];
            buffer[j] = tmp;
        }
    }
    return ret;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = dev_open,
    .read = dev_read,
    .write = dev_write,
};

static int __init mydevice_init(void) {
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

    printk(KERN_INFO "Device registered with major number %d\n", major);
    return 0;
}

static void __exit mydevice_exit(void) {
    device_destroy(my_class, MKDEV(major, 0)); // 删除设备节点
    class_destroy(my_class); // 删除设备类别
    unregister_chrdev(major, DEVICE_NAME); // 注销字符设备
    printk(KERN_INFO "Device unregistered\n");
}

module_init(mydevice_init);
module_exit(mydevice_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple Linux character device with custom operations");