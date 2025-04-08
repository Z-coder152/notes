/*
 * A simple sysfs interface for the generic PWM framework
 *
 * Copyright (C) 2013 H Hartley Sweeten <hsweeten@visionengravers.com>
 *
 * Based on previous work by Lars Poeschel <poeschel@lemonage.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/kdev_t.h>
#include <linux/pwm.h>

struct pwm_export {
	struct device child; //继承自 struct device 结构体
	struct pwm_device *pwm; //指向PWM设备的结构体指针
	struct mutex lock;  //互斥锁，用于同步访问
};

static struct pwm_export *child_to_pwm_export(struct device *child)
{
    /*container_of 是一个宏，用于从结构体成员指针获取包含该成员的结构体指针;
    返回包含 child 成员的 struct pwm_export 结构体的指针*/
	return container_of(child, struct pwm_export, child);
}

static struct pwm_device *child_to_pwm_device(struct device *child)
{
    /*若已知指针child，则可通过函数 child_to_pwm_export 获得结构体 pwm_export的地址，
    进而再取出 pwm设备的地址*/
	struct pwm_export *export = child_to_pwm_export(child);

	return export->pwm;
}

//读period时的回调函数( cat /sys/class/pwm/pwmchip0/pwm0/period 时回调)
static ssize_t period_show(struct device *child,
			   struct device_attribute *attr,
			   char *buf)
{
	const struct pwm_device *pwm = child_to_pwm_device(child);
	struct pwm_state state;

	pwm_get_state(pwm, &state);

	return sprintf(buf, "%u\n", state.period);
}

/*在这个函数中，传参使用的是 child 成员而不是 pwm 成员，主要是因为 child 成员
是一个 struct device 类型的指针，而 pwm 成员是一个 struct pwm_device 类型的指针。具体原因如下：
(1)统一接口：
在 Linux 内核中，很多接口函数（如 store 和 show 函数）都使用 struct device 类型的指针作为参数。
这是因为 struct device 是设备模型中的通用结构体，可以表示各种类型的设备。
通过使用 struct device 类型的指针，可以使这些接口函数具有通用性，适用于不同类型的设备。
(2)设备模型的一部分：
struct pwm_export 结构体中的 child 成员是一个 struct device 类型的结构体，表示导出的 PWM 设备。
通过传递 child 成员，可以利用设备模型中的各种功能，如设备注册、属性管理等。
(3)获取关联的 PWM 设备：
通过 child_to_pwm_export 函数，可以从 struct device 指针获取包含该设备的 struct pwm_export 结构体指针。
进一步地，通过 struct pwm_export 结构体中的 pwm 成员，可以获取关联的 struct pwm_device 结构体指针。*/

//写period时的回调函数(echo 20000000 >/sys/class/pwm/pwmchip0/pwm0/period 时回调它)
static ssize_t period_store(struct device *child,
			    struct device_attribute *attr,
			    const char *buf, size_t size)
{
	struct pwm_export *export = child_to_pwm_export(child);
	struct pwm_device *pwm = export->pwm;
	struct pwm_state state;
	unsigned int val;
	int ret;

	ret = kstrtouint(buf, 0, &val);
	if (ret)
		return ret;

	mutex_lock(&export->lock);
	pwm_get_state(pwm, &state);
	state.period = val;
	ret = pwm_apply_state(pwm, &state);
	mutex_unlock(&export->lock);

	return ret ? : size;
}

static ssize_t duty_cycle_show(struct device *child,
			       struct device_attribute *attr,
			       char *buf)
{
	const struct pwm_device *pwm = child_to_pwm_device(child);
	struct pwm_state state;

	pwm_get_state(pwm, &state);

	return sprintf(buf, "%u\n", state.duty_cycle);
}

