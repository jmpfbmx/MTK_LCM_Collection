/*
 * Copyright (C) 2015 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */
#include <linux/string.h>
#include <linux/wait.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/pinctrl/consumer.h>
#include <linux/of_gpio.h>
#include <asm-generic/gpio.h>

#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>

#ifdef CONFIG_OF
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/regulator/consumer.h>
#include <linux/clk.h>
#endif

#include <upmu_common.h>

#include "lcm_drv.h"
/*#include "ddp_irq.h"*/

#ifdef CONFIG_AMAZON_METRICS_LOG
#include <linux/metricslog.h>
#endif

#define FRAME_WIDTH  (600)
#define FRAME_HEIGHT (1024)
#define REGFLAG_DELAY 0xFC
/* END OF REGISTERS MARKER */
#define REGFLAG_END_OF_TABLE 0xFD

#define FITI_STARRY			0x1		/* STARRY, using FITI IC */
#define FITI_KD				0x2		/* KD, using FITI IC */
#define FITI_INX			0x3		/* INX, using FITI IC */
#define FITI_INX_KD			0x4		/* INX_KD, using FITI IC */
#define FITI_HSD_KD			0x5		/* HSD_KD, using FITI IC */
#define FITI_UNKNOWN	0xFF

#define GPIO_OUT_ONE 1
#define GPIO_OUT_ZERO 0
#define GPIO_OUT_HIGH 1
#define GPIO_OUT_LOW 0
#define TRUE 1
#define FALSE 0

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static struct LCM_UTIL_FUNCS lcm_util = {
	.set_reset_pin = NULL,
	.set_gpio_out = NULL,
	.udelay = NULL,
	.mdelay = NULL,
};

#define SET_RESET_PIN(v)      					(lcm_util.set_reset_pin((v)))
#define SET_GPIO_OUT(n, v)	        			(lcm_util.set_gpio_out((n), (v)))
#define UDELAY(n) 						(lcm_util.udelay(n))
#define MDELAY(n) 						(lcm_util.mdelay(n))

static unsigned int vendor_id = 0;

static unsigned char lcm_id;

static struct regulator *lcm_vgp;

// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)						lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)			lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)						lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   		lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

#define   LCM_DSI_CMD_MODE	0
struct LCM_setting_table
{
    unsigned char cmd;
	unsigned char count;
	unsigned char para_list[64];
};

#ifdef BUILD_LK

#ifdef GPIO_LCD_PWR
#define GPIO_LCD_PWR_EN      GPIO_LCD_PWR
#else
#define GPIO_LCD_PWR_EN		(0xFFFFFFFF)
#endif

#ifdef GPIO_LCM_RST
#define GPIO_LCD_RST_EN		GPIO_LCM_RST
#else
#define GPIO_LCD_RST_EN		(0xFFFFFFFF)
#endif
#endif

static struct regulator *lcm_vgp;
static unsigned int GPIO_LCD_PWR_EN;
static unsigned int GPIO_LCD_RST_EN;

static void lcm_get_gpio_infor(struct device *dev)
{
	pr_debug("LCM: lcm_get_gpio_infor is going\n");

	GPIO_LCD_PWR_EN = of_get_named_gpio(dev->of_node, "lcm_pwr_gpio", 0);
	gpio_request(GPIO_LCD_PWR_EN, "GPIO_LCD_PWR_EN");
	GPIO_LCD_RST_EN = of_get_named_gpio(dev->of_node, "lcm_reset_gpio", 0);
	gpio_request(GPIO_LCD_RST_EN, "GPIO_LCD_RST_EN");
	pr_debug("LCM: GPIO_LCD_PWR_EN=%d,GPIO_LCD_RST_EN=%d.\n",GPIO_LCD_PWR_EN,GPIO_LCD_RST_EN);
}

static void lcm_set_gpio_output(unsigned int GPIO, unsigned int output)
{
	gpio_direction_output(GPIO, output);
	gpio_set_value(GPIO, output);
}

