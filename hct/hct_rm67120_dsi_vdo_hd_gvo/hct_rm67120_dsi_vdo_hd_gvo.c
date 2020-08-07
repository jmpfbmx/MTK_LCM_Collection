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
	#include <platform/mt_gpio.h>
	#include <string.h>
#elif defined(BUILD_UBOOT)
	#include <asm/arch/mt_gpio.h>
#else
#endif


// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  									(720)
#define FRAME_HEIGHT 									(1280)
#define LCM_ID                       						        (0x0080)

#define LCM_DENSITY	(320)
/* physical size in um */
#define LCM_PHYSICAL_WIDTH    (62000)
#define LCM_PHYSICAL_HEIGHT   (110000)
#define REGFLAG_DELAY             							(0XFFFE)
#define REGFLAG_END_OF_TABLE      							(0xFFFF)	// END OF REGISTERS MARKER


#define LCM_DSI_CMD_MODE								0

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))

#define UDELAY(n) 									(lcm_util.udelay(n))
#define MDELAY(n) 									(lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)						lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)			lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)						lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)			lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)
#define set_gpio_lcd_enp(cmd) 					lcm_util.set_gpio_lcd_enp_bias(cmd)
#define set_gpio_lcd_enn(cmd) 					lcm_util.set_gpio_lcd_enn_bias(cmd)

 struct LCM_setting_table {
	unsigned int cmd;
    unsigned char count;
    unsigned char para_list[64];
};