static ssize_t duty_cycle_store(struct device *child,
				struct device_attribute *attr,
				const char *buf, size_t size)
{
	struct pwm_export *export = child_to_pwm_export(child);
	struct pwm_device *pwm = export->pwm;
	struct pwm_state state;
	unsigned int val;
	int ret;

	ret = kstrtouint(buf, 0, &val);
	if (ret)
		return ret;

	mutex_lock(&export->lock);
	pwm_get_state(pwm, &state);
	state.duty_cycle = val;
	ret = pwm_apply_state(pwm, &state);
	mutex_unlock(&export->lock);

	return ret ? : size;
}

static ssize_t enable_show(struct device *child,
			   struct device_attribute *attr,
			   char *buf)
{
	const struct pwm_device *pwm = child_to_pwm_device(child);
	struct pwm_state state;

	pwm_get_state(pwm, &state);

	return sprintf(buf, "%d\n", state.enabled);
}

static ssize_t enable_store(struct device *child,
			    struct device_attribute *attr,
			    const char *buf, size_t size)
{
	struct pwm_export *export = child_to_pwm_export(child);
	struct pwm_device *pwm = export->pwm;
	struct pwm_state state;
	int val, ret;

	ret = kstrtoint(buf, 0, &val);
	if (ret)
		return ret;

	mutex_lock(&export->lock);

	pwm_get_state(pwm, &state);

	switch (val) {
	case 0:
		state.enabled = false;
		break;
	case 1:
		state.enabled = true;
		break;
	default:
		ret = -EINVAL;
		goto unlock;
	}

	ret = pwm_apply_state(pwm, &state);

unlock:
	mutex_unlock(&export->lock);
	return ret ? : size;
}

static ssize_t polarity_show(struct device *child,
			     struct device_attribute *attr,
			     char *buf)
{
	const struct pwm_device *pwm = child_to_pwm_device(child);
	const char *polarity = "unknown";
	struct pwm_state state;

	pwm_get_state(pwm, &state);

	switch (state.polarity) {
	case PWM_POLARITY_NORMAL:
		polarity = "normal";
		break;

	case PWM_POLARITY_INVERSED:
		polarity = "inversed";
		break;
	}

	return sprintf(buf, "%s\n", polarity);
}

static ssize_t polarity_store(struct device *child,
			      struct device_attribute *attr,
			      const char *buf, size_t size)
{
	struct pwm_export *export = child_to_pwm_export(child);
	struct pwm_device *pwm = export->pwm;
	enum pwm_polarity polarity;
	struct pwm_state state;
	int ret;

	if (sysfs_streq(buf, "normal"))
		polarity = PWM_POLARITY_NORMAL;
	else if (sysfs_streq(buf, "inversed"))
		polarity = PWM_POLARITY_INVERSED;
	else
		return -EINVAL;

	mutex_lock(&export->lock);
	pwm_get_state(pwm, &state);
	state.polarity = polarity;
	ret = pwm_apply_state(pwm, &state);
	mutex_unlock(&export->lock);

	return ret ? : size;
}

static ssize_t capture_show(struct device *child,
			    struct device_attribute *attr,
			    char *buf)
{
	struct pwm_device *pwm = child_to_pwm_device(child);
	struct pwm_capture result;
	int ret;

	ret = pwm_capture(pwm, &result, jiffies_to_msecs(HZ));
	if (ret)
		return ret;

	return sprintf(buf, "%u %u\n", result.period, result.duty_cycle);
}

static ssize_t ramp_time_show(struct device *child,
			      struct device_attribute *attr,
			      char *buf)
{
	const struct pwm_device *pwm = child_to_pwm_device(child);
	struct pwm_state state;

	pwm_get_state(pwm, &state);

	return sprintf(buf, "%u\n", state.ramp_time);
}

static ssize_t ramp_time_store(struct device *child,
			       struct device_attribute *attr,
			       const char *buf, size_t size)
{
	struct pwm_export *export = child_to_pwm_export(child);
	struct pwm_device *pwm = export->pwm;
	struct pwm_state state;
	unsigned int val;
	int ret;

	ret = kstrtouint(buf, 0, &val);
	if (ret)
		return ret;

	mutex_lock(&export->lock);
	pwm_get_state(pwm, &state);
	state.ramp_time = val;
	ret = pwm_apply_state(pwm, &state);
	mutex_unlock(&export->lock);

	return ret ? : size;
}

