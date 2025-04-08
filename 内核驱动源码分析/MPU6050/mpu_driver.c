#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/delay.h>

#include <asm/uaccess.h>

#include "mpu_driver.h"

MODULE_LICENSE("GPL");

//定义MPU6050内部寄存器地址(源自于mpu6050寄存器手册)
#define SMPLRT_DIV		0x19
#define CONFIG			0x1A
#define GYRO_CONFIG		0x1B
#define ACCEL_CONFIG	0x1C
#define ACCEL_XOUT_H	0x3B
#define ACCEL_XOUT_L	0x3C
#define ACCEL_YOUT_H	0x3D
#define ACCEL_YOUT_L	0x3E
#define ACCEL_ZOUT_H	0x3F
#define ACCEL_ZOUT_L	0x40
#define TEMP_OUT_H		0x41
#define TEMP_OUT_L		0x42
#define GYRO_XOUT_H		0x43
#define GYRO_XOUT_L		0x44
#define GYRO_YOUT_H		0x45
#define GYRO_YOUT_L		0x46
#define GYRO_ZOUT_H		0x47
#define GYRO_ZOUT_L		0x48
#define PWR_MGMT_1		0x6B

#define MPU6050_MAJOR 500
#define MPU6050_MINOR 5

struct mpu6050_device {
	struct cdev cdev;
	struct i2c_client *client; //指向i2c设备的指针, 这里mpu设备是 从设备(slave)，这里写成client
};
struct mpu6050_device *mpu6050; 

/*指定从 哪个i2c客户端设备 的哪个寄存器里读取 字节数据*/
static int mpu6050_read_byte(struct i2c_client *client, unsigned char reg)
{
	int ret;

	char txbuf[1] = { reg };
	char rxbuf[1];

	struct i2c_msg msg[2] = {   //设定 I2C消息格式（固定格式，参见内核 i2c-core.c文件中的 i2c_msg结构体）
		{client->addr, 0, 1, txbuf},       //addr：从设备地址，flags：读写标志（1为读），len：数据长度，buf：数据缓冲区
		{client->addr, I2C_M_RD, 1, rxbuf} 
	};

	ret = i2c_transfer(client->adapter, msg, ARRAY_SIZE(msg)); //传输I2C消息（是读还是写 在msg中指定）
	if (ret < 0) {
		printk("ret = %d\n", ret);
		return ret;
	}

	return rxbuf[0];
}

static int mpu6050_write_byte(struct i2c_client *client, unsigned char reg, unsigned char val)
{
	char txbuf[2] = {reg, val};

	struct i2c_msg msg[1] = {
		{client->addr, 0, 2, txbuf}, //指定这条消息是 “写消息”
	};

	printk("i2c write: addr=0x%02x, reg=0x%02x, val=0x%02x\n", client->addr, reg, val);

	int ret = i2c_transfer(client->adapter, msg, ARRAY_SIZE(msg));
	if (ret < 0) {
        printk("i2c write failed: %d\n", ret);
        return ret;
    }
	printk("i2c write success\n");

	return 0;
}

static int mpu6050_open(struct inode *inode, struct file *file) 
{
	return 0;
}

static int mpu6050_release(struct inode *inode, struct file *file) 
{
	return 0;
}

