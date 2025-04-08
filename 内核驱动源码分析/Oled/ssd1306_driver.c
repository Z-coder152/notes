#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/delay.h>

#include <asm/uaccess.h>

#include "ssd1306_driver.h"

MODULE_LICENSE("GPL");


#define SSD1306_MAJOR 500
#define SSD1306_MINOR 6

struct ssd1306_device {
	struct cdev cdev;
	struct i2c_client *client;
};
struct ssd1306_device *ssd1306; 

static int ssd1306_read_byte(struct i2c_client *client, unsigned char reg)
{
	int ret;

	char txbuf[1] = { reg };
	char rxbuf[1];

	struct i2c_msg msg[2] = {   //设定 I2C消息格式
		{client->addr, 0, 1, txbuf},       //0         指定i2c设备里的寄存器地址   txbuf 存放其地址
		{client->addr, I2C_M_RD, 1, rxbuf} //I2C_M_RD  表示读i2c设备里该寄存器的数据
	};

	ret = i2c_transfer(client->adapter, msg, ARRAY_SIZE(msg)); //传输I2C消息
	if (ret < 0) {
		printk("ret = %d\n", ret);
		return ret;
	}

	return rxbuf[0];
}

static int ssd1306_write_byte(struct i2c_client *client, unsigned char val)
{
    int ret = 0;
	char txbuf[2] = {0x00, val};
T
	struct i2c_msg msg[1] = {
		{client->addr, 0, sizeof(txbuf), txbuf}, //指定这条消息是 “写消息”
	};

	printk("i2c write: addr=0x%02x, val=0x%02x\n", client->addr, val);

	ret = i2c_transfer(client->adapter, msg, ARRAY_SIZE(msg));
	if (ret < 0) {
        printk("i2c write failed: %d\n", ret);
        return ret;
    }
	printk("i2c write success\n");

	return 0;
}

static int ssd1306_open(struct inode *inode, struct file *file) 
{
	return 0;
}

static int ssd1306_release(struct inode *inode, struct file *file) 
{
	return 0;
}

static long ssd1306_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	
	struct i2c_client *client = ssd1306->client;

	switch(cmd) {
	case OLED_OPEN:
		ssd1306_write_byte(client, 0xAF);
		break;
	case OLED_CLOSE:
        ssd1306_write_byte(client, 0xAE);
		break;
	default:
		printk("invalid argument\n");
		return -EINVAL;
	}
	return 0;
}

struct file_operations ssd1306_fops = {
	.owner 		= THIS_MODULE,
	.open		= ssd1306_open,
	.release 	= ssd1306_release,
	.unlocked_ioctl = ssd1306_ioctl,
};

static int ssd1306_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int ret;
	dev_t devno = MKDEV(SSD1306_MAJOR, SSD1306_MINOR);
	printk("match OK!\n");

	ssd1306 = kzalloc(sizeof(*ssd1306), GFP_KERNEL);
	if (ssd1306 == NULL) {
		return -ENOMEM;
	}

	ssd1306->client = client;

	ret = register_chrdev_region(devno, 1, "ssd1306_zcl");
	if (ret < 0) {
		printk("failed to register char device region!\n");
		goto err1;
	}

	cdev_init(&ssd1306->cdev, &ssd1306_fops);
	ssd1306->cdev.owner = THIS_MODULE;
	ret = cdev_add(&ssd1306->cdev, devno, 1);
	if (ret < 0) {
		printk("failed to add device\n");
		goto err2;
	}
	
	ssd1306_write_byte(client, 0xAE);
    ssd1306_write_byte(client, 0xD5);
    ssd1306_write_byte(client, 0x80);
    ssd1306_write_byte(client, 0xA8);
    ssd1306_write_byte(client, 0x3F);

    ssd1306_write_byte(client, 0xD3);
    ssd1306_write_byte(client, 0x00);

    ssd1306_write_byte(client, 0x40);//起始行
    ssd1306_write_byte(client, 1);

    ssd1306_write_byte(client, 0x8D);
    ssd1306_write_byte(client, 0x14);

    ssd1306_write_byte(client, 0x20);
    ssd1306_write_byte(client, 0x00);

    ssd1306_write_byte(client, 0xA1);
    ssd1306_write_byte(client, 0xC0);

    ssd1306_write_byte(client, 0xDA);
    ssd1306_write_byte(client, 0x12);

    ssd1306_write_byte(client, 0x81);
    ssd1306_write_byte(client, 0xEF);

    ssd1306_write_byte(client, 0xD9);
    ssd1306_write_byte(client, 0xF1);

    ssd1306_write_byte(client, 0xDB);
    ssd1306_write_byte(client, 0x30);

    ssd1306_write_byte(client, 0xA4);
    ssd1306_write_byte(client, 0xA6);
    ssd1306_write_byte(client, 0xAF); //开启显示


	return 0;
err2:
	unregister_chrdev_region(devno, 1);
err1:
	kfree(ssd1306);
	return ret;
}

static int ssd1306_remove(struct i2c_client *client)
{
	dev_t devno = MKDEV(SSD1306_MAJOR, SSD1306_MINOR);
	cdev_del(&ssd1306->cdev);
	unregister_chrdev_region(devno, 1);
	kfree(ssd1306);

	return 0;
}

static const struct i2c_device_id ssd1306_id[] = {
	{ "ssd1306", 0},
	{}
}; 

static struct of_device_id ssd1306_dt_match[] = {
	{.compatible = "oled_ssd1306" },
	{/*nothing to be done*/},
};

struct i2c_driver ssd1306_driver = {
	.driver = {
		.name 			= "ssd1306",
		.owner 			= THIS_MODULE,
		.of_match_table = of_match_ptr(ssd1306_dt_match),
	},
	.probe 		= ssd1306_probe,
	.remove 	= ssd1306_remove,
	.id_table 	= ssd1306_id,
};

module_i2c_driver(ssd1306_driver);