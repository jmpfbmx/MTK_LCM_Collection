#ifndef BUILD_LK
#include <linux/string.h>
#include <linux/kernel.h> //added by leechee 02172016
#endif

#include "lcm_drv.h"

#ifdef BUILD_LK
#include <platform/upmu_common.h>
#include <platform/upmu_hw.h>
#include <platform/mt_gpio.h>
#include <platform/mt_i2c.h>
#include <platform/mt_pmic.h>
#include <string.h>
#else
#include <mach/gpio_const.h>
#include <mt-plat/upmu_common.h>
#include <mach/upmu_sw.h>
#include <mach/upmu_hw.h>
#include <mt-plat/mt_gpio.h>
#endif
/*#include <cust_gpio_usage.h>*/
#define I2C_I2C_LCD_BIAS_CHANNEL 1

/*PWR ENABLE PINS CONFIG*/
#define PWR_LCM_5V_ENP (GPIO58 | 0x80000000)
#define PWR_LCM_5V_ENN (GPIO89 | 0x80000000)
#define PWR_LCM_1V8_EN (GPIO21 | 0x80000000)
#define PWR_LCM_RST_EN (GPIO158 | 0x80000000)

#define GPIO_LCD_ID0 (GPIO56 | 0x80000000)
#define GPIO_LCD_ID1 (GPIO57 | 0x80000000)



static const unsigned int BL_MIN_LEVEL = 20;
static LCM_UTIL_FUNCS lcm_util;

/*add by wuyujing,support for cacb,start*/
static unsigned int cabc_level = CABC_OFF;
static unsigned int lcm_isok = 0;
/*add by wuyujing,support for cacb,end*/

#define SET_RESET_PIN(v) (lcm_util.set_reset_pin((v)))
#define MDELAY(n) (lcm_util.mdelay(n))
#define UDELAY(n) (lcm_util.udelay(n))
#define RAR(n) (lcm_util.rar(n))
#define dsi_set_cmdq_V22(cmdq, cmd, count, ppara, force_update) \
    lcm_util.dsi_set_cmdq_V22(cmdq, cmd, count, ppara, force_update)

/* --------------------------------------------------------------------------- */
/* Local Functions */
/* --------------------------------------------------------------------------- */

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update) \
	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update) \
	lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define dsi_set_null(cmd, count, ppara, force_update) \
	lcm_util.dsi_set_null(cmd, count, ppara, force_update)
#define wrtie_cmd(cmd) \
	lcm_util.dsi_write_cmd(cmd)
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
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#endif
/*****************************************************************************
 * Define
 *****************************************************************************/


// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  (720)
#define FRAME_HEIGHT (1280)

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))

#define REGFLAG_DELAY             							0XFE
#define REGFLAG_END_OF_TABLE      						0xFD	//0x00   // END OF REGISTERS MARKER
#define LCM_ID_DJ_ILI9881C 								0x55


//unsigned int lcm_compare_id(void);
// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)									lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)				lcm_util.dsi_write_regs(addr, pdata, byte_nums)
/*@///{:LAVA marked by leechee 02172016*/
//#define read_reg(cmd)											lcm_util.dsi_dcs_read_lcm_reg(cmd)
/*@///}*/
#define read_reg_v2(cmd, buffer, buffer_size)   				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)



//#define LCM_DSI_CMD_MODE                       0

struct LCM_setting_table {
	unsigned cmd;
	unsigned char count;
	unsigned char para_list[64];
};

/*****************
  lcm ۾ R61318
 ******************/
