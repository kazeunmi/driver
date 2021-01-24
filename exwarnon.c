// exwarnon.c

#include <linux/module.h>
#include <linux/fs.h>

#define DRIVER_NAME "exwarnon"
#define DRIVER_MAJOR 1  // Intentional bug: 1 is reserved for mem device

struct file_operations exwarnon_fops = {
    .owner   = THIS_MODULE,
};

static int exwarnon_init(void)
{
    int result;

    result = register_chrdev(DRIVER_MAJOR, DRIVER_NAME, &exwarnon_fops);
    WARN_ON(result < 0);  // Not to forget to add error handling later

    return 0;
}

static void exwarnon_exit(void)
{
    unregister_chrdev(DRIVER_MAJOR, DRIVER_NAME);
}

module_init(exwarnon_init);
module_exit(exwarnon_exit);
MODULE_LICENSE("Dual MIT/GPL");