static ssize_t double_period_show(struct device *child,
				  struct device_attribute *attr,
				  char *buf)
{
	const struct pwm_device *pwm = child_to_pwm_device(child);
	struct pwm_state state;

	pwm_get_state(pwm, &state);

	return sprintf(buf, "%u\n", state.double_period);
}

static ssize_t double_period_store(struct device *child,
				   struct device_attribute *attr,
				   const char *buf, size_t size)
{
	struct pwm_export *export = child_to_pwm_export(child);
	struct pwm_device *pwm = export->pwm;
	struct pwm_state state;
	unsigned int val;
	int ret;

	ret = kstrtouint(buf, 0, &val);
	if (ret)
		return ret;

	mutex_lock(&export->lock);
	pwm_get_state(pwm, &state);
	state.double_period = val;
	ret = pwm_apply_state(pwm, &state);
	mutex_unlock(&export->lock);

	return ret ? : size;
}

static ssize_t capture_window_length_show(struct device *child,
					  struct device_attribute *attr,
					  char *buf)
{
	const struct pwm_device *pwm = child_to_pwm_device(child);
	struct pwm_state state;

	pwm_get_state(pwm, &state);

	return sprintf(buf, "%u\n", state.capture_win_len);
}

static ssize_t capture_window_length_store(struct device *child,
					   struct device_attribute *attr,
					   const char *buf, size_t size)
{
	struct pwm_export *export = child_to_pwm_export(child);
	struct pwm_device *pwm = export->pwm;
	struct pwm_state state;
	unsigned int val;
	int ret;

	ret = kstrtouint(buf, 0, &val);
	if (ret)
		return ret;

	mutex_lock(&export->lock);
	pwm_get_state(pwm, &state);
	state.capture_win_len = val;
	ret = pwm_apply_state(pwm, &state);
	mutex_unlock(&export->lock);

	return ret ? : size;
}

static DEVICE_ATTR_RW(period);
static DEVICE_ATTR_RW(duty_cycle);
static DEVICE_ATTR_RW(enable);
static DEVICE_ATTR_RW(polarity);
static DEVICE_ATTR_RO(capture);
static DEVICE_ATTR_RW(ramp_time);
static DEVICE_ATTR_RW(double_period);
static DEVICE_ATTR_RW(capture_window_length);

static struct attribute *pwm_attrs[] = {
	&dev_attr_period.attr,
	&dev_attr_duty_cycle.attr,
	&dev_attr_enable.attr,
	&dev_attr_polarity.attr,
	&dev_attr_capture.attr,
	&dev_attr_ramp_time.attr,
	&dev_attr_double_period.attr,
	&dev_attr_capture_window_length.attr,
	NULL
};
ATTRIBUTE_GROUPS(pwm);

static void pwm_export_release(struct device *child)
{
	struct pwm_export *export = child_to_pwm_export(child);

	kfree(export);
}

static int pwm_export_child(struct device *parent, struct pwm_device *pwm)
{
	struct pwm_export *export;
	char *pwm_prop[2];
	int ret;

    /*使用 test_and_set_bit 函数检查并设置 PWMF_EXPORTED 标志位。
    如果标志位已设置，返回 -EBUSY，表示设备已被导出。*/
	if (test_and_set_bit(PWMF_EXPORTED, &pwm->flags))
		return -EBUSY;

    /*调用 kzalloc 函数，为导出的 PWM 设备分配内存。如果内存分配失败，
    清除 PWMF_EXPORTED 标志位，并返回 -ENOMEM。*/
	export = kzalloc(sizeof(*export), GFP_KERNEL);
	if (!export) {
		clear_bit(PWMF_EXPORTED, &pwm->flags);
		return -ENOMEM;
	}

	export->pwm = pwm;
	mutex_init(&export->lock); //初始化互斥锁

	export->child.release = pwm_export_release; //设置导出的 PWM 设备的 release 回调函数
	export->child.parent = parent;              //设置导出的 PWM 设备的父设备
	export->child.devt = MKDEV(0, 0);           //设置导出的 PWM 设备的设备号
	export->child.groups = pwm_groups;          //设置导出的 PWM 设备的属性组

    /*调用 dev_set_name 函数，为导出的 PWM 设备设置名称，格式为“pwm%u”，其中 %u 为 PWM 设备的编号。*/
	dev_set_name(&export->child, "pwm%u", pwm->hwpwm);

    /*调用 device_register 函数，注册导出的 PWM 设备。如果注册失败，清除 PWMF_EXPORTED 标志位，
    释放导出的 PWM 设备的内存，并返回错误码。*/
	ret = device_register(&export->child);
	if (ret) {
		clear_bit(PWMF_EXPORTED, &pwm->flags);
		kfree(export);
		return ret;
	}
	pwm_prop[0] = kasprintf(GFP_KERNEL, "EXPORT=pwm%u", pwm->hwpwm);
	pwm_prop[1] = NULL;
	kobject_uevent_env(&parent->kobj, KOBJ_CHANGE, pwm_prop);
	kfree(pwm_prop[0]);

	return 0;
}