static struct LCM_setting_table lcm_initialization_setting[] = {
	{0xFF,3,{0x98,0x81,0x03}},
	//GIP_1
	{0x01,1,{0x00}},
	{0x02,1,{0x00}},
	{0x03,1,{0x53}},
	{0x04,1,{0x53}},
	{0x05,1,{0x13}},
	{0x06,1,{0x04}},
	{0x07,1,{0x02}},
	{0x08,1,{0x02}},
	{0x09,1,{0x00}},
	{0x0a,1,{0x00}},
	{0x0b,1,{0x00}},
	{0x0c,1,{0x00}},
	{0x0d,1,{0x00}},
	{0x0e,1,{0x00}},
	{0x0f,1,{0x00}},
	{0x10,1,{0x00}},
	{0x11,1,{0x00}},
	{0x12,1,{0x00}},
	{0x13,1,{0x00}},
	{0x14,1,{0x00}},
	{0x15,1,{0x00}},
	{0x16,1,{0x00}},
	{0x17,1,{0x00}},
	{0x18,1,{0x00}},
	{0x19,1,{0x00}},
	{0x1a,1,{0x00}},
	{0x1b,1,{0x00}},
	{0x1c,1,{0x00}},
	{0x1d,1,{0x00}},
	{0x1e,1,{0xC0}},
	{0x1f,1,{0x80}},
	{0x20,1,{0x02}},
	{0x21,1,{0x09}},
	{0x22,1,{0x00}},
	{0x23,1,{0x00}},
	{0x24,1,{0x00}},
	{0x25,1,{0x00}},
	{0x26,1,{0x00}},
	{0x27,1,{0x00}},
	{0x28,1,{0x55}},
	{0x29,1,{0x03}},
	{0x2a,1,{0x00}},
	{0x2b,1,{0x00}},
	{0x2c,1,{0x00}},
	{0x2d,1,{0x00}},
	{0x2e,1,{0x00}},
	{0x2f,1,{0x00}},
	{0x30,1,{0x00}},
	{0x31,1,{0x00}},
	{0x32,1,{0x00}},
	{0x33,1,{0x00}},
	{0x34,1,{0x03}},
	{0x35,1,{0x00}},
	{0x36,1,{0x05}},
	{0x37,1,{0x00}},
	{0x38,1,{0x3C}},
	{0x39,1,{0x00}},
	{0x3a,1,{0x00}},
	{0x3b,1,{0x00}},
	{0x3c,1,{0x00}},
	{0x3d,1,{0x00}},
	{0x3e,1,{0x00}},
	{0x3f,1,{0x00}},
	{0x40,1,{0x00}},
	{0x41,1,{0x00}},
	{0x42,1,{0x00}},
	{0x43,1,{0x00}},
	{0x44,1,{0x00}},
	//GIP_2
	{0x50,1,{0x01}},
	{0x51,1,{0x23}},
	{0x52,1,{0x45}},
	{0x53,1,{0x67}},
	{0x54,1,{0x89}},
	{0x55,1,{0xab}},
	{0x56,1,{0x01}},
	{0x57,1,{0x23}},
	{0x58,1,{0x45}},
	{0x59,1,{0x67}},
	{0x5a,1,{0x89}},
	{0x5b,1,{0xab}},
	{0x5c,1,{0xcd}},
	{0x5d,1,{0xef}},

