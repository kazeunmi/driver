// exerror.c

#include <linux/module.h>
#include <linux/gpio.h>

static int test = 0;
module_param(test, int, S_IRUGO);

static int exerror_init(void)
{
    int result;
    struct class* pclass;
    void* p;

    switch (test) {
      case 1:
        // Ask for GPIO 5 -> success
        result = gpio_to_irq(5);
        trace_printk("gpio_to_irq(5) = %d\n", result);
        if (result < 0) {
            trace_printk("returning %d\n", result);
            return result;
        }
        // Ask for GPIO 99 -> ENXIO
        result = gpio_to_irq(99);
        trace_printk("gpio_to_irq(99) = %d\n", result);
        if (result < 0) {
            trace_printk("returning %d\n", result);
            return result;
        }
        break;

      case 2:
        // Try to create existing class -> EEXIST
	pclass = class_create(THIS_MODULE, "tty");
        trace_printk("class_create() = %p\n", pclass);
        if (IS_ERR(pclass)) {
            trace_printk("returning %d\n", (int)PTR_ERR(pclass));
            return PTR_ERR(pclass);
        }
        break;

      case 3:
        // Try allocate huge memory -> NULL
        p = kmalloc(1000*1000*1000, GFP_KERNEL);
        trace_printk("kmalloc() = %p\n", p);
        if (!p) {
            trace_printk("returning %d\n", -ENOMEM);
            return -ENOMEM;
        }

      default:
        break;
    }

    return 0;
}

static void exerror_exit(void)
{
}

module_init(exerror_init);
module_exit(exerror_exit);
MODULE_LICENSE("Dual MIT/GPL");
