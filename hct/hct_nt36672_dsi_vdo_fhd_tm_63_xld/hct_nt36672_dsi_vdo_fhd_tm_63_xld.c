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
#define FRAME_HEIGHT 										(2340)

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

#define dsi_set_cmdq_V3(para_tbl,size,force_update)		lcm_util.dsi_set_cmdq_V3(para_tbl,size,force_update)
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)						lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)			lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)						lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)			lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)
#define set_gpio_lcd_enn(cmd)                               	lcm_util.set_gpio_lcd_enn_bias(cmd)
#define set_gpio_tp_incell_rst(cmd)                         lcm_util.set_gpio_tp_incell_rst(cmd)

 struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};
extern int PMU_db_pos_neg_setting_delay(int ms);
extern int PMU_db_pos_neg_disable_delay(int ms);
extern int PMU_db_pos_neg_setting_delay_hct(int ms, int vol);

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
	{0xFF,1,{0x10}},
	{0xFB,1,{0x01}},
	{0x3B,5,{0x0a,0x0a,0x0a,0x0a,0x0a}},
	{0xFF,1,{0x20}},
	{0xFB,1,{0x01}},
	{0x44,1,{0x0A}},	//customer ID 		read DA				
	{0x0E,1,{0xBF}},//VGHO =10V 
	{0x0F,1,{0xA4}},	//VGLO=-7V								
	{0x06,1,{0xAE}},
	{0x07,1,{0x9E}},	 										
      {0x95,1,{0xEB}}, //D7 GVDDP=5.0V     AF 4.6  
      {0x96,1,{0xEB}}, //D7 GVDDN=-5.0V   AF 4.6
	{0x89,1,{0x12}}, //VCOM			
	{0x8A,1,{0x12}}, //VCOM	
	{0x6D,1,{0x44}}, //Power save 20171130 ISOPA_POS/NEG	
	{0x62,1,{0xB2}}, //YIF external SYNC change to internal clk num
	{0x78,1,{0x01}}, //re-scan: line num 	00h¡ª¡ª>1 line ,01h¡ª¡ª>2 line
//--Commnd 2 Pae 3-------- 	
	{0xFF,1,{0x23}},
	{0xFB,1,{0x01}},
	{0x07,1,{0x20}},//20h,FOSC=30.75MHZ
	{0x08,1,{0x04}},//PWMDIV[7:0]=01h
	{0x09,1,{0x00}},	//DUTY_COUNT[7:0]=04h, pwm frequency= FOCS/((256+DUTY_COUNT[7:0])*1*(PWMDIV[7:0]))