static int pwm_unexport_match(struct device *child, void *data)
{
	return child_to_pwm_device(child) == data;
}

static int pwm_unexport_child(struct device *parent, struct pwm_device *pwm)
{
	struct device *child;
	char *pwm_prop[2];

	if (!test_and_clear_bit(PWMF_EXPORTED, &pwm->flags))
		return -ENODEV;

	child = device_find_child(parent, pwm, pwm_unexport_match);
	if (!child)
		return -ENODEV;

	pwm_prop[0] = kasprintf(GFP_KERNEL, "UNEXPORT=pwm%u", pwm->hwpwm);
	pwm_prop[1] = NULL;
	kobject_uevent_env(&parent->kobj, KOBJ_CHANGE, pwm_prop);
	kfree(pwm_prop[0]);

	/* for device_find_child() */
	put_device(child);
	device_unregister(child);
	pwm_put(pwm);

	return 0;
}

/*用于处理将 PWM 设备导出到 sysfs 的请求。它从输入缓冲区读取 PWM 设备的编号，
将其转换为无符号整数，然后从 pwm_chip 中请求相应的 PWM 设备，并将其导出到 sysfs;
store代表写,对应 echo 3 > /sys/class/pwm/pwmchip0/export*/
static ssize_t export_store(struct device *parent,
			    struct device_attribute *attr,
			    const char *buf, size_t len)
{
	struct pwm_chip *chip = dev_get_drvdata(parent);
	struct pwm_device *pwm;
	unsigned int hwpwm;
	int ret;

    /*调用 kstrtouint 函数，将输入缓冲区 buf 中的字符串转换为无符号整数，并存储在 hwpwm 中*/
	ret = kstrtouint(buf, 0, &hwpwm);
	if (ret < 0)
		return ret;

	if (hwpwm >= chip->npwm)
		return -ENODEV;

    /*调用 pwm_request_from_chip 函数，从 chip 中请求编号为 hwpwm 的 PWM 设备，并将其存储在 pwm 中*/
	pwm = pwm_request_from_chip(chip, hwpwm, "sysfs");
	if (IS_ERR(pwm))
		return PTR_ERR(pwm);

    /*调用 pwm_export_child 函数，将 pwm 导出到 sysfs*/
	ret = pwm_export_child(parent, pwm);
	if (ret < 0)
		pwm_put(pwm);

	return ret ? : len;
}
static DEVICE_ATTR_WO(export); //关联属性文件export 的属性dev_attr_unexport和 其回调函数 export_store； WO表示只写


static ssize_t unexport_store(struct device *parent,
			      struct device_attribute *attr,
			      const char *buf, size_t len)
{
	struct pwm_chip *chip = dev_get_drvdata(parent);
	unsigned int hwpwm;
	int ret;

	ret = kstrtouint(buf, 0, &hwpwm);
	if (ret < 0)
		return ret;

	if (hwpwm >= chip->npwm)
		return -ENODEV;

	ret = pwm_unexport_child(parent, &chip->pwms[hwpwm]);

	return ret ? : len;
}
static DEVICE_ATTR_WO(unexport);

