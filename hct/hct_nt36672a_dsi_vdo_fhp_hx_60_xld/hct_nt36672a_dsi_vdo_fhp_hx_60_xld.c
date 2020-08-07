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

#ifdef BUILD_LK
	#include <platform/mt_gpio.h>
	#include <string.h>
#elif defined(BUILD_UBOOT)
	#include <asm/arch/mt_gpio.h>
#else
	//#include <mach/mt_gpio.h>
#endif
// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  										(1080)
#define FRAME_HEIGHT 										(2246)

#define LCM_ID_NT36672 										(0x80)

#define REGFLAG_DELAY             							0xAB
#define REGFLAG_END_OF_TABLE      							0xAA   // END OF REGISTERS MARKER


#define LCM_DSI_CMD_MODE									0

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

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)										lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)
#define set_gpio_lcd_enp(cmd)                               lcm_util.set_gpio_lcd_enp_bias(cmd)
#define set_gpio_lcd_enn(cmd)                               lcm_util.set_gpio_lcd_enn_bias(cmd)
#define set_gpio_tp_incell_rst(cmd)                         lcm_util.set_gpio_tp_incell_rst(cmd)

 struct LCM_setting_table {
	unsigned int cmd;
    unsigned char count;
    unsigned char para_list[64];
};

extern int PMU_db_pos_neg_setting_delay(int ms);
extern int PMU_db_pos_neg_setting_delay_hct(int ms, int vol);
extern int PMU_db_pos_neg_disable_delay(int ms);
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
{0x06,1,{0xA8}},
{0x07,1,{0x9E}},
{0x0E,1,{0xB5}},
{0x0F,1,{0xA4}},
{0x61,1,{0x81}},
{0x62,1,{0xC4}},
{0x63,1,{0x01}},
{0x68,1,{0x03}},
{0x69,1,{0x99}},
{0x6D,1,{0x44}},
{0x78,1,{0x00}},
{0x89,1,{0x13}}, 
{0x94,1,{0x00}},
{0x95,1,{0xFF}},
{0x96,1,{0xFF}},
{0xFB,1,{0x01}},
{0xFF,1,{0x24}},
{0x00,1,{0x00}},
{0x01,1,{0x00}},
{0x02,1,{0x00}},
{0x03,1,{0x00}},
{0x04,1,{0x00}},
{0x05,1,{0x00}},
{0x06,1,{0x00}},
{0x07,1,{0x00}},
{0x08,1,{0x00}},
{0x09,1,{0x00}},
{0x0A,1,{0x20}}, 
{0x0B,1,{0x00}},
{0x0C,1,{0x10}},
{0x0D,1,{0x12}},
{0x0E,1,{0x14}},
{0x0F,1,{0x1E}},
{0x10,1,{0x0D}},
{0x11,1,{0x0A}},
{0x12,1,{0x01}},
{0x13,1,{0x03}},
{0x14,1,{0x04}},
{0x15,1,{0x05}},
{0x16,1,{0x06}},
{0x17,1,{0x00}},
{0x18,1,{0x00}},
{0x19,1,{0x00}},
{0x1A,1,{0x00}},
{0x1B,1,{0x00}},
{0x1C,1,{0x00}},
{0x1D,1,{0x00}},
{0x1E,1,{0x00}},
{0x1F,1,{0x00}},
{0x20,1,{0x00}},
{0x21,1,{0x20}}, 
{0x22,1,{0x00}},
{0x23,1,{0x10}},
{0x24,1,{0x12}},
{0x25,1,{0x14}},
{0x26,1,{0x1E}},
{0x27,1,{0x0D}},
{0x28,1,{0x0A}},
{0x29,1,{0x01}},
{0x2A,1,{0x03}},
{0x2B,1,{0x04}},
{0x2D,1,{0x05}},
{0x2F,1,{0x06}},
{0x31,1,{0x06}},
{0x32,1,{0x0B}},
{0x33,1,{0x04}},
{0x34,1,{0x03}},
{0x35,1,{0x49}},
{0x37,1,{0x01}}, 
{0x38,1,{0x70}}, 
{0x39,1,{0x70}},
{0x3F,1,{0x70}},
{0x41,1,{0x06}},
{0x42,1,{0x0B}},
{0x4C,1,{0x09}},
{0x4D,1,{0x09}},
{0x60,1,{0x80}},
{0x61,1,{0xC6}},
{0x78,1,{0x80}}, 
{0x79,1,{0x00}},
{0x7A,1,{0x08}},
{0x7B,1,{0x9D}},
{0x7C,1,{0x80}},
{0x7D,1,{0x22}},
{0x80,1,{0x42}},
{0x82,1,{0x11}},
{0x83,1,{0x22}},
{0x84,1,{0x33}},
{0x85,1,{0x00}},
{0x86,1,{0x00}},
{0x87,1,{0x00}},
{0x88,1,{0x11}},
{0x89,1,{0x22}},
{0x8A,1,{0x33}},
{0x8B,1,{0x00}},
{0x8C,1,{0x00}},
{0x8D,1,{0x00}},
{0x8E,1,{0xF0}},
{0x92,1,{0x76}},
{0x93,1,{0x10}},
{0x94,1,{0x0A}},
{0xB3,1,{0x0A}},
{0xB4,1,{0x0A}},
{0xB5,1,{0x14}},
{0xDC,1,{0x29}},
{0xDD,1,{0x04}}, 
{0xDE,1,{0x03}},
{0xDF,1,{0x76}},
{0xE0,1,{0x76}},
{0xEB,1,{0x0A}},
{0xFB,1,{0x01}},
{0xFF,1,{0x25}},
{0xD0,1,{0x01}},
{0xD4,1,{0x05}},
{0xD5,1,{0x05}},
{0xD6,1,{0x00}},
{0x21,1,{0x1D}},
{0x22,1,{0x1D}},
{0x24,1,{0x76}},
{0x25,1,{0x76}},
{0x30,1,{0x2F}},
{0x31,1,{0x2F}},
{0x38,1,{0x2F}},
{0x3F,1,{0x11}},
{0x40,1,{0x75}},
{0x41,1,{0x75}},
{0x42,1,{0x33}},
{0x43,1,{0x22}},
{0x44,1,{0x22}},
{0x4B,1,{0x31}},
{0x4C,1,{0x75}},
{0x4D,1,{0x22}},
{0x84,1,{0x60}},
{0xFB,1,{0x01}},
{0xFF,1,{0x26}},
{0x06,1,{0xC8}},
{0x0C,1,{0x13}},
{0x0F,1,{0x0A}},
{0x10,1,{0x0B}},
{0x12,1,{0x70}},
{0x19,1,{0x0C}},
{0x1A,1,{0x25}},
{0x1C,1,{0xAF}},
{0x1D,1,{0x0B}},
{0x1E,1,{0xAC}},
{0x23,1,{0x09}},
{0x24,1,{0x09}},
{0x25,1,{0x06}},
{0x28,1,{0x04}},
{0x29,1,{0xE0}},
{0x2A,1,{0xE1}},
{0x2F,1,{0x81}},
{0x33,1,{0x19}},
{0x34,1,{0x89}},
{0x37,1,{0x19}},
{0x38,1,{0x10}},
{0xAE,1,{0x88}}, 
{0xFB,1,{0x01}},
{0xFF,1,{0x27}},
{0x1E,1,{0x28}},
{0xFB,1,{0x01}},
{0xFF,1,{0xF0}},
{0xA2,1,{0x00}},
{0xFB,1,{0x01}},
{0xFF,1,{0x20}},
{0xFB,1,{0x01}},

