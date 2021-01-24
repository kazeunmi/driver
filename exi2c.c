// exi2c.c

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/i2c.h>
#include <asm/uaccess.h>

#define DEVICE_NAME "exi2c"

static struct i2c_device_id exi2c_idtable[] = {
    {DEVICE_NAME, 0},
    {}
};
MODULE_DEVICE_TABLE(i2c, exi2c_idtable);

// Init sequence from SSD1306 app note
static u8 data_init[] =
{
    0xa8, 0x3f, 0xd3, 0x00, 0x40, 0xa1, 0xc8, 0xda, 0x12,
    0x81, 0x80, 0xa4, 0xa6, 0xd5, 0x80, 0x8d, 0x14, 0xaf,
    0x20, 0x02,
};
                         
static int exi2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    int i;
    int page;

    trace_printk("id->name=%s client->addr=%x\n", id->name, client->addr);

    // Send initial commands
    for (i = 0; i < sizeof(data_init); i++) {
        i2c_smbus_write_byte_data(client, 0, data_init[i]);
    }

    // Send picture
    for (page = 0; page < 8; page++) {
        i2c_smbus_write_byte_data(client, 0, 0xb0 + page);
        for (i = 0; i < 128; i++) {
            i2c_smbus_write_byte_data(client, 0x40, 0xf0);
        }
    }
    return 0;
}

static int exi2c_remove(struct i2c_client *client)
{
    return 0;
}

static struct i2c_driver exi2c_driver = {
    .driver = {
        .name = DEVICE_NAME,
        .owner = THIS_MODULE,
    },
    .id_table = exi2c_idtable,
    .probe = exi2c_probe,
    .remove = exi2c_remove,
};

static int exi2c_init(void)
{
    trace_printk("exi2c_init()\n");

    i2c_add_driver(&exi2c_driver);
    return 0;
}

static void exi2c_exit(void)
{
    i2c_del_driver(&exi2c_driver);
}

// Note: module_i2c_driver(exi2c_driver) can be used instead
module_init(exi2c_init);
module_exit(exi2c_exit);
MODULE_LICENSE("Dual MIT/GPL");
