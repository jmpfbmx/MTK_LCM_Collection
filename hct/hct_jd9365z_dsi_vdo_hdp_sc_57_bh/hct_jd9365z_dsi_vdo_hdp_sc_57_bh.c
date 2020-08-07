/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/***************************************************************
**************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/


#ifndef BUILD_LK
#include <linux/string.h>
#include <linux/kernel.h>
#endif

#include "lcm_drv.h"
#include "lcm_i2c.h"
#ifdef BUILD_LK
	#include <platform/mt_gpio.h>
	#include <string.h>
//	#include "cust_gpio_usage.h"
	#include "lcm_define.h"
#elif defined(BUILD_UBOOT)
	#include <asm/arch/mt_gpio.h>
#else
	//#include <mach/mt_gpio.h>
#endif
#ifdef MTK_ROUND_CORNER_SUPPORT
//#include "data_rgba4444_roundedpattern.h"
#endif



// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH                 (720)
#define FRAME_HEIGHT                (1520)
#define LCM_ID                      (0x9365)

#define REGFLAG_DELAY             				(0XFE)
#define REGFLAG_END_OF_TABLE      				(0x100)	

#define LCM_DSI_CMD_MODE									0
// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    					(lcm_util.set_reset_pin((v)))

#define UDELAY(n) 						(lcm_util.udelay(n))
#define MDELAY(n) 						(lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------
#define dsi_set_cmdq_V3(para_tbl,size,force_update)		lcm_util.dsi_set_cmdq_V3(para_tbl,size,force_update)
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)						lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)			lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)						lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)			lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

#define   LCM_DSI_CMD_MODE							0
 struct LCM_setting_table {
    unsigned char cmd;
    unsigned char count;
    unsigned char para_list[64];
};

//extern int mt_dsi_pinctrl_set(unsigned int pin , unsigned int level);
//extern int PMU_db_pos_neg_setting_delay(int ms);
//extern int PMU_db_pos_neg_disable_delay(int ms);
/*
static void lcm_power_5v_en(unsigned char enabled)
{
	if (enabled)
	{
	mt_dsi_pinctrl_set(LCM_POWER_ENN, 1);
	}	
	else
	{
	mt_dsi_pinctrl_set(LCM_POWER_ENN, 0);
	}
	
}


static void lcm_power_n5v_en(unsigned char enabled)
{
	if (enabled)
	{
	mt_dsi_pinctrl_set(LCM_POWER_ENP, 1);
	}	
	else
	{
	mt_dsi_pinctrl_set(LCM_POWER_ENP, 0);
	}
}
*/
static struct LCM_setting_table lcm_initialization_setting[] = { 
{0xDF, 3,{0x93,0x65,0xF8}},

{0xCC, 1,{0x32}},
{0xB0, 6,{0x01,0x03,0x02,0x00,0x64,0x06}},
{0xB2, 2,{0x00,0x3D}},

{0xB3, 2,{0x00,0x40}},

{0xB7, 6,{0x00,0xE7,0x00,0x00,0xE7,0x00}},

{0xB9, 4,{0x08,0x04,0x13,0x07}},

{0xBB,12,{0x03,0x01,0x44,0x00,0x36,0x14,0x28,0x04,0xCC,0xCC,0xCC}},

{0xC0, 2,{0x2A,0x03}},

{0xC1, 2,{0x00,0x12}},

{0xC3, 6,{0x04,0x02,0x0D,0x69,0x01,0x6A}},

{0xC4, 6,{0x24,0xF8,0xB4,0x6F,0x0E,0x09}},

{0xC8, 38,{0x7F,0x57,0x43,0x34,0x2E,0x1F,0x25,0x12,0x30,0x31,0x32,0x4F,0x40,0x4C,0x41,0x40,0x32,0x23,0x0F,0x7F,0x57,0x43,0x34,0x2E,0x1F,0x25,0x12,0x30,0x31,0x32,0x4F,0x40,0x4C,0x41,0x40,0x32,0x23,0x0F}},

{0xD0, 22,{0x57,0x58,0x4A,0x48,0x46,0x44,0x1F,0x1F,0x1F,0x1F,0x5F,0x5F,0x1F,0x1F,0x5F,0x5F,0x5F,0x1F,0x40,0x1F,0x1F,0x1F}},

{0xD1, 22,{0x57,0x58,0x4B,0x49,0x47,0x45,0x1F,0x1F,0x1F,0x1F,0x5F,0x5F,0x1F,0x1F,0x5F,0x5F,0x5F,0x1F,0x41,0x1F,0x1F,0x1F}},

{0xD2, 22,{0x17,0x18,0x05,0x07,0x09,0x0B,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x01,0x1F,0x1F,0x1F}},

{0xD3, 22,{0x17,0x18,0x04,0x06,0x08,0x0A,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x00,0x1F,0x1F,0x1F}},

{0xD4,22,{0x01,0x10,0x00,0x04,0x04,0x10,0x01,0x02,0x00,0x50,0x00,0x00,0x00,0x01,0x02,0x03,0x60,0x71,0x07,0x00,0x50,0x0A}},

{0xD5,27,{0x00,0x0A,0x3F,0x02,0x30,0x00,0x00,0x06,0x60,0x00,0x88,0x50,0x01,0x56,0x07,0x00,0x00,0x1F,0x00,0x00,0x00,0x03,0x60,0xE9,0x02,0x00,0x00}},

{0xDE, 1,{0x02}},

{0xB2, 2,{0x32,0x18}},

{0xB7, 4,{0x39,0x03,0x03,0x07}},

{0xBB, 6,{0x21,0x22,0x23,0x24,0x34,0x35}},
{0xDE, 1,{0x03}},

{0xD3, 1,{0x77}},

{0xDE, 1,{0x00}},

{0xBE, 2,{0x1E,0xC2}},
{0x35, 1,{0x00}},
{0x11, 1,{0x00}},

{REGFLAG_DELAY,120,{}},

{0x29, 1,{0x00}},
{REGFLAG_DELAY, 50, {0}},




//{REGFLAG_END_OF_TABLE,0x00,{}}

};


