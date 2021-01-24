// exinterrupt.c

#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

#define DEVICE_NAME "exinterrupt" 

static int irq;

static irqreturn_t exinterrupt_isr(int irq, void *data)
{
    trace_printk("exinterrupt_isr\n");

    return IRQ_HANDLED;
}

static struct file_operations exinterrupt_fops = {
    .owner = THIS_MODULE,
};

static struct miscdevice exinterrupt_misc_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &exinterrupt_fops,
};

static int __init exinterrupt_init(void)
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

    // Register interrupt handler
    ret = request_irq(irq, exinterrupt_isr, IRQF_TRIGGER_FALLING, "exinterrupt", NULL);
    if (ret) {
        pr_err("request_irq failed\n");
        return ret;
    }

    // Register misc device
    misc_register(&exinterrupt_misc_device);

    return 0;
}

static void __exit exinterrupt_exit(void)
{
    misc_deregister(&exinterrupt_misc_device);
    free_irq(irq, NULL);	
    gpio_free(5);
}

module_init(exinterrupt_init);
module_exit(exinterrupt_exit);
MODULE_LICENSE("Dual MIT/GPL");