/* get LDO supply */
static int lcm_get_vgp_supply(struct device *dev)
{
	int ret;
	struct regulator *lcm_vgp_ldo;

	pr_debug("LCM: lcm_get_vgp_supply is going\n");

	lcm_vgp_ldo = devm_regulator_get(dev, "reg-lcm");
	if (IS_ERR(lcm_vgp_ldo)) {
		ret = PTR_ERR(lcm_vgp_ldo);
		dev_err(dev, "failed to get reg-lcm LDO, %d\n", ret);
		return ret;
	}

	pr_debug("LCM: lcm get supply ok.\n");

	ret = regulator_enable(lcm_vgp_ldo);
	/* get current voltage settings */
	ret = regulator_get_voltage(lcm_vgp_ldo);
	pr_debug("lcm LDO voltage = %d in LK stage\n", ret);

	lcm_vgp = lcm_vgp_ldo;

	return ret;
}

int lcm_vgp_supply_enable(void)
{
	int ret;
	unsigned int volt;

	pr_debug("LCM: lcm_vgp_supply_enable\n");

	if (NULL == lcm_vgp)
		return 0;

	pr_debug("LCM: set regulator voltage lcm_vgp voltage to 3.3V\n");
	/* set voltage to 3.3V */
	ret = regulator_set_voltage(lcm_vgp, 3300000, 3300000);
	if (ret != 0) {
		pr_err("LCM: lcm failed to set lcm_vgp voltage: %d\n", ret);
		return ret;
	}

	/* get voltage settings again */
	volt = regulator_get_voltage(lcm_vgp);
	if (volt == 3300000)
		pr_debug("LCM: check regulator voltage=3300000 pass!\n");
	else
		pr_err("LCM: check regulator voltage=3300000 fail! (voltage: %d)\n", volt);

	ret = regulator_enable(lcm_vgp);
	if (ret != 0) {
		pr_err("LCM: Failed to enable lcm_vgp: %d\n", ret);
		return ret;
	}

	return ret;
}

int lcm_vgp_supply_disable(void)
{
	int ret = 0;
	unsigned int isenable;

	if (NULL == lcm_vgp)
		return 0;

	/* disable regulator */
	isenable = regulator_is_enabled(lcm_vgp);

	pr_debug("LCM: lcm query regulator enable status[0x%d]\n", isenable);

	if (isenable) {
		ret = regulator_disable(lcm_vgp);
		if (ret != 0) {
			pr_err("LCM: lcm failed to disable lcm_vgp: %d\n", ret);
			return ret;
		}
		/* verify */
		isenable = regulator_is_enabled(lcm_vgp);
		if (!isenable)
			pr_err("LCM: lcm regulator disable pass\n");
	}

	return ret;
}

static int lcm_driver_probe(struct device *dev, void const *data)
{
	pr_debug("LCM: lcm_driver_probe \n");
	lcm_get_vgp_supply(dev);
	lcm_get_gpio_infor(dev);

	return 0;
}
static const struct of_device_id lcm_platform_of_match[] = {
	{
		.compatible = "lcm,lcm_dts_otm7290b_mustang",
		.data = 0,
	}, {
		/* sentinel */
	}
};


MODULE_DEVICE_TABLE(of, platform_of_match);

static int lcm_platform_probe(struct platform_device *pdev)
{
	const struct of_device_id *id;

	id = of_match_node(lcm_platform_of_match, pdev->dev.of_node);
	if (!id)
		return -ENODEV;

	return lcm_driver_probe(&pdev->dev, id->data);
}



static struct platform_driver lcm_driver = {
	.probe = lcm_platform_probe,
	.driver = {
		   .name = "lcm_dts_otm7290b_mustang",
		   .owner = THIS_MODULE,
		   .of_match_table = lcm_platform_of_match,
		   },
};

static int __init lcm_init(void)
{
	pr_debug("LCM: lcm_init\n");
	if (platform_driver_register(&lcm_driver)) {
		pr_err("LCM: failed to register disp driver\n");
		return -ENODEV;
	}

	return 0;
}

