
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
        #include <mt-plat/mt_gpio.h>
#endif

/* Local Constants */
#define FRAME_WIDTH  (720)
#define FRAME_HEIGHT (1280)

/* Local Variables */
#define SET_RESET_PIN(v) (lcm_util.set_reset_pin((v)))
#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))

/**
 * REGFLAG_DELAY, used to trigger MDELAY,
 * REGFLAG_END_OF_TABLE, used to mark the end of LCM_setting_table.
 * their values dosen't matter until they,
 * match with any LCM_setting_table->cmd.
 */
#define REGFLAG_DELAY 			0xFE
#define REGFLAG_END_OF_TABLE 	0XFF

/* Local Functions */
#define dsi_set_cmdq_V3(para_tbl,size,force_update)         lcm_util.dsi_set_cmdq_V3(para_tbl,size,force_update)
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define read_reg_v2(cmd, buffer, buffer_size)	            lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)
#define write_regs(addr, pdata, byte_nums)	                lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)   lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define wrtie_cmd(cmd)	lcm_util.dsi_write_cmd(cmd)

/* LCM Driver Implementations */

static LCM_UTIL_FUNCS lcm_util = { 0 };

struct LCM_setting_table {
	unsigned char cmd;
	unsigned char count;
	unsigned char para_list[64];
};

static struct LCM_setting_table lcm_initialization_setting[] =
{
    { 0xF0, 0x02, {0x5A, 0x5A}},
    { 0xF1, 0x02, {0x5A, 0x5A}},
    { 0xFC, 0x02, {0x5A, 0x5A}},
    { 0xCA, 0x04, {0x00, 0x54, 0x05, 0x28}},
    { 0xB1, 0x06, {0x25, 0x40, 0xE8, 0x10, 0x00, 0x22}},
    { 0xB2, 0x01, {0x21}},
    { 0xE3, 0x01, {0x26}},
    { 0xF2, 0x04, {0x0C, 0x10, 0x0E, 0x0D}},
    { 0xF5, 0x12, {0x6E, 0x78, 0x10, 0x6A, 0x27, 0x1D, 0x4C, 0x4C, 0x03, 0x03, 0x04, 0x22, 0x11, 0x31, 0x50, 0x2A, 0x16, 0x75}},
    { 0xF6, 0x07, {0x04, 0x8C, 0x0F, 0x80, 0x46, 0x00, 0x00}},
    { 0xF7, 0x16, {0x04, 0x14, 0x16, 0x18, 0x1A, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x24, 0x25, 0x0C}},
    { 0xF8, 0x16, {0x05, 0x15, 0x17, 0x19, 0x1B, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x24, 0x25, 0x0D}},
    { 0xED, 0x0B, {0xC0, 0x58, 0x38, 0x58, 0x38, 0x58, 0x38, 0x04, 0x04, 0x11, 0x22}},
    { 0xEE, 0x18, {0x67, 0x89, 0x45, 0x23, 0x55, 0x55, 0x55, 0x55, 0x43, 0x32, 0x87, 0xA9, 0x33, 0x33, 0x33, 0x33, 0x89, 0x01, 0x00, 0x00, 0x01, 0x89, 0x00, 0x00}},
    { 0xEF, 0x20, {0x3D, 0x09, 0x00, 0x40, 0x06, 0x67, 0x45, 0x23, 0x01, 0x00, 0x00, 0x00, 0x00, 0x04, 0x80, 0x80, 0x84, 0x07, 0x09, 0x89, 0x08, 0x12, 0x21, 0x21, 0x03, 0x03, 0x44, 0x33, 0x00, 0x00, 0x00, 0x00}},
    { 0xFA, 0x11, {0x10, 0x30, 0x15, 0x1A, 0x11, 0x15, 0x1C, 0x1A, 0x1E, 0x28, 0x2C, 0x2B, 0x2B, 0x2A, 0x2A, 0x20, 0x2E}},
    { 0xFB, 0x11, {0x10, 0x30, 0x15, 0x1A, 0x11, 0x15, 0x1C, 0x1A, 0x1E, 0x28, 0x2C, 0x2B, 0x2B, 0x2A, 0x2A, 0x20, 0x2E}},
    { 0xC5, 0x01, {0x21}},
    { 0xFE, 0x01, {0x48}},
    { 0xC8, 0x02, {0x24, 0x53}},
    { 0x11, 0x01, {0x00}},
    { REGFLAG_DELAY, 120, {0x00}},
    { 0x29, 0x01, {0x00}},
    { REGFLAG_DELAY, 20, {0x00}},
    { REGFLAG_END_OF_TABLE, 0x00, {0x00}}
};

static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = 
{
	{ 0x28, 0x01, {0x00}},
    { REGFLAG_DELAY, 20, {0x00}},
    { 0xF5, 0x12, {0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
    { REGFLAG_DELAY, 20, {0x00}},
    { 0x10, 0x01, {0x00}},
    { REGFLAG_DELAY, 120, {0x00}},
    { REGFLAG_END_OF_TABLE, 0x00, {0x00}}
};

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	int i;
	for(i = 0; i < count; i++)
	{
		switch (table[i].cmd) {
			case REGFLAG_DELAY :
			MDELAY(table[i].count);
			break;
			case REGFLAG_END_OF_TABLE :
			break;
			default:
			dsi_set_cmdq_V2(table[i].cmd, table[i].count, table[i].para_list, force_update);
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

	params->dsi.vertical_sync_active = 4;
    params->dsi.vertical_backporch = 8;
    params->dsi.horizontal_backporch = 48;
    params->dsi.PLL_CLOCK = 215;
    params->width = 720;
    params->type = 2;
    params->dsi.data_format.format = 2;
    params->dsi.PS = 2;
    params->dsi.horizontal_active_pixel = 720;
    params->height = 1280;
    params->dsi.vertical_active_line = 1280;
    params->dsi.mode = 3;
    params->dsi.LANE_NUM = 3;
    params->dsi.vertical_frontporch = 16;
    params->dsi.horizontal_sync_active = 16;
    params->dsi.horizontal_frontporch = 16;

}

static void lcm_init(void)
{

	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(20);
	SET_RESET_PIN(1);
	MDELAY(50);
	SET_RESET_PIN(0);
	MDELAY(20);
	SET_RESET_PIN(1);
	MDELAY(120);

	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);

}

static unsigned int lcm_compare_id(void)
{
	return 1;
}

static void lcm_suspend(void)
{
	push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);

	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(20);
	SET_RESET_PIN(1);
	MDELAY(120);
}

static void lcm_resume(void)
{
	lcm_init();
}


/* Get LCM Driver Hooks */
LCM_DRIVER s6d7aa0_dsi_vdo_common_lcm_drv =
{
  .name           = "s6d7aa0_dsi_vdo_common",
  .set_util_funcs = lcm_set_util_funcs,
  .get_params     = lcm_get_params,
  .init           = lcm_init,
  .suspend        = lcm_suspend,
  .resume         = lcm_resume,
  .compare_id     = lcm_compare_id,
};
