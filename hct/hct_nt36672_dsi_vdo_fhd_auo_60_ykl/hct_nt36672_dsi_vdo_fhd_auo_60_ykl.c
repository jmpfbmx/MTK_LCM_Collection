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




// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  										(1080)
#define FRAME_HEIGHT 										(2160)

#define LCM_ID_NT36672 										(0x80)

#define REGFLAG_DELAY             							0xAB
#define REGFLAG_END_OF_TABLE      							0xAA   // END OF REGISTERS MARKER


//#define LCM_DSI_CMD_MODE									0

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

#define UDELAY(n) 											(lcm_util.udelay(n))
#define MDELAY(n) 											(lcm_util.mdelay(n))


//static unsigned int lcm_esd_test = FALSE;      ///only for ESD test

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
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};
extern int PMU_db_pos_neg_setting_delay(int ms);
extern int PMU_db_pos_neg_disable_delay(int ms);
//============9885initialCode========
//注意最大分包长度等级设置为最高，最多123个参数
/////////////////////////////////////////////////////////////////
/*static void lcm_power_5v_en(unsigned char enabled)
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
}*/
static void push_table(struct LCM_setting_table *table, unsigned int count,
		unsigned char force_update)
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
			dsi_set_cmdq_V2(cmd, table[i].count,
					table[i].para_list, force_update);
		}
	}

}
static struct LCM_setting_table lcm_initialization_setting[] = {
{0xFF,1,{0x20}},
{0xFB,1,{0x01}},
{0x01,1,{0x33}},
{0x06,1,{0x99}},
{0x07,1,{0x9E}},
{0x0E,1,{0x30}},
{0x0F,1,{0x2E}},
{0x1D,1,{0x33}},
{0x6D,1,{0x66}},
{0x68,1,{0x03}},
{0x69,1,{0x99}},
{0x89,1,{0x0F}},
{0x95,1,{0xCD}},//CD
{0x96,1,{0xCD}},//CD
{0xFF,1,{0x24}},
{0xFB,1,{0x01}},
{0x00,1,{0x01}},
{0x01,1,{0x1C}},
{0x02,1,{0x0B}},
{0x03,1,{0x0C}},
{0x04,1,{0x29}},
{0x05,1,{0x0F}},
{0x06,1,{0x0F}},
{0x07,1,{0x03}},
{0x08,1,{0x05}},
{0x09,1,{0x22}},
{0x0A,1,{0x00}},
{0x0B,1,{0x24}},
{0x0C,1,{0x13}},
{0x0D,1,{0x13}},
{0x0E,1,{0x15}},
{0x0F,1,{0x15}},
{0x10,1,{0x17}},
{0x11,1,{0x17}},
{0x12,1,{0x01}},
{0x13,1,{0x1C}},
{0x14,1,{0x0B}},
{0x15,1,{0x0C}},
{0x16,1,{0x29}},
{0x17,1,{0x0F}},
{0x18,1,{0x0F}},
{0x19,1,{0x04}},
{0x1A,1,{0x06}},
{0x1B,1,{0x23}},
{0x1C,1,{0x0F}},
{0x1D,1,{0x24}},
{0x1E,1,{0x13}},
{0x1F,1,{0x13}},
{0x20,1,{0x15}},
{0x21,1,{0x15}},
{0x22,1,{0x17}},
{0x23,1,{0x17}},
{0x2F,1,{0x04}},
{0x30,1,{0x08}},
{0x31,1,{0x04}},
{0x32,1,{0x08}},
{0x33,1,{0x04}},
{0x34,1,{0x04}},
{0x35,1,{0x00}},
{0x37,1,{0x09}},
{0x38,1,{0x75}},
{0x39,1,{0x75}},
{0x3B,1,{0xC0}},
{0x3F,1,{0x75}},
{0x60,1,{0x10}},
{0x61,1,{0x00}},
{0x68,1,{0xC2}},
{0x78,1,{0x80}},
{0x79,1,{0x23}},
{0x7A,1,{0x10}},
{0x7B,1,{0x9B}},
{0x7C,1,{0x80}},
{0x7D,1,{0x06}}, 
{0x7E,1,{0x02}},
{0x8E,1,{0xF0}},
{0x92,1,{0x76}},
{0x93,1,{0x0A}},
{0x94,1,{0x0A}},
{0x99,1,{0x33}},
{0x9B,1,{0xFF}},
{0x9F,1,{0x00}},
{0xA3,1,{0x91}},
{0xB3,1,{0x00}},
{0xB4,1,{0x00}},
{0xB5,1,{0x04}},
{0xDC,1,{0x40}},
{0xDD,1,{0x03}},
{0xDE,1,{0x01}},
{0xDF,1,{0x3D}},
{0xE0,1,{0x3D}},
{0xE1,1,{0x22}},
{0xE2,1,{0x24}},
{0xE3,1,{0x0A}},
{0xE4,1,{0x0A}},
{0xE8,1,{0x01}},
{0xE9,1,{0x10}},
{0xED,1,{0x40}},
{0xFF,1,{0x25}},
{0xFB,1,{0x01}},
{0x0A,1,{0x81}},
{0x0B,1,{0xCD}},
{0x0C,1,{0x01}},
{0x17,1,{0x82}},
{0x21,1,{0x1B}},
{0x22,1,{0x1B}},
{0x24,1,{0x76}},
{0x25,1,{0x76}},
{0x30,1,{0x2A}},
{0x31,1,{0x2A}},
{0x38,1,{0x2A}},
{0x3F,1,{0x11}}, 
{0x40,1,{0x3A}},
{0x4B,1,{0x31}},
{0x4C,1,{0x3A}},
{0x58,1,{0x22}},
{0x59,1,{0x05}},
{0x5A,1,{0x0A}},
{0x5B,1,{0x0A}},
{0x5C,1,{0x25}},
{0x5D,1,{0x80}},
{0x5E,1,{0x80}},
{0x5F,1,{0x28}},
{0x62,1,{0x3F}},
{0x63,1,{0x82}},
{0x65,1,{0x00}},
{0x66,1,{0xDD}},
{0x6C,1,{0x6D}},
{0x71,1,{0x6D}},
{0x78,1,{0x25}},
{0xC3,1,{0x00}},
{0xFF,1,{0x26}},
{0xFB,1,{0x01}},
{0x06,1,{0xC8}},
{0x12,1,{0x5A}},
{0x19,1,{0x09}},
{0x1A,1,{0x84}},
{0x1C,1,{0xFA}},
{0x1D,1,{0x09}},
{0x1E,1,{0x0B}},
{0x99,1,{0x20}},
{0xFF,1,{0x27}},
{0xFB,1,{0x01}},
{0x13,1,{0x08}},
{0x14,1,{0x43}},
{0x16,1,{0xB8}},
{0x17,1,{0xB8}},
{0x7A,1,{0x02}},
{0xFF,1,{0x10}},
{0xFB,1,{0x01}},
{0x51,1,{0xFF}},
{0x53,1,{0x24}},
{0x55,1,{0x00}},
//{0xff,1,{0x25}},
//{0xEC,1,{0x01}},


{0x11, 0,{0x00}},

{REGFLAG_DELAY, 150, {}},

{0x29, 0,{0x00}},
{REGFLAG_DELAY, 20, {}},

};