static void __exit lcm_exit(void)
{
	platform_driver_unregister(&lcm_driver);
	pr_err("LCM: Unregister lcm driver done\n");
}
late_initcall(lcm_init);
module_exit(lcm_exit);
MODULE_AUTHOR("mediatek");
MODULE_DESCRIPTION("Display subsystem Driver");
MODULE_LICENSE("GPL");

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

    for(i = 0; i < count; i++) {

        unsigned cmd;
        cmd = table[i].cmd;

        switch (cmd) {

            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;

            case REGFLAG_END_OF_TABLE :
                break;

            default:
                dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
            }
	}
}

static void lcm_set_util_funcs(const struct LCM_UTIL_FUNCS *util)
{
	memcpy(&lcm_util, util, sizeof(struct LCM_UTIL_FUNCS));
}

static struct LCM_setting_table lcm_initialization_setting_starry[] =
{
	{0x11,	0,	{ }},
	{REGFLAG_DELAY, 120, { }},
	{0x29,  0,      { }},
	{REGFLAG_DELAY, 20, {}},
	{REGFLAG_END_OF_TABLE, 0x00, { }},
};

static struct LCM_setting_table lcm_initialization_setting_kd[] =
{
	{0x11,	0,	{ }},
	{REGFLAG_DELAY, 120, { }},
	{0x29,  0,      { }},
	{REGFLAG_DELAY, 20, {}},
	{REGFLAG_END_OF_TABLE, 0x00, { }},
};

static struct LCM_setting_table lcm_initialization_setting_inx[] =
{
	{0x11,	0,	{ }},
	{REGFLAG_DELAY, 120, { }},
	{0x29,  0,      { }},
	{REGFLAG_DELAY, 20, {}},
	{REGFLAG_END_OF_TABLE, 0x00, { }},
};

static struct LCM_setting_table lcm_initialization_setting_inx_kd[] =
{
	{0x11,	0,	{ }},
	{REGFLAG_DELAY, 120, { }},
	{0x29,  0,      { }},
	{REGFLAG_DELAY, 20, {}},
	{REGFLAG_END_OF_TABLE, 0x00, { }},
};

static struct LCM_setting_table lcm_initialization_setting_hsd_kd[] =
{
	{0x11,	0,	{ }},
	{REGFLAG_DELAY, 120, { }},
	{0x29,  0,      { }},
	{REGFLAG_DELAY, 20, {}},
	{REGFLAG_END_OF_TABLE, 0x00, { }},
};

static struct LCM_setting_table lcm_suspend_setting[] =
{
	{0x28,  0 , { }},
	{REGFLAG_DELAY, 20, { }},
	{0x10, 0 , { }},
	{REGFLAG_DELAY, 120, { }},
	{REGFLAG_END_OF_TABLE, 0x00, { }}
};

static void lcm_get_auo_starry_params(struct LCM_PARAMS *params)
{
	memset(params, 0, sizeof(struct LCM_PARAMS));

	params->type   = LCM_TYPE_DSI;

	params->width  = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

	params->dsi.mode   = BURST_VDO_MODE;    //SYNC_PULSE_VDO_MODE;//BURST_VDO_MODE;//SYNC_EVENT_VDO_MODE;

	// DSI
	/* Command mode setting */
	//1 Three lane or Four lane
	params->dsi.LANE_NUM			= LCM_FOUR_LANE;
	//The following defined the fomat for data coming from LCD engine.
	params->dsi.data_format.color_order 	= LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq   	= LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding     	= LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format      	= LCM_DSI_FORMAT_RGB888;

	// Highly depends on LCD driver capability.
	// Not support in MT6573
	params->dsi.packet_size=256;

	// Video mode setting
	params->dsi.intermediat_buffer_num = 0;//because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage

	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
	params->dsi.word_count=FRAME_WIDTH*3;

	params->dsi.vertical_sync_active			= 1;
	params->dsi.vertical_backporch				= 25;
	params->dsi.vertical_frontporch				= 35;
	params->dsi.vertical_active_line			= FRAME_HEIGHT;

	params->dsi.horizontal_sync_active			= 1;
	params->dsi.horizontal_backporch			= 51;
	params->dsi.horizontal_frontporch			= 45;
	params->dsi.horizontal_active_pixel			= FRAME_WIDTH;

	// Bit rate calculation
	//1 Every lane speed
	params->dsi.PLL_CLOCK = 145;
	params->dsi.ssc_disable = 1;		//1:disable ssc , 0:enable ssc
	params->dsi.HS_TRAIL = 4;
	params->dsi.CLK_TRAIL = 5;
}


