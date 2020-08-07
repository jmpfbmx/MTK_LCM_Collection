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

#define LOG_TAG "LCM"

#ifndef BUILD_LK
#include <linux/string.h>
#include <linux/kernel.h>
#endif

#include "lcm_drv.h"

#ifdef BUILD_LK
#else
#include <linux/regulator/mediatek/mtk_regulator.h>
#endif
#ifdef BUILD_LK
#define LCM_LOGI(string, args...)  dprintf(0, "[LK/"LOG_TAG"]"string, ##args)
#define LCM_LOGD(string, args...)  dprintf(1, "[LK/"LOG_TAG"]"string, ##args)
#else
#define LCM_LOGI(fmt, args...)  pr_debug("[KERNEL/"LOG_TAG"]"fmt, ##args)
#define LCM_LOGD(fmt, args...)  pr_debug("[KERNEL/"LOG_TAG"]"fmt, ##args)
#endif



static const unsigned int BL_MIN_LEVEL = 20;
static LCM_UTIL_FUNCS lcm_util;


#define SET_RESET_PIN(v)	(lcm_util.set_reset_pin((v)))
#define MDELAY(n)		(lcm_util.mdelay(n))
#define UDELAY(n)		(lcm_util.udelay(n))

#define dsi_set_cmdq_V22(cmdq, cmd, count, ppara, force_update) \
	lcm_util.dsi_set_cmdq_V22(cmdq, cmd, count, ppara, force_update)
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update) \
	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update) \
		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd) lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums) \
		lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd) \
	  lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size) \
		lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

#ifndef BUILD_LK
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/i2c.h>
#include <linux/irq.h>
/* #include <linux/jiffies.h> */
/* #include <linux/delay.h> */
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#endif

/* static unsigned char lcd_id_pins_value = 0xFF; */
static const unsigned char LCD_MODULE_ID = 0x01;
#define LCM_DSI_CMD_MODE									0
#define FRAME_WIDTH										(1080)
#define FRAME_HEIGHT									(1920)

/* physical size in um */
#define LCM_PHYSICAL_WIDTH									(64800)
#define LCM_PHYSICAL_HEIGHT									(115200)

#define REGFLAG_DELAY		0xFFFC
#define REGFLAG_UDELAY	0xFFFB
#define REGFLAG_END_OF_TABLE	0xFFFD
#define REGFLAG_RESET_LOW	0xFFFE
#define REGFLAG_RESET_HIGH	0xFFFF

#ifdef CONFIG_LCT_CABC_MODE_SUPPORT
extern int cabc_mode_mode;
#define CABC_MODE_SETTING_UI	1
#define CABC_MODE_SETTING_MV	2
#define CABC_MODE_SETTING_DIS	3
#define CABC_MODE_SETTING_NULL	0
//static int reg_mode = 0;
#define dsi_set_cmdq_V22(cmdq, cmd, count, ppara, force_update) \
lcm_util.dsi_set_cmdq_V22(cmdq, cmd, count, ppara, force_update)
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

struct LCM_setting_table {
	unsigned int cmd;
	unsigned char count;
	unsigned char para_list[64];
};

static struct LCM_setting_table lcm_suspend_setting[] = {
	{0x28, 0, {} },
	{REGFLAG_DELAY, 20, {} },
	{0x10, 0, {} },
	{REGFLAG_DELAY, 120, {} },
	{0x4F, 1, {0x01} },
	{REGFLAG_DELAY, 120, {} }
};

static struct LCM_setting_table lcm_initialization_setting[] = {
	{0x00,1,{0x00}},
	{0xFF,3,{0x19,0x11,0x01}},//Enable command2

	{0x00,1,{0x80}},
	{0xFF,2,{0x19,0x11}},

	{0x00,1,{0x92}},
	{0xB3,2,{0x18,0x04}},//Panel Mode, swap G

	{0x00,1,{0xB5}},
	{0xCA,1,{0x04}},  // Dimming frame adjust by zhuxiaming 

	{0x00,1,{0x8A}},
	{0xC0,1,{0x04}},	//Panel Scan Mode

	{0x00,1,{0x8B}},
	{0xC0,1,{0x88}},	//Panel Driving Mode

	{0x00,1,{0x80}},
	{0xC4,2,{0x41,0x02}},

