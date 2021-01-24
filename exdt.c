// exdt.c

#include <linux/module.h>
#include <linux/i2c.h>
#include <asm/uaccess.h>

#define DEVICE_NAME "exdt"

static const struct of_device_id exdt_of_match_table[] = {
    { .compatible = "ex,exdt" },
    { },
};
MODULE_DEVICE_TABLE(of, exdt_of_match_table);

static int exdt_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    if (id) {
        trace_printk("id->name=%s\n", id->name);
    }
    if (client) {
        trace_printk("client->addr=%x\n", client->addr);
    }

    return 0;
}

static int exdt_remove(struct i2c_client *client)
{
    return 0;
}

static struct i2c_driver exdt_driver = {
    .driver = {
        .name = DEVICE_NAME,
        .owner = THIS_MODULE,
        .of_match_table = exdt_of_match_table,
    },
    .probe = exdt_probe,
    .remove = exdt_remove,
};

// Instead of module_init and module_exit
module_i2c_driver(exdt_driver);
MODULE_LICENSE("Dual MIT/GPL");
