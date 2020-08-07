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
#define FRAME_HEIGHT                (1440)
#define LCM_ID                      (0x9366)

#define REGFLAG_DELAY             				(0XFE)
#define REGFLAG_END_OF_TABLE      				(0x100)	

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
extern int PMU_db_pos_neg_setting_delay(int ms);
extern int PMU_db_pos_neg_disable_delay(int ms);
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
{0xE0,1,{0x00}},

{0xE1,1,{0x93}},
{0xE2,1,{0x66}},
{0xE3,1,{0xF9}},
{0x80,1,{0x03}},

{0xE0,1,{0x01}},

{0x00,1,{0x01}},
{0x01,1,{0x34}},
{0x03,1,{0x01}},
{0x04,1,{0x34}},

{0x0A,1,{0x08}},

{0x13,1,{0x00}},
{0x14,1,{0x99}},
{0x15,1,{0x51}},
{0x16,1,{0x51}},

{0x17,1,{0x00}},
{0x18,1,{0xA4}},
{0x19,1,{0x00}},
{0x1A,1,{0x00}},
{0x1B,1,{0xA4}},
{0x1C,1,{0x00}},

{0x1F,1,{0x30}},
{0x20,1,{0x2D}},
{0x21,1,{0x19}},
{0x22,1,{0x0D}},
{0x23,1,{0x02}},
{0x24,1,{0xFE}},

{0x26,1,{0xDF}},
{0x35,1,{0x13}},

{0x37,1,{0x09}},//05

{0x38,1,{0x04}},
{0x39,1,{0x01}},
{0x3A,1,{0x03}},
{0x3C,1,{0x60}},
{0x3D,1,{0x18}},
{0x3E,1,{0x80}},
{0x3F,1,{0x4E}},
{0x3D,1,{0xFF}},
{0x3E,1,{0xFF}},
{0x3F,1,{0xFF}},
{0x4B,1,{0x04}},

{0x40,1,{0x04}},
{0x41,1,{0xB4}},
{0x42,1,{0x70}},
{0x43,1,{0x24}},
{0x44,1,{0x0C}},
{0x45,1,{0x64}},

{0x55,1,{0x05}},
{0x56,1,{0x01}},
{0x57,1,{0x6D}},
{0x58,1,{0x0A}},
{0x59,1,{0x8A}},
{0x5A,1,{0x28}},
{0x5B,1,{0x23}},
{0x5C,1,{0x15}},

{0x5D,1,{0x75}},
{0x5E,1,{0x61}},
{0x5F,1,{0x53}},
{0x60,1,{0x47}},
{0x61,1,{0x43}},
{0x62,1,{0x34}},
{0x63,1,{0x39}},
{0x64,1,{0x22}},
{0x65,1,{0x3B}},
{0x66,1,{0x3A}},
{0x67,1,{0x3B}},
{0x68,1,{0x5C}},
{0x69,1,{0x4D}},
{0x6A,1,{0x57}},
{0x6B,1,{0x4B}},
{0x6C,1,{0x4B}},
{0x6D,1,{0x3E}},
{0x6E,1,{0x2E}},
{0x6F,1,{0x1A}},

{0x70,1,{0x75}},
{0x71,1,{0x61}},
{0x72,1,{0x53}},
{0x73,1,{0x47}},
{0x74,1,{0x43}},
{0x75,1,{0x34}},
{0x76,1,{0x39}},
{0x77,1,{0x22}},
{0x78,1,{0x3B}},
{0x79,1,{0x3A}},
{0x7A,1,{0x3B}},
{0x7B,1,{0x5C}},
{0x7C,1,{0x4D}},
{0x7D,1,{0x57}},
{0x7E,1,{0x4B}},
{0x7F,1,{0x4B}},
{0x80,1,{0x3E}},
{0x81,1,{0x2E}},
{0x82,1,{0x1A}},

{0xE0,1,{0x02}},


{0x00,1,{0x5D}},
{0x01,1,{0x51}},
{0x02,1,{0x5D}},
{0x03,1,{0x5D}},
{0x04,1,{0x5E}},
{0x05,1,{0x5F}},
{0x06,1,{0x51}},
{0x07,1,{0x41}},
{0x08,1,{0x45}},
{0x09,1,{0x5D}},
{0x0A,1,{0x5D}},
{0x0B,1,{0x4B}},
{0x0C,1,{0x49}},
{0x0D,1,{0x5D}},
{0x0E,1,{0x5D}},
{0x0F,1,{0x47}},
{0x10,1,{0x5F}},
{0x11,1,{0x5D}},
{0x12,1,{0x5D}},
{0x13,1,{0x5D}},
{0x14,1,{0x5D}},
{0x15,1,{0x5D}},


