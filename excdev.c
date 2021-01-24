// excdev.c

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#define DRIVER_NAME "excdev"
#define MINOR_BASE 0
#define MINOR_RANGE 3

static unsigned int excdev_major;
static struct cdev excdev_cdev;
static struct class *excdev_class;

static int excdev_open(struct inode *inode, struct file *file)
{
    return 0;
}

static int excdev_close(struct inode *inode, struct file *file)
{
    return 0;
}

struct file_operations s_excdev_fops = {
    .owner   = THIS_MODULE,
    .open    = excdev_open,
    .release = excdev_close,
};

static int excdev_init(void)
{
    int iminor;
    int result;
    dev_t dev;

    // Allocate major number
    result = alloc_chrdev_region(&dev, MINOR_BASE, MINOR_RANGE, DRIVER_NAME);
    if (result != 0) {
        goto error0;
    }
    excdev_major = MAJOR(dev);

    // Register cdev
    cdev_init(&excdev_cdev, &s_excdev_fops);
    excdev_cdev.owner = THIS_MODULE;
    result = cdev_add(&excdev_cdev, dev, MINOR_RANGE);
    if (result != 0) {
        goto error1;
    }

    // Create /sys/class/excdev
    excdev_class = class_create(THIS_MODULE, "excdev");
    if (IS_ERR(excdev_class)) {
        goto error2;
    }

    // Create /dev/excdev%d
    for (iminor = MINOR_BASE; iminor < MINOR_BASE + MINOR_RANGE; iminor++) {
        device_create(excdev_class, NULL, MKDEV(excdev_major, iminor), NULL, "excdev%d", iminor);
    }

    return 0;

 error2:
    cdev_del(&excdev_cdev);
 error1:
    unregister_chrdev_region(dev, MINOR_RANGE);
 error0:
    return -1;
}

static void excdev_exit(void)
{
    int iminor;
    dev_t dev = MKDEV(excdev_major, MINOR_BASE);

    // Delete /sys/class/excdev/excdev%d
    for (iminor = MINOR_BASE; iminor < MINOR_BASE + MINOR_RANGE; iminor++) {
        device_destroy(excdev_class, MKDEV(excdev_major, iminor));
    }

    // Delete /class/excdev
    class_destroy(excdev_class);

    // Delete cdev
    cdev_del(&excdev_cdev);

    // Free major number
    unregister_chrdev_region(dev, MINOR_RANGE);
}

module_init(excdev_init);
module_exit(excdev_exit);
MODULE_LICENSE("Dual MIT/GPL");