//--Commnd 2 Pae 4--------,1,{ 
	{0xFF,1,{0x24}},
	{0xFB,1,{0x01}},
	{0x00,1,{0x1C}},	//CGOUT HiZ
	{0x01,1,{0x1C}},//CGOUT HiZ
	{0x02,1,{0x1C}},//CGOUT HiZ
	{0x03,1,{0x1C}},	//CGOUT HiZ
	{0x04,1,{0x1C}},	//CGOUT HiZ
	{0x05,1,{0x1C}},	//CGOUT HiZ
	{0x06,1,{0x1C}},	//CGOUT HiZ
	{0x07,1,{0x1C}},	//CGOUT HiZ
	{0x08,1,{0x1C}},	//CGOUT HiZ
	{0x09,1,{0x10}},
	{0x0A,1,{0x12}},
	{0x0B,1,{0x14}},	
	{0x0C,1,{0x20}},
	{0x0D,1,{0x1E}},//YIF GRESET_R
	{0x0E,1,{0x0D}},//YIF GAS_R
	{0x0F,1,{0x06}},//YIF CKV4_R
	{0x10,1,{0x04}},//YIF	CKV2_R			
	{0x11,1,{0x03}},//YIF CKV1_R
	{0x12,1,{0x05}},//YIF CKV3_R
	{0x13,1,{0x09}},	//YIF U2D_R
	{0x14,1,{0x0A}},//YIF D2U_R
	{0x15,1,{0x01}},	//YIF	STV_R		
	{0x16,1,{0x1C}},//YIF
	{0x17,1,{0x1C}},	//CGOUT HiZ
	{0x18,1,{0x1C}},	//CGOUT HiZ
	{0x19,1,{0x1C}},	//CGOUT HiZ	
	{0x1A,1,{0x1C}},	//CGOUT HiZ
	{0x1B,1,{0x1C}},//CGOUT HiZ
	{0x1C,1,{0x1C}},//CGOUT HiZ
	{0x1D,1,{0x1C}},	//CGOUT HiZ
	{0x1E,1,{0x1C}},	//CGOUT HiZ
	{0x1F,1,{0x1C}},	//CGOUT HiZ	
	{0x20,1,{0x10}},	
	{0x21,1,{0x12}},	
	{0x22,1,{0x14}},	
	{0x23,1,{0x20}},
	{0x24,1,{0x1E}},//YIF GRESET_L	 
	{0x25,1,{0x0D}},	//YIF GAS_L	
	{0x26,1,{0x05}},	//YIF CKV3_L
	{0x27,1,{0x03}},	//YIF CKV1_L
	{0x28,1,{0x06}},		//YIF CKV4_L
	{0x29,1,{0x04}},		//YIF CKV2_	L
	{0x2A,1,{0x09}},		//YIF U2D_L
	{0x2B,1,{0x0A}},	 //YIF D2U_R
	{0x2D,1,{0x01}},	//YIF	STV_L
	{0x2F,1,{0x1C}},//YIF 
	{0x31,1,{0x06}},//YIF GCK_CTRLB_LAG 1-2
	{0x32,1,{0x0A}},//YIF GCK_CTRLB_LAG 2-1
	{0x33,1,{0x04}},//CKV rising
	{0x34,1,{0x02}},	//dummy CKV num				
	{0x35,1,{0x00}},	
	{0x37,1,{0x03}},//CKV START
	{0x38,1,{0x6C}},//YIF CKV END
	{0x39,1,{0x6C}},//YIF line N+1 CKV width
	{0x3D,1,{0x6C}},//YIF tp term 3 CKV width
	{0x3F,1,{0x6C}},//YIF line N CKV width
	{0x41,1,{0x06}}, //Forward CKV3-CKV4-CKV1-CKV2
	{0x42,1,{0x0A}}, // Backward CKV2-CKV1-CKV4-CKV3
	{0x4C,1,{0x1C}},	//CGOUT HiZ
	{0x4D,1,{0x1C}},	//CGOUT HiZ
	{0x60,1,{0x90}},//YIF
	{0x61,1,{0x24}}, 
	{0x72,1,{0x00}},//,//YIF
	{0x73,1,{0x00}}, // 00pixel inversion ,1F sub pixle inversion
	{0x74,1,{0x00}}, //YIF
	{0x75,1,{0x00}}, // 00pixel inversion ,1F sub pixle inversion
	{0x79,1,{0x04}},  //km 00->04
	{0x7C,1,{0x80}},  //km 
	{0x7A,1,{0x03}},//MUX rising position  08 RGBRGB  ,03 RGBBGR
	{0x7B,1,{0x9B}},//MUXW
	{0x7D,1,{0x08}},//MUX_GAP 08    //
	{0x80,1,{0x47}},	//source OP output hold time 43h->47h
	{0x81,1,{0x07}},	//source delay time 07h			
	{0x82,1,{0x13}},//11 RGBRGB ,13 RGBBGR
	{0x83,1,{0x22}},
	{0x84,1,{0x31}},//33 RGBRGB ,31 RGBBGR
	{0x85,1,{0x00}},
	{0x86,1,{0x00}},
	{0x87,1,{0x00}},
	{0x88,1,{0x13}},//11 RGBRGB ,13 RGBBGR
	{0x89,1,{0x22}},
	{0x8A,1,{0x31}},//33 RGBRGB ,31 RGBBGR
	{0x8B,1,{0x00}},
	{0x8C,1,{0x00}},
	{0x8D,1,{0x00}},
	{0x8E,1,{0xE4}},	// E0 RGBRGB ,E4 RGBBGR
 	{0x8F,1,{0x01}},// 00 RGBRGB ,01 RGBBGR
 	{0x90,1,{0x80}},// 00 RGBRGB ,80 RGBBGR	
	{0x92,1,{0x75}},//YIF RTNA
	{0xB4,1,{0x00}},//YIF APO U2D/D2U UD abnormal level set to L
	{0xC2,1,{0x8A}},//OPPO 6.3 Need for 45HZ
	{0xDC,1,{0x09}},
	{0xDD,1,{0x03}},	//yif STV rising edge position	
	{0xDE,1,{0x02}},//yif
	{0xDF,1,{0x00}},//yif STV START position
	{0xE0,1,{0x10}},//STV END position
	{0xEB,1,{0x03}},//yif Line N+1 mux start position
