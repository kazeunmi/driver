// exmodule.c

#include <linux/module.h>

static int exmodule_init(void)
{
    printk(KERN_INFO "exmodule_init()\n");
    return 0;
}

static void exmodule_exit(void)
{
    printk(KERN_INFO "exmodule_exit()\n");
}

module_init(exmodule_init);
module_exit(exmodule_exit);
MODULE_LICENSE("Dual MIT/GPL");