	{0x00,1,{0x00}},
	{0xD8,2,{0x17,0x17}},//GVDDP/GVDDN=4V

	{0x00,1,{0x00}},
	{0xE0,1,{0x00}},//Gamma Separate Change

	{0x00,1,{0x90}},
	{0xC5,1,{0x88}},//GAP 

	{0x00,1,{0x91}},
	{0xC5,1,{0x80}},//SAP 

	{0x00,1,{0x83}},//VGH=9.9V 
	{0xC5,1,{0xa6}},

	{0x00,1,{0x84}},//VGL=-6.4V, 
	{0xC5,1,{0x98}},

	{0x00,1,{0xA0}},//VGHO=9.6V, 
	{0xC5,1,{0xa3}},

	{0x00,1,{0xA1}},//VGLO=-6.1V,
	{0xC5,1,{0x95}},

	{0x00,1,{0xB1}},//Gamma Calibration control disable
	{0xC5,1,{0x08}},
	{0x00,1,{0xB2}},//Gamma chop = 2line/2frame
	{0xC5,1,{0x22}},
	{0x00,1,{0x80}},
	{0xc2,6,{0x82,0x00,0x09,0x00,0x81,0x00}},
	{0x00,1,{0xb0}},
	{0xc2,14,{0x80,0x08,0x00,0x09,0x00,0x80,0x08,0x11,0x80,0x08,0x22,0x80,0x08,0x33}},
	{0x00,1,{0xc0}},
	{0xc2,14,{0x80,0x08,0x44,0x09,0x00,0x80,0x08,0x55,0x80,0x08,0x66,0x80,0x08,0x77}},
	{0x00,1,{0xd0}},
	{0xc2,4,{0x77,0x77,0x77,0x77}},
	{0x00,1,{0xe0}},
	{0xc2,6,{0x00,0x00,0x00,0x00,0x00,0x00}},
	{0x00,1,{0xf0}},
	{0xc2,5,{0x80,0x05,0x00,0x09,0x00}},
	{0x00,1,{0x90}},
	{0xc2,11,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
	{0x00,1,{0x80}},
	{0xc3,8,{0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22}},
	{0x00,1,{0x90}},
	{0xc3,4,{0x20,0x20,0x02,0x02}},
	{0x00,1,{0x90}},
	
	{0xcb,3,{0x03,0x00,0x00}},
	{0x00,1,{0xc0}},
	{0xcb,12,{0xf4,0x04,0x04,0x04,0x04,0x04,0xf0,0x00,0xf4,0x00,0xf0,0xf0}},
	{0x00,1,{0xf0}},
	{0xcb,3,{0x00,0x30,0xf0}},

	{0x00,1,{0x80}},
	{0xcd,1,{0x05}},
	{0x00,1,{0xf5}},
	{0xc2,5,{0x11,0x01,0x0c,0x00,0x00}},
	{0x00,1,{0x80}},
	{0xa5,2,{0x0c,0x00}},
	{0x00,1,{0xC0}},
	{0x94,7,{0x0a,0x01,0x08,0x08,0x01,0x08,0x06}},
	{0x00,1,{0x00}},
	{0xE1,37,	{0x31,0x3f,0x66,0x93,0x00,0xb0,0xd0,0x08,0x37,0x50,0x1c,0x4e,0x73,0x99,0x55,0xc4,0xe9,0x5e,0x86,0x55,0xac,0xd5,0xfb,0x29,0x95,0x56,0x6b,0xa3,0xd8,0xaa,0x12,0x42,0x85,0xd8,0xff,0xfd,0x03}},
	{0x00,1,{0x00}},
	{0xE2,37,	{0x31,0x3f,0x66,0x93,0x00,0xb0,0xd0,0x08,0x37,0x50,0x1c,0x4e,0x73,0x99,0x55,0xc4,0xe9,0x5e,0x86,0x55,0xac,0xd5,0xfb,0x29,0x95,0x56,0x6b,0xa3,0xd8,0xaa,0x12,0x42,0x85,0xd8,0xff,0xfd,0x03}},
	{0x00,1,{0x00}},
	{0xE3,37,	{0x31,0x3f,0x66,0x93,0x00,0xb0,0xd0,0x08,0x37,0x50,0x1c,0x4e,0x73,0x99,0x55,0xc4,0xe9,0x5e,0x86,0x55,0xac,0xd5,0xfb,0x29,0x95,0x56,0x6b,0xa3,0xd8,0xaa,0x12,0x42,0x85,0xd8,0xff,0xfd,0x03}},
	{0x00,1,{0x00}},
	{0xE4,37,	{0x31,0x3f,0x66,0x93,0x00,0xb0,0xd0,0x08,0x37,0x50,0x1c,0x4e,0x73,0x99,0x55,0xc4,0xe9,0x5e,0x86,0x55,0xac,0xd5,0xfb,0x29,0x95,0x56,0x6b,0xa3,0xd8,0xaa,0x12,0x42,0x85,0xd8,0xff,0xfd,0x03}},
	{0x00,1,{0x00}},
	{0xE5,37,	{0x31,0x3f,0x66,0x93,0x00,0xb0,0xd0,0x08,0x37,0x50,0x1c,0x4e,0x73,0x99,0x55,0xc4,0xe9,0x5e,0x86,0x55,0xac,0xd5,0xfb,0x29,0x95,0x56,0x6b,0xa3,0xd8,0xaa,0x12,0x42,0x85,0xd8,0xff,0xfd,0x03}},
	{0x00,1,{0x00}},
	{0xE6,37,		{0x31,0x3f,0x66,0x93,0x00,0xb0,0xd0,0x08,0x37,0x50,0x1c,0x4e,0x73,0x99,0x55,0xc4,0xe9,0x5e,0x86,0x55,0xac,0xd5,0xfb,0x29,0x95,0x56,0x6b,0xa3,0xd8,0xaa,0x12,0x42,0x85,0xd8,0xff,0xfd,0x03}},
	
	{0x11,1,{0x00}},
	{REGFLAG_DELAY, 120, {}},  
	{0x29,1,{0x00}},
	{REGFLAG_DELAY, 20, {}}, 

	{0x51,1,{0x00}}, //0xff
	{0x53,1,{0x2C}},//CABC on
	{0x55,1,{0x01}},//default UI mode 
                                         
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
#ifdef CONFIG_LCT_CABC_MODE_SUPPORT
static struct LCM_setting_table lcm_setting_ui[] = {
{0x53,1,{0x2c}},
{0x55,1,{0x01}},//CABC UI mode
{REGFLAG_END_OF_TABLE, 0x00, {}}
};
static struct LCM_setting_table lcm_setting_dis[] = {
{0x53,1,{0x2c}},
{0x55,1,{0x00}},//CABC off
{REGFLAG_END_OF_TABLE, 0x00, {}}
};
static struct LCM_setting_table lcm_setting_mv[] = {
{0x53,1,{0x2c}},
{0x55,1,{0x03}},//CABC Video mode
{REGFLAG_END_OF_TABLE, 0x00, {}}
};
#endif
static struct LCM_setting_table bl_level[] = {
	{0x51, 1, {0x00} },  //0xFF
	{REGFLAG_END_OF_TABLE, 0x00, {} }
};

static void push_table(void *cmdq, struct LCM_setting_table *table,
	unsigned int count, unsigned char force_update)
{
	unsigned int i;
	unsigned cmd;

	for (i = 0; i < count; i++) {
		cmd = table[i].cmd;

		switch (cmd) {

		case REGFLAG_DELAY:
			if (table[i].count <= 10)
				MDELAY(table[i].count);
			else
				MDELAY(table[i].count);
			break;

		case REGFLAG_UDELAY:
			UDELAY(table[i].count);
			break;

		case REGFLAG_END_OF_TABLE:
			break;

		default:
			dsi_set_cmdq_V22(cmdq, cmd, table[i].count, table[i].para_list, force_update);
		}
	}
}


static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
	memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
	memset(params, 0, sizeof(LCM_PARAMS));

	params->type = LCM_TYPE_DSI;

	params->width = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;
	/*LCT add physical size by zengjianxiong begin*/
	params->physical_width = LCM_PHYSICAL_WIDTH/1000;
	params->physical_height = LCM_PHYSICAL_HEIGHT/1000;
	params->physical_width_um = LCM_PHYSICAL_WIDTH;
	params->physical_height_um = LCM_PHYSICAL_HEIGHT;
	/*LCT add physical size by zengjianxiong end*/

#if (LCM_DSI_CMD_MODE)
	params->dsi.mode = CMD_MODE;
	params->dsi.switch_mode = SYNC_PULSE_VDO_MODE;
	lcm_dsi_mode = CMD_MODE;
#else
	params->dsi.mode = BURST_VDO_MODE;
#endif

	/* DSI */
	/* Command mode setting */
	params->dsi.LANE_NUM = LCM_FOUR_LANE;
	/* The following defined the fomat for data coming from LCD engine. */
	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding = LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format = LCM_DSI_FORMAT_RGB888;

	/* Highly depends on LCD driver capability. */
	params->dsi.packet_size = 256;
	/* video mode timing */

	params->dsi.PS = LCM_PACKED_PS_24BIT_RGB888;

	params->dsi.vertical_sync_active				= 2;//15
	params->dsi.vertical_backporch					= 6;//20
	params->dsi.vertical_frontporch					= 14; //20
	params->dsi.vertical_active_line				= FRAME_HEIGHT;

	params->dsi.horizontal_sync_active				= 10; //20
	params->dsi.horizontal_backporch				= 50;  //32//140
	params->dsi.horizontal_frontporch				= 80;//36  //100
	params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
//	params->dsi.ssc_disable = 1;
/*add by lct jiatianbao start 20170321*/
#ifdef CONFIG_LCT_DEVINFO_SUPPORT
      params->vendor="O-film";
      params->ic="otm1911a";
      params->info="1080*1920";
#endif
/*add by lct jiatianbao end 20170321*/
#if (LCM_DSI_CMD_MODE)
	params->dsi.PLL_CLOCK = 150;	/* this value must be in MTK suggested table */
#else
	params->dsi.PLL_CLOCK = 458;//220	/* this value must be in MTK suggested table */
#endif

	params->dsi.cont_clock=0;
	params->dsi.clk_lp_per_line_enable = 0;
	params->dsi.esd_check_enable = 0;
	params->dsi.customization_esd_check_enable = 0;
	params->dsi.lcm_esd_check_table[0].cmd = 0x0A;
	params->dsi.lcm_esd_check_table[0].count = 1;
	params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9C;

}
#if 0
static struct mtk_regulator disp_bias_pos;
static struct mtk_regulator disp_bias_neg;
static int regulator_inited;

static int display_bias_regulator_init(void)
{
	int ret;

	if (!regulator_inited) {
		/* please only get regulator once in a driver */
		ret = mtk_regulator_get(NULL, "dsv_pos", &disp_bias_pos);
		if (ret < 0) { /* handle return value */
			pr_err("get dsv_pos fail\n");
			return -1;
		}

		ret = mtk_regulator_get(NULL, "dsv_neg", &disp_bias_neg);
		if (ret < 0) { /* handle return value */
			pr_err("get dsv_pos fail\n");
			return -1;
		}

		regulator_inited = 1;
		return ret;
	}
	return 0;
}
#endif
#if 0
static int display_bias_enable(void)
{
	int ret = 0;

	ret = display_bias_regulator_init();
	if (ret)
		return ret;

	/* set voltage with min & max*/
	ret = mtk_regulator_set_voltage(&disp_bias_pos, 5500000, 5500000);
	if (ret < 0)
		pr_err("set voltage disp_bias_pos fail\n");

	ret = mtk_regulator_set_voltage(&disp_bias_neg, 5500000, 5500000);
	if (ret < 0)
		pr_err("set voltage disp_bias_neg fail\n");

#if 0
	/* get voltage */
	ret = mtk_regulator_get_voltage(&disp_bias_pos);
	if (ret < 0)
		pr_err("get voltage disp_bias_pos fail\n");
	pr_debug("pos voltage = %d\n", ret);

	ret = mtk_regulator_get_voltage(&disp_bias_neg);
	if (ret < 0)
		pr_err("get voltage disp_bias_neg fail\n");
	pr_debug("neg voltage = %d\n", ret);
#endif
	/* enable regulator */
	ret = mtk_regulator_enable(&disp_bias_pos, true);
	if (ret < 0)
		pr_err("enable regulator disp_bias_pos fail\n");

	ret = mtk_regulator_enable(&disp_bias_neg, true);
	if (ret < 0)
		pr_err("enable regulator disp_bias_neg fail\n");
	return ret;
}
#endif
//add by yangjiangzhu for hbm
#ifdef CONFIG_LCT_HBM_SUPPORT
static unsigned int last_level=0;
static unsigned int hbm_enable=0;
extern int lct_klcm_otm1911a_enable_hbm(int enable);
static void lcm_setbacklight_hbm(unsigned int level)
{
	
	if(level==0)
	{
		level = last_level;
		hbm_enable = 0;
 		lct_klcm_otm1911a_enable_hbm(0);
	}
	else{
		hbm_enable = 1;
		lct_klcm_otm1911a_enable_hbm(1);
	    }
	bl_level[0].para_list[0] = level;
	push_table(NULL, bl_level,
		   sizeof(bl_level) / sizeof(struct LCM_setting_table), 1);
}
#endif
#ifdef CONFIG_LCT_CABC_MODE_SUPPORT
static void push_table_v22(void *handle, struct LCM_setting_table *table, unsigned int count,
		       unsigned char force_update)
{
	unsigned int i;

	for (i = 0; i < count; i++) {

		unsigned cmd;
		void *cmdq = handle;
		cmd = table[i].cmd;

		switch (cmd) {

		case REGFLAG_DELAY:
			MDELAY(table[i].count);
			break;

		case REGFLAG_END_OF_TABLE:
			break;

		default:
			dsi_set_cmdq_V22(cmdq, cmd, table[i].count, table[i].para_list, force_update);
		}
	}

}

static void lcm_cabc_cmdq(void *handle, unsigned int mode)
{

	switch(mode)
	{
		case CABC_MODE_SETTING_UI:
			{
				push_table_v22(handle,lcm_setting_ui,
			   sizeof(lcm_setting_ui) / sizeof(struct LCM_setting_table), 1);
			}
		break;
		case CABC_MODE_SETTING_MV:
			{
				push_table_v22(handle,lcm_setting_mv,
			   sizeof(lcm_setting_mv) / sizeof(struct LCM_setting_table), 1);
			}
		break;
		case CABC_MODE_SETTING_DIS:
			{
				push_table_v22(handle,lcm_setting_dis,
			   sizeof(lcm_setting_dis) / sizeof(struct LCM_setting_table), 1);
			}
		break;
		default:
		{
			push_table_v22(handle,lcm_setting_ui,
			   sizeof(lcm_setting_ui) / sizeof(struct LCM_setting_table), 1);
		}

	}
}
#endif
#if 0
static int display_bias_disable(void)
{
	int ret = 0;

	ret = display_bias_regulator_init();
	if (ret)
		return ret;

	ret |= mtk_regulator_enable(&disp_bias_neg, false);
	if (ret < 0)
		pr_err("disable regulator disp_bias_neg fail\n");

	ret |= mtk_regulator_enable(&disp_bias_pos, false);
	if (ret < 0)
		pr_err("disable regulator disp_bias_pos fail\n");

	return ret;
}
#endif 
static void lcm_init_power(void)
{
#ifndef CONFIG_FPGA_EARLY_PORTING
	/* display bias is likely inited in lk !!
	 * for kernel regulator system, we need to enable it first before disable!
	 * so here, if bias is not enabled, we enable it first
	 */
	display_bias_enable();
#endif
}

static void lcm_suspend_power(void)
{
#ifndef CONFIG_FPGA_EARLY_PORTING
	display_bias_disable();
#endif
}

static void lcm_resume_power(void)
{
#ifndef CONFIG_FPGA_EARLY_PORTING
	SET_RESET_PIN(0);
	display_bias_enable();
#endif

}

static void lcm_init(void)
{
	SET_RESET_PIN(1);
	MDELAY(10);

	SET_RESET_PIN(0);
	MDELAY(5);//10

	SET_RESET_PIN(1);
	MDELAY(10);//120
	push_table(NULL, lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}


static void lcm_suspend(void)
{
	push_table(NULL, lcm_suspend_setting, sizeof(lcm_suspend_setting) / sizeof(struct LCM_setting_table), 1);
	SET_RESET_PIN(0);
	MDELAY(10);
}

static void lcm_resume(void)
{
	lcm_init();
}

static unsigned int lcm_compare_id(void)
{
  	int   array[4];
	char  buffer[10];
	char  id0,id1,id2,id3,id4=0;
	
	//MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(5);//10
	SET_RESET_PIN(0);
	MDELAY(2);//5
	SET_RESET_PIN(1);
	MDELAY(5);//120

	array[0] = 0x00053700;
	dsi_set_cmdq(array, 1, 1);
	read_reg_v2(0xA1, buffer, 5);
	id0  =  buffer[0];
	id1  =  buffer[1];
	id2  =  buffer[2];
	id3  =  buffer[3];
	id4  =  buffer[4]; 
    printk("[Simon]LCM otm1911a %s: id0 = 0x%08x, id1 = 0x%08x, id2 = 0x%08x, id3 = 0x%08x\n", __func__, id0, id1, id2, id3);
 
	if ((id0 == 0x01)&&(id1 == 0xD5)&&(id2 == 0x01)&&(id3 == 0xE2))
		return 1;
	else
        return 0;
}

static unsigned int lcm_ata_check(unsigned char *buffer)
{
#ifndef BUILD_LK
	unsigned int ret = 0;
	unsigned int x0 = FRAME_WIDTH / 4;
	unsigned int x1 = FRAME_WIDTH * 3 / 4;

	unsigned char x0_MSB = ((x0 >> 8) & 0xFF);
	unsigned char x0_LSB = (x0 & 0xFF);
	unsigned char x1_MSB = ((x1 >> 8) & 0xFF);
	unsigned char x1_LSB = (x1 & 0xFF);

	unsigned int data_array[3];
	unsigned char read_buf[4];

	LCM_LOGI("ATA check size = 0x%x,0x%x,0x%x,0x%x\n", x0_MSB, x0_LSB, x1_MSB, x1_LSB);
	data_array[0] = 0x0005390A;	/* HS packet */
	data_array[1] = (x1_MSB << 24) | (x0_LSB << 16) | (x0_MSB << 8) | 0x2a;
	data_array[2] = (x1_LSB);
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0] = 0x00043700;	/* read id return two byte,version and id */
	dsi_set_cmdq(data_array, 1, 1);

	read_reg_v2(0x2A, read_buf, 4);

	if ((read_buf[0] == x0_MSB) && (read_buf[1] == x0_LSB)
	    && (read_buf[2] == x1_MSB) && (read_buf[3] == x1_LSB))
		ret = 1;
	else
		ret = 1;

	x0 = 0;
	x1 = FRAME_WIDTH - 1;

	x0_MSB = ((x0 >> 8) & 0xFF);
	x0_LSB = (x0 & 0xFF);
	x1_MSB = ((x1 >> 8) & 0xFF);
	x1_LSB = (x1 & 0xFF);

	data_array[0] = 0x0005390A;	/* HS packet */
	data_array[1] = (x1_MSB << 24) | (x0_LSB << 16) | (x0_MSB << 8) | 0x2a;
	data_array[2] = (x1_LSB);
	dsi_set_cmdq(data_array, 3, 1);
	return ret;
#else
	return 1;
#endif
}

static void lcm_setbacklight_cmdq(void *handle, unsigned int level)
{
unsigned int level_hight = 255;
	LCM_LOGI("%s,lcm otm1911a backlight: level = %d\n", __func__, level);
if(hbm_enable == 0){
		if(level <= 4)
		level = 4;
	bl_level[0].para_list[0] = level;

	push_table(handle, bl_level, sizeof(bl_level) / sizeof(struct LCM_setting_table), 1);
}
else{	
		
	bl_level[0].para_list[0] = level_hight;
	push_table(handle, bl_level, sizeof(bl_level) / sizeof(struct LCM_setting_table), 1);
}
	#ifdef CONFIG_LCT_HBM_SUPPORT
		last_level = level;
	#endif	
}

LCM_DRIVER lct_otm1911a_ofilm_1080p_vdo_lcm_drv = {

	.name = "lct_otm1911a_ofilm_1080p_vdo",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params = lcm_get_params,
	.init = lcm_init,
	.suspend = lcm_suspend,
	.resume = lcm_resume,
	.compare_id = lcm_compare_id,
	.init_power = lcm_init_power,
	.resume_power = lcm_resume_power,
	.suspend_power = lcm_suspend_power,
	.set_backlight_cmdq = lcm_setbacklight_cmdq,
	.ata_check = lcm_ata_check,
#ifdef CONFIG_LCT_CABC_MODE_SUPPORT
	.set_cabc_cmdq = lcm_cabc_cmdq,
#endif
#ifdef CONFIG_LCT_HBM_SUPPORT
	.set_backlight_hbm = lcm_setbacklight_hbm,
#endif
};