//--Commnd 2 Pae 5--------,1,
      	{0xFF,1,{0x25}},
      	{0xFB,1,{0x01}},
	{0x05,1,{0x00}},//Auto porch detect disable
      	{0x21,1,{0x1B}},//enter touch term mux width (Line N)
      	{0x22,1,{0x1B}},//exit touch term mux width (Line N+1)
      	{0x24,1,{0x75}},//YIF RTNA_Line_N
      	{0x25,1,{0x75}},//YIF RTNA_Line_N+1
	{0x30,1,{0x2F}}, //yif add GRESET setting
      	{0x38,1,{0x2F}},//VCOMSW Start position 
      	{0x40,1,{0xAA}},//YIF ADD GRESET setting
	{0x4C,1,{0xAA}}, //YIF VCOMSW pluse width
      	{0x66,1,{0xC8}},//yif U2D power on setting    
      	{0x67,1,{0x01}},//yif UD POWER OFF level
      	{0x68,1,{0x50}},//Power on CKH setting  
      	{0x69,1,{0x10}},//yif MUX POWER OFF setting
      	{0x6B,1,{0x00}}, 
	{0x71,1,{0x5D}}, //YIF:MUX POWER OFF setting  STV start active position  
      	{0x77,1,{0x62}},//20171212 62h
	{0x7D,1,{0x40}}, //YIF ADD CKV POWER ON setting
      	{0x7E,1,{0x1D}}, //YIF GCK_POFF/PON ACT    CKV start active position  
      	{0x84,1,{0x68}},	//YIF GAS power power on/off setting		
	{0x8D,1,{0x00}},//YIF GAS power off setting
	{0xBF,1,{0x00}},//YIF ADD CKV power on setting
	{0xC0,1,{0x4D}},//YIF GRESET POWER OFF setting
	{0xC1,1,{0x9A}},  //YIF GRESET POWER OFF setting
	{0xC2,1,{0xD2}}, //YIF VCOMSW POWER ON setting
      	{0xD9,1,{0x48}}, //MUX_EQ enable
      	{0xDA,1,{0x00}}, //MUX_EQ1
      	{0xDB,1,{0x32}}, //MUX_EQ2  32
      	{0xDC,1,{0x00}}, //MUX_EQ3	
//--Commnd 2 Toch page 1--------	
      	{0xFF,1,{0x26}}, 
      	{0xFB,1,{0x01}}, 
      	{0x06,1,{0xFF}},//TP vsync rising 20180202
      	{0x0C,1,{0x11}},//YIF set TP units
      	{0x0F,1,{0x09}},//set start position for TSVD
      	{0x10,1,{0x0A}},//set end position for TSVD
      	{0x12,1,{0x82}},//YIF set lins per unit(125)
      	{0x19,1,{0x0B}},//set TSHD period
      	{0x1A,1,{0x49}},//YIF set TSHD period
      	{0x1C,1,{0xAF}},//set MUX polsrity at TP term	FA:Set MUX H when TP term
      	{0x1D,1,{0x0A}},//set TSHD_NTV period
      	{0x1E,1,{0xCC}}, 	//YIF set TSHD_NTV period
      	{0x98,1,{0xF1}}, //TP vsync clock ratio 20180202	
	{0xA9,1,{0x12}}, //YIF
	{0xAA,1,{0x10}},  //YIF re-scan line2 setting
	{0xAE,1,{0x8A}}, //8Ah£ºre-scan 1st line:Line N-1 data,re-scan 2nd line:Line N data
//--Commnd 2 Toch page 2--------	
	{0xFF,1,{0x27}},
	{0xFB,1,{0x01}},
	{0x13,1,{0x00}},	//for audible noise solution 20180202	
	{0x1E,1,{0x25}},	//source pre-chang 
//--------Commad 3	--------	
	{0xFF,1,{0xF0}},
	{0xFB,1,{0x01}},
	{0xA2,1,{0x00}},		//Data path fine tune

