// exsysfs.c

#include <linux/module.h>
#include <linux/miscdevice.h>

#define DEVICE_NAME "exsysfs"

static ssize_t get_data(struct device *dev, struct device_attribute *dev_attr, char * buf)
{
    return sprintf(buf, "data = 0x%08x\n", 0x12345678);
}
static DEVICE_ATTR(data, S_IRUGO, get_data, NULL);

static struct miscdevice exsysfs_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
};

static int exsysfs_init(void)
{
    misc_register(&exsysfs_device);
    device_create_file(exsysfs_device.this_device, &dev_attr_data);
    return 0;
}

static void exsysfs_exit(void)
{
    device_remove_file(exsysfs_device.this_device, &dev_attr_data);
    misc_deregister(&exsysfs_device);
}

module_init(exsysfs_init);
module_exit(exsysfs_exit);
MODULE_LICENSE("Dual MIT/GPL");
