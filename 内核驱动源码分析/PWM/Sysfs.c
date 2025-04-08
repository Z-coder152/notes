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
	struct device child; //�̳��� struct device �ṹ��
	struct pwm_device *pwm; //ָ��PWM�豸�Ľṹ��ָ��
	struct mutex lock;  //������������ͬ������
};

static struct pwm_export *child_to_pwm_export(struct device *child)
{
    /*container_of ��һ���꣬���ڴӽṹ���Աָ���ȡ�����ó�Ա�Ľṹ��ָ��;
    ���ذ��� child ��Ա�� struct pwm_export �ṹ���ָ��*/
	return container_of(child, struct pwm_export, child);
}

static struct pwm_device *child_to_pwm_device(struct device *child)
{
    /*����ָ֪��child�����ͨ������ child_to_pwm_export ��ýṹ�� pwm_export�ĵ�ַ��
    ������ȡ�� pwm�豸�ĵ�ַ*/
	struct pwm_export *export = child_to_pwm_export(child);

	return export->pwm;
}

//��periodʱ�Ļص�����( cat /sys/class/pwm/pwmchip0/pwm0/period ʱ�ص�)
static ssize_t period_show(struct device *child,
			   struct device_attribute *attr,
			   char *buf)
{
	const struct pwm_device *pwm = child_to_pwm_device(child);
	struct pwm_state state;

	pwm_get_state(pwm, &state);

	return sprintf(buf, "%u\n", state.period);
}

/*����������У�����ʹ�õ��� child ��Ա������ pwm ��Ա����Ҫ����Ϊ child ��Ա
��һ�� struct device ���͵�ָ�룬�� pwm ��Ա��һ�� struct pwm_device ���͵�ָ�롣����ԭ�����£�
(1)ͳһ�ӿڣ�
�� Linux �ں��У��ܶ�ӿں������� store �� show ��������ʹ�� struct device ���͵�ָ����Ϊ������
������Ϊ struct device ���豸ģ���е�ͨ�ýṹ�壬���Ա�ʾ�������͵��豸��
ͨ��ʹ�� struct device ���͵�ָ�룬����ʹ��Щ�ӿں�������ͨ���ԣ������ڲ�ͬ���͵��豸��
(2)�豸ģ�͵�һ���֣�
struct pwm_export �ṹ���е� child ��Ա��һ�� struct device ���͵Ľṹ�壬��ʾ������ PWM �豸��
ͨ������ child ��Ա�����������豸ģ���еĸ��ֹ��ܣ����豸ע�ᡢ���Թ���ȡ�
(3)��ȡ������ PWM �豸��
ͨ�� child_to_pwm_export ���������Դ� struct device ָ���ȡ�������豸�� struct pwm_export �ṹ��ָ�롣
��һ���أ�ͨ�� struct pwm_export �ṹ���е� pwm ��Ա�����Ի�ȡ������ struct pwm_device �ṹ��ָ�롣*/

//дperiodʱ�Ļص�����(echo 20000000 >/sys/class/pwm/pwmchip0/pwm0/period ʱ�ص���)
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

    /*ʹ�� test_and_set_bit ������鲢���� PWMF_EXPORTED ��־λ��
    �����־λ�����ã����� -EBUSY����ʾ�豸�ѱ�������*/
	if (test_and_set_bit(PWMF_EXPORTED, &pwm->flags))
		return -EBUSY;

    /*���� kzalloc ������Ϊ������ PWM �豸�����ڴ档����ڴ����ʧ�ܣ�
    ��� PWMF_EXPORTED ��־λ�������� -ENOMEM��*/
	export = kzalloc(sizeof(*export), GFP_KERNEL);
	if (!export) {
		clear_bit(PWMF_EXPORTED, &pwm->flags);
		return -ENOMEM;
	}

	export->pwm = pwm;
	mutex_init(&export->lock); //��ʼ��������

	export->child.release = pwm_export_release; //���õ����� PWM �豸�� release �ص�����
	export->child.parent = parent;              //���õ����� PWM �豸�ĸ��豸
	export->child.devt = MKDEV(0, 0);           //���õ����� PWM �豸���豸��
	export->child.groups = pwm_groups;          //���õ����� PWM �豸��������

    /*���� dev_set_name ������Ϊ������ PWM �豸�������ƣ���ʽΪ��pwm%u�������� %u Ϊ PWM �豸�ı�š�*/
	dev_set_name(&export->child, "pwm%u", pwm->hwpwm);

    /*���� device_register ������ע�ᵼ���� PWM �豸�����ע��ʧ�ܣ���� PWMF_EXPORTED ��־λ��
    �ͷŵ����� PWM �豸���ڴ棬�����ش����롣*/
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