///CMD2_Page0
{0xFF,1,{0x20}},
{0xFB,1,{0x01}},
//Gamma2.5
//R(+)
{0xB0,16,{0x00,0x00,0x00,0x54,0x00,0x95,0x00,0xC3,0x00,0xE4,0x01,0x00,0x01,0x18,0x01,0x29}},
{0xB1,16,{0x01,0x3E,0x01,0x73,0x01,0x98,0x01,0xD3,0x01,0xFE,0x02,0x3E,0x02,0x72,0x02,0x74}},
{0xB2,16,{0x02,0xA6,0x02,0xE0,0x03,0x06,0x03,0x35,0x03,0x53,0x03,0x7C,0x03,0x88,0x03,0x93}},
{0xB3,14,{0x03,0x9F,0x03,0xAF,0x03,0xB6,0x03,0xBA,0x03,0xBB,0x03,0xFF,0x00,0x00}},
//G(+)
{0xB4,16,{0x00,0x00,0x00,0x54,0x00,0x99,0x00,0xC6,0x00,0xE8,0x01,0x03,0x01,0x19,0x01,0x2D}},
{0xB5,16,{0x01,0x3D,0x01,0x72,0x01,0x9A,0x01,0xD3,0x01,0xFE,0x02,0x3E,0x02,0x73,0x02,0x74}},
{0xB6,16,{0x02,0xA7,0x02,0xE0,0x03,0x07,0x03,0x36,0x03,0x54,0x03,0x7C,0x03,0x89,0x03,0x97}},
{0xB7,14,{0x03,0xA4,0x03,0xAF,0x03,0xB7,0x03,0xBB,0x03,0xBD,0x03,0xFF,0x00,0x00}},
//B(+)
{0xB8,16,{0x00,0x00,0x00,0x6F,0x00,0xAF,0x00,0xD8,0x00,0xF9,0x01,0x11,0x01,0x27,0x01,0x3A}},
{0xB9,16,{0x01,0x4A,0x01,0x7E,0x01,0xA4,0x01,0xDC,0x02,0x06,0x02,0x44,0x02,0x78,0x02,0x79}},
{0xBA,16,{0x02,0xAB,0x02,0xE3,0x03,0x0A,0x03,0x39,0x03,0x59,0x03,0x8E,0x03,0xA1,0x03,0xB3}},
{0xBB,14,{0x03,0xC3,0x03,0xC4,0x03,0xC5,0x03,0xC6,0x03,0xC7,0x03,0xFF,0x00,0x00}},
//CMD2_Page1
{0xFF,1,{0x21}},
{0xFB,1,{0x01}},
//R(-)
{0xB0,16,{0x00,0x00,0x00,0x54,0x00,0x95,0x00,0xC3,0x00,0xE4,0x01,0x00,0x01,0x18,0x01,0x29}},
{0xB1,16,{0x01,0x3E,0x01,0x73,0x01,0x98,0x01,0xD3,0x01,0xFE,0x02,0x3E,0x02,0x72,0x02,0x74}},
{0xB2,16,{0x02,0xA6,0x02,0xE0,0x03,0x06,0x03,0x35,0x03,0x53,0x03,0x7C,0x03,0x88,0x03,0x93}},
{0xB3,14,{0x03,0x9F,0x03,0xAF,0x03,0xB6,0x03,0xBA,0x03,0xBB,0x03,0xFF,0x00,0x00}},
//G(-)
{0xB4,16,{0x00,0x00,0x00,0x54,0x00,0x99,0x00,0xC6,0x00,0xE8,0x01,0x03,0x01,0x19,0x01,0x2D}},
{0xB5,16,{0x01,0x3D,0x01,0x72,0x01,0x9A,0x01,0xD3,0x01,0xFE,0x02,0x3E,0x02,0x73,0x02,0x74}},
{0xB6,16,{0x02,0xA7,0x02,0xE0,0x03,0x07,0x03,0x36,0x03,0x54,0x03,0x7C,0x03,0x89,0x03,0x97}},
{0xB7,14,{0x03,0xA4,0x03,0xAF,0x03,0xB7,0x03,0xBB,0x03,0xBD,0x03,0xFF,0x00,0x00}},
//B(-)
{0xB8,16,{0x00,0x00,0x00,0x6F,0x00,0xAF,0x00,0xD8,0x00,0xF9,0x01,0x11,0x01,0x27,0x01,0x3A}},
{0xB9,16,{0x01,0x4A,0x01,0x7E,0x01,0xA4,0x01,0xDC,0x02,0x06,0x02,0x44,0x02,0x78,0x02,0x79}},
{0xBA,16,{0x02,0xAB,0x02,0xE3,0x03,0x0A,0x03,0x39,0x03,0x59,0x03,0x8E,0x03,0xA1,0x03,0xB3}},
{0xBB,14,{0x03,0xC3,0x03,0xC4,0x03,0xC5,0x03,0xC6,0x03,0xC7,0x03,0xFF,0x00,0x00}},
   //al amma Seting------Test amma
	 
	 
      
      {0xFF,1,{0x10}},
      {0xFB,1,{0x01}},
     
      {0x11,1,{0x00}}, 
      {REGFLAG_DELAY,120,{}},	                                              
      {0x29,1,{0x00}},            
      {REGFLAG_DELAY,50,{}},      
    
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
//	{0x01, 1, {0x00}},
//	{REGFLAG_DELAY, 50, {}},
	
	{0x28, 1, {0x00}},
	{REGFLAG_DELAY, 50, {}},

	// Sleep Mode On
	{0x10, 1, {0x00}},
	{REGFLAG_DELAY, 120, {}},

	// Deep Sleep Mode On
	{0x4f, 1, {0x01}},
	{REGFLAG_DELAY, 50, {}},

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

    params->physical_width = 70;
    params->physical_height = 144;
 
    params->dsi.vertical_sync_active                =  2;
    params->dsi.vertical_backporch                    = 8;//16 25 30 35 12 8
    params->dsi.vertical_frontporch                    = 10;
    params->dsi.vertical_active_line                = FRAME_HEIGHT; 

 

	params->dsi.horizontal_sync_active = 10; //50--40
	params->dsi.horizontal_backporch = 100;
	params->dsi.horizontal_frontporch = 100;//  60-->75
    params->dsi.horizontal_active_pixel                = FRAME_WIDTH;

	params->dsi.PLL_CLOCK = 600;

	//params->dsi.cont_clock = 1;
	//params->dsi.clk_lp_per_line_enable = 1;
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
    set_gpio_tp_incell_rst(0);
	MDELAY(10);

	
	set_gpio_lcd_enn(1);  //1.8
   	MDELAY(10);
	set_gpio_tp_incell_rst(1);
	MDELAY(15);
	PMU_db_pos_neg_setting_delay_hct(10, 58);
	MDELAY(5);

	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(20);
	SET_RESET_PIN(1);

	//MDELAY(15);
	//set_gpio_tp_incell_rst(1);
	MDELAY(120);

	push_table(lcm_initialization_setting,sizeof(lcm_initialization_setting) /sizeof(struct LCM_setting_table), 1);
}