/*
static struct LCM_setting_table lcm_sleep_out_setting[] = {
	// Sleep Out
	{0x11, 1, {0x00}},
	{REGFLAG_DELAY, 120, {}},

	// Display ON
	{0x29, 1, {0x00}},
	{REGFLAG_DELAY, 10, {}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
*/
/*
static struct LCM_setting_table lcm_sleep_in_setting[] = {
	// Display off sequence
	{0x01, 1, {0x00}},
	{REGFLAG_DELAY, 50, {}},
	
	{0x28, 1, {0x00}},
	{REGFLAG_DELAY, 50, {}},

	// Sleep Mode On
	{0x10, 1, {0x00}},
	{REGFLAG_DELAY, 50, {}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
*/


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
		params->dsi.LANE_NUM				= LCM_FOUR_LANE;
	
	//The following defined the fomat for data coming from LCD engine.
	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;
	
	
	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
	
#if (LCM_DSI_CMD_MODE)
	params->dsi.intermediat_buffer_num = 0;//because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage
	params->dsi.word_count=FRAME_WIDTH*3;	//DSI CMD mode need set these two bellow params, different to 6577
#else
	params->dsi.intermediat_buffer_num = 0;	//because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage
#endif

  // Video mode setting

    params->dsi.packet_size=256;

 

    params->dsi.vertical_sync_active                =  2;

    params->dsi.vertical_backporch                    = 8;//16 25 30 35 12 8

    params->dsi.vertical_frontporch                    = 10;

    params->dsi.vertical_active_line                = FRAME_HEIGHT; 

 

    params->dsi.horizontal_sync_active                = 30;//56

    params->dsi.horizontal_backporch                = 85; //104

    params->dsi.horizontal_frontporch                = 20;//20

    params->dsi.horizontal_active_pixel                = FRAME_WIDTH;

 

 

    params->dsi.PLL_CLOCK=450;//230

	params->dsi.cont_clock = 1;
	params->dsi.clk_lp_per_line_enable = 1;
	params->dsi.ssc_disable = 1;	
	
	params->dsi.esd_check_enable = 1;
	params->dsi.customization_esd_check_enable = 1;
    params->dsi.lcm_esd_check_table[0].cmd          = 0x0a;
    params->dsi.lcm_esd_check_table[0].count        = 1;
    params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9c;


}

