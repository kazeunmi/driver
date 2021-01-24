// exchrdev.c

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define CHRDEV_NAME "exchrdev"
#define CHRDEV_MAJOR 120

// For simplicity, one memory per driver
#define CAPACITY 1024
static char memory[CAPACITY];

static int exchrdev_open(struct inode *inode, struct file *file)
{
    return 0;
}

static int exchrdev_close(struct inode *inode, struct file *file)
{
    return 0;
}

static ssize_t exchrdev_read(struct file *file, char __user *buf,
                             size_t count, loff_t *pos)
{
    // Using kernel convenience function here.
    ssize_t nread = simple_read_from_buffer(buf, count, pos, memory, CAPACITY);
    return nread;
}

static ssize_t exchrdev_write(struct file *file, const char __user *buf,
                              size_t count, loff_t *pos)
{
    // Using kernel convenience function here.
    ssize_t nwritten = simple_write_to_buffer(memory, CAPACITY, pos, buf, count);
    return nwritten;
}

struct file_operations exchrdev_fops = {
    .owner   = THIS_MODULE,
    .open    = exchrdev_open,
    .release = exchrdev_close,
    .read    = exchrdev_read,
    .write   = exchrdev_write,
};

static int exchrdev_init(void)
{
    printk(KERN_INFO "exchrdev_init()\n");
    register_chrdev(CHRDEV_MAJOR, CHRDEV_NAME, &exchrdev_fops);
    return 0;
}

static void exchrdev_exit(void)
{
    printk(KERN_INFO "exchrdev_exit()\n");
    unregister_chrdev(CHRDEV_MAJOR, CHRDEV_NAME);
}

module_init(exchrdev_init);
module_exit(exchrdev_exit);
MODULE_LICENSE("Dual MIT/GPL");