static void lcm_get_auo_kd_params(struct LCM_PARAMS *params)
{
	memset(params, 0, sizeof(struct LCM_PARAMS));

	params->type   = LCM_TYPE_DSI;

	params->width  = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

	params->dsi.mode   = BURST_VDO_MODE;    //SYNC_PULSE_VDO_MODE;//BURST_VDO_MODE;//SYNC_EVENT_VDO_MODE;

	// DSI
	/* Command mode setting */
	//1 Three lane or Four lane
	params->dsi.LANE_NUM			= LCM_FOUR_LANE;
	//The following defined the fomat for data coming from LCD engine.
	params->dsi.data_format.color_order 	= LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq   	= LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding     	= LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format      	= LCM_DSI_FORMAT_RGB888;

	// Highly depends on LCD driver capability.
	// Not support in MT6573
	params->dsi.packet_size=256;

	// Video mode setting
	params->dsi.intermediat_buffer_num = 0;//because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage

	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
	params->dsi.word_count=FRAME_WIDTH*3;

	params->dsi.vertical_sync_active			= 1;
	params->dsi.vertical_backporch				= 25;
	params->dsi.vertical_frontporch				= 35;
	params->dsi.vertical_active_line			= FRAME_HEIGHT;

	params->dsi.horizontal_sync_active			= 1;
	params->dsi.horizontal_backporch			= 51;
	params->dsi.horizontal_frontporch			= 45;
	params->dsi.horizontal_active_pixel			= FRAME_WIDTH;

	// Bit rate calculation
	//1 Every lane speed
	params->dsi.PLL_CLOCK = 145;
	params->dsi.ssc_disable = 1;		//1:disable ssc, 0:enable ssc
	params->dsi.HS_TRAIL = 4;
	params->dsi.CLK_TRAIL = 5;
}


static void lcm_get_inx_inx_params(struct LCM_PARAMS *params)
{
	memset(params, 0, sizeof(struct LCM_PARAMS));

	params->type   = LCM_TYPE_DSI;

	params->width  = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

	params->dsi.mode   = BURST_VDO_MODE;    //SYNC_PULSE_VDO_MODE;//BURST_VDO_MODE;//SYNC_EVENT_VDO_MODE;

	// DSI
	/* Command mode setting */
	//1 Three lane or Four lane
	params->dsi.LANE_NUM			= LCM_FOUR_LANE;
	//The following defined the fomat for data coming from LCD engine.
	params->dsi.data_format.color_order 	= LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq   	= LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding     	= LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format      	= LCM_DSI_FORMAT_RGB888;

	// Highly depends on LCD driver capability.
	// Not support in MT6573
	params->dsi.packet_size=256;

	// Video mode setting
	params->dsi.intermediat_buffer_num = 0;//because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage

	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
	params->dsi.word_count=FRAME_WIDTH*3;

	params->dsi.vertical_sync_active			= 1;
	params->dsi.vertical_backporch				= 25;
	params->dsi.vertical_frontporch				= 35;
	params->dsi.vertical_active_line			= FRAME_HEIGHT;

	params->dsi.horizontal_sync_active			= 1;
	params->dsi.horizontal_backporch			= 51;
	params->dsi.horizontal_frontporch			= 45;
	params->dsi.horizontal_active_pixel			= FRAME_WIDTH;

	// Bit rate calculation
	//1 Every lane speed
	params->dsi.PLL_CLOCK = 145;
	params->dsi.ssc_disable = 1;		//1:disable ssc , 0:enable ssc
	params->dsi.HS_TRAIL = 4;
	params->dsi.CLK_TRAIL = 5;
}