//static int vcom=0x40;
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

	params->type   = LCM_TYPE_DSI;

	params->width  = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

	// enable tearing-free
	params->dbi.te_mode = LCM_DBI_TE_MODE_DISABLED;
	params->dbi.te_edge_polarity = LCM_POLARITY_RISING;

	#if (LCM_DSI_CMD_MODE)
	params->dsi.mode = CMD_MODE;
	#else
	params->dsi.mode = SYNC_PULSE_VDO_MODE;//BURST_VDO_MODE;
	#endif

	// DSI
	/* Command mode setting */
		params->dsi.LANE_NUM				= LCM_THREE_LANE;//LCM_FOUR_LANE;

	//The following defined the fomat for data coming from LCD engine.
	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

	params->dsi.packet_size = 256;
	// Video mode setting
    params->physical_width = 65;
    params->physical_height = 129;

	params->dsi.vertical_sync_active		= 4;
	params->dsi.vertical_backporch			= 6;
	params->dsi.vertical_frontporch			= 14;
	params->dsi.vertical_active_line		= FRAME_HEIGHT;	//= FRAME_HEIGHT; 

	params->dsi.horizontal_sync_active		= 20;
	params->dsi.horizontal_backporch		= 20;
	params->dsi.horizontal_frontporch		= 25;
	params->dsi.horizontal_active_pixel		= FRAME_WIDTH;	//	= FRAME_WIDTH;


		params->dsi.PLL_CLOCK = 296;//240;221 j208//this value must be in MTK suggested table
	params->dsi.ssc_disable		= 1;
	params->dsi.ssc_range 		= 1;
	params->dsi.esd_check_enable=0;
	params->dsi.customization_esd_check_enable=0;
	params->dsi.lcm_esd_check_table[0].cmd=0x0A;
	params->dsi.lcm_esd_check_table[0].count=1;
	params->dsi.lcm_esd_check_table[0].para_list[0] = 0x1C;
}