static void lcm_init(void)
{
#if 1
    	//MDELAY(10);
	PMU_db_pos_neg_setting_delay(12);
//	set_gpio_tp_incell_rst(0);
//	MDELAY(50);
//	set_gpio_tp_incell_rst(1);
//	MDELAY(50);
	SET_RESET_PIN(1);
	MDELAY(50);
	SET_RESET_PIN(0);
	MDELAY(50);
	SET_RESET_PIN(1);
	MDELAY(300);
#endif


	push_table(lcm_initialization_setting,sizeof(lcm_initialization_setting) /sizeof(struct LCM_setting_table), 1);
}

static void lcm_suspend(void)
{

    unsigned int data_array[16];


    data_array[0]=0x00280500;
    dsi_set_cmdq(data_array, 1, 1);
    MDELAY(10);
    data_array[0]=0x00100500;

    dsi_set_cmdq(data_array, 1, 1);
    MDELAY(120);

	//power off
	
  	SET_RESET_PIN(0);     //reset down
	MDELAY(15);
	PMU_db_pos_neg_disable_delay(10);
	MDELAY(10);
   // SET_RESET_PIN(0);
   // MDELAY(20);

}


    //for LGE backlight IC mapping table


    // Refresh value of backlight level.

//static unsigned int lcm_compare_id(void);

static void lcm_resume(void)
{
	lcm_init();
}

#if 1
static unsigned int lcm_compare_id(void)
{

	unsigned int array[4];
	char buffer[5];
	char id_high=0;
	int id2=0;

	PMU_db_pos_neg_setting_delay(12);
//	set_gpio_tp_incell_rst(0);
//	MDELAY(50);
//	set_gpio_tp_incell_rst(1);
//	MDELAY(50);
	SET_RESET_PIN(1);
	MDELAY(50);
	SET_RESET_PIN(0);
	MDELAY(50);
	SET_RESET_PIN(1);
	MDELAY(300);
 
	array[0] = 0x00013700;

	dsi_set_cmdq(array, 1, 1);
	read_reg_v2(0x04, buffer, 2);

	id_high = buffer[1];
/*
 read_reg_v2(0xdf, buffer, 1);
 id_low = buffer[0];
 id1 = (id_high<<8) | id_low;
*/
	 #if defined(BUILD_LK)
	  printf("nt36672 %s id_high = 0x%04x, id2 = 0x%04x\n", __func__, id_high,id2);
	 #else
	  printk("nt36672 %s id_high = 0x%04x, id2 = 0x%04x\n", __func__, id_high,id2);
	 #endif
	 return (LCM_ID_NT36672 == id_high)?1:0;

}
#endif
LCM_DRIVER hct_nt36672_dsi_vdo_fhd_auo_60_ykl = 
{
	.name			= "hct_nt36672_dsi_vdo_fhd_auo_60_ykl",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,	
    .compare_id    = lcm_compare_id,    

#if (LCM_DSI_CMD_MODE)
    //.update         = lcm_update,
#endif	//wqtao
};