{0xB0,16,{ 0x00, 0x13, 0x00, 0x27, 0x00, 0x4E, 0x00, 0x6D, 0x00, 0x87, 0x00, 0x9F, 0x00, 0xB4, 0x00, 0xC7}},       
{0xB1,16,{ 0x00, 0xD8, 0x01, 0x0F, 0x01, 0x39, 0x01, 0x78, 0x01, 0xA8, 0x01, 0xF3, 0x02, 0x31, 0x02, 0x33}},       
{0xB2,16,{ 0x02, 0x6C, 0x02, 0xAE, 0x02, 0xD8, 0x03, 0x0E, 0x03, 0x30, 0x03, 0x5B, 0x03, 0x68, 0x03, 0x76}},       
{0xB3,12,{ 0x03, 0x84, 0x03, 0x97, 0x03, 0xB0, 0x03, 0xC0, 0x03, 0xD6, 0x03, 0xD9}},                               
{0xB4,16,{ 0x00, 0x13, 0x00, 0x27, 0x00, 0x4E, 0x00, 0x6D, 0x00, 0x87, 0x00, 0x9F, 0x00, 0xB4, 0x00, 0xC7}},       
{0xB5,16,{ 0x00, 0xD8, 0x01, 0x0F, 0x01, 0x39, 0x01, 0x78, 0x01, 0xA8, 0x01, 0xF3, 0x02, 0x31, 0x02, 0x33}},       
{0xB6,16,{ 0x02, 0x6C, 0x02, 0xAE, 0x02, 0xD8, 0x03, 0x0E, 0x03, 0x30, 0x03, 0x5B, 0x03, 0x68, 0x03, 0x76}},       
{0xB7,12,{ 0x03, 0x84, 0x03, 0x97, 0x03, 0xB0, 0x03, 0xC0, 0x03, 0xD6, 0x03, 0xD9}},                              
{0xB8,16,{ 0x00, 0x13, 0x00, 0x27, 0x00, 0x4E, 0x00, 0x6D, 0x00, 0x87, 0x00, 0x9F, 0x00, 0xB4, 0x00, 0xC7}},      
{0xB9,16,{ 0x00, 0xD8, 0x01, 0x0F, 0x01, 0x39, 0x01, 0x78, 0x01, 0xA8, 0x01, 0xF3, 0x02, 0x31, 0x02, 0x33}},       
{0xBA,16,{ 0x02, 0x6C, 0x02, 0xAE, 0x02, 0xD8, 0x03, 0x0E, 0x03, 0x30, 0x03, 0x5B, 0x03, 0x68, 0x03, 0x76}},       
{0xBB,12,{ 0x03, 0x84, 0x03, 0x97, 0x03, 0xB0, 0x03, 0xC0, 0x03, 0xD6, 0x03, 0xD9}},                               

