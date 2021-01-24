// exmisc.c

#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>

#define DEVICE_NAME "exmisc"

#define CAPACITY 1024
static char memory[CAPACITY];

static int exmisc_open(struct inode *inode, struct file *file)
{
    return 0;
}

static int exmisc_close(struct inode *inode, struct file *file)
{
    return 0;
}

static ssize_t exmisc_read(struct file *file, char __user *buf,
                           size_t count, loff_t *pos)
{
    ssize_t nread = simple_read_from_buffer(buf, count, pos, memory, CAPACITY);
    return nread;
}

static ssize_t exmisc_write(struct file *file, const char __user *buf,
                            size_t count, loff_t *pos)
{
    ssize_t nwritten = simple_write_to_buffer(memory, CAPACITY, pos, buf, count);
    return nwritten;
}

struct file_operations exmisc_fops = {
    .owner   = THIS_MODULE,
    .open    = exmisc_open,
    .release = exmisc_close,
    .read    = exmisc_read,
    .write   = exmisc_write,
};

static struct miscdevice exmisc_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &exmisc_fops,
};

static int exmisc_init(void)
{
    misc_register(&exmisc_device);
    return 0;
}

static void exmisc_exit(void)
{
    misc_deregister(&exmisc_device);
}

module_init(exmisc_init);
module_exit(exmisc_exit);
MODULE_LICENSE("Dual MIT/GPL");
