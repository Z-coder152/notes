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
    /* 1. 申请内存(第一个参数是设备的父设备，第二个参数是申请的内存大小，
     * 第三个参数是内存的分配方式)
     */
    key_dev = devm_kzalloc(&pdev->dev, sizeof(struct key_dev), GFP_KERNEL);
    if (!key_dev){
        dev_err(&pdev->dev, "devm_kzalloc failed!\n");
        goto err_devm_kzalloc;
    }
    key_dev->dev = &pdev->dev;
    key_dev->name = pdev->name;
    /* 2. 获取gpio引脚编号，可以看作进一步解析设备树信息（第一个参数为匹配成功的设备树节点；
     *   第二个参数为节点中的gpio 属性；第三个参数为gpio的索引，
     *   表示设备树“key-gpio”属性中的第0个节点）
     */
    key_dev->gpio_num = of_get_named_gpio(pdev->dev.of_node, "key-gpio", 0);
    if (!gpio_is_valid(key_dev->gpio_num)){
        dev_err(&pdev->dev, "can't get key-gpio-num!\n");
        goto err_of_get_named_gpio;
    }
    /* 3. 申请gpio资源（第一个参数为设备结构体指针，
     *    第二个参数为上一步得到的gpio引脚，第三个参数为申请的GPIO的名称）
     */
    ret = devm_gpio_request(&pdev->dev, key_dev->gpio_num, "key-gpio");
    if (ret){
        dev_err(&pdev->dev, "request gpio failed!\n");
        goto err_devm_gpio_request;
    }
    
    pr_info("gpio num = %d\r\n", key->gpio_num); //打印调试信息
    key->irq_num=gpio_to_irq(key->gpio_num); //根据IO编号获取其中断号(注：这里是软件编号只是为了区分，不是硬件芯片手册上的编号)
    pr_info("irq num = %d \n", key->irq_num);

    /* 4. 注册中断处理函数（第一个参数为中断号，第二个参数为中断处理函数，
     *    第三个参数为中断触发方式，第四个参数为中断处理函数的名称，
     *    第五个参数用于 多设备共享中断线）
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
    //因devm分配的资源，在rmmod时会自动释放，故不用再加下面的释放函数
    //devm_free_irq(&pdev->dev,irq_num,NULL);	 //释放中断
    //devm_gpio_free(&pdev->dev,gpio_num); //释放gpio管脚
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