static void lcm_get_inx_kd_params(struct LCM_PARAMS *params)
{
	memset(params, 0, sizeof(struct LCM_PARAMS));

	params->type   = LCM_TYPE_DSI;

	params->width  = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

	params->dsi.mode   = BURST_VDO_MODE;    //SYNC_PULSE_VDO_MODE;//BURST_VDO_MODE;//SYNC_EVENT_VDO_MODE;

	// DSI
	/* Command mode setting */
	//1 Three lane or Four lane
	params->dsi.LANE_NUM			= LCM_FOUR_LANE;
	//The following defined the fomat for data coming from LCD engine.
	params->dsi.data_format.color_order 	= LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq   	= LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding     	= LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format      	= LCM_DSI_FORMAT_RGB888;

	// Highly depends on LCD driver capability.
	// Not support in MT6573
	params->dsi.packet_size=256;

	// Video mode setting
	params->dsi.intermediat_buffer_num = 0;//because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage

	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
	params->dsi.word_count=FRAME_WIDTH*3;

	params->dsi.vertical_sync_active			= 1;
	params->dsi.vertical_backporch				= 25;
	params->dsi.vertical_frontporch				= 35;
	params->dsi.vertical_active_line			= FRAME_HEIGHT;

	params->dsi.horizontal_sync_active			= 1;
	params->dsi.horizontal_backporch			= 51;
	params->dsi.horizontal_frontporch			= 45;
	params->dsi.horizontal_active_pixel			= FRAME_WIDTH;

	// Bit rate calculation
	//1 Every lane speed
	params->dsi.PLL_CLOCK = 145;
	params->dsi.ssc_disable = 1;		//1:disable ssc , 0:enable ssc
	params->dsi.HS_TRAIL = 4;
	params->dsi.CLK_TRAIL = 5;
}

static void lcm_get_hsd_kd_params(struct LCM_PARAMS *params)
{
	memset(params, 0, sizeof(struct LCM_PARAMS));

	params->type   = LCM_TYPE_DSI;

	params->width  = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

	params->dsi.mode   = BURST_VDO_MODE;    //SYNC_PULSE_VDO_MODE;//BURST_VDO_MODE;//SYNC_EVENT_VDO_MODE;

	// DSI
	/* Command mode setting */
	//1 Three lane or Four lane
	params->dsi.LANE_NUM			= LCM_FOUR_LANE;
	//The following defined the fomat for data coming from LCD engine.
	params->dsi.data_format.color_order 	= LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq   	= LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding     	= LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format      	= LCM_DSI_FORMAT_RGB888;

	// Highly depends on LCD driver capability.
	// Not support in MT6573
	params->dsi.packet_size=256;

	// Video mode setting
	params->dsi.intermediat_buffer_num = 0;//because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage

	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
	params->dsi.word_count=FRAME_WIDTH*3;

	params->dsi.vertical_sync_active			= 1;
	params->dsi.vertical_backporch				= 25;
	params->dsi.vertical_frontporch				= 35;
	params->dsi.vertical_active_line			= FRAME_HEIGHT;

	params->dsi.horizontal_sync_active			= 1;
	params->dsi.horizontal_backporch			= 51;
	params->dsi.horizontal_frontporch			= 45;
	params->dsi.horizontal_active_pixel			= FRAME_WIDTH;

	// Bit rate calculation
	//1 Every lane speed
	params->dsi.PLL_CLOCK = 145;
	params->dsi.ssc_disable = 1;		//1:disable ssc , 0:enable ssc
	params->dsi.HS_TRAIL = 4;
	params->dsi.CLK_TRAIL = 5;
}