static long mpu6050_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	union mpu6050_data data;
	struct i2c_client *client = mpu6050->client;

	switch(cmd) {
	case GET_ACCEL:
		data.accel.x = mpu6050_read_byte(client, ACCEL_XOUT_L);//读取x轴加速度低8位
		data.accel.x |= mpu6050_read_byte(client, ACCEL_XOUT_H) << 8;//读取x轴加速度高8位，再和低8位合并

		data.accel.y = mpu6050_read_byte(client, ACCEL_YOUT_L);
		data.accel.y |= mpu6050_read_byte(client, ACCEL_YOUT_H) << 8;

		data.accel.z = mpu6050_read_byte(client, ACCEL_ZOUT_L);
		data.accel.z |= mpu6050_read_byte(client, ACCEL_ZOUT_H) << 8;
		break;

	case GET_GYRO:

		data.gyro.x = mpu6050_read_byte(client, GYRO_XOUT_L);
		data.gyro.x |= mpu6050_read_byte(client, GYRO_XOUT_H) << 8;

		data.gyro.y = mpu6050_read_byte(client, GYRO_YOUT_L);
		data.gyro.y |= mpu6050_read_byte(client, GYRO_YOUT_H) << 8;

		data.gyro.z = mpu6050_read_byte(client, GYRO_ZOUT_L);
		data.gyro.z |= mpu6050_read_byte(client, GYRO_ZOUT_H) << 8;
		break;

	case GET_TEMP:	
		data.temp = mpu6050_read_byte(client, TEMP_OUT_L);
		data.temp |= mpu6050_read_byte(client, TEMP_OUT_H) << 8;
		break;

	default:
		printk("invalid argument\n");
		return -EINVAL;
	}
	//将内核空间的数据复制到用户空间，并检查复制操作是否成功
	if (copy_to_user((void *)arg, &data, sizeof(data)))
		return -EFAULT;

	return sizeof(data);
}

//将文件操作和函数实现相关联
struct file_operations mpu6050_fops = {
	.owner 		= THIS_MODULE,
	.open		= mpu6050_open,
	.release 	= mpu6050_release,
	.unlocked_ioctl = mpu6050_ioctl,
};

static int mpu6050_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	/*字符设备初始化通用步骤：合成设备号->申请空间->注册设备->设备初始化->设备添加进内核*/
	int ret;
	dev_t devno = MKDEV(MPU6050_MAJOR, MPU6050_MINOR);
	printk("match OK!\n");

	mpu6050 = kzalloc(sizeof(*mpu6050), GFP_KERNEL);
	if (mpu6050 == NULL) {
		return -ENOMEM;
	}

	mpu6050->client = client;

	ret = register_chrdev_region(devno, 1, "mpu6050");
	if (ret < 0) {
		printk("failed to register char device region!\n");
		goto err1;
	}

	cdev_init(&mpu6050->cdev, &mpu6050_fops);
	mpu6050->cdev.owner = THIS_MODULE;
	ret = cdev_add(&mpu6050->cdev, devno, 1);
	if (ret < 0) {
		printk("failed to add device\n");
		goto err2;
	}
	
	mpu6050_write_byte(client, PWR_MGMT_1, 0x00);
	mpu6050_write_byte(client, SMPLRT_DIV, 0x07);
	mpu6050_write_byte(client, CONFIG, 0x06);
	mpu6050_write_byte(client, GYRO_CONFIG, 0xF8);
	mpu6050_write_byte(client, ACCEL_CONFIG, 0x19);

	return 0;
err2:
	unregister_chrdev_region(devno, 1);
err1:
	kfree(mpu6050);
	return ret;
}

static int mpu6050_remove(struct i2c_client *client)
{
	dev_t devno = MKDEV(MPU6050_MAJOR, MPU6050_MINOR);
	cdev_del(&mpu6050->cdev);
	unregister_chrdev_region(devno, 1);
	kfree(mpu6050);

	return 0;
}

static const struct i2c_device_id mpu6050_id[] = {
	{ "mpu6050", 0},
	{}
}; 

static struct of_device_id mpu6050_dt_match[] = {
	{.compatible = "invensense,mpu6050" },
	{/*northing to be done*/},
};

struct i2c_driver mpu6050_driver = {
	.driver = {
		.name 			= "mpu6050",
		.owner 			= THIS_MODULE,
		.of_match_table = of_match_ptr(mpu6050_dt_match),
	},
	.probe 		= mpu6050_probe,
	.remove 	= mpu6050_remove,
	.id_table 	= mpu6050_id,
};

module_i2c_driver(mpu6050_driver);