/*
 * drivers/pwm/pwm-tegra.c
 *
 * Tegra pulse-width-modulation controller driver
 *
 * Copyright (c) 2010-2020, NVIDIA CORPORATION. All rights reserved.
 *
 * Based on arch/arm/plat-mxc/pwm.c by Sascha Hauer <s.hauer@pengutronix.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

 #include <linux/clk.h>
 #include <linux/err.h>
 #include <linux/io.h>
 #include <linux/module.h>
 #include <linux/of.h>
 #include <linux/of_device.h>
 #include <linux/pwm.h>
 #include <linux/platform_device.h>
 #include <linux/pinctrl/consumer.h>
 #include <linux/slab.h>
 #include <linux/reset.h>
 
 #define PWM_ENABLE	BIT(31) // 0x80000000 , 31λΪ1����ʾʹ��PWM���
 #define PWM_DUTY_WIDTH	8   // 8λ����ʾռ�ձȵ�λ��
 #define PWM_DUTY_SHIFT	16  
 #define PWM_SCALE_WIDTH	13 // 13λ����ʾƵ�ʷ�Ƶ��λ��
 #define PWM_SCALE_SHIFT	0
 
 #define CLK_1MHZ	1000000UL
 
 struct tegra_pwm_soc {
     unsigned int num_channels;
     unsigned long max_clk_limit;
 };
 
 struct tegra_pwm_chip { //�����豸�ࣨ�������
     struct pwm_chip chip; //�̳��� �ں˵�pwm_chip ����
     struct device *dev;  //�̳���ͳһ�豸ģ�͵�device����
 
     struct clk *clk;
     struct reset_control *rst;
 
     unsigned long clk_rate;
 
     void __iomem *regs; // PWM�Ĵ�������ַ
 
     const struct tegra_pwm_soc *soc; //pwm�豸�ĸ�������
     unsigned long		max_clk_limit;
     int (*clk_enable)(struct clk *clk);
     void (*clk_disable)(struct clk *clk);
 };
 
 //ʹ���ں��ṩ��container_of�꣬������pwm_chip�ṹ��ָ����tegra_pwm_chip�ṹ��ָ��
 static inline struct tegra_pwm_chip *to_tegra_pwm_chip(struct pwm_chip *chip)
 {
     return container_of(chip, struct tegra_pwm_chip, chip);
 }
 
 /*�� Tegra PWM �������ļĴ����ж�ȡһ�� 32 λ��ֵ��������˵��
  *�����ݴ���� PWM ͨ�����(num)����Ĵ����ĵ�ַ�����Ӹõ�ַ��ȡһ�� 32 λ��ֵ
  */
 static inline u32 pwm_readl(struct tegra_pwm_chip *chip, unsigned int num)
 {
     return readl(chip->regs + (num << 4));
 }
 
  /*�� Tegra PWM �������ļĴ�����дһ�� 32 λ��ֵ��������˵��
  *�����ݴ���� PWM ͨ�����(num)����Ĵ����ĵ�ַ�����Ӹõ�ַдһ�� 32 λ��ֵ
  */
 static inline void pwm_writel(struct tegra_pwm_chip *chip, unsigned int num,
                   unsigned long val)
 {
     writel(val, chip->regs + (num << 4));
 }
 
 /*�����������βα��ֺ��ļ�"pwm.h"���`pwm_ops`�ṹ����ĺ���ָ��һ�£�
  *����ͳһ�ӿڣ������ϲ���ã������൱�ڽ�`pwm_ops`�ṹ����ĺ���ʵ���ˣ�
  *ʵ���� config ��enable �� disable ��������;
  *����tegra_pwm_config �������������������� PWM ͨ����ռ�ձȺ�Ƶ�ʣ�
  *������˵�����Ǹ��ݴ����ռ�ձȺ����ڣ��������Ӧ�ļĴ���ֵ����д�� PWM �������ļĴ����У�
  *ϸ�ڲ���׷����
  */ 
 static int tegra_pwm_config(struct pwm_chip *chip, struct pwm_device *pwm,
                 int duty_ns, int period_ns)
 {
     struct tegra_pwm_chip *pc = to_tegra_pwm_chip(chip);
     unsigned long long c = duty_ns, hz;
     unsigned long rate;
     u32 val = 0;
     int err;
 
     /*
      * Convert from duty_ns / period_ns to a fixed number of duty ticks
      * per (1 << PWM_DUTY_WIDTH) cycles and make sure to round to the
      * nearest integer during division.
      */
     c *= (1 << PWM_DUTY_WIDTH);
     c = DIV_ROUND_CLOSEST_ULL(c, period_ns);
 
     val = (u32)c << PWM_DUTY_SHIFT;
 
     /*
      * Its okay to ignore the fraction part since we will be trying to set
      * slightly lower value to rate than the actual required rate
      */
     rate = NSEC_PER_SEC/period_ns;
 
     /*
      *  Period in nano second has to be <= highest allowed period
      *  based on the max clock rate of the pwm controller.
      *
      *  higher limit = max clock limit >> PWM_DUTY_WIDTH
      */
     if (rate > (pc->max_clk_limit >> PWM_DUTY_WIDTH))
         return -EINVAL;
 
     /*
      * Compute the prescaler value for which (1 << PWM_DUTY_WIDTH)
      * cycles at the PWM clock rate will take period_ns nanoseconds.
      */
     if (pc->soc->num_channels == 1) {
         /*
          * Rate is multiplied with 2^PWM_DUTY_WIDTH so that it matches with the
          * hieghest applicable rate that the controller can provide. Any further
          * lower value can be derived by setting PFM bits[0:12]
          * Higher mark is taken since BPMP has round-up mechanism implemented.
          */
         rate = rate << PWM_DUTY_WIDTH;
 
         err = clk_set_rate(pc->clk, rate);
         if (err < 0)
             return -EINVAL;
 
         rate = clk_get_rate(pc->clk) >> PWM_DUTY_WIDTH;
     } else {
         /*
          * This is the case for SoCs who support multiple channels:
          *
          * clk_set_rate() can not be called again in config because T210
          * or any prior chip supports one pwm-controller and multiple channels.
          * Hence in this case cached clock rate will be considered which was
          * stored during probe.
          */
         rate = pc->clk_rate >> PWM_DUTY_WIDTH;
     }
 
     /* Consider precision in PWM_SCALE_WIDTH rate calculation */
     hz = DIV_ROUND_CLOSEST_ULL(100ULL * NSEC_PER_SEC, period_ns);
     rate = DIV_ROUND_CLOSEST_ULL(100ULL * rate, hz);
 
     /*
      * Since the actual PWM divider is the register's frequency divider
      * field minus 1, we need to decrement to get the correct value to
      * write to the register.
      */
     if (rate > 0)
         rate--;
 
     /*
      * Make sure that the rate will fit in the register's frequency
      * divider field.
      */
     if (rate >> PWM_SCALE_WIDTH)
         return -EINVAL;
 
     val |= rate << PWM_SCALE_SHIFT;
 
     /*
      * If the PWM channel is disabled, make sure to turn on the clock
      * before writing the register. Otherwise, keep it enabled.
      */
     if (!pwm_is_enabled(pwm)) {
         err = pc->clk_enable(pc->clk);
         if (err < 0)
             return err;
     } else {
         val |= PWM_ENABLE;
     }
 
     pwm_writel(pc, pwm->hwpwm, val);
 
     /*
      * If the PWM is not enabled, turn the clock off again to save power.
      */
     if (!pwm_is_enabled(pwm))
         pc->clk_disable(pc->clk);
 
     return 0;
 }
 
 /*��������ָ����PWMͨ��������������PWMͨ����ص�ʱ�ӣ�
  *��������Ӧ�ļĴ���λ(��31λ)��ʹ��PWM����� 
  */
 static int tegra_pwm_enable(struct pwm_chip *chip, struct pwm_device *pwm)
 {
     struct tegra_pwm_chip *pc = to_tegra_pwm_chip(chip);
     int rc = 0;
     u32 val;
 
     rc = pc->clk_enable(pc->clk);
     if (rc < 0)
         return rc;
    /*��������������ж��壬���ڶ�ȡpc��ָ��Ľṹ���е�reg��Ա��
     *pwm->hwpwm Ϊָ��pwmͨ����
     *����ֵ�� 32λ�Ĵ����е�ֵ���ȶ������������õ�31λΪ1����ʾʹ��PWM��������д�ؼĴ�����
     */
     val = pwm_readl(pc, pwm->hwpwm);
     val |= PWM_ENABLE;
     pwm_writel(pc, pwm->hwpwm, val);
 
     return 0;
 }
 
 static void tegra_pwm_disable(struct pwm_chip *chip, struct pwm_device *pwm)
 {
     struct tegra_pwm_chip *pc = to_tegra_pwm_chip(chip);
     u32 val;
 
     val = pwm_readl(pc, pwm->hwpwm);
     val &= ~PWM_ENABLE;
     pwm_writel(pc, pwm->hwpwm, val);
 
     pc->clk_disable(pc->clk);
 }
 
 /*"���"��
  *������ĺ���ʵ����������ṹ���У�
  *����ṹ���ǡ�pwm.h���ļ����������pwm��������������pwm_ops����
  *ͨ�����ַ��������ص�������pwm����������������������
  */
 static const struct pwm_ops tegra_pwm_ops = {
     .config = tegra_pwm_config,
     .enable = tegra_pwm_enable,
     .disable = tegra_pwm_disable,
     .owner = THIS_MODULE,
 };
 
 //��ʼ��PWM������
 static int tegra_pwm_probe(struct platform_device *pdev)
 {
     struct tegra_pwm_chip *pwm;
     struct resource *r;
     bool no_clk_sleeping_in_ops;
     struct clk *parent_clk;
     struct clk *parent_slow;
     u32 pval;
     int ret;
    
     /*�����ڴ�ռ䣬��СΪtegra_pwm_chip�ṹ���С��
      *�������뵽���ڴ�ռ����㣬Ȼ�����뵽���ڴ�ռ丳ֵ��pwmָ�룻
      *��һ��������һ��ָ���豸��ָ�룬���ָ�������豸ģ���ṩ�ģ�
      */
     pwm = devm_kzalloc(&pdev->dev, sizeof(*pwm), GFP_KERNEL);
     if (!pwm)
         return -ENOMEM;
 
    /*���豸���л�ȡƥ������ݣ�����ȡtegra_pwm_soc�ṹ��ĵ�ַ��
     *������ȡ�������ݸ�ֵ��pwm->socָ�룻
     */
     pwm->soc = of_device_get_match_data(&pdev->dev);
     pwm->dev = &pdev->dev;
 
    /*���豸����ȡPWM�������Ļ���ַ����PWM�������ļĴ�������ַ��
     *������ȡ���Ļ���ַ ͨ��ioremapӳ����ں������õ����ַ��
     *Ȼ�󸳸�pwm->regsָ�룻
     */
     r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
     pwm->regs = devm_ioremap_resource(&pdev->dev, r);
     if (IS_ERR(pwm->regs))
         return PTR_ERR(pwm->regs);
    
     /*��pwm��������Ϊƽ̨������˽�����ݣ�
      *����removeʱͨ�� platform_get_drvdata�õ�,�Ӷ�������ʹ��ȫ�ֱ���
      */
     platform_set_drvdata(pdev, pwm);
 
     /*����of_property_read_bool()����ֵ��bool���ͣ�
      *��һ������Ϊ�豸���ڵ㣬�ڶ�������Ϊ��������
      *������Դ�����ֵΪtrue���򷵻�true�����򷵻�false��
      */
     no_clk_sleeping_in_ops = of_property_read_bool(pdev->dev.of_node,
                                "nvidia,no-clk-sleeping-in-ops");
     dev_info(&pdev->dev, "PWM clk can%s sleep in ops\n",
             no_clk_sleeping_in_ops ? "not" : "");
 
    /*���豸���л�ȡ����ֵ�������ȡ�ɹ���������ֵ����pval��
     *���򣬷��ش����룻
     */
     ret = of_property_read_u32(pdev->dev.of_node,
                    "pwm-minimum-frequency-hz", &pval);
     if (!ret)
         pwm->max_clk_limit = pval * 256 * (1 << PWM_SCALE_WIDTH);
     else
         pwm->max_clk_limit = pwm->soc->max_clk_limit;
 
    //��ȡ�豸���е���Ϊ"pwm" ��ʱ�����Ե�ֵ
     pwm->clk = devm_clk_get(&pdev->dev, "pwm");
     if (IS_ERR(pwm->clk))
         return PTR_ERR(pwm->clk);
 
     parent_clk = devm_clk_get(&pdev->dev, "parent");
     if (!IS_ERR(parent_clk)) {
         struct device *dev = &pdev->dev;
 
         /*
          * Set PWM frequency to lower so that it can switch
          * to parent with higher clock rate.
          */
         ret = clk_set_rate(pwm->clk, CLK_1MHZ);
         if (ret < 0) {
             dev_err(dev, "Failed to set 1M clock rate: %d\n", ret);
             return ret;
         }
 
         ret = clk_set_parent(pwm->clk, parent_clk);
         if (ret < 0) {
             dev_err(dev, "Failed to set parent clk: %d\n", ret);
             return ret;
         }
 
         /* Set clock to maximum clock limit */
         ret = clk_set_rate(pwm->clk, pwm->max_clk_limit);
         if (ret < 0) {
             dev_err(dev, "Failed to set max clk rate: %d\n", ret);
             return ret;
         }
     }
 
     /* Read PWM clock rate from source */
     pwm->clk_rate = clk_get_rate(pwm->clk);
 
     /* Limit the maximum clock rate */
     if (pwm->max_clk_limit &&
         (pwm->clk_rate > pwm->max_clk_limit)) {
         ret = clk_set_rate(pwm->clk, pwm->max_clk_limit);
         if (ret < 0) {
             dev_err(&pdev->dev, "Failed to set max clk rate: %d\n",
                 ret);
             return ret;
         }
         pwm->clk_rate = clk_get_rate(pwm->clk);
     }
 
     if (pwm->clk_rate <= pwm->max_clk_limit)
         goto parent_done;
     /*
      * If clk_rate is still higher than the max_clk_limit then
      * switch to slow parent if exist
      */
     parent_slow = devm_clk_get(&pdev->dev, "slow-parent");
     if (IS_ERR(parent_slow)) {
         dev_warn(&pdev->dev, "Source clock %lu is higher than required %lu\n",
              pwm->clk_rate, pwm->max_clk_limit);
         goto parent_done;
     }
 
     ret = clk_set_parent(pwm->clk, parent_slow);
     if (ret < 0) {
         dev_err(&pdev->dev, "Failed to set slow-parent: %d\n", ret);
         return ret;
     }
 
     /* Set clock to maximum clock limit */
     ret = clk_set_rate(pwm->clk, pwm->max_clk_limit);
     if (ret < 0) {
         dev_err(&pdev->dev, "Failed to set max clk rate: %d\n", ret);
         return ret;
     }
     pwm->clk_rate = clk_get_rate(pwm->clk);
 
 parent_done:
     if (no_clk_sleeping_in_ops) {
         ret = clk_prepare(pwm->clk);
         if (ret) {
             dev_err(&pdev->dev, "PWM clock prepare failed\n");
             return ret;
         }
         pwm->clk_enable = clk_enable;
         pwm->clk_disable = clk_disable;
     } else {
         pwm->clk_enable = clk_prepare_enable;
         pwm->clk_disable = clk_disable_unprepare;
     }
 
     //���豸���л�ȡ��Ϊ"pwm"�ĸ�λ���������Ե�ֵ
     pwm->rst = devm_reset_control_get(&pdev->dev, "pwm");
     if (IS_ERR(pwm->rst)) {
         ret = PTR_ERR(pwm->rst);
         dev_err(&pdev->dev, "Reset control is not found: %d\n", ret);
         return ret;
     }
 
     reset_control_deassert(pwm->rst);
     
    /*���������ʼ��PWM�������Ļ�����Ϣ(ʹ���豸���е���Ϣ��
     *������ĳ�ʼ����Ϣһ�𹹳���PWM��������������Ϣ)
     */
     pwm->chip.dev = &pdev->dev;
     pwm->chip.ops = &tegra_pwm_ops;
     pwm->chip.base = -1;
     pwm->chip.npwm = pwm->soc->num_channels;
 
    /*ע��PWM������
     *����˳��-> pwmchip_add_with_polarity -> pwmchip_sysfs_export -> device_create
     */
     ret = pwmchip_add(&pwm->chip);
     if (ret < 0) {
         dev_err(&pdev->dev, "pwmchip_add() failed: %d\n", ret);
         reset_control_assert(pwm->rst);
         return ret;
     }
 
     return 0;
 }
 
 /*�ͷ�PWM����������Դ������ʱ�ӡ���λ��������PWM�������Ļ���ַ�ȣ�
  */
 static int tegra_pwm_remove(struct platform_device *pdev)
 {
     struct tegra_pwm_chip *pc = platform_get_drvdata(pdev);
     unsigned int i;
     int err;
 
     if (WARN_ON(!pc))
         return -ENODEV;
 
     err = pc->clk_enable(pc->clk);
     if (err < 0)
         return err;
 
     for (i = 0; i < pc->chip.npwm; i++) {
         struct pwm_device *pwm = &pc->chip.pwms[i];
 
         if (!pwm_is_enabled(pwm))
             if (pc->clk_enable(pc->clk) < 0)
                 continue;
 
         pwm_writel(pc, i, 0);
 
         pc->clk_disable(pc->clk);
     }
 
     reset_control_assert(pc->rst);
     clk_disable_unprepare(pc->clk);
 
     return pwmchip_remove(&pc->chip);
 }
 
 #ifdef CONFIG_PM_SLEEP
 static int tegra_pwm_suspend(struct device *dev)
 {
     return pinctrl_pm_select_sleep_state(dev);
 }
 
 static int tegra_pwm_resume(struct device *dev)
 {
     return pinctrl_pm_select_default_state(dev);
 }
 #endif
 
 static const struct tegra_pwm_soc tegra20_pwm_soc = {
     .num_channels = 4, //ͨ����4 ��ָֻ�� pwm0 pwm1 pwm2 pwm3)
     .max_clk_limit = 48000000UL, /* 48 MHz  ����ʱ��Ƶ��*/
 };
 
 static const struct tegra_pwm_soc tegra186_pwm_soc = {
     .num_channels = 1,
     .max_clk_limit = 102000000UL, /*102 MHz */
 };
 
 static const struct tegra_pwm_soc tegra194_pwm_soc = {
     .num_channels = 1,
     .max_clk_limit = 408000000UL, /*408 MHz */
 };
 
 //����ƥ���豸��
 static const struct of_device_id tegra_pwm_of_match[] = {
     { .compatible = "nvidia,tegra20-pwm", .data = &tegra20_pwm_soc },
     { .compatible = "nvidia,tegra186-pwm", .data = &tegra186_pwm_soc },
     { .compatible = "nvidia,tegra194-pwm", .data = &tegra194_pwm_soc },
     { }
 };
 
 MODULE_DEVICE_TABLE(of, tegra_pwm_of_match);
 
 static const struct dev_pm_ops tegra_pwm_pm_ops = {
     SET_SYSTEM_SLEEP_PM_OPS(tegra_pwm_suspend, tegra_pwm_resume)
 };
 
 static struct platform_driver tegra_pwm_driver = {
     .driver = {
         .name = "tegra-pwm",
         .of_match_table = tegra_pwm_of_match,
         .pm = &tegra_pwm_pm_ops,
     },
     .probe = tegra_pwm_probe,
     .remove = tegra_pwm_remove,
 };
 
 module_platform_driver(tegra_pwm_driver);
 
 MODULE_LICENSE("GPL");
 MODULE_AUTHOR("NVIDIA Corporation");
 MODULE_ALIAS("platform:tegra-pwm");
 