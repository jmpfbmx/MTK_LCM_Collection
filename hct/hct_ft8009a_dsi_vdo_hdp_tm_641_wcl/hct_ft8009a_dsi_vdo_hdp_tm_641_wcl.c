
/*****************************************************************************
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

#define FRAME_WIDTH  										(720)
#define FRAME_HEIGHT 										(1560)

#define LCM_ID_FT8009A                						(0x8009)

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
#define hct_lcm_power_settings(mode, value, mdelay1, mdelay2) 					lcm_util.hct_lcm_power_settings(mode, value, mdelay1, mdelay2)
#define set_gpio_tp_incell_rst(cmd)                         lcm_util.set_gpio_tp_incell_rst(cmd)

 struct LCM_setting_table {
	unsigned int cmd;
    unsigned char count;
    unsigned char para_list[64];
};

#if defined(BUILD_LK)
#else
extern int fts_write_reg(u8 addr, u8 value);
#endif
extern int RT5081_db_pos_neg_setting(void);
extern int RT5081_db_pos_neg_disable(void);
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
//============CMD WR enable=============
{0x00,1,{0x00}},
{0xFF,3,{0x80,0x09,0x01}},
{0x00,1,{0x80}},
{0xFF,2,{0x80,0x09}},
//======voltage set==========
{0x00,1,{0x93}}, //VGH_N 17V
{0xC5,1,{0x6E}}, 
{0x00,1,{0x97}}, //VGH_I 17V
{0xC5,1,{0x6E}},
{0x00,1,{0x9E}},
{0xC5,1,{0x05}}, //2AVDD-AVEE *3
{0x00,1,{0x9A}}, //VGL_N -11V 
{0xC5,1,{0x41}}, //AVEE-AVDD *2
{0x00,1,{0x9C}}, //VGL_I -11V
{0xC5,1,{0x41}}, //AVEE-AVDD *2
//SD Ibias
{0x00,1,{0x80}},
{0xA4,1,{0x2C}},
{0x00,1,{0xB3}},
{0xC5,1,{0xC7}}, //Set reg_hvreg_en_vglo1_sink=0
{0x00,1,{0xB6}},
{0xC5,2,{0x5A,0x5A}},  //VGHO1_N_I 15V
{0x00,1,{0xB8}},
{0xC5,2,{0x2D,0x2D}}, //VGLO1_N_I -10V
{0x00,1,{0x00}},
{0xD8,2,{0x31,0x31}}, //GVDDP/N 5.3V
{0x00,1,{0x01}},
{0xD9,2,{0x60,0x60}}, //VCOM -2V
{0x00,1,{0x82}},
{0xC5,1,{0x15}}, //LVD
{0x00,1,{0x83}},
{0xC5,1,{0x07}}, //LVD Enable
//==========gamma==============
{0x00,1,{0x00}},
{0xE1,16,{0x2D,0x32,0x3B,0x45,0x4B,0x53,0x5E,0x68,0x67,0x71,0x70,0x83,0x82,0x73,0x76,0x6C}},
{0x00,1,{0x10}},
{0xE1,8,{0x65,0x5A,0x49,0x3D,0x33,0x20,0x11,0x0F}},
{0x00,1,{0x00}},
{0xE2,16,{0x2D,0x32,0x3B,0x45,0x4B,0x53,0x5E,0x68,0x67,0x71,0x70,0x83,0x82,0x73,0x76,0x6C}},
{0x00,1,{0x10}},
{0xE2,8,{0x65,0x5A,0x49,0x3D,0x33,0x20,0x11,0x0F}},
//==============GOA setting==================================================
//===========//Norm Power-OFF LVD====================
{0x00,1,{0x80}},
{0xCB,7,{0xC1,0xC1,0x01,0x01,0x01,0x01,0xC1}},
{0x00,1,{0x87}},
{0xCB,1,{0xC1}},
{0x00,1,{0x88}},
{0xCB,8,{0xC1,0xC1,0x01,0x01,0x01,0x01,0x01,0x01}},
{0x00,1,{0x90}},
{0xCB,7,{0x01,0x01,0x01,0x01,0xC1,0xC1,0x01}},
{0x00,1,{0x97}},
{0xCB,2,{0x01,0x01}},
//=========POWROF BLANK===================
{0x00,1,{0xB9}},
{0xCB,7,{0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
//==============U2D==================
{0x00,1,{0x80}},
{0xCC,8,{0x3F,0x3F,0x3F,0x24,0x3F,0x00,0x1A,0x3F}},
{0x00,1,{0x88}},
{0xCC,8,{0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x10,0x11}},
{0x00,1,{0x90}},
{0xCC,6,{0x08,0x3F,0x24,0x00,0x22,0x23}},
{0x00,1,{0x80}},
{0xCD,8,{0x3F,0x3F,0x3F,0x25,0x3F,0x01,0x1B,0x3F}},
{0x00,1,{0x88}},
{0xCD,8,{0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19}},
{0x00,1,{0x90}},
{0xCD,6,{0x09,0x3F,0x25,0x01,0x22,0x23}},	
//=============D2U===================
{0x00,1,{0xA0}},
{0xCC,8,{0x3F,0x3F,0x3F,0x25,0x3F,0x01,0x09,0x3F}},
{0x00,1,{0xA8}},
{0xCC,8,{0x19,0x18,0x17,0x16,0x15,0x14,0x13,0x12}},
{0x00,1,{0xB0}},
{0xCC,6,{0x1B,0x3F,0x25,0x01,0x22,0x23}},
{0x00,1,{0xA0}},
{0xCD,8,{0x3F,0x3F,0x3F,0x24,0x3F,0x00,0x08,0x3F}},
{0x00,1,{0xA8}},
{0xCD,8,{0x11,0x10,0x0F,0x0E,0x0D,0x0C,0x0B,0x0A}},
{0x00,1,{0xB0}},
{0xCD,6,{0x1A,0x3F,0x24,0x00,0x22,0x23}},	
//=============GOFF or RST==============
{0x00,1,{0xE7}},
{0xCB,1,{0x01}},
//============RST1===============	
{0x00,1,{0xA0}},
{0xC3,4,{0x0B,0x00,0x19,0x11}}, //20190225
//=================RST2=====================	
{0x00,1,{0xA8}},
{0xC3,4,{0x0B,0x00,0x0D,0x05}},
//============VST=================
{0x00,1,{0x98}},
{0xC2,2,{0x90,0x07}},
//==============VST==================	
{0x00,1,{0x9C}},
{0xC2,2,{0x84,0x07}},
//==========VEND1================
{0x00,1,{0xA0}},
{0xC2,3,{0x84,0x07,0x20}},

//============VEND2======================
{0x00,1,{0xA4}},
{0xC2,3,{0x08,0x07,0x20}},
//============CLKA==========================	
{0x00,1,{0xC0}},
{0xC2,7,{0x88,0x88,0x02,0x07,0x0F,0x50,0x0F}},
{0x00,1,{0xC7}},
{0xC2,7,{0x87,0x88,0x02,0x07,0x0F,0x50,0x0F}},
{0x00,1,{0xD0}},
{0xC2,7,{0x86,0x88,0x02,0x07,0x0F,0x50,0x0F}},
{0x00,1,{0xD7}},
{0xC2,7,{0x85,0x88,0x02,0x07,0x0F,0x50,0x0F}},	
//=============CLKB===========================
{0x00,1,{0xE0}},
{0xC2,7,{0x00,0x00,0x02,0x07,0x0F,0x50,0x0F}},
{0x00,1,{0xE7}},
{0xC2,7,{0x01,0x00,0x02,0x07,0x0F,0x50,0x0F}},
{0x00,1,{0xF0}},
{0xC2,7,{0x02,0x00,0x02,0x07,0x0F,0x50,0x0F}},
{0x00,1,{0xF7}},
{0xC2,7,{0x03,0x00,0x02,0x07,0x0F,0x50,0x0F}},
//=================CLKC==============================
{0x00,1,{0x80}},
{0xC3,7,{0x04,0x00,0x02,0x07,0x0F,0x50,0x0F}},
{0x00,1,{0x87}},
{0xC3,7,{0x05,0x00,0x02,0x07,0x0F,0x50,0x0F}},
{0x00,1,{0x90}},
{0xC3,7,{0x06,0x00,0x02,0x07,0x0F,0x50,0x0F}},
{0x00,1,{0x97}},
{0xC3,7,{0x07,0x00,0x02,0x07,0x0F,0x50,0x0F}},
//==================CLKD==============================
{0x00,1,{0xC0}},
{0xCD,7,{0x0c,0x08,0x02,0x07,0x0F,0x50,0x0F}},
{0x00,1,{0xC7}},
{0xCD,7,{0x0d,0x08,0x02,0x07,0x0F,0x50,0x0F}},

{0x00,1,{0xD0}},
{0xCD,7,{0x0e,0x08,0x02,0x07,0x0F,0x50,0x0F}},
{0x00,1,{0xD7}},
{0xCD,7,{0x0f,0x08,0x02,0x07,0x0F,0x50,0x0F}},
//20190304
{0x00,1,{0xC7}},
{0xC3,6,{0x00,0x03,0x00,0x00,0x03,0x00}},
{0x00,1,{0xC2}},
{0xC3,1,{0x02}},
//===============GOFF1=====================
{0x00,1,{0xE0}},
{0xC3,2,{0x36,0x01}},
//================GOFF2===============
{0x00,1,{0xE4}},
{0xC3,2,{0x36,0x02}},
//===============Power on====================
{0x00,1,{0x99}},
{0xCB,7,{0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
{0x00,1,{0xA0}},
{0xCB,1,{0x00}},
{0x00,1,{0xA1}},
{0xCB,8,{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
{0x00,1,{0xA9}},
{0xCB,7,{0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
{0x00,1,{0xB0}},
{0xCB,2,{0x00,0x00}},
//==============SKIP========================
{0x00,1,{0xB2}},
{0xCB,7,{0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
//======Long V mode=====================
//FIFO Mode
{0x00,1,{0x00}},
{0x1C,1,{0x00}},	
//reg_tcon_video
{0x00,1,{0xA0}},
{0xC1,1,{0xE0}},
{0x00,1,{0xA8}},
{0xC0,1,{0x04}},
//
{0x00,1,{0x80}},
{0xC0,6,{0x00,0xAE,0x00,0xF0,0x00,0x17}},
//reg_tcon_tp_rtn_det_en
{0x00,1,{0xE0}},
{0xCE,1,{0x00}},
//
{0x00,1,{0xF0}},
{0xCE,1,{0x00}},
//RTN = SWT + dri + nop
{0x00,1,{0x8A}},
{0xC0,2,{0x1C,0x01}},
//LH Disable
{0x00,1,{0x80}},
{0xCE,1,{0x00}},
//VB Enable
{0x00,1,{0xD0}},
{0xCE,1,{0x01}},
//VB_TERM1B_HI
{0x00,1,{0xD5}},
{0xCE,1,{0x00}},
//VB_TERM1B_LO
{0x00,1,{0xD6}},
{0xCE,1,{0xD9}},
//======Source set=====================
//========LR shift==========
{0x00,1,{0xAF}},
{0xC4,1,{0x24}},
{0x00,1,{0xA3}},
{0xF3,1,{0x80}},
{0x00,1,{0xE6}},
{0xC0,1,{0x40}},
{0x00,1,{0xB1}},
{0xC4,4,{0x15,0x15,0x15,0x15}},
//===========SW_PANSET============
//720
{0x00,1,{0xA1}},
{0xB3,2,{0x02,0xD0}},
//1560
{0x00,1,{0xA3}},
{0xB3,2,{0x06,0x18}},
{0x00,1,{0xA6}},
{0xB3,1,{0x12}},
//----------------------LCD initial code End----------------------//
{0x11,0,{}},
{REGFLAG_DELAY,120, {}},
{0x29,0,{}},
{0x35,1,{0x00}},
{REGFLAG_DELAY,250, {}}
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

static struct LCM_setting_table lcm_sleep_in_setting[] = {
	// Display off sequence
	
	{0x28, 1, {0x00}},
	{REGFLAG_DELAY, 60, {}},

	// Sleep Mode On
	{0x10, 1, {0x00}},
	{REGFLAG_DELAY, 60, {}},
	
	//deepsleep
 	{0x00,1,{0x00}},
	{0xFF,3,{0x87,0x19,0x01}},
	{0x00,1,{0x80}},
	{0xFF,2,{0x87,0x19}},
	{0x00,1,{0x00}},
 	{0xF7,4,{0x5A,0xA5,0x95,0x27}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};



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

    params->physical_width = 68;
    params->physical_height = 148;
 
    params->dsi.vertical_sync_active                =  1;//2

    params->dsi.vertical_backporch                    =23 ;//16 25 30 35 12 8 8

    params->dsi.vertical_frontporch                    = 240;

    params->dsi.vertical_active_line                = FRAME_HEIGHT; 

 

    params->dsi.horizontal_sync_active                =16 ;//56 30

    params->dsi.horizontal_backporch                =16; //104 85

    params->dsi.horizontal_frontporch                =16 ;//20 20

    params->dsi.horizontal_active_pixel                = FRAME_WIDTH;

 

 

    params->dsi.PLL_CLOCK=270;//255

	params->dsi.cont_clock = 1;
	params->dsi.clk_lp_per_line_enable = 0;
	params->dsi.ssc_disable = 1;	
	
	params->dsi.esd_check_enable = 1;
	params->dsi.customization_esd_check_enable = 1;
    params->dsi.lcm_esd_check_table[0].cmd          = 0x0a;
    params->dsi.lcm_esd_check_table[0].count        = 1;
    params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9c;


}

static void lcm_init(void)
{
	SET_RESET_PIN(0);
   	MDELAY(10);

	set_gpio_lcd_enn(1);  //1.8
    	MDELAY(10);
	PMU_db_pos_neg_setting_delay_hct(10, 58);
   	MDELAY(5);

    	SET_RESET_PIN(1);
    	MDELAY(5);
    	SET_RESET_PIN(0);
    	MDELAY(5);
    	SET_RESET_PIN(1);
    	MDELAY(120);
	push_table(lcm_initialization_setting,sizeof(lcm_initialization_setting) /sizeof(struct LCM_setting_table), 1);
}

static void lcm_suspend(void)
{

	push_table(lcm_sleep_in_setting,sizeof(lcm_sleep_in_setting) /sizeof(struct LCM_setting_table), 1);

	PMU_db_pos_neg_disable_delay(10);
	MDELAY(5);
	SET_RESET_PIN(0);
	MDELAY(5);
	//set_gpio_lcd_enn(0);
    	MDELAY(10);
}

static unsigned int lcm_compare_id(void);
static void lcm_resume(void)
{
	lcm_init();
}

extern int IMM_GetOneChannelValue(int dwChannel, int data[4], int* rawdata);
#define AUX_IN0_LCD_ID  2
#define ADC_MIN_VALUE   0x600
#define ADC_MAX_VALUE   0xCD0

static unsigned int lcm_compare_id(void)
{

	int array[4];
     	char buffer[5];
     	char id_high=0;
     	char id_low=0;
     	int id=0;

	int adcdata[4] = {0};
	int rawdata = 0;
	int ret = 0;

	set_gpio_lcd_enn(1);  //1.8
   	MDELAY(10);
	PMU_db_pos_neg_setting_delay_hct(10, 58);
 
	SET_RESET_PIN(1);
	MDELAY(5);
	SET_RESET_PIN(0);
	MDELAY(20);
	SET_RESET_PIN(1);
	MDELAY(120);

	ret = IMM_GetOneChannelValue(AUX_IN0_LCD_ID, adcdata, &rawdata);
	#if defined(BUILD_LK)
	printf("hct_ft8009a_dsi_vdo_hdp_tm_641_wcl adc = %x adcdata= %x %x, ret=%d, ADC_Min_VALUE=%x\r\n", rawdata, adcdata[0], adcdata[1],ret,ADC_MIN_VALUE);
	#else
	printk("hct_ft8009a_dsi_vdo_hdp_tm_641_wcl adc = %x adcdata= %x %x, ret=%d, ADC_Min_VALUE=%x\r\n", rawdata, adcdata[0], adcdata[1],ret,ADC_MIN_VALUE);
	#endif
	if(rawdata > ADC_MAX_VALUE)
	{ 
	     array[0]=0x00043700;
	     dsi_set_cmdq(array, 1, 1);
	 
	     read_reg_v2(0xA1, buffer,4);
	     id_high = buffer[2]; ///////////////////////0x80

	     id_low = buffer[3]; ////////////////////////0x09
	     id =(id_high << 8) | id_low;
	 
	     #if defined(BUILD_LK)
	     printf("FT8009a_wcl compare-LK:0x%02x,0x%02x,0x%02x\n", id_high, id_low, id);
	     #else
	     printk("FT8009a_wcl compare:0x%02x,0x%02x,0x%02x,\n", id_high, id_low, id);
	     #endif
	 
	     return (id == LCM_ID_FT8009A)?1:0;
	}else{
		return 0;
	}
}

LCM_DRIVER hct_ft8009a_dsi_vdo_hdp_tm_641_wcl = 
{
	.name			= "hct_ft8009a_dsi_vdo_hdp_tm_641_wcl",
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