/*���ڴ��� PWM �豸������ sysfs �������������뻺������ȡ PWM �豸�ı�ţ�
����ת��Ϊ�޷���������Ȼ��� pwm_chip ��������Ӧ�� PWM �豸�������䵼���� sysfs;
store����д,��Ӧ echo 3 > /sys/class/pwm/pwmchip0/export*/
static ssize_t export_store(struct device *parent,
			    struct device_attribute *attr,
			    const char *buf, size_t len)
{
	struct pwm_chip *chip = dev_get_drvdata(parent);
	struct pwm_device *pwm;
	unsigned int hwpwm;
	int ret;

    /*���� kstrtouint �����������뻺���� buf �е��ַ���ת��Ϊ�޷������������洢�� hwpwm ��*/
	ret = kstrtouint(buf, 0, &hwpwm);
	if (ret < 0)
		return ret;

	if (hwpwm >= chip->npwm)
		return -ENODEV;

    /*���� pwm_request_from_chip �������� chip ��������Ϊ hwpwm �� PWM �豸��������洢�� pwm ��*/
	pwm = pwm_request_from_chip(chip, hwpwm, "sysfs");
	if (IS_ERR(pwm))
		return PTR_ERR(pwm);

    /*���� pwm_export_child �������� pwm ������ sysfs*/
	ret = pwm_export_child(parent, pwm);
	if (ret < 0)
		pwm_put(pwm);

	return ret ? : len;
}
static DEVICE_ATTR_WO(export); //���������ļ�export ������dev_attr_unexport�� ��ص����� export_store�� WO��ʾֻд


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

/*������һ����̬������ npwm��������ʾ PWM оƬ�� PWM ͨ������(��/sys/class/..Ŀ¼��ʹ�á�cat npwm��
 ����ʱ���Զ��������溯��������Ϣ��ӡ�� ��ʾΪ 4)*/
static ssize_t npwm_show(struct device *parent, struct device_attribute *attr,
			 char *buf)
{
	const struct pwm_chip *chip = dev_get_drvdata(parent);

	return sprintf(buf, "%u\n", chip->npwm);
}
static DEVICE_ATTR_RO(npwm);

/*������һ����̬���������� pwm_chip_attrs�������������ԣ�
 export��unexport �� npwm���Լ�һ�� NULL ��ֹ����
 ��Ӧ��Ŀ¼ /sys/class/pwm/pwmchip0 �´����������ļ�������Ӧ�ò����*/
static struct attribute *pwm_chip_attrs[] = { //������һ����̬���������� pwm_chip_attrs
	&dev_attr_export.attr,                    //�﷨������� һ������Ķ��壬�� int a[] = {1,2,3,4,5};
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

static struct class pwm_class = { // ���/sysĿ¼�½ṹ��/sys/class/pwm/pwmchip0 �����
    /*�����豸������*/
	.name = "pwm",
    /*�����豸���������Ϊ��ǰģ�顣
     *THIS_MODULE ��һ���꣬��ʾ��ǰ�ں�ģ�顣
     *���������ں���ж��ģ��ʱ�������ü�������*/
	.owner = THIS_MODULE,
	.dev_groups = pwm_chip_groups,
};

/*����ƥ���豸����ͨ���Ƚ��豸 parent ���������������봫��� data ��ȷ���Ƿ�ƥ�䡣
����������������� data ��ȣ��򷵻� 1����ʾƥ��ɹ������򣬷��� 0����ʾƥ��ʧ�ܡ�*/
static int pwmchip_sysfs_match(struct device *parent, const void *data)
{
	return dev_get_drvdata(parent) == data;
}

void pwmchip_sysfs_export(struct pwm_chip *chip) //����PWM�豸���ʵ�ֺ���,���ڽ������� pwm_chip ������ sysfs
{
	struct device *parent; //���豸������ṹ

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

void pwmchip_sysfs_unexport(struct pwm_chip *chip) //ж��PWM�豸���ʵ�ֺ���
{
    struct device *parent;
    unsigned int i;

    // ��������� PWM оƬƥ����豸
    parent = class_find_device(&pwm_class, NULL, chip, pwmchip_sysfs_match);
    if (!parent)
        return;

    // ���� PWM оƬ�е����� PWM �豸
    for (i = 0; i < chip->npwm; i++) {
        struct pwm_device *pwm = &chip->pwms[i];

        // ��� PWM �豸�ѵ�������ȡ������
        if (test_bit(PWMF_EXPORTED, &pwm->flags))
            pwm_unexport_child(parent, pwm);
    }

    // �ͷ��豸��ע���豸
    put_device(parent);
    device_unregister(parent);
}

static int __init pwm_sysfs_init(void)
{
	return class_register(&pwm_class);  //��PWM�豸��ע�ᵽ�ں��豸ģ���У�����/sys/class/pwmĿ¼������/sys/class/pwm/pwmchip0Ŀ¼��
}
subsys_initcall(pwm_sysfs_init);  //��ʼ��������ϵͳ��ʼ��ʱ����
