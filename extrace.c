// extrace.c

#include <linux/module.h>

static int extrace_init(void)
{
    pr_err("printed by pr_err\n");
    pr_warn("printed by pr_warn\n");
    pr_notice("printed by pr_notice\n");
    pr_info("printed by pr_info\n");
    printk("printed by printk\n");

    trace_printk("printed by trace_printk\n");
    return 0;
}

static void extrace_exit(void)
{
    trace_printk("printed by trace_printk\n");
}

module_init(extrace_init);
module_exit(extrace_exit);
MODULE_LICENSE("Dual MIT/GPL");