{0x16,1,{0x5D}},
{0x17,1,{0x50}},
{0x18,1,{0x5D}},
{0x19,1,{0x5D}},
{0x1A,1,{0x5E}},
{0x1B,1,{0x5F}},
{0x1C,1,{0x50}},
{0x1D,1,{0x40}},
{0x1E,1,{0x44}},
{0x1F,1,{0x5D}},
{0x20,1,{0x5D}},
{0x21,1,{0x4A}},
{0x22,1,{0x48}},
{0x23,1,{0x5D}},
{0x24,1,{0x5D}},
{0x25,1,{0x46}},
{0x26,1,{0x5F}},
{0x27,1,{0x5D}},
{0x28,1,{0x5D}},
{0x29,1,{0x5D}},
{0x2A,1,{0x5D}},
{0x2B,1,{0x5D}},




{0x2C,1,{0x1D}},
{0x2D,1,{0x10}},
{0x2E,1,{0x1D}},
{0x2F,1,{0x1D}},
{0x30,1,{0x1F}},
{0x31,1,{0x1E}},
{0x32,1,{0x00}},
{0x33,1,{0x10}},
{0x34,1,{0x06}},
{0x35,1,{0x1D}},
{0x36,1,{0x1D}},
{0x37,1,{0x08}},
{0x38,1,{0x0A}},
{0x39,1,{0x1D}},
{0x3A,1,{0x1D}},
{0x3B,1,{0x04}},
{0x3C,1,{0x1F}},
{0x3D,1,{0x1D}},
{0x3E,1,{0x1D}},
{0x3F,1,{0x1D}},
{0x40,1,{0x1D}},
{0x41,1,{0x1D}},


{0x42,1,{0x1D}},
{0x43,1,{0x11}},
{0x44,1,{0x1D}},
{0x45,1,{0x1D}},
{0x46,1,{0x1F}},
{0x47,1,{0x1E}},
{0x48,1,{0x01}},
{0x49,1,{0x11}},
{0x4A,1,{0x07}},
{0x4B,1,{0x1D}},
{0x4C,1,{0x1D}},
{0x4D,1,{0x09}},
{0x4E,1,{0x0B}},
{0x4F,1,{0x1D}},
{0x50,1,{0x1D}},
{0x51,1,{0x05}},
{0x52,1,{0x1F}},
{0x53,1,{0x1D}},
{0x54,1,{0x1D}},
{0x55,1,{0x1D}},
{0x56,1,{0x1D}},
{0x57,1,{0x1D}},


{0x58,1,{0x41}},
{0x59,1,{0x00}},
{0x5A,1,{0x00}},
{0x5B,1,{0x10}},
{0x5C,1,{0x02}},
{0x5D,1,{0x60}},
{0x5E,1,{0x01}},
{0x5F,1,{0x02}},
{0x60,1,{0x40}},
{0x61,1,{0x03}},
{0x62,1,{0x04}},
{0x63,1,{0x14}},
{0x64,1,{0x1A}},
{0x65,1,{0x55}},
{0x66,1,{0xB1}},
{0x67,1,{0x73}},
{0x68,1,{0x04}},
{0x69,1,{0x16}},
{0x6A,1,{0x58}},
{0x6B,1,{0x0A}},
{0x6C,1,{0x00}},
{0x6D,1,{0x00}},
{0x6E,1,{0x00}},
{0x6F,1,{0x88}},
{0x70,1,{0x00}},
{0x71,1,{0x00}},
{0x72,1,{0x06}},
{0x73,1,{0x7B}},
{0x74,1,{0x00}},
{0x75,1,{0xBC}},
{0x76,1,{0x00}},
{0x77,1,{0x05}},
{0x78,1,{0x34}},
{0x79,1,{0x00}},
{0x7A,1,{0x00}},
{0x7B,1,{0x00}},
{0x7C,1,{0x00}},
{0x7D,1,{0x03}},
{0x7E,1,{0x7B}},
{0x80,1,{0x06}},

{0xE0,1,{0x04}},

