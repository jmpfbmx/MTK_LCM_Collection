
/*----------------------------------------------------------------
* Author : Rubén Espínola (ruben1863@github.com)
* Contact : rubenes2003@gmail.com
* Supported device: DOOGEE X5 PRO
* Copyright 2019 © Rubén Espínola
 *---------------------------------------------------------------*/

#include "lcm_drv.h"

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH                                         (720)
#define FRAME_HEIGHT                                        (1280)

#define LCM_ID                       (0x68200)

#define REGFLAG_DELAY             							(0XFFFE)
#define REGFLAG_END_OF_TABLE      							(0xFFFF)


// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static struct LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v) (lcm_util.set_reset_pin((v)))
#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V3(para_tbl,size,force_update)         lcm_util.dsi_set_cmdq_V3(para_tbl,size,force_update)
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define read_reg_v2(cmd, buffer, buffer_size)	            lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)
#define write_regs(addr, pdata, byte_nums)	                lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)   lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define wrtie_cmd(cmd)	lcm_util.dsi_write_cmd(cmd)

struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};

static struct LCM_setting_table lcm_initialization_setting[] = {
    
{
	{0XFE, 1, {0X00,0X01,0X01}},
	{0X24, 1, {0XC0}},
	{0X25, 1, {0X53}},
	{0X26, 1, {0X00}},
	{0X2B, 1, {0XE5}},
	{0X27, 1, {0X0A}},
	{0X29, 1, {0X0A}},
	{0X2F, 1, {0X44}},
	{0X34, 1, {0X55}},
	{0X1B, 1, {0X00}},
	{0X12, 1, {0X08}},
	{0X46, 1, {0X86}},
	{0X52, 1, {0XA0}},
	{0X53, 1, {0X00}},
	{0X54, 1, {0XA0}},
	{0X55, 1, {0X00}},
	{0X5F, 1, {0X12}},
	{0XFE, 1, {0X03}},
	{0X00, 1, {0X05}},
	{0X01, 1, {0X16}},
	{0X02, 1, {0X01}},
	{0X03, 1, {0X08}},
	{0X04, 1, {0X00}},
	{0X05, 1, {0X00}},
	{0X06, 1, {0X50}},
	{0X07, 1, {0X00}},
	{0X08, 1, {0X00}},
	{0X09, 1, {0X00}},
	{0X0A, 1, {0X00}},
	{0X0B, 1, {0X00}},
	{0X0C, 1, {0X00}},
	{0X0D, 1, {0X00}},
	{0X0E, 1, {0X05}},
	{0X0F, 1, {0X06}},
	{0X10, 1, {0X07}},
	{0X11, 1, {0X08}},
	{0X12, 1, {0X00}},
	{0X13, 1, {0X40}},
	{0X14, 1, {0X00}},
	{0X15, 1, {0XC5}},
	{0X16, 1, {0X08}},
	{0X17, 1, {0X09}},
	{0X18, 1, {0X0A}},
	{0X19, 1, {0X0B}},
	{0X1A, 1, {0X0C}},
	{0X1B, 1, {0X00}},
	{0X1C, 1, {0X40}},
	{0X1D, 1, {0X00}},
	{0X1E, 1, {0X85}},
	{0X1F, 1, {0X08}},
	{0X20, 1, {0X00}},
	{0X21, 1, {0X00}},
	{0X22, 1, {0X05}},
	{0X23, 1, {0X04}},
	{0X24, 1, {0X06}},
	{0X25, 1, {0X2D}},
	{0X26, 1, {0X00}},
	{0X27, 1, {0X08}},
	{0X28, 1, {0X0A}},
	{0X29, 1, {0X2D}},
	{0X2A, 1, {0X00}},
	{0X2B, 1, {0X00}},
	{0X2D, 1, {0X00}},
	{0X2F, 1, {0X00}},
	{0X30, 1, {0X00}},
	{0X31, 1, {0X00}},
	{0X32, 1, {0X00}},
	{0X33, 1, {0X00}},
	{0X34, 1, {0X00}},
	{0X35, 1, {0X00}},
	{0X36, 1, {0X00}},
	{0X37, 1, {0X00}},
	{0X38, 1, {0X00}},
	{0X39, 1, {0X00}},
	{0X3A, 1, {0X00}},
	{0X3B, 1, {0X00}},
	{0X3D, 1, {0X00}},
	{0X3F, 1, {0X00}},
	{0X40, 1, {0X00}},
	{0X41, 1, {0X00}},
	{0X42, 1, {0X00}},
	{0X43, 1, {0X00}},
	{0X44, 1, {0X00}},
	{0X45, 1, {0X00}},
	{0X46, 1, {0X00}},
	{0X47, 1, {0X00}},
	{0X48, 1, {0X00}},
	{0X49, 1, {0X00}},
	{0X4A, 1, {0X00}},
	{0X4B, 1, {0X00}},
	{0X4C, 1, {0X00}},
	{0X4D, 1, {0X00}},
	{0X4E, 1, {0X00}},
	{0X4F, 1, {0X00}},
	{0X50, 1, {0X00}},
	{0X51, 1, {0X00}},
	{0X52, 1, {0X00}},
	{0X53, 1, {0X00}},
	{0X54, 1, {0X00}},
	{0X55, 1, {0X00}},
	{0X56, 1, {0X00}},
	{0X58, 1, {0X00}},
	{0X59, 1, {0X00}},
	{0X5A, 1, {0X00}},
	{0X5B, 1, {0X00}},
	{0X5C, 1, {0X00}},
	{0X5D, 1, {0X00}},
	{0X5E, 1, {0X00}},
	{0X5F, 1, {0X00}},
	{0X60, 1, {0X00}},
	{0X61, 1, {0X00}},
	{0X62, 1, {0X00}},
	{0X63, 1, {0X00}},
	{0X64, 1, {0X00}},
	{0X65, 1, {0X00}},
	{0X66, 1, {0X00}},
	{0X67, 1, {0X00}},
	{0X68, 1, {0X00}},
	{0X69, 1, {0X00}},
	{0X6A, 1, {0X00}},
	{0X6B, 1, {0X00}},
	{0X6C, 1, {0X00}},
	{0X6D, 1, {0X00}},
	{0X6E, 1, {0X00}},
	{0X6F, 1, {0X00}},
	{0X70, 1, {0X00}},
	{0X71, 1, {0X00}},
	{0X72, 1, {0X00}},
	{0X73, 1, {0X00}},
	{0X74, 1, {0X01}},
	{0X75, 1, {0X01}},
	{0X76, 1, {0X01}},
	{0X77, 1, {0X01}},
	{0X78, 1, {0X01}},
	{0X79, 1, {0X01}},
	{0X7A, 1, {0X00}},
	{0X7B, 1, {0X00}},
	{0X7C, 1, {0X00}},
	{0X7D, 1, {0X00}},
	{0X7E, 1, {0X01}},
	{0X7F, 1, {0X09}},
	{0X80, 1, {0X0B}},
	{0X81, 1, {0X0D}},
	{0X82, 1, {0X0F}},
	{0X83, 1, {0X3F}},
	{0X84, 1, {0X3F}},
	{0X85, 1, {0X1C}},
	{0X86, 1, {0X1C}},
	{0X87, 1, {0X3F}},
	{0X88, 1, {0X3F}},
	{0X89, 1, {0X3F}},
	{0X8A, 1, {0X3F}},
	{0X8B, 1, {0X3F}},
	{0X8C, 1, {0X3F}},
	{0X8D, 1, {0X3F}},
	{0X8E, 1, {0X3F}},
	{0X8F, 1, {0X3F}},
	{0X90, 1, {0X3F}},
	{0X91, 1, {0X1D}},
	{0X92, 1, {0X1C}},
	{0X93, 1, {0X03}},
	{0X94, 1, {0X02}},
	{0X95, 1, {0X1C}},
	{0X96, 1, {0X1D}},
	{0X97, 1, {0X3F}},
	{0X98, 1, {0X3F}},
	{0X99, 1, {0X3F}},
	{0X9A, 1, {0X3F}},
	{0X9B, 1, {0X3F}},
	{0X9C, 1, {0X3F}},
	{0X9D, 1, {0X3F}},
	{0X9E, 1, {0X3F}},
	{0X9F, 1, {0X3F}},
	{0XA0, 1, {0X3F}},
	{0XA2, 1, {0X1C}},
	{0XA3, 1, {0X1C}},
	{0XA4, 1, {0X3F}},
	{0XA5, 1, {0X3F}},
	{0XA6, 1, {0X0E}},
	{0XA7, 1, {0X0C}},
	{0XA9, 1, {0X0A}},
	{0XAA, 1, {0X08}},
	{0XAB, 1, {0X00}},
	{0XAC, 1, {0X02}},
	{0XAD, 1, {0X0E}},
	{0XAE, 1, {0X0C}},
	{0XAF, 1, {0X0A}},
	{0XB0, 1, {0X08}},
	{0XB1, 1, {0X3F}},
	{0XB2, 1, {0X3F}},
	{0XB3, 1, {0X1C}},
	{0XB4, 1, {0X1C}},
	{0XB5, 1, {0X3F}},
	{0XB6, 1, {0X3F}},
	{0XB7, 1, {0X3F}},
	{0XB8, 1, {0X3F}},
	{0XB9, 1, {0X3F}},
	{0XBA, 1, {0X3F}},
	{0XBB, 1, {0X3F}},
	{0XBC, 1, {0X3F}},
	{0XBD, 1, {0X3F}},
	{0XBE, 1, {0X3F}},
	{0XBF, 1, {0X1C}},
	{0XC0, 1, {0X1D}},
	{0XC1, 1, {0X00}},
	{0XC2, 1, {0X01}},
	{0XC3, 1, {0X1D}},
	{0XC4, 1, {0X1C}},
	{0XC5, 1, {0X3F}},
	{0XC6, 1, {0X3F}},
	{0XC7, 1, {0X3F}},
	{0XC8, 1, {0X3F}},
	{0XC9, 1, {0X3F}},
	{0XCA, 1, {0X3F}},
	{0XCB, 1, {0X3F}},
	{0XCC, 1, {0X3F}},
	{0XCD, 1, {0X3F}},
	{0XCE, 1, {0X3F}},
	{0XCF, 1, {0X1C}},
	{0XD0, 1, {0X1C}},
	{0XD1, 1, {0X3F}},
	{0XD2, 1, {0X3F}},
	{0XD3, 1, {0X09}},
	{0XD4, 1, {0X0B}},
	{0XD5, 1, {0X0D}},
	{0XD6, 1, {0X0F}},
	{0XD7, 1, {0X03}},
	{0XDF, 1, {0X00}},
	{0XDC, 1, {0X02}},
	{0XDE, 1, {0X08}},
	{0XFE, 1, {0X0E}},
	{0X01, 1, {0X75}},
	{0XFE, 1, {0X0C}},
	{0X08, 1, {0X06}},
	{0X09, 1, {0X06}},
	{0XFE, 1, {0X04}},
	{0X60, 1, {0X00}},
	{0X61, 1, {0X0C}},
	{0X62, 1, {0X12}},
	{0X63, 1, {0X0E}},
	{0X64, 1, {0X07}},
	{0X65, 1, {0X13}},
	{0X66, 1, {0X0F}},
	{0X67, 1, {0X0B}},
	{0X68, 1, {0X16}},
	{0X69, 1, {0X0C}},
	{0X6A, 1, {0X0E}},
	{0X6B, 1, {0X08}},
	{0X6C, 1, {0X0F}},
	{0X6D, 1, {0X0B}},
	{0X6E, 1, {0X04}},
	{0X6F, 1, {0X00}},
	{0X70, 1, {0X00}},
	{0X71, 1, {0X0C}},
	{0X72, 1, {0X12}},
	{0X73, 1, {0X0E}},
	{0X74, 1, {0X07}},
	{0X75, 1, {0X13}},
	{0X76, 1, {0X0F}},
	{0X77, 1, {0X0B}},
	{0X78, 1, {0X16}},
	{0X79, 1, {0X0C}},
	{0X7A, 1, {0X0E}},
	{0X7B, 1, {0X08}},
	{0X7C, 1, {0X0F}},
	{0X7D, 1, {0X0B}},
	{0X7E, 1, {0X04}},
	{0X7F, 1, {0X00}},
	{0XFE, 1, {0X00}},
	{0X58, 1, {0XAD}},
	{0X35, 0, {0X00}},
	{0X11, 0, {0X00}},
	{REGFLAG_DELAY, 120, {}},
	{0X29, 0, {0X00}},
	{REGFLAG_DELAY, 20, {}},
	{REGFLAG_END_OF_TABLE, 0X00, {}}	
};	

static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = 
{
	{REGFLAG_DELAY, 20, {}},
    {0x28, 1, {0x00}},
    {REGFLAG_DELAY, 20, {}},
    {0x10, 1, {0x00}},
    {REGFLAG_DELAY, 20, {}},
    {REGFLAG_END_OF_TABLE, 0x00, {}}
};

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


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const struct LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(struct LCM_UTIL_FUNCS));
}