static void lcm_reset(void)
{
	pr_info("[otm7290b] %s, - Reset\n", __func__);

	lcm_set_gpio_output(GPIO_LCD_RST_EN,GPIO_OUT_HIGH);
	MDELAY(5);
	lcm_set_gpio_output(GPIO_LCD_RST_EN,GPIO_OUT_LOW);
	MDELAY(2);
	lcm_set_gpio_output(GPIO_LCD_RST_EN,GPIO_OUT_HIGH);
	MDELAY(25);
}

static void lcd_bias_power_en(unsigned char enabled)
{
	printk("[OTM7290B][K] %s : %s\n", __func__, enabled ? "on" : "off");

	if (enabled)
	{
		lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ONE);
	}
	else
	{
		lcm_set_gpio_output(GPIO_LCD_PWR_EN, GPIO_OUT_ZERO);
	}
}
static char *lcm_get_vendor_type(void)
{
	switch (vendor_id) {
	case FITI_KD: return "FITI_KD\0";
	case FITI_INX: return "FITI_INX\0";
	case FITI_INX_KD: return "FITI_INX_KD\0";
	case FITI_HSD_KD: return "FITI_HSD_KD\0";
	case FITI_STARRY: return "FITI_STARRY\0";
	default: return "UNKNOWN\0";
	}
}

static void init_muatsng_fiti_starry_lcm(void)
{
	push_table(lcm_initialization_setting_starry, sizeof(lcm_initialization_setting_starry) / sizeof(struct LCM_setting_table), 1);
}

static void init_muatsng_fiti_kd_lcm(void)
{
	push_table(lcm_initialization_setting_kd, sizeof(lcm_initialization_setting_kd) / sizeof(struct LCM_setting_table), 1);
}

static void init_muatsng_fiti_inx_lcm(void)
{
	push_table(lcm_initialization_setting_inx, sizeof(lcm_initialization_setting_inx) / sizeof(struct LCM_setting_table), 1);
}

static void init_muatsng_fiti_inx_kd_lcm(void)
{
	push_table(lcm_initialization_setting_inx_kd, sizeof(lcm_initialization_setting_inx_kd) / sizeof(struct LCM_setting_table), 1);
}

static void init_muatsng_fiti_hsd_kd_lcm(void)
{
	push_table(lcm_initialization_setting_hsd_kd, sizeof(lcm_initialization_setting_hsd_kd) / sizeof(struct LCM_setting_table), 1);
}

static int __init setup_lcm_id(char *str)
{
	int id;

	if (get_option(&str, &id))
	{
		lcm_id = (unsigned char)id;
	}
	return 0;
}
__setup("nt35521_id=", setup_lcm_id);

static void get_lcm_id(void)
{
	vendor_id = (unsigned int)lcm_id;
}

static void lcm_init_lcm(void)
{
	get_lcm_id();

	if (vendor_id == FITI_STARRY)
		init_muatsng_fiti_starry_lcm(); 	/*FITI STARRY panel*/
	else if (vendor_id == FITI_KD)
		init_muatsng_fiti_kd_lcm(); 		/*FITI KD panel*/
	else if (vendor_id == FITI_INX)
		init_muatsng_fiti_inx_lcm(); 		/*FITI INX panel*/
	else if (vendor_id == FITI_INX_KD)
		init_muatsng_fiti_inx_kd_lcm(); 		/*FITI INX_KD panel*/
	else if (vendor_id == FITI_HSD_KD)
		init_muatsng_fiti_hsd_kd_lcm(); 		/*FITI HSD_KD panel*/
	else
		init_muatsng_fiti_kd_lcm(); 	/*Default FITI KD panel*/

	printk("[OTM7290B]lcm_init_lcm func:Kernel Mustang otm7290b %s lcm init ok!\n",lcm_get_vendor_type());
}