{0xFF,1,{0x21}},  
{0xFB,1,{0x01}},  

{0xB0,16,{ 0x00, 0x13, 0x00, 0x27, 0x00, 0x4E, 0x00, 0x6D, 0x00, 0x87, 0x00, 0x9F, 0x00, 0xB4, 0x00, 0xC7}},      
{0xB1,16,{ 0x00, 0xD8, 0x01, 0x0F, 0x01, 0x39, 0x01, 0x78, 0x01, 0xA8, 0x01, 0xF3, 0x02, 0x31, 0x02, 0x33}},      
{0xB2,16,{ 0x02, 0x6C, 0x02, 0xAE, 0x02, 0xD8, 0x03, 0x0E, 0x03, 0x30, 0x03, 0x5B, 0x03, 0x68, 0x03, 0x76}},      
{0xB3,12,{ 0x03, 0x84, 0x03, 0x97, 0x03, 0xB0, 0x03, 0xC0, 0x03, 0xD6, 0x03, 0xD9}},                              
{0xB4,16,{ 0x00, 0x13, 0x00, 0x27, 0x00, 0x4E, 0x00, 0x6D, 0x00, 0x87, 0x00, 0x9F, 0x00, 0xB4, 0x00, 0xC7}},      
{0xB5,16,{ 0x00, 0xD8, 0x01, 0x0F, 0x01, 0x39, 0x01, 0x78, 0x01, 0xA8, 0x01, 0xF3, 0x02, 0x31, 0x02, 0x33}},      
{0xB6,16,{ 0x02, 0x6C, 0x02, 0xAE, 0x02, 0xD8, 0x03, 0x0E, 0x03, 0x30, 0x03, 0x5B, 0x03, 0x68, 0x03, 0x76}},      
{0xB7,12,{ 0x03, 0x84, 0x03, 0x97, 0x03, 0xB0, 0x03, 0xC0, 0x03, 0xD6, 0x03, 0xD9}},                                    
{0xB8,16,{ 0x00, 0x13, 0x00, 0x27, 0x00, 0x4E, 0x00, 0x6D, 0x00, 0x87, 0x00, 0x9F, 0x00, 0xB4, 0x00, 0xC7}},      
{0xB9,16,{ 0x00, 0xD8, 0x01, 0x0F, 0x01, 0x39, 0x01, 0x78, 0x01, 0xA8, 0x01, 0xF3, 0x02, 0x31, 0x02, 0x33}},      
{0xBA,16,{ 0x02, 0x6C, 0x02, 0xAE, 0x02, 0xD8, 0x03, 0x0E, 0x03, 0x30, 0x03, 0x5B, 0x03, 0x68, 0x03, 0x76}},      
{0xBB,12,{ 0x03, 0x84, 0x03, 0x97, 0x03, 0xB0, 0x03, 0xC0, 0x03, 0xD6, 0x03, 0xD9}},                              