	//GIP_3
	{0x5e,1,{0x01}},
	{0x5f,1,{0x14}},
	{0x60,1,{0x15}},
	{0x61,1,{0x0C}},
	{0x62,1,{0x0D}},
	{0x63,1,{0x0E}},
	{0x64,1,{0x0F}},
	{0x65,1,{0x10}},
	{0x66,1,{0x11}},
	{0x67,1,{0x08}},
	{0x68,1,{0x02}},
	{0x69,1,{0x0A}},
	{0x6a,1,{0x02}},
	{0x6b,1,{0x02}},
	{0x6c,1,{0x02}},
	{0x6d,1,{0x02}},
	{0x6e,1,{0x02}},
	{0x6f,1,{0x02}},
	{0x70,1,{0x02}},
	{0x71,1,{0x02}},
	{0x72,1,{0x06}},
	{0x73,1,{0x02}},
	{0x74,1,{0x02}},
	{0x75,1,{0x14}},
	{0x76,1,{0x15}},
	{0x77,1,{0x0F}},
	{0x78,1,{0x0E}},
	{0x79,1,{0x0D}},
	{0x7a,1,{0x0C}},
	{0x7b,1,{0x11}},
	{0x7c,1,{0x10}},
	{0x7d,1,{0x06}},
	{0x7e,1,{0x02}},
	{0x7f,1,{0x0A}},
	{0x80,1,{0x02}},
	{0x81,1,{0x02}},
	{0x82,1,{0x02}},
	{0x83,1,{0x02}},
	{0x84,1,{0x02}},
	{0x85,1,{0x02}},
	{0x86,1,{0x02}},
	{0x87,1,{0x02}},
	{0x88,1,{0x08}},
	{0x89,1,{0x02}},
	{0x8A,1,{0x02}},
	//CMD_Page 4}},
	{0xFF,3,{0x98,0x81,0x04}},
	{0x00,1,{0x80}},                //80=4lane
	{0x6C,1,{0x15}},                //Set VCORE voltage =1.5V
	{0x6E,1,{0x2F}},              //di_pwr_reg=0 for power mode
	{0x6F,1,{0x55}},                // reg vcl + pumping ratio 
	{0x3A,1,{0xA4}},                //POWER SAVING
	{0x8D,1,{0x1F}},              //VGL clamp -10
	{0x87,1,{0xBA}},               //ESD
	{0x26,1,{0x76}},            
	{0xB2,1,{0xD1}},
	{0x88,1,{0x0B}},
	//-----bist mode---------------------}},
	//{0x2F,1,{0x01}},
	//{0x2D,1,{0x01}},
	//{0x2E,1,{0x00}},
	//{0x22,1,{0xC3}},
	//-----------------------}},
	//CMD_Page 1}},
	{0xFF,3,{0x98,0x81,0x01}},
	{0x22,1,{0x09}},               //BGR,1,{0x SS}},
	{0x31,1,{0x00}},               //column inversion
	{0x53,1,{0x8A}},               //VCOM1
	{0x55,1,{0x88}},              //VCOM2
	{0x50,1,{0xa6}},               // VREG1OUT=4.7V
	{0x51,1,{0xa6}},               // VREG2OUT=-4.7V
	{0x60,1,{0x2B}},               //SDT
	{0xA0,1,{0x08}},               //VP255 Gamma P
	{0xA1,1,{0x1E}},               //VP251
	{0xA2,1,{0x2C}},               //VP247
	{0xA3,1,{0x15}},               //VP243
	{0xA4,1,{0x18}},               //VP239
	{0xA5,1,{0x2A}},               //VP231
	{0xA6,1,{0x1F}},               //VP219
	{0xA7,1,{0x1F}},               //VP203
	{0xA8,1,{0x85}},               //VP175
	{0xA9,1,{0x1C}},               //VP144
	{0xAA,1,{0x2A}},               //VP111
	{0xAB,1,{0x72}},              //VP80
	{0xAC,1,{0x1A}},              //VP52
	{0xAD,1,{0x18}},              //VP36
	{0xAE,1,{0x4C}},              //VP24
	{0xAF,1,{0x20}},              //VP16
	{0xB0,1,{0x26}},               //VP12
	{0xB1,1,{0x4A}},               //VP8
	{0xB2,1,{0x57}},              //VP4
	{0xB3,1,{0x2C}},              //VP0
	{0xC0,1,{0x08}},              //VN255 GAMMA N
	{0xC1,1,{0x1B}},              //VN251
	{0xC2,1,{0x27}},              //VN247
	{0xC3,1,{0x12}},              //VN243
	{0xC4,1,{0x14}},              //VN239
	{0xC5,1,{0x25}},              //VN231
	{0xC6,1,{0x1A}},              //VN219
	{0xC7,1,{0x1D}},              //VN203
	{0xC8,1,{0x7A}},              //VN175
	{0xC9,1,{0x1A}},               //VN144
	{0xCA,1,{0x28}},               //VN111
	{0xCB,1,{0x6B}},               //VN80
	{0xCC,1,{0x1F}},               //VN52
	{0xCD,1,{0x1D}},               //VN36
	{0xCE,1,{0x52}},               //VN24
	{0xCF,1,{0x24}},               //VN16
	{0xD0,1,{0x2D}},               //VN12
	{0xD1,1,{0x47}},               //VN8
	{0xD2,1,{0x55}},               //VN4
	{0xD3,1,{0x2C}},               //VN0