static void lcm_suspend(void)
{
#ifdef CONFIG_AMAZON_METRICS_LOG
	char buf[128];
	snprintf(buf, sizeof(buf), "%s:lcd:suspend=1;CT;1:NR", __func__);
	log_to_metrics(ANDROID_LOG_INFO, "LCDEvent", buf);
#endif

	push_table(lcm_suspend_setting, sizeof(lcm_suspend_setting) / sizeof(struct LCM_setting_table), 1);
}

static void lcm_suspend_power(void)
{
	printk("[OTM7290B][K] %s\n", __func__);

	MDELAY(20);

	lcd_bias_power_en(FALSE);

	MDELAY(60);

	lcm_set_gpio_output(GPIO_LCD_RST_EN,GPIO_OUT_LOW);

	lcm_vgp_supply_disable();	/* disable VGP1 3.3V*/
}

static void lcm_resume_power(void)
{
	printk("[OTM7290B][K] %s\n", __func__);

	lcm_vgp_supply_enable();

	MDELAY(5);

	lcd_bias_power_en(TRUE);
}

static void lcm_resume(void)
{
#ifdef CONFIG_AMAZON_METRICS_LOG
	char buf[128];
	snprintf(buf, sizeof(buf), "%s:lcd:resume=1;CT;1:NR", __func__);
	log_to_metrics(ANDROID_LOG_INFO, "LCDEvent", buf);
#endif

	get_lcm_id();

	MDELAY(10);
	lcm_reset();

	if (vendor_id == FITI_STARRY)
		init_muatsng_fiti_starry_lcm(); 	/*FITI STARRY panel*/
	else if (vendor_id == FITI_KD)
		init_muatsng_fiti_kd_lcm(); 		/*FITI KD panel*/
	else if (vendor_id == FITI_INX)
		init_muatsng_fiti_inx_lcm(); 		/*FITI INX panel*/
	else if (vendor_id == FITI_INX_KD)
		init_muatsng_fiti_inx_kd_lcm(); 		/*FITI INX_KD panel*/
	else if (vendor_id == FITI_HSD_KD)
		init_muatsng_fiti_hsd_kd_lcm(); 		/*FITI HSD_KD panel*/
	else
		init_muatsng_fiti_kd_lcm(); 	/*Default FITI KD panel*/
}


struct LCM_DRIVER otm7290b_wsvga_dsi_vdo_mustang_auo_starry_lcm_drv = {
	.name           = "otm7290b_wsvga_dsi_vdo_mustang_auo_starry",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_auo_starry_params,
	.init           = lcm_init_lcm,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.resume_power	= lcm_resume_power,
	.suspend_power	= lcm_suspend_power,
};


struct LCM_DRIVER otm7290b_wsvga_dsi_vdo_mustang_auo_kd_lcm_drv = {
	.name           = "otm7290b_wsvga_dsi_vdo_mustang_auo_kd",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_auo_kd_params,
	.init           = lcm_init_lcm,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.resume_power	= lcm_resume_power,
	.suspend_power	= lcm_suspend_power,
};

struct LCM_DRIVER otm7290b_wsvga_dsi_vdo_mustang_inx_inx_lcm_drv = {
	.name           = "otm7290b_wsvga_dsi_vdo_mustang_inx_inx",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_inx_inx_params,
	.init           = lcm_init_lcm,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.resume_power	= lcm_resume_power,
	.suspend_power	= lcm_suspend_power,
};

struct LCM_DRIVER otm7290b_wsvga_dsi_vdo_mustang_inx_kd_lcm_drv = {
	.name           = "otm7290b_wsvga_dsi_vdo_mustang_inx_kd",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_inx_kd_params,
	.init           = lcm_init_lcm,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.resume_power	= lcm_resume_power,
	.suspend_power	= lcm_suspend_power,
};

struct LCM_DRIVER otm7290b_wsvga_dsi_vdo_mustang_hsd_kd_lcm_drv = {
	.name           = "otm7290b_wsvga_dsi_vdo_mustang_hsd_kd",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_hsd_kd_params,
	.init           = lcm_init_lcm,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.resume_power	= lcm_resume_power,
	.suspend_power	= lcm_suspend_power,
};