static unsigned int lcm_ata_check(unsigned char *buffer)
{
#ifndef BUILD_LK
	return 1;
#endif
}
static void lcm_init(void)
{
/*
	lcm_power_5v_en(1);

	MDELAY(15);
	lcm_power_n5v_en(1);
	MDELAY(20);
#ifdef BUILD_LK
#ifdef __HCT_MTK_KTD2151__
	LCM_DATA_T2 i2c_data;
	
	i2c_data.cmd = 0x00;
	i2c_data.data = 0x0F; //5.5v
	lcm_i2c_set_data(LCM_I2C_WRITE, &i2c_data);
	i2c_data.cmd = 0x01;
	i2c_data.data = 0x0F;
	lcm_i2c_set_data(LCM_I2C_WRITE, &i2c_data);
#endif
#endif
*/
	SET_RESET_PIN(0);     //reset down
#ifdef GPIO_LCM_LDO_1V8_EN_PIN
    lcm_util.set_gpio_mode(GPIO_LCM_LDO_1V8_EN_PIN, GPIO_MODE_00);
    lcm_util.set_gpio_dir(GPIO_LCM_LDO_1V8_EN_PIN, GPIO_DIR_OUT);
    lcm_util.set_gpio_out(GPIO_LCM_LDO_1V8_EN_PIN, GPIO_OUT_ONE);  //shm
#else   
    mt_dsi_pinctrl_set(LCM_POWER_DM_NO, 1);
#endif
	MDELAY(10);
#ifdef GPIO_LCM_LDO_2V8_EN_PIN
    lcm_util.set_gpio_mode(GPIO_LCM_LDO_2V8_EN_PIN, GPIO_MODE_00);
    lcm_util.set_gpio_dir(GPIO_LCM_LDO_2V8_EN_PIN, GPIO_DIR_OUT);
    lcm_util.set_gpio_out(GPIO_LCM_LDO_2V8_EN_PIN, GPIO_OUT_ONE);
#else   
    mt_dsi_pinctrl_set(LCM_POWER_DP_NO, 1);
#endif
	MDELAY(20);
	SET_RESET_PIN(1);     //reset up
	MDELAY(10);            //1ms
	SET_RESET_PIN(0);     //reset down
	MDELAY(10);            //1ms
	SET_RESET_PIN(1);     //reset up
	MDELAY(120);

	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);	

}

static void lcm_suspend(void)
{
	unsigned int data_array[16];
        MDELAY(50);
	data_array[0]=0x00E01500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	data_array[0]=0x16E71500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(1);
	data_array[0]=0x00280500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(10);
	data_array[0]=0x00100500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(250);

	SET_RESET_PIN(0);     //reset down
	MDELAY(15);
/*
	lcm_power_n5v_en(0);
	MDELAY(15);
	lcm_power_5v_en(0);
	MDELAY(20);
*/
	//MDELAY(5);

}
static void lcm_resume(void)
{
	lcm_init();
}


static unsigned int lcm_compare_id(void)
{
	int array[4];
	char buffer[5];
	char id_high=0;
	char id_low=0;
	int id1=0;
	int id2=0;

	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(120);
   
    array[0]=0x00043902;    
    array[1]=0x068198ff;    // PAGE 6
	dsi_set_cmdq(array, 2, 1);
    MDELAY(10);
	array[0] = 0x00033700;
	dsi_set_cmdq(array, 1, 1);
	read_reg_v2(0x04, buffer, 3);

	id_high = buffer[0];

	id_low = buffer[1];
	id1 = (id_high<<8) | id_low;

	#if defined(BUILD_LK)
	printf("hct_jd9365z_dsi_vdo_hdp_sc_57_bh,%04x %04x %04x\n",buffer[0],buffer[1],buffer[2]);
	printf("hct_jd9365z_dsi_vdo_hdp_sc_57_bh %s id1 = 0x%04x, id2 = 0x%04x\n", __func__, id1,id2);
	#else

	printk("hct_jd9365z_dsi_vdo_hdp_sc_57_bh %s id1 = 0x%04x, id2 = 0x%04x\n", __func__, id1,id2);
	#endif
/*
	lcm_power_n5v_en(0);
	MDELAY(15);
	lcm_power_5v_en(0);
*/
	return (LCM_ID == id1)?1:0;

}

// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER hct_jd9365z_dsi_vdo_hdp_sc_57_bh = 
{
	.name		= "hct_jd9365z_dsi_vdo_hdp_sc_57_bh",
	.set_util_funcs = lcm_set_util_funcs,
	.compare_id     = lcm_compare_id,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,	
    //.set_backlight    = lcm_setbacklight,
	.ata_check	= lcm_ata_check,
#if defined(LCM_DSI_CMD_MODE)
    //.update         = lcm_update,
#endif
};

