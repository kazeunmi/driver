// exwaitqueue.c

#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/wait.h>

#define DEVICE_NAME "exwaitqueue" 

static int irq;
static wait_queue_head_t queue;
static volatile int condition;

static irqreturn_t exwaitqueue_isr(int irq, void *data)
{
    trace_printk("Interrupt handler\n");
    condition = 1;
    wake_up_interruptible(&queue);

    return IRQ_HANDLED;
}

static int exwaitqueue_open(struct inode *inode, struct file *file)
{
    return 0;
}

static int exwaitqueue_close(struct inode *inode, struct file *file)
{
    return 0;
}

static ssize_t exwaitqueue_read(struct file *file, char __user *buf,
                             size_t count, loff_t *pos)
{
    trace_printk("read() waits for interrupt\n");
    condition = 0;
    wait_event_interruptible(queue, condition == 1);
    trace_printk("read() is woken up\n");

    return 0;
}

static struct file_operations exwaitqueue_fops = {
    .owner   = THIS_MODULE,
    .open    = exwaitqueue_open,
    .release = exwaitqueue_close,
    .read    = exwaitqueue_read,
};

static struct miscdevice exwaitqueue_misc_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &exwaitqueue_fops,
};

static int __init exwaitqueue_init(void)
{
    int ret;

    // Initialize waitqueue
    init_waitqueue_head(&queue);

    // Register GPIO to use
    ret = gpio_request_one(5, GPIOF_IN, "GPIO5");
    if (ret) {
        pr_err("gpio_request_array failed\n");
        return ret;
    }
	
    // Get IRQ for this GPIO
    ret = gpio_to_irq(5);
    if (ret < 0) {
        pr_err("gpio_to_irq failed\n");
        return ret;
    }
    irq = ret;

    // Register interrupt handler
    ret = request_irq(irq, exwaitqueue_isr, IRQF_TRIGGER_FALLING, "exwaitqueue", NULL);
    if (ret) {
        pr_err("request_irq failed\n");
        return ret;
    }

    // Register misc device
    misc_register(&exwaitqueue_misc_device);

    return 0;
}

static void __exit exwaitqueue_exit(void)
{
    misc_deregister(&exwaitqueue_misc_device);
    free_irq(irq, NULL);	
    gpio_free(5);
}

module_init(exwaitqueue_init);
module_exit(exwaitqueue_exit);
MODULE_LICENSE("Dual MIT/GPL");