static struct LCM_setting_table lcm_initialization_setting[] = {
/*
Note :
Data ID will depends on the following rule.
count of parameters > 1      => Data ID = 0x39
count of parameters = 1      => Data ID = 0x15
count of parameters = 0      => Data ID = 0x05
Struclcm_deep_sleep_mode_in_settingture Format :
{DCS command, count of parameters, {parameter list}}
{REGFLAG_DELAY, milliseconds of time, {}},
...
Setting ending by predefined flag
{REGFLAG_END_OF_TABLE, 0x00, {}}
*/
	{0xF0, 5, {0x55, 0xAA, 0x52, 0x08, 0x00}},
	{0xC0, 8, {0xC7, 0x00, 0x00,0x00, 0x1E, 0x10, 0x60, 0xE5}},
	{0xC1, 8, {0xC0, 0x01, 0x00,0x00, 0x1D, 0x00, 0xF0, 0xC8}},
	{0xC2, 8, {0xC0, 0x02, 0x00, 0x00, 0x1D, 0x2A, 0xA0, 0x9F}},	
	{0xC3, 8, {0xC0, 0x02, 0x00, 0x00, 0x1E, 0x2A, 0xA0, 0x9F}},
	{0xC4, 8, {0xC0, 0x02, 0x00, 0x00, 0x1D, 0x10, 0x80, 0xB8}},
	{0xC5, 8, {0xC0, 0x02, 0x00, 0x00, 0x1E, 0x10, 0xA0, 0xB8}},
	{0xC6, 8, {0xC7, 0x00, 0x02, 0x00, 0x1E, 0x10, 0xA0, 0xEC}},
	{0xC7, 8, {0xC7, 0x00, 0x00, 0x00, 0x1F, 0x10, 0x60, 0xE5}},
	{0xC8, 1, {0xFF}},
	{0xB0, 5, {0x00, 0x08, 0x0C, 0x14, 0x14}},
	{0xBA, 1, {0x20}},
	{0xBB, 7, {0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55}},
	{0xF0, 5, {0x55, 0xAA, 0x52, 0x08, 0x02}},
	{0xE1, 1, {0x00}},
	{0xCA, 1, {0x04}},
	{0xE2, 1, {0x0A}},
	{0xE3, 1, {0x00}},
	{0xE7, 1, {0x00}},
	{0xED, 8, {0x48, 0x00, 0xE0, 0x13, 0x08, 0x00, 0x92, 0x08}},
	{0xFD, 6, {0x00, 0x08, 0x1C, 0x00, 0x00, 0x01}},
	{0xC3, 11, {0x11, 0x24, 0x04, 0x0A, 0x01, 0x04, 0x00, 0x1C, 0x10, 0xF0, 0x00}},
	{0xEA, 5, {0x7F, 0x20, 0x00, 0x00, 0x00}},
	{0xF0, 5, {0x55, 0xAA, 0x52, 0x08, 0x01}},
	{0xB0, 3, {0x01, 0x01, 0x01}},
	{0xB1, 3, {0x05, 0x05, 0x05}},
	{0xB2, 3, {0xD0, 0xD0, 0xD0}},
	{0xB4, 3, {0x37, 0x37, 0x37}},
	{0xB5, 3, {0x05, 0x05, 0x05}},
	{0xB6, 3, {0x54, 0x54, 0x54}},
	{0xB7, 3, {0x24, 0x24, 0x24}},
	{0xB8, 3, {0x24, 0x24, 0x24}},
	{0xB9, 3, {0x14, 0x14, 0x14}},
	{0xBA, 3, {0x14, 0x14, 0x14}},
	{0xBC, 3, {0x00, 0xF8, 0xB2}},
	{0xBE, 3, {0x23, 0x00, 0x70}},
	{0xCA, 1, {0x80}},
	{0xCB, 12, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
	{0xCC, 12, {0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F}},
	{0xF0, 5, {0x55, 0xAA, 0x52, 0x08, 0x03}},
	{0xF1, 6, {0x10, 0x00, 0x00, 0x00, 0x01, 0x30}},
	{0xF6, 1, {0x0A}},
	{0xF0, 5, {0x55, 0xAA, 0x52, 0x08, 0x05}},
	{0xC0, 7, {0x06, 0x02, 0x02, 0x22, 0x00, 0x00, 0x01}},
	{0x35, 1, {0x00}},
        {0x36, 1, {0x02}},//左右镜像调整
	{0x51, 1, {0x00}},
	{0x11, 1, {0x00}},
	{REGFLAG_DELAY,100,{}},
	{0x29, 1, {0x00}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
#if 0
static struct LCM_setting_table lcm_sleep_out_setting[] = {
	// Sleep Out
	{0x11, 1, {0x00}},
	{REGFLAG_DELAY, 120, {}},

	///Display ON
	{0x29, 1, {0x00}},
	{REGFLAG_DELAY, 10, {}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_sleep_in_setting[] = {
	// Display off sequence
	{0x28, 1, {0x00}},
    {REGFLAG_DELAY, 20, {}},

	// Sleep Mode On
	{0x10, 1, {0x00}},
	{REGFLAG_DELAY, 50, {}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
#endif
static struct LCM_setting_table lcm_backlight_level_setting[] = {
    {0x51, 1, {0xFF}},
    //{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

	for (i = 0; i < count; i++) {

		unsigned cmd;
		cmd = table[i].cmd;

		switch (cmd) {

		case REGFLAG_DELAY:
			MDELAY(table[i].count);
			break;

		case REGFLAG_END_OF_TABLE:
			break;

		default:
                dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
		}
	}

}


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS * util)
{
	memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS * params)
{
	memset(params, 0, sizeof(LCM_PARAMS));

	params->type = LCM_TYPE_DSI;

	params->width = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;
	//params->density = LCM_DENSITY;

	params->physical_width = LCM_PHYSICAL_WIDTH/1000;
	params->physical_height = LCM_PHYSICAL_HEIGHT/1000;
//	params->physical_width_um = LCM_PHYSICAL_WIDTH;
//	params->physical_height_um = LCM_PHYSICAL_HEIGHT;

	// enable tearing-free
	params->dbi.te_mode = LCM_DBI_TE_MODE_DISABLED;
	params->dbi.te_edge_polarity = LCM_POLARITY_RISING;

#if (LCM_DSI_CMD_MODE)
	params->dsi.mode = CMD_MODE;
#else
	params->dsi.mode   = SYNC_PULSE_VDO_MODE;//SYNC_EVENT_VDO_MODE;//BURST_VDO_MODE;////
#endif

	// DSI
	/* Command mode setting */
	params->dsi.LANE_NUM                = LCM_FOUR_LANE;
	//The following defined the fomat for data coming from LCD engine.
	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;
	
	
	params->dsi.packet_size=256;
//	Video mode setting       
//    params->dsi.intermediat_buffer_num = 2;
	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
	
//	params->dsi.word_count=FRAME_WIDTH*3;	//DSI CMD mode need set these two bellow params, different to 6577
	//params->dsi.vertical_active_line=FRAME_HEIGHT;
	params->dsi.vertical_sync_active				= 10;
	params->dsi.vertical_backporch					= 20;
	params->dsi.vertical_frontporch				        = 20;
	params->dsi.vertical_active_line				= FRAME_HEIGHT; 

	params->dsi.horizontal_sync_active				= 8;
	params->dsi.horizontal_backporch				= 20;
	params->dsi.horizontal_frontporch				= 20;
	params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

	// Bit rate calculation
	params->dsi.ssc_disable=1; 
	params->dsi.PLL_CLOCK=220;
	params->dsi.clk_lp_per_line_enable=0;
	params->dsi.esd_check_enable = 0;
	params->dsi.customization_esd_check_enable = 0;
	params->dsi.lcm_esd_check_table[0].cmd = 0x0a;
	params->dsi.lcm_esd_check_table[0].count = 1;
	params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9c;	

}

static void lcm_init(void)
{
#ifdef BUILD_LK
   printf("hctwang-%s\n", __func__);
#else
   printk("hctwang-%s\n", __func__);
#endif

    SET_RESET_PIN(0);
    MDELAY(10);

    set_gpio_lcd_enp(1);//3.3V
    MDELAY(10);

    set_gpio_lcd_enn(1);
    MDELAY(10);

    SET_RESET_PIN(1);
    MDELAY(20);

    push_table(lcm_initialization_setting,sizeof(lcm_initialization_setting) /sizeof(struct LCM_setting_table), 1);
}

static void lcm_suspend(void)
{
    unsigned int data_array[16];

    data_array[0] = 0x00280500;
    dsi_set_cmdq(data_array, 1, 1);
    MDELAY(50);
    data_array[0] = 0x00100500;
    dsi_set_cmdq(data_array, 1, 1);
    MDELAY(50);

#ifdef BUILD_LK
   printf("hctwang-%s\n", __func__);
#else
   printk("hctwang-%s\n", __func__);
#endif
    SET_RESET_PIN(0);
    MDELAY(10);

    set_gpio_lcd_enp(0);//3.3V
    MDELAY(5);
}


static void lcm_resume(void)
{   
	lcm_init();
}

#if (LCM_DSI_CMD_MODE)
static void lcm_update(unsigned int x, unsigned int y,
		unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0 >> 8) & 0xFF);
	unsigned char x0_LSB = (x0 & 0xFF);
	unsigned char x1_MSB = ((x1 >> 8) & 0xFF);
	unsigned char x1_LSB = (x1 & 0xFF);
	unsigned char y0_MSB = ((y0 >> 8) & 0xFF);
	unsigned char y0_LSB = (y0 & 0xFF);
	unsigned char y1_MSB = ((y1 >> 8) & 0xFF);
	unsigned char y1_LSB = (y1 & 0xFF);

	unsigned int data_array[16];

	data_array[0] = 0x00053902;
	data_array[1] =
		(x1_MSB << 24) | (x0_LSB << 16) | (x0_MSB << 8) | 0x2a;
	data_array[2] = (x1_LSB);
	data_array[3] = 0x00053902;
	data_array[4] =
		(y1_MSB << 24) | (y0_LSB << 16) | (y0_MSB << 8) | 0x2b;
	data_array[5] = (y1_LSB);
	data_array[6] = 0x002c3909;

	dsi_set_cmdq(data_array, 7, 0);

}
#endif

static unsigned int lcm_compare_id(void)
{
		int array[4];
		char buffer[5];
		char id_high=0;
		char id_low=0;
		int id1=0;


		SET_RESET_PIN(1);
		MDELAY(10);
		SET_RESET_PIN(0);
		MDELAY(10);
		SET_RESET_PIN(1);
		MDELAY(200);
		
		array[0] = 0x00023700;
		dsi_set_cmdq(array, 1, 1);

		read_reg_v2(0x04, buffer, 2);
		id_high = buffer[0];
		id_low = buffer[1];
		id1 = (id_high<<8) | id_low;

		#if defined(BUILD_LK)
		printf("hct_rm67120_dsi_vdo_hd_gvo %s id1 = 0x%04x(0x%04x,0x%04x)\n", __func__, id1, id_high, id_low);
		#else
		printk("hct_rm67120_dsi_vdo_hd_gvo %s id1 = 0x%04x(0x%04x,0x%04x)\n", __func__, id1, id_high, id_low);
		#endif
		return (LCM_ID == id1)?1:0;

}

static void lcm_setbacklight(unsigned int level)
{
	//unsigned int data_array[16];
    unsigned int default_level = 50;
    unsigned int mapped_level = 0;

#ifdef BUILD_LK
   printf("HCT_lcm_setbacklight level = %d\n", level);
#else
   printk("HCT_lcm_setbacklight level = %d\n", level);
#endif
    //for LGE backlight IC mapping table
    if(level > 255)
            level = 255;

    if(level >0)
            mapped_level = default_level+(level)*(255-default_level)/(255);
    else
            mapped_level=0;


    lcm_backlight_level_setting[0].para_list[0] = mapped_level;
    MDELAY(16);
    push_table(lcm_backlight_level_setting, sizeof(lcm_backlight_level_setting) / sizeof(struct LCM_setting_table), 1);

}
// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER hct_rm67120_dsi_vdo_hd_gvo = 
{
	.name			= "hct_rm67120_dsi_vdo_hd_gvo",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,	
	.compare_id     = lcm_compare_id,
	.set_backlight = lcm_setbacklight,
};