static void lcm_suspend(void)
{

//	unsigned int data_array[16];
//
//	data_array[0]=0x00280500;
//	dsi_set_cmdq(data_array, 1, 1);
//	MDELAY(10);
//
//	data_array[0]=0x00100500;
//	dsi_set_cmdq(data_array, 1, 1);
//	MDELAY(120);

	push_table(lcm_sleep_in_setting,sizeof(lcm_sleep_in_setting) /sizeof(struct LCM_setting_table), 1);
//power off
	//set_gpio_tp_incell_rst(0);
	//MDELAY(15);
  	//SET_RESET_PIN(0);     //reset down
	MDELAY(10);
	PMU_db_pos_neg_disable_delay(10);
	MDELAY(10);
	//set_gpio_lcd_enn(0);  //1.8

	MDELAY(10);
}

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

	set_gpio_lcd_enn(1);  //1.8
    	MDELAY(10);
	PMU_db_pos_neg_setting_delay_hct(10, 58);
	MDELAY(5);

	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(20);
	SET_RESET_PIN(1);
	MDELAY(15);
    	set_gpio_tp_incell_rst(1);
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

LCM_DRIVER hct_nt36672_dsi_vdo_fhd_tm_63_xld = 
{
	.name			= "hct_nt36672_dsi_vdo_fhd_tm_63_xld",
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

