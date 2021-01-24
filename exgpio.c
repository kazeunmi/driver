// exgpio.c

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/io.h>

#define DEVICE_NAME "exgpio"

#define GPIO_BASE 0x3F200000
#define GPIO_LEN 0x100

#define GPFSEL 0x00
#define GPSET  0x1C
#define GPCLR  0x28
#define GPLEV  0x34

static char* gpiobase;

static int exgpio_open(struct inode *inode, struct file *file)
{
    u32 val32;

    // Prepare I/O address
    gpiobase = ioremap_nocache(GPIO_BASE, GPIO_LEN);
 
    // Select function for GPIO4 (3bit for each, 1 is for output)
    val32 = readl(gpiobase + GPFSEL);
    val32 &= ~(7 << (4 * 3));
    val32 |= 1 << (4 * 3);
    writel(val32, gpiobase + GPFSEL);

    return 0;
}

static int exgpio_close(struct inode *inode, struct file *file)
{
    // Drop I/O address
    iounmap(gpiobase);
    return 0;
}

static ssize_t exgpio_write(struct file *file, const char __user *buf,
                            size_t count, loff_t *pos)
{
    // Read current value
    uint32_t val32 = readl(gpiobase + GPLEV);
    int bit4 = (val32 >> 4) & 1;

    // Flip the value
    writel(1 << 4, gpiobase + ((bit4)? GPCLR : GPSET));
    return count;
}

struct file_operations exgpio_fops = {
    .owner   = THIS_MODULE,
    .open    = exgpio_open,
    .release = exgpio_close,
    .write   = exgpio_write,
};

static struct miscdevice exgpio_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &exgpio_fops,
};

static int exgpio_init(void)
{
    misc_register(&exgpio_device);
    return 0;
}

static void exgpio_exit(void)
{
    misc_deregister(&exgpio_device);
}

module_init(exgpio_init);
module_exit(exgpio_exit);
MODULE_LICENSE("Dual MIT/GPL");
