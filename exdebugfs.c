// exdebugfs.c

#include <linux/module.h>
#include <linux/debugfs.h>

#define DRIVER_NAME "exdebugfs"

struct dentry *debug_entry_dir;
static unsigned int param1;

static int exdebugfs_init(void)
{
    // Create debugfs directory
    debug_entry_dir = debugfs_create_dir(DRIVER_NAME, NULL);
    if (debug_entry_dir == NULL) {
        printk(KERN_ERR "debugfs_create_dir failed\n");
        return -ENOMEM;
    }

    // Create debugfs parameter
    debugfs_create_u32("param1_u32", S_IRUGO | S_IWUGO, debug_entry_dir, &param1);
    debugfs_create_x32("param1_x32", S_IRUGO | S_IWUGO, debug_entry_dir, &param1);

    return 0;
}

static void exdebugfs_exit(void)
{
    debugfs_remove_recursive(debug_entry_dir);
}

module_init(exdebugfs_init);
module_exit(exdebugfs_exit);
MODULE_LICENSE("Dual MIT/GPL");