	//CMD_Page 2
	{0xFF,3,{0x98,0x81,0x02}},
	{0x06,1,{0x20}},         //PWM=30k
	{0x07,1,{0x00}},         //PWM=30k

	//CMD_Page 0
	{0xFF,3,{0x98,0x81,0x00}},
	//{0x35,1,{0x00}},              //TE on
	//{0x36,1,{0x3}},
	//{0x51,1,{0x80}},              //CABC on
	{0x53,1,{0x2C}},              //CABC on
	{0x51,2,{0x0, 0x00}},              //CABC on

	{0x11,0x0,{0x00}},
	{REGFLAG_DELAY,120,{}},
	{0x29,0x0,{0x00}},
	{REGFLAG_DELAY,20,{}},
	{REGFLAG_END_OF_TABLE,0x00,{}}	

};

#if 0
static struct LCM_setting_table lcm_set_window[] = {
	{0x2A,	4,	{0x00, 0x00, (FRAME_WIDTH>>8), (FRAME_WIDTH&0xFF)}},
	{0x2B,	4,	{0x00, 0x00, (FRAME_HEIGHT>>8), (FRAME_HEIGHT&0xFF)}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_sleep_out_setting[] = {
	// Sleep Out
	{0x11, 1, {0x00}},
	{REGFLAG_DELAY, 120, {}},

	// Display ON
	{0x29, 1, {0x00}},
	{REGFLAG_DELAY, 20, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_sleep_mode_in_setting[] = {
	// Display off sequence
	{0x28, 1, {0x00}},
	{REGFLAG_DELAY, 30, {}},

	// Sleep Mode On
	{0x10, 1, {0x00}},
	{REGFLAG_DELAY, 100, {}},
	{0xB0,1,{0x04}},
	{0xB1,1,{0x01}},
	{REGFLAG_DELAY, 100, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_compare_id_setting[] = {
	// Display off sequence
	{0xF0,	5,	{0x55, 0xaa, 0x52,0x08,0x00}},
	{REGFLAG_DELAY, 10, {}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_backlight_level_setting[] = {
	{0x51, 1, {0xFF}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};
#endif

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

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
	memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}

static void lcm_get_params(LCM_PARAMS *params)
{
	memset(params, 0, sizeof(LCM_PARAMS));

	params->type   = LCM_TYPE_DSI;

	params->width  = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

	// enable tearing-free

	params->dsi.mode   = BURST_VDO_MODE;//BURST_VDO_MODE;

	// DSI
	params->dsi.LANE_NUM                = LCM_FOUR_LANE;
	//The following defined the fomat for data coming from LCD engine.

	params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

	params->dsi.packet_size=256;

	// Video mode setting		
	params->dsi.intermediat_buffer_num = 0;//because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage

	// Video mode setting		
	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
	params->dsi.word_count=720*3;

	params->dsi.vertical_sync_active				= 6;  //min 4 sr add start
	params->dsi.vertical_backporch					= 20;  //min 18
	params->dsi.vertical_frontporch					= 10;   //min 8
	params->dsi.vertical_active_line				= FRAME_HEIGHT; 

	params->dsi.horizontal_sync_active				= 30; //>20
	params->dsi.horizontal_backporch				= 55; //>=130
	params->dsi.horizontal_frontporch				= 55; //>100 sr add end
	params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

#if 1	/*modify by wuyujing, disable ssc, avoid interference RF*/
	//params->dsi.compatibility_for_nvk = 0;		// this parameter would be set to 1 if DriverIC is NTK's and when force match DSI clock for NTK's
	params->dsi.ssc_disable = 1;
	params->dsi.ssc_range = 0; 
#endif

	params->dsi.PLL_CLOCK=211;//dish  //208; //250 //248 

	/*add by wuyujing, for esd check,start*/
	params->dsi.noncont_clock = 1;
	params->dsi.noncont_clock_period = 1;
	params->dsi.esd_check_enable = 1;
	params->dsi.customization_esd_check_enable = 1;
	params->dsi.lcm_esd_check_table[0].cmd          = 0x0A;
	params->dsi.lcm_esd_check_table[0].count        = 1;
	params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9C;
	/*add by wuyujing, for esd check,end*/

}

static unsigned int lcm_compare_id(void)
{	
	unsigned int id0 =1;
	unsigned int id1 =1;

	mt_set_gpio_mode(GPIO_LCD_ID0, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_LCD_ID0, GPIO_DIR_IN);
	mt_set_gpio_pull_enable(GPIO_LCD_ID0, GPIO_PULL_ENABLE);
	mt_set_gpio_pull_select(GPIO_LCD_ID0, GPIO_PULL_UP);

	mt_set_gpio_mode(GPIO_LCD_ID0, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_LCD_ID0, GPIO_DIR_IN);
	mt_set_gpio_pull_enable(GPIO_LCD_ID1, GPIO_PULL_ENABLE);
	mt_set_gpio_pull_select(GPIO_LCD_ID1, GPIO_PULL_UP);

	id0 = mt_get_gpio_in(GPIO_LCD_ID0);
	id1 = mt_get_gpio_in(GPIO_LCD_ID1);

#ifdef BUILD_LK
	_dprintf("%s,LK ILi988c debug: ILi988c id0 =%x id1 =%x\n",__func__,id0,id1);
#else
	printk("%s,kernel ILi988c debug: ILi988c id0 =%x id1 =%x\n",__func__,id0,id1);
#endif

	if((id0 == 0) && (id1 == 0) )
		return 1;
	else
		return 0;
}

static void lcm_init(void)
{
	MDELAY(1);
	mt_set_gpio_mode(PWR_LCM_5V_ENP, GPIO_MODE_00);
	mt_set_gpio_dir(PWR_LCM_5V_ENP, GPIO_DIR_OUT);
	mt_set_gpio_out(PWR_LCM_5V_ENP, GPIO_OUT_ONE);
	MDELAY(1);

	mt_set_gpio_mode(PWR_LCM_5V_ENN, GPIO_MODE_00);
	mt_set_gpio_dir(PWR_LCM_5V_ENN, GPIO_DIR_OUT);
	mt_set_gpio_out(PWR_LCM_5V_ENN, GPIO_OUT_ONE);
	MDELAY(10);

	mt_set_gpio_mode(PWR_LCM_RST_EN, GPIO_MODE_00);
	mt_set_gpio_dir(PWR_LCM_RST_EN, GPIO_DIR_OUT);
	mt_set_gpio_out(PWR_LCM_RST_EN, GPIO_OUT_ONE);

	MDELAY(10);
	mt_set_gpio_out(PWR_LCM_RST_EN, GPIO_OUT_ZERO);

	MDELAY(10);
	mt_set_gpio_out(PWR_LCM_RST_EN, GPIO_OUT_ONE);

	MDELAY(120);

	pr_info("%s\n", __func__);
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);

}

static void lcm_suspend(void)
{
	unsigned int data_array[16];

	lcm_isok = 0;

	data_array[0] = 0x00280500; 						  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(20);

	data_array[0] = 0x00100500; 						  
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(120);
	data_array[0] = 0x00022902; 						  
	data_array[1] = 0x000000b0;
	dsi_set_cmdq(data_array, 2, 1);
	data_array[0] = 0x00000500;

	dsi_set_cmdq(data_array, 1, 1);

	data_array[0] = 0x00000500;

	dsi_set_cmdq(data_array, 1, 1);
	data_array[0] = 0x00022902; 						  
	data_array[1] = 0x000001b1;
	dsi_set_cmdq(data_array, 2, 1);

	MDELAY(10);

	mt_set_gpio_out(PWR_LCM_RST_EN, GPIO_OUT_ZERO);
	MDELAY(5);		

	mt_set_gpio_out(PWR_LCM_5V_ENN, GPIO_OUT_ZERO);
	MDELAY(1);

	mt_set_gpio_out(PWR_LCM_5V_ENP, GPIO_OUT_ZERO);

	pr_info("%s\n", __func__);
	MDELAY(1);
}


static void lcm_resume(void)
{
#ifndef BUILD_LK
	pr_info("%s\n", __func__);
#endif
	lcm_init();
}

/*add by wuyujing,support for cacb,start*/
static struct LCM_setting_table lcm_cabc_level_setting[] = {
    {0x55, 1, {0x01}},
    {REGFLAG_END_OF_TABLE, 0x00, {}}
};

static void lcm_set_cabc(unsigned int level)
{
	unsigned char val = 0x0;

	switch (level){
	case CABC_OFF:
		val = 0x0;
		break;
	case CABC_UI:
		val = 0x1;
		break;
	case CABC_STILL:
		val = 0x2;
		break;
	case CABC_MOVING:
		val = 0x3;
		break;
	default:
		pr_err("invalid param:%d\n", level);
		return;
	}

	pr_info("%s cabc level:%d val:%x\n", __func__, level, val);

	lcm_cabc_level_setting[0].para_list[0] = val;
	cabc_level = level;
	push_table(lcm_cabc_level_setting,sizeof(lcm_cabc_level_setting) / sizeof(struct LCM_setting_table),1);
}

static void lcm_get_cabc(unsigned int *level)
{
      *level = cabc_level;
}

static void lcm_is_lcmon(unsigned int *isok)
{
	*isok = lcm_isok;
}
/*add by wuyujing,support for cacb,end*/


/**
 *add by wuyujing,backlight curve y=power(x, 2.2)
 *modify vlaue for adjust auto backlight.
 */
static unsigned char backlight_curve[256] = {
          0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
          0,   0,   0,   0,   0,   1,   1,   1,   1,   1,
          1,   1,   1,   1,   1,   2,   2,   2,   2,   2,
          2,   2,   3,   3,   3,   3,   3,   3,   3,   3,
          3,   4,   4,   4,   5,   5,   5,   5,   6,   6,
          6,   6,   7,   7,   7,   8,   8,   8,   9,   9,
          9,  10,  10,  10,  11,  11,  11,  12,  12,  12,
         13,  13,  13,  14,  14,  14,  15,  15,  16,  16,
         17,  17,  18,  18,  18,  19,  19,  19,  20,  20,
         21,  21,  22,  22,  23,  23,  24,  24,  25,  25,
         26,  26,  27,  27,  28,  28,  29,  30,  30,  31,
         32,  33,  33,  34,  35,  35,  36,  37,  38,  39,
         39,  40,  41,  42,  43,  43,  44,  45,  46,  46,
         47,  47,  48,  49,  49,  50,  51,  52,  53,  54,
         55,  56,  57,  58,  59,  60,  60,  61,  62,  63,
         64,  65,  66,  67,  67,  68,  69,  70,  71,  71,
         72,  73,  74,  75,  76,  77,  78,  79,  80,  81,
         82,  83,  84,  85,  87,  88,  89,  90,  91,  93,
         94,  95,  96,  97,  98,  99, 100, 101, 102, 103,
        105, 106, 107, 109, 110, 111, 113, 114, 116, 117,
        118, 119, 120, 121, 123, 124, 126, 127, 129, 130,
        132, 133, 135, 137, 138, 140, 141, 143, 145, 146,
        148, 149, 151, 153, 154, 156, 158, 159, 161, 163,
        165, 166, 168, 170, 172, 173, 175, 177, 179, 181,
        182, 184, 186, 188, 190, 192, 194, 196, 197, 199,
        184, 186, 188, 190, 192, 194
};

static void lcm_setbacklight_cmdq(void *handle, unsigned int level)
{
	unsigned char cmd = 0x51;
	unsigned char count = 2;
	unsigned char val[3] = {0};
	unsigned int index = level & 0xff;
	unsigned int real_level = 0;
	unsigned int map_val = 0;

#define LOWEST_BRIGHTNESS_OFFSET        (1)
#define HIGHEST_BRIGHTNESS_MAX          (255)

	if (index > HIGHEST_BRIGHTNESS_MAX)
		index = HIGHEST_BRIGHTNESS_MAX;

	if (0 == index)
		real_level = 0;
	else
		real_level = backlight_curve[index] + LOWEST_BRIGHTNESS_OFFSET;

	/* full light  map from 0xff to 0xfff*/
	map_val = real_level * 0xfff / 0xff;

	/*page 0*/
	cmd = 0xff;
	count = 3;
	val[0] = 0x98;
	val[1] = 0x81;
	val[2] = 0x00;
	dsi_set_cmdq_V22(handle, cmd, count, val, 1);

	/*set brightness*/
	cmd = 0x51;
	count = 2;
	val[0] = (map_val >> 8) & 0xff ;
	val[1] = map_val & 0xff;

	pr_info("%s level:%d real_level:%d val:%d\n", __func__, level, real_level, val[0] << 8 | val[1]);

	dsi_set_cmdq_V22(handle, cmd, count, val, 1);

	MDELAY(20);
	lcm_isok = 1;
}

#if 0
static unsigned int lcm_esd_check()
{

#ifndef BUILD_LK

	unsigned char buffer[1];
	unsigned int array[16];

#if defined(BUILD_LK)
	printf("[csh] R61318: lcm_esd_check enter\n");
#else
	printk("[csh] R61318: lcm_esd_check enter\n");
#endif

	array[0] = 0x00013700;// read id return two byte,version and id
	dsi_set_cmdq(array, 1, 1);
	read_reg_v2(0x0A, buffer, 1);

#if defined(BUILD_LK)
	//printf("csh lcm_esd_check  0x0A = %x\n",buffer[0]);
#else
	//printk("csh lcm_esd_check  0x0A = %x\n",buffer[0]);
#endif

	if(buffer[0] != 0x9C)//0x9C
	{

	#if defined(BUILD_LK)
		printf("[csh] 0x0A !=0x9C\n");
	#else
		printk("[csh] 0x0A !=0x9C\n");
	#endif

		return 1;
	}

return 0;

#endif

}

static unsigned int lcm_esd_recover()
{
	unsigned int data_array[16];

#if defined(BUILD_LK)
	//printf("csh lcm_esd_recover enter");
#else
	//printk("csh lcm_esd_recover enter");
#endif

	lcm_init();
	return 1;
}
#endif

LCM_DRIVER ili9881c_dsi_hd_vdo_dj_sp504_drv = 
{
	.name			= "ili9881c_dsi_hd_vdo_dj_sp504",
	.set_util_funcs 	= lcm_set_util_funcs,
	.get_params     	= lcm_get_params,
	.init           		= lcm_init,
	.suspend        	= lcm_suspend,
	.resume         	= lcm_resume,
	.compare_id     	= lcm_compare_id,
	.set_backlight_cmdq  = lcm_setbacklight_cmdq,
	//.esd_check 		= lcm_esd_check,
	//.esd_recover 	= lcm_esd_recover,
#if defined(LCM_DSI_CMD_MODE)
	.update         = lcm_update,
#endif
	/*add by wuyujing, support cabc*/
	.set_cabc		= lcm_set_cabc,
	.get_cabc		= lcm_get_cabc,
	.is_lcmon		= lcm_is_lcmon,
};