{0x00,1,{0x02}},
{0x02,1,{0x23}},
{0x03,1,{0x8F}},

{0x09,1,{0x11}},

{0x0E,1,{0x2A}},

{0x9A,1,{0x01}},
{0x9B,1,{0x05}},

{0xA9,1,{0x01}},
{0xAA,1,{0x68}},

{0xAC,1,{0x19}},
{0xAD,1,{0x15}},
{0xAE,1,{0x15}},

{0xE0,1,{0x00}},
{0xE6,1,{0x02}},
{0xE7,1,{0x06}},

{0xE0,1,{0x04}},
{0x97,1,{0x02}},
{0x98,1,{0x12}},
{0xA3,1,{0x06}},

{0xE0,1,{0x00}},
{0x35,1,{0x00}},

{0x11,1,{0x01}},
{REGFLAG_DELAY,120,{}},

{0x29,1,{0x01}},
{REGFLAG_DELAY,20,{}},




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
	params->dsi.LANE_NUM				= LCM_FOUR_LANE;//LCM_THREE_LANE;//LCM_FOUR_LANE;

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
	params->dsi.vertical_backporch			= 8;
	params->dsi.vertical_frontporch			= 36;
	params->dsi.vertical_active_line		= FRAME_HEIGHT;	//= FRAME_HEIGHT; 

	params->dsi.horizontal_sync_active		= 8;
	params->dsi.horizontal_backporch		= 30;
	params->dsi.horizontal_frontporch		= 30;
	params->dsi.horizontal_active_pixel		= FRAME_WIDTH;	//	= FRAME_WIDTH;


	params->dsi.PLL_CLOCK = 216;
	params->dsi.ssc_disable		= 1;
	params->dsi.ssc_range 		= 1;
	params->dsi.esd_check_enable=1;
	params->dsi.customization_esd_check_enable=1;
	params->dsi.lcm_esd_check_table[0].cmd=0x0A;
	params->dsi.lcm_esd_check_table[0].count=1;
	params->dsi.lcm_esd_check_table[0].para_list[0] = 0x1C;
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
	MDELAY(10);
    mt_dsi_pinctrl_set(LCM_POWER_DM_NO, 0);
	MDELAY(10);
    mt_dsi_pinctrl_set(LCM_POWER_DM_NO, 1);
	MDELAY(10);
	PMU_db_pos_neg_setting_delay(12);
	SET_RESET_PIN(1);     //reset up
	MDELAY(10);            //1ms
	SET_RESET_PIN(0);     //reset down
	MDELAY(10);            //1ms
	SET_RESET_PIN(1);     //reset up
	MDELAY(30);

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
	PMU_db_pos_neg_disable_delay(10);
	MDELAY(10);
    mt_dsi_pinctrl_set(LCM_POWER_DM_NO, 0);
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
	MDELAY(200);
/*
	lcm_power_5v_en(1);
	MDELAY(15);
	lcm_power_n5v_en(1);
*/
	PMU_db_pos_neg_setting_delay(12);
	array[0] = 0x00033700;
	dsi_set_cmdq(array, 1, 1);
	read_reg_v2(0x04, buffer, 3);

	id_high = buffer[0];

	id_low = buffer[1];
	id1 = (id_high<<8) | id_low;

	#if defined(BUILD_LK)
	printf("hct_jd9366d_dsi_vdo_hdp_panda_57_dzx,%04x %04x %04x\n",buffer[0],buffer[1],buffer[2]);
	printf("hct_jd9366d_dsi_vdo_hdp_panda_57_dzx %s id1 = 0x%04x, id2 = 0x%04x\n", __func__, id1,id2);
	#else

	printk("hct_jd9366d_dsi_vdo_hdp_panda_57_dzx %s id1 = 0x%04x, id2 = 0x%04x\n", __func__, id1,id2);
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
LCM_DRIVER hct_jd9366d_dsi_vdo_hdp_panda_57_dzx = 
{
	.name		= "hct_jd9366d_dsi_vdo_hdp_panda_57_dzx",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,	
	.compare_id     = lcm_compare_id,	
#if 0//defined(LCM_DSI_CMD_MODE)
    //.set_backlight    = lcm_setbacklight,
    //.esd_check   = lcm_esd_check, 
    //.esd_recover   = lcm_esd_recover, 
    .update         = lcm_update,
#endif
};

