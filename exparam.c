// exparam.c

#include <linux/module.h>
#include <linux/fs.h>

#define DRIVER_NAME "exparam"

// Module parameters
static int major = 120;
module_param(major, int, S_IRUGO);

struct file_operations exchrdev_fops = {
    .owner   = THIS_MODULE,
};

static int exparam_init(void)
{
    register_chrdev(major, DRIVER_NAME, &exchrdev_fops);
    return 0;
}

static void exparam_exit(void)
{
    unregister_chrdev(major, DRIVER_NAME);
}

module_init(exparam_init);
module_exit(exparam_exit);
MODULE_LICENSE("Dual MIT/GPL");