/*定义了一个静态的属性 npwm，用于显示 PWM 芯片的 PWM 通道数量(在/sys/class/..目录下使用“cat npwm”
 命令时，自动调用下面函数进行信息打印， 显示为 4)*/
static ssize_t npwm_show(struct device *parent, struct device_attribute *attr,
			 char *buf)
{
	const struct pwm_chip *chip = dev_get_drvdata(parent);

	return sprintf(buf, "%u\n", chip->npwm);
}
static DEVICE_ATTR_RO(npwm);

/*定义了一个静态的属性数组 pwm_chip_attrs，包含三个属性：
 export、unexport 和 npwm，以及一个 NULL 终止符。
 对应在目录 /sys/class/pwm/pwmchip0 下创建这三个文件，方便应用层调用*/
static struct attribute *pwm_chip_attrs[] = { //定义了一个静态的属性数组 pwm_chip_attrs
	&dev_attr_export.attr,                    //语法理解可类比 一般数组的定义，如 int a[] = {1,2,3,4,5};
	&dev_attr_unexport.attr,
	&dev_attr_npwm.attr,
	NULL,
};
/*#define ATTRIBUTE_GROUPS(name) \
    const struct attribute_group *name##_groups[] = { \
        &name##_group, \
        NULL, \
    } */
ATTRIBUTE_GROUPS(pwm_chip);

static struct class pwm_class = { // 结合/sys目录下结构：/sys/class/pwm/pwmchip0 来理解
    /*设置设备类名称*/
	.name = "pwm",
    /*设置设备类的所有者为当前模块。
     *THIS_MODULE 是一个宏，表示当前内核模块。
     *这有助于内核在卸载模块时进行引用计数管理*/
	.owner = THIS_MODULE,
	.dev_groups = pwm_chip_groups,
};

/*用于匹配设备。它通过比较设备 parent 的驱动程序数据与传入的 data 来确定是否匹配。
如果驱动程序数据与 data 相等，则返回 1，表示匹配成功；否则，返回 0，表示匹配失败。*/
static int pwmchip_sysfs_match(struct device *parent, const void *data)
{
	return dev_get_drvdata(parent) == data;
}

void pwmchip_sysfs_export(struct pwm_chip *chip) //导出PWM设备类的实现函数,用于将给定的 pwm_chip 导出到 sysfs
{
	struct device *parent; //父设备，链表结构

	/*
	 * If device_create() fails the pwm_chip is still usable by
	 * the kernel its just not exported.
	 */
	parent = device_create(&pwm_class, chip->dev, MKDEV(0, 0), chip,
			       "pwmchip%d", chip->base);
	if (IS_ERR(parent)) {
		dev_warn(chip->dev,
			 "device_create failed for pwm_chip sysfs export\n");
	}
}

void pwmchip_sysfs_unexport(struct pwm_chip *chip) //卸载PWM设备类的实现函数
{
    struct device *parent;
    unsigned int i;

    // 查找与给定 PWM 芯片匹配的设备
    parent = class_find_device(&pwm_class, NULL, chip, pwmchip_sysfs_match);
    if (!parent)
        return;

    // 遍历 PWM 芯片中的所有 PWM 设备
    for (i = 0; i < chip->npwm; i++) {
        struct pwm_device *pwm = &chip->pwms[i];

        // 如果 PWM 设备已导出，则取消导出
        if (test_bit(PWMF_EXPORTED, &pwm->flags))
            pwm_unexport_child(parent, pwm);
    }

    // 释放设备并注销设备
    put_device(parent);
    device_unregister(parent);
}

static int __init pwm_sysfs_init(void)
{
	return class_register(&pwm_class);  //将PWM设备类注册到内核设备模型中；创建/sys/class/pwm目录；创建/sys/class/pwm/pwmchip0目录；
}
subsys_initcall(pwm_sysfs_init);  //初始化函数，系统初始化时调用
