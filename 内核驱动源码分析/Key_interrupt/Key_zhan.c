#include <linux/init.h>                        
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <asm/io.h>
#include <linux/interrupt.h> 

#ifdef CONFIG_OF
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#endif

struct key_dev{
    struct device *dev;
    const char *name;
    unsigned int gpio_num;
    unsigned int irq_num;
};

irqreturn_t key_interrupt_handler(int irqno, void *devid)
{
    pr_info("do interrupt irqno = %d\n", irqno);
    return IRQ_HANDLED;
}

static int key_probe(struct platform_device *pdev)
{
    int ret = -1;
    struct key_dev *key_dev;
    /* 1. �����ڴ�(��һ���������豸�ĸ��豸���ڶ���������������ڴ��С��
     * �������������ڴ�ķ��䷽ʽ)
     */
    key_dev = devm_kzalloc(&pdev->dev, sizeof(struct key_dev), GFP_KERNEL);
    if (!key_dev){
        dev_err(&pdev->dev, "devm_kzalloc failed!\n");
        goto err_devm_kzalloc;
    }
    key_dev->dev = &pdev->dev;
    key_dev->name = pdev->name;
    /* 2. ��ȡgpio���ű�ţ����Կ�����һ�������豸����Ϣ����һ������Ϊƥ��ɹ����豸���ڵ㣻
     *   �ڶ�������Ϊ�ڵ��е�gpio ���ԣ�����������Ϊgpio��������
     *   ��ʾ�豸����key-gpio�������еĵ�0���ڵ㣩
     */
    key_dev->gpio_num = of_get_named_gpio(pdev->dev.of_node, "key-gpio", 0);
    if (!gpio_is_valid(key_dev->gpio_num)){
        dev_err(&pdev->dev, "can't get key-gpio-num!\n");
        goto err_of_get_named_gpio;
    }
    /* 3. ����gpio��Դ����һ������Ϊ�豸�ṹ��ָ�룬
     *    �ڶ�������Ϊ��һ���õ���gpio���ţ�����������Ϊ�����GPIO�����ƣ�
     */
    ret = devm_gpio_request(&pdev->dev, key_dev->gpio_num, "key-gpio");
    if (ret){
        dev_err(&pdev->dev, "request gpio failed!\n");
        goto err_devm_gpio_request;
    }
    
    pr_info("gpio num = %d\r\n", key->gpio_num); //��ӡ������Ϣ
    key->irq_num=gpio_to_irq(key->gpio_num); //����IO��Ż�ȡ���жϺ�(ע��������������ֻ��Ϊ�����֣�����Ӳ��оƬ�ֲ��ϵı��)
    pr_info("irq num = %d \n", key->irq_num);

    /* 4. ע���жϴ���������һ������Ϊ�жϺţ��ڶ�������Ϊ�жϴ�������
     *    ����������Ϊ�жϴ�����ʽ�����ĸ�����Ϊ�жϴ����������ƣ�
     *    ������������� ���豸�����ж��ߣ�
     */
    ret = devm_request_irq(&pdev->dev, key->irq_num, key_interrupt_handler, IRQF_TRIGGER_FALLING, key_dev->name, key_dev);
    if(ret < 0){
        dev_err(&pdev->dev, "request irq failed!\n");
        goto err_devm_request_irq;
    }

    pr_notice("keys_probe ok \n");
    return 0;

err_devm_request_irq:
    devm_gpio_free(&pdev->dev, key_dev->gpio_num);
err_devm_gpio_request:
err_of_get_named_gpio:
    devm_kfree(&pdev->dev, key_dev);
err_devm_kzalloc:
    return ret;

}

static int key_remove(struct platform_device *pdev)
{   
    //��devm�������Դ����rmmodʱ���Զ��ͷţ��ʲ����ټ�������ͷź���
    //devm_free_irq(&pdev->dev,irq_num,NULL);	 //�ͷ��ж�
    //devm_gpio_free(&pdev->dev,gpio_num); //�ͷ�gpio�ܽ�
    printk("keys_remove  ok 1\n");    
    return 0;
}

static const struct of_device_id key_of_match[] = {
    { .compatible = "zhan,key_interrupt" },
    {},
}; 

MODULE_DEVICE_TABLE(of, key_of_match);

struct platform_driver key_driver = {
    .driver = {
        .name = "zhan_key_interrupt",
        .owner = THIS_MODULE,
        .of_match_table = key_of_match,
    },
    .probe = key_probe,
    .remove = key_remove,
};

module_platform_driver(key_driver);
MODULE_LICENSE("GPL");
