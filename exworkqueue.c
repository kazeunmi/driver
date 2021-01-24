// exworkqueue.c

#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/workqueue.h>
#include <linux/delay.h>
#include <linux/fs.h>

#define DEVICE_NAME "exworkqueue"

static struct work_struct work;
static struct delayed_work delayed_work;

static void exworkqueue_work(struct work_struct *work)
{
    int i;

    for (i = 0; i < 5; i++) {
        trace_printk("exworkqueue_work(): %d\n", i);
        msleep(10);
    }
}

static void exworkqueue_delayed_work(struct work_struct *work)
{
    trace_printk("exworkqueue_delayed_work()\n");
}

struct file_operations exworkqueue_fops = {
    .owner   = THIS_MODULE,
};

static struct miscdevice exworkqueue_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &exworkqueue_fops,
};

static int exworkqueue_init(void)
{
    // Make sure previously scheduled works are done
    flush_scheduled_work();

    // Schedule new work
    INIT_WORK(&work, exworkqueue_work);
    schedule_work(&work);

    // Schedule new delayed work
    INIT_DELAYED_WORK(&delayed_work, exworkqueue_delayed_work);
    schedule_delayed_work(&delayed_work, msecs_to_jiffies(50));

    misc_register(&exworkqueue_device);
    return 0;
}

static void exworkqueue_exit(void)
{
    // Make sure all the works are completed
    flush_scheduled_work();
    
    misc_deregister(&exworkqueue_device);
}

module_init(exworkqueue_init);
module_exit(exworkqueue_exit);
MODULE_LICENSE("Dual MIT/GPL");