static void lcm_get_params(struct LCM_PARAMS *params)
{
	memset(params, 0, sizeof(struct LCM_PARAMS));
	
	params->type = 2;
	params->physical_width = 63;
	params->dsi.data_format.format = 2;
	params->physical_height = 110;
	params->dsi.PS = 2;
	params->dbi.te_mode = 0;
	params->dbi.te_edge_polarity = 0;
	params->dsi.data_format.color_order = 0;
	params->dsi.data_format.trans_seq = 0;
	params->dsi.data_format.padding = 0;
	params->dsi.intermediat_buffer_num = 0;
	params->dsi.vertical_sync_active = 2;
	params->dsi.packet_size = 256;
	params->width = 720;
	params->dsi.vertical_backporch = 14;
	params->height = 1280;
	params->dsi.vertical_frontporch = 16;
	params->dsi.horizontal_backporch = 16;
	params->dsi.mode = 1;
	params->dsi.horizontal_frontporch = 8;
	params->dsi.LANE_NUM = 3;
	params->dsi.vertical_active_line = 1280;
	params->dsi.horizontal_sync_active = 4;
	params->dsi.horizontal_active_pixel = 720;
	params->dsi.PLL_CLOCK = 235;
	params->dsi.ssc_disable = 1;
}

static void lcm_init(void)
{
	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(50);
	SET_RESET_PIN(1);
	MDELAY(120);

    push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}


