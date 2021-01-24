// exthreaded.c

#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

#define DEVICE_NAME "exthreaded" 

static int irq;

static irqreturn_t exthreaded_isr(int irq, void *data)
{
    trace_printk("Interrupt handler\n");
    return IRQ_WAKE_THREAD;  // Invokes exthreaded_thread
}

static irqreturn_t exthreaded_thread(int irq, void *data)
{
    trace_printk("Thread\n");
    return IRQ_HANDLED;
}

static struct file_operations exthreaded_fops = {
    .owner = THIS_MODULE,
};

static struct miscdevice exthreaded_misc_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &exthreaded_fops,
};

static int __init exthreaded_init(void)
{
    int ret;

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

    // Register threaded interrupt handler
    ret = request_threaded_irq(irq, exthreaded_isr, exthreaded_thread,
                               IRQF_TRIGGER_FALLING, "exthreaded", NULL);
    if (ret) {
        pr_err("request_irq failed\n");
        return ret;
    }

    // Register a character device for communication with user space
    misc_register(&exthreaded_misc_device);

    return 0;
}

static void __exit exthreaded_exit(void)
{

    misc_deregister(&exthreaded_misc_device);
    free_irq(irq, NULL);	
    gpio_free(5);
}

module_init(exthreaded_init);
module_exit(exthreaded_exit);
MODULE_LICENSE("Dual MIT/GPL");