{0xFF,1,{0x10}},
{0x0A,1,{0x9C}},
{0x35,1,{0x00}},
{0xFB,1,{0x01}},
{0xFF,1,{0x23}},
{0x08,1,{0x04}},
{0x09,1,{0x80}},    
{0xFB,1,{0x01}},
{0xFF,1,{0x10}},
{0xFB,1,{0x01}},


        {0x11, 1, {0x00}},                   
        {REGFLAG_DELAY,120,{}},         
        {0x29, 1, {0x00}},              
        {REGFLAG_DELAY,20,{}},          
        {REGFLAG_END_OF_TABLE, 0x00, {}}                                   

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

    params->physical_width = 70;
    params->physical_height = 138;
 
    params->dsi.vertical_sync_active                =  4;//2

    params->dsi.vertical_backporch                    =6 ;//16 25 30 35 12 8 8

    params->dsi.vertical_frontporch                    = 10;

    params->dsi.vertical_active_line                = FRAME_HEIGHT; 

 

    params->dsi.horizontal_sync_active                =4 ;//56 30

    params->dsi.horizontal_backporch                =60; //104 85

    params->dsi.horizontal_frontporch                =60 ;//20 20

    params->dsi.horizontal_active_pixel                = FRAME_WIDTH;

 

 

    params->dsi.PLL_CLOCK=492;//230 450  550

	params->dsi.cont_clock = 1;
	params->dsi.clk_lp_per_line_enable = 1;
	params->dsi.ssc_disable = 1;	
	
	params->dsi.esd_check_enable = 0;
	params->dsi.customization_esd_check_enable = 0;
    params->dsi.lcm_esd_check_table[0].cmd          = 0x0a;
    params->dsi.lcm_esd_check_table[0].count        = 1;
    params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9c;


}

static void lcm_init(void)
{
    SET_RESET_PIN(0);
	MDELAY(10);
    set_gpio_tp_incell_rst(0);
	MDELAY(10);
	set_gpio_lcd_enn(0);  //1.8
	MDELAY(10);
	set_gpio_lcd_enn(1);  //1.8
    MDELAY(5);

    PMU_db_pos_neg_setting_delay(10);
    MDELAY(15);

	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(20);
	SET_RESET_PIN(1);
	MDELAY(15);
    set_gpio_tp_incell_rst(1);
	MDELAY(120);

	push_table(lcm_initialization_setting,sizeof(lcm_initialization_setting) /sizeof(struct LCM_setting_table), 1);
}

static void lcm_suspend(void)
{
	unsigned int array[5];
 	//array[0] = 0x00011500;// read id return two byte,version and id
	//dsi_set_cmdq(array, 1, 1);
	//MDELAY(100);
	/*array[0] = 0x01FE1500;
	dsi_set_cmdq(array, 1, 1);
	MDELAY(5);
	array[0] = 0x00461500;
	dsi_set_cmdq(array, 1, 1);
	MDELAY(5);*/
	array[0] = 0x00280500;
	dsi_set_cmdq(array, 1, 1);
	MDELAY(20);
 	array[0] = 0x00100500;// read id return two byte,version and id
	dsi_set_cmdq(array, 1, 1);
	MDELAY(120);

	SET_RESET_PIN(0);
	MDELAY(5);
	set_gpio_tp_incell_rst(0);
	MDELAY(10);

	PMU_db_pos_neg_disable_delay(10);
	MDELAY(5);

	set_gpio_lcd_enn(0);  //1.8
}

static unsigned int lcm_compare_id(void);
static void lcm_resume(void)
{
	lcm_init();
}


static unsigned int lcm_compare_id(void)
{

	unsigned int array[4];
	char buffer[5];
	char id_high=0;
	int id2=0;

    SET_RESET_PIN(0);

	set_gpio_lcd_enn(1);  //1.8
    MDELAY(5);
 
    PMU_db_pos_neg_setting_delay_hct(10, 54);
    MDELAY(15);
 
	SET_RESET_PIN(1);
	MDELAY(5);
	SET_RESET_PIN(0);
	MDELAY(20);
	SET_RESET_PIN(1);
	MDELAY(120);
 
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

LCM_DRIVER hct_nt36672a_dsi_vdo_fhp_hx_60_xld = 
{
	.name			= "hct_nt36672a_dsi_vdo_fhp_hx_60_xld",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,	
	.compare_id     = lcm_compare_id,	

#if (LCM_DSI_CMD_MODE)
    //.update         = lcm_update,
#endif	//wqtao
};