static void lcm_suspend(void)
{
	push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
	SET_RESET_PIN(0);
	MDELAY(50);
}

static void lcm_resume(void)
{
    lcm_init();
}

static unsigned int lcm_esd_check(void)
{
	unsigned char buffer[1] = {0};

	read_reg_v2(0x0A, buffer, 1);

#ifndef BUILD_LK
    if(buffer[0] == 156)
    {
		#ifndef BUILD_LK
			printk("RM68190 lcm_esd_check false \n");
        #endif
        return false;
    }
	else
    {      
        #ifndef BUILD_LK
			printk("RM68190 lcm_esd_check true \n");
        #endif
        return true;
    }
#endif
}

static unsigned int lcm_esd_recover(void)
{
	unsigned int data_array[16];
	
	printk("RM68190 lcm_esd_recover enter\n");
	
	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(30);
	SET_RESET_PIN(1);
	MDELAY(130);

	lcm_init();
	
	data_array[0] = 0x00320500;
	dsi_set_cmdq(&data_array, 1, 1);
	MDELAY(50);

	return 1;
}

static unsigned int lcm_compare_id(void)
{
/* enable only if used more than 1 lcm driver
	unsigned int data_array[16]
	unsigned char buffer[3];
	unsigned char id_high = 0;
	unsigned char id_low = 0;
	
	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(120);
	data_array[0] = 0x01FE1500;
	dsi_set_cmdq(data_array, 1, 1);
	
	data_array[0] = 0x00013700;
	dsi_set_cmdq(data_array, 1, 1);
	
	read_reg_v2(0xDE, buffer, 1);
	id_high = buffer[0];
	
	read_reg_v2(0xDF, buffer, 1);
	id_low = buffer[0] | (id_high << 8);
	return (LCM_ID == id) ? 1 : 0;
*/
	return 1
}

// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER hct_rm68200_dsi_vdo_hd_cpt_lcm_drv =
{
    .name           = "hct_rm68200_dsi_vdo_hd_cpt",
    .set_util_funcs = lcm_set_util_funcs,
    .get_params     = lcm_get_params,
    .init           = lcm_init,
    .suspend        = lcm_suspend,
    .resume         = lcm_resume,   
    .compare_id     = lcm_compare_id,
    .esd_check      = lcm_esd_check,   
    .esd_recover    = lcm_esd_recover,	
};
