/******************************************************************************
 *
 * Copyright (C), 2015, TP-LINK TECHNOLOGIES CO., LTD.
 *
 * Filename: ft8716_fhd_dsi_vdo_rt5081.c
 *
 * Author: Li Liwen
 *
 * Mail : liliwen@tp-link.com.cn
 *
 * Description:
 *
 * Last modified: 2016-06-22 17:07
 *
******************************************************************************/
#define LOG_TAG "LCM"

#ifndef BUILD_LK
#include <linux/string.h>
#include <linux/kernel.h>
#endif

#include "lcm_drv.h"

#ifdef BUILD_LK
#include <platform/upmu_common.h>
#include <platform/mt_gpio.h>
#include <platform/mt_i2c.h>
#include <platform/mt_pmic.h>
//#include <platform/mtk_auxadc_sw.h>
//#include <platform/mtk_auxadc_hw.h>
#include <string.h>
#elif defined(BUILD_UBOOT)
#include <asm/arch/mt_gpio.h>
#else
#include "mt-plat/upmu_common.h"
//#include <mt-plat/mt_gpio.h>
#include <mach/gpio_const.h>
#ifdef CONFIG_MTK_LEGACY
#include <mach/mt_pm_ldo.h>
#include <mach/mt_gpio.h>

#ifndef CONFIG_FPGA_EARLY_PORTING
#include <cust_gpio_usage.h>
#include <cust_i2c.h>
#include <linux/time.h>
#endif

#endif
#endif

#define CONFIG_FT8716_DEBUG

#define DB_USE_EXT_PIN_CTRL
//#define DB_USE_I2C_CTRL

#ifdef CONFIG_FT8716_DEBUG
#ifdef BUILD_LK
#define LCM_LOGI(string, args...)  dprintf(ALWAYS, "[LK/"LOG_TAG"]"" FT8716: "string, ##args)
#define LCM_LOGD(string, args...)  dprintf(ALWAYS, "[LK/"LOG_TAG"]"" FT8716: "string, ##args)
#else
#define LCM_LOGI(fmt, args...)  printk("[KERNEL/"LOG_TAG"]"" FT8716: "fmt, ##args)
#define LCM_LOGD(fmt, args...)  printk("[KERNEL/"LOG_TAG"]"" FT8716: "fmt, ##args)
#endif
#else
#ifdef BUILD_LK
#define LCM_LOGI(string, args...)
#define LCM_LOGD(string, args...)
#else
#define LCM_LOGI(fmt, args...)  pr_debug("[KERNEL/"LOG_TAG"]"" FT8716: "fmt, ##args)
#define LCM_LOGD(fmt, args...)  pr_debug("[KERNEL/"LOG_TAG"]"" FT8716: "fmt, ##args)
#endif
#endif

/* TPS65132 */
#ifdef BUILD_LK
#define GPIO_65132_VSN_EN GPIO_LCD_BIAS_ENN_PIN
#define GPIO_65132_VSP_EN GPIO_LCD_BIAS_ENP_PIN
//#define GPIO_LCD_ID       GPIO_LCD_IO_9_PIN
#endif

/* LCDs */
#define TIANMA_VOL      (0)
#define TRUELY_VOL      (1)
#define UNKNOWN_VOL     (999)
#define VOL_THRESHOLD_MIN1 (0)          //1st supply
#define VOL_THRESHOLD_MAX1 (300)
#define VOL_THRESHOLD_MIN2 (700)       //2nd supply
#define VOL_THRESHOLD_MAX2 (1100)
#define VOL_THRESHOLD_MIN3 (1200)      //3nd supply
#define VOL_THRESHOLD_MAX3 (1500)

#define AUX_IN2_NTC  (12) /* to choose different LCDs */
extern int IMM_GetOneChannelValue(int dwChannel, int data[4], int *rawdata);
extern int rt5081_db_reg_update_bits_lcm_drv(u8 addr, u8 mask, u8 data);//llw
static LCM_UTIL_FUNCS lcm_util;
extern atomic_t double_enable;
static DEFINE_MUTEX(lcd_lock);
static int lcd_power_on_flag = 1;

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))
#define MDELAY(n)        (lcm_util.mdelay(n))
#define UDELAY(n)        (lcm_util.udelay(n))

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update) \
    lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update) \
        lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd) lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums) \
        lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd) \
      lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size) \
        lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)
#define set_gpio_lcd_enp(cmd) \
        lcm_util.set_gpio_lcd_enp_bias(cmd)
#define set_gpio_lcd_enn(cmd) \
        lcm_util.set_gpio_lcd_enn_bias(cmd)
#define get_gpio_lcd_id() \
        lcm_util.get_gpio_lcd_id()
#define set_gpio_lcd_tp_rst(cmd) \
        lcm_util.set_gpio_lcd_tp_rst(cmd)
#define set_tp_regulator(cmd) \
        lcm_util.set_tp_regulator(cmd)

#ifndef BUILD_LK

extern int mt_set_gpio_out(unsigned long pin, unsigned long output);
extern int mt_set_gpio_mode(unsigned long pin, unsigned long mode);
extern int mt_set_gpio_dir(unsigned long pin, unsigned long dir);

#endif

#define LCM_DSI_CMD_MODE        (0)
#define FRAME_WIDTH             (1080)
#define FRAME_HEIGHT            (1920)

#define REGFLAG_DELAY           0xFFFC
#define REGFLAG_UDELAY          0xFFFB
#define REGFLAG_END_OF_TABLE    0xFFFD
#define REGFLAG_RESET_LOW       0xFFFE
#define REGFLAG_RESET_HIGH      0xFFFF

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

struct LCM_setting_table {
    unsigned int cmd;
    unsigned char count;
    unsigned char para_list[64];
};

static struct LCM_setting_table lcm_suspend_setting[] = {
    {0x28, 0, {} },
    {REGFLAG_DELAY, 80, {} },
    {0x10, 0, {} },
    {REGFLAG_DELAY, 120, {} },
};

static struct LCM_setting_table init_setting[] = {
    /* [liliwen start] Add initial code for forward scan */
    {0x00, 1, {0x00} },
    {0xFF, 3, {0x87, 0x16, 0x01} },
    {0x00, 1, {0x80} },
    {0xFF, 2, {0x87, 0x16} },
    {0x00, 1, {0xB4} },
    {0xC0, 1, {0x00} },
    /* [liliwen end] */
    {0x11, 0, {} },// Sleep Out
    {REGFLAG_DELAY, 140, {} },
    {0x29, 0, {} },// Display on
    {REGFLAG_DELAY, 20, {} },
};

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
    unsigned int i;
    unsigned cmd;

    for (i = 0; i < count; i++) {
        cmd = table[i].cmd;

        switch (cmd) {

        case REGFLAG_DELAY:
            if (table[i].count <= 10)
                MDELAY(table[i].count);
            else
                MDELAY(table[i].count);
            break;

        case REGFLAG_UDELAY:
            UDELAY(table[i].count);
            break;

        case REGFLAG_END_OF_TABLE:
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

    params->type = LCM_TYPE_DSI;

    params->width = FRAME_WIDTH;
    params->height = FRAME_HEIGHT;
    params->physical_width = 68;
    params->physical_height = 121;

#if (LCM_DSI_CMD_MODE)
    params->dsi.mode = CMD_MODE;
    params->dsi.switch_mode = SYNC_PULSE_VDO_MODE;
#else
    params->dsi.mode = SYNC_PULSE_VDO_MODE;
    params->dsi.switch_mode = CMD_MODE;
#endif

#ifndef CONFIG_FPGA_EARLY_PORTING
#if (LCM_DSI_CMD_MODE)
    params->dsi.PLL_CLOCK = 350;    /* this value must be in MTK suggested table */
#else
    /* [yanlin start] Change MIPI clock to 500MHz, fps>60 */
    params->dsi.PLL_CLOCK = 500;    /* this value must be in MTK suggested table */
    /* [yanlin end] */
#endif
#else
    params->dsi.pll_div1 = 0;
    params->dsi.pll_div2 = 0;
    params->dsi.fbk_div = 0x1;
#endif

    params->dsi.switch_mode_enable = 0;

    /* DSI */
    /* Command mode setting */
    params->dsi.LANE_NUM = LCM_FOUR_LANE;
    /* The following defined the fomat for data coming from LCD engine. */
    params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
    params->dsi.data_format.trans_seq = LCM_DSI_TRANS_SEQ_MSB_FIRST;
    params->dsi.data_format.padding = LCM_DSI_PADDING_ON_LSB;
    params->dsi.data_format.format = LCM_DSI_FORMAT_RGB888;

    /* Highly depends on LCD driver capability. */
    params->dsi.packet_size = 256;
    /* video mode timing */

    params->dsi.PS = LCM_PACKED_PS_24BIT_RGB888;

    params->dsi.vertical_sync_active = 2;
    params->dsi.vertical_backporch = 16;
    params->dsi.vertical_frontporch = 16;
    params->dsi.vertical_active_line = FRAME_HEIGHT;

    params->dsi.horizontal_sync_active = 4;
    params->dsi.horizontal_backporch = 32;
    params->dsi.horizontal_frontporch = 32;
    params->dsi.horizontal_active_pixel = FRAME_WIDTH;


    /* [liliwen start] change MIPI CLK */
    params->dsi.clk_lp_per_line_enable = 1;
    params->dsi.cont_clock = 0;
    /* [liliwen end] */
    params->dsi.esd_check_enable = 1;
    params->dsi.customization_esd_check_enable = 1;
    params->dsi.lcm_esd_check_table[0].cmd          = 0x0A;
    params->dsi.lcm_esd_check_table[0].count        = 1;
    params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9C;
    /* [liliwen start] ESD check: Add 0x0D check */
    params->dsi.lcm_esd_check_table[1].cmd          = 0x0D;
    params->dsi.lcm_esd_check_table[1].count        = 1;
    params->dsi.lcm_esd_check_table[1].para_list[0] = 0x00;
    /* [liliwen end] */

    params->dsi.lane_swap_en = 1;
    params->dsi.lane_swap[MIPITX_PHY_PORT_0][MIPITX_PHY_LANE_0] = MIPITX_PHY_LANE_2;
    params->dsi.lane_swap[MIPITX_PHY_PORT_0][MIPITX_PHY_LANE_1] = MIPITX_PHY_LANE_3;
    params->dsi.lane_swap[MIPITX_PHY_PORT_0][MIPITX_PHY_LANE_2] = MIPITX_PHY_LANE_0;
    params->dsi.lane_swap[MIPITX_PHY_PORT_0][MIPITX_PHY_LANE_3] = MIPITX_PHY_LANE_1;
    params->dsi.lane_swap[MIPITX_PHY_PORT_0][MIPITX_PHY_LANE_CK] = MIPITX_PHY_LANE_CK;
    params->dsi.lane_swap[MIPITX_PHY_PORT_0][MIPITX_PHY_LANE_RX] = MIPITX_PHY_LANE_2;

}

#define POWER_ON    1
#define POWER_OFF   0
/*static void tp_power_switch(int enable)
{
    unsigned int value = enable ? 1 : 0;
    pmic_set_register_value(MT6351_PMIC_RG_VLDO28_EN_0,value);
    pmic_set_register_value(MT6351_PMIC_RG_VLDO28_EN_1,value);
}*/

int get_lcd_id_by_aux(void)
{
    int ret = TIANMA_VOL, data[4], ret_value = 0;
    int Channel = AUX_IN2_NTC;
    static int valid_id = -1;

    ret_value = IMM_GetOneChannelValue(Channel, data, &ret);
    if (ret_value == -1) {/* AUXADC is busy */
        ret = valid_id;
    } else {
        valid_id = ret;
    }

    LCM_LOGI("[lcd_auxadc_get_data(AUX_IN2_NTC)]: ret=%d, data[0]=%d, data[1]=%d\n", ret, data[0], data[1]);

    /* Mt_auxadc_hal.c */
    /* #define VOLTAGE_FULL_RANGE  1500 // VA voltage */
    /* #define AUXADC_PRECISE      4096 // 12 bits */
    if (ret != -1) {
        ret = ret * 1500 / 4096;
    }
    /* ret = ret*1800/4096;//82's ADC power */
    LCM_LOGI("APtery output mV = %d\n", ret);

    if (ret > VOL_THRESHOLD_MIN1 && ret < VOL_THRESHOLD_MAX1) {
        ret = TIANMA_VOL;
    } else if (ret > VOL_THRESHOLD_MIN2 && ret < VOL_THRESHOLD_MAX2) {
        ret = TRUELY_VOL;
    } else {
        ret = UNKNOWN_VOL;
    }
    return ret;
}
EXPORT_SYMBOL_GPL(get_lcd_id_by_aux);

static unsigned int lcm_compare_id(void)
{
    int ret = 0;
    ret = get_lcd_id_by_aux();
    if (ret == TIANMA_VOL || ret == TRUELY_VOL) {
        return 1;  /* ft LCD */
    } else{
        return 0;
    }
}

static void rt5081_vsp_enable(int enable)
{
#ifdef DB_USE_I2C_CTRL
    unsigned char cmd = 0x0;
    unsigned char mask = 0xFF;
    unsigned char data = 0xFF;
    int ret = 0;

    cmd = 0xB1;
    mask = 0x01 << 6;

    if (enable) {
        data = 0x01 << 6;

        ret = rt5081_db_reg_update_bits_lcm_drv(cmd, mask, data);

        if (ret < 0)
            LCM_LOGI("ft8716----rt5081----cmd=%0x--i2c write error----\n", cmd);
        else
            LCM_LOGI("ft8716----rt5081----cmd=%0x--i2c write success----\n", cmd);
    } else {
        data = 0x0;

        ret = rt5081_db_reg_update_bits_lcm_drv(cmd, mask, data);

        if (ret < 0)
            LCM_LOGI("ft8716----rt5081----cmd=%0x--i2c write error----\n", cmd);
        else
            LCM_LOGI("ft8716----rt5081----cmd=%0x--i2c write success----\n", cmd);
    }

#else
    if (enable) {
        set_gpio_lcd_enp(1);
    } else {
        set_gpio_lcd_enp(0);
    }

#endif

}

static void rt5081_vsn_enable(int enable)
{
#ifdef DB_USE_I2C_CTRL
    unsigned char cmd = 0x0;
    unsigned char mask = 0xFF;
    unsigned char data = 0xFF;
    int ret = 0;

    cmd = 0xB1;
    mask = 0x01 << 3;

    if (enable) {
        data = 0x01 << 3;

        ret = rt5081_db_reg_update_bits_lcm_drv(cmd, mask, data);

        if (ret < 0)
            LCM_LOGI("ft8716----rt5081----cmd=%0x--i2c write error----\n", cmd);
        else
            LCM_LOGI("ft8716----rt5081----cmd=%0x--i2c write success----\n", cmd);
    } else {
        data = 0x0;

        ret = rt5081_db_reg_update_bits_lcm_drv(cmd, mask, data);

        if (ret < 0)
            LCM_LOGI("ft8716----rt5081----cmd=%0x--i2c write error----\n", cmd);
        else
            LCM_LOGI("ft8716----rt5081----cmd=%0x--i2c write success----\n", cmd);
    }

#else
    if (enable) {
        set_gpio_lcd_enn(1);
    } else {
        set_gpio_lcd_enn(0);
    }

#endif

}

static void lcd_power_on_timing(void){
    LCM_LOGI("ft8716----lcd_power_on_timing_start\n");
    mutex_lock(&lcd_lock);
    LCM_LOGI("ft8716----on !!!!, lcd_power_on_flag: %d\n", lcd_power_on_flag);
    if(lcd_power_on_flag){
        LCM_LOGI("ft8716----skip, lcd_power_on_flag: %d\n", lcd_power_on_flag);
        goto skip_power_on;
    }
    SET_RESET_PIN(1);
    set_gpio_lcd_tp_rst(1);

    //now perform timing                                    //standard timing
    //vddi default on                                       //vddi on
    MDELAY(4);                                              //2ms

    rt5081_vsp_enable(1);

    MDELAY(15);

    rt5081_vsn_enable(1);

    MDELAY(2);                                              //1ms
    set_gpio_lcd_tp_rst(0);                                 //tp rst 0
    SET_RESET_PIN(0);                                       //lcd rst 0
    MDELAY(8);                                              //5ms

    SET_RESET_PIN(1);                                       //lcd rst 1
    set_gpio_lcd_tp_rst(1);                                 //tp rst 1
    MDELAY(15);                                             //10ms

    push_table(init_setting, sizeof(init_setting) / sizeof(struct LCM_setting_table), 1);
    lcd_power_on_flag = 1;

skip_power_on:
    mutex_unlock(&lcd_lock);
    LCM_LOGI("ft8716----lcd_power_on_timing_end\n");
}

static void lcd_power_off_timing(void){
    LCM_LOGI("ft8716----lcd_power_off_timing_start\n");
    mutex_lock(&lcd_lock);
    LCM_LOGI("ft8716----off !!!!, lcd_power_on_flag: %d\n", lcd_power_on_flag);
    if(!lcd_power_on_flag){
        LCM_LOGI("ft8716----skip, lcd_power_on_flag: %d\n", lcd_power_on_flag);
        goto skip_power_off;
    }
    push_table(lcm_suspend_setting, sizeof(lcm_suspend_setting) / sizeof(struct LCM_setting_table), 1);
    if(atomic_read(&double_enable) == 1){
        //do nothing
    }else
    {
        set_gpio_lcd_tp_rst(0);
        SET_RESET_PIN(0);
        MDELAY(10);

        rt5081_vsn_enable(0);

        MDELAY(15);

        rt5081_vsp_enable(0);

        MDELAY(5);
    }
    lcd_power_on_flag = 0;
skip_power_off:
    mutex_unlock(&lcd_lock);
    LCM_LOGI("ft8716----lcd_power_off_timing_end\n");
}
static void lcm_init(void)
{
    LCM_LOGI("ft8716----lcd_init_start\n");
    lcd_power_on_timing();
    LCM_LOGI("ft8716----lcd_init_end\n");
}

static void lcm_suspend(void)
{

    LCM_LOGI("ft8716----lcd_suspend_start\n");
    lcd_power_off_timing();
    LCM_LOGI("ft8716----lcd_suspend_end\n");
}

static void lcm_resume(void)
{
    LCM_LOGI("ft8716----lcd_resume_start\n");
    lcd_power_on_timing();
    LCM_LOGI("ft8716----lcd_resume_end\n");
}

static void lcm_update(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
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
    data_array[1] = (x1_MSB << 24) | (x0_LSB << 16) | (x0_MSB << 8) | 0x2a;
    data_array[2] = (x1_LSB);
    dsi_set_cmdq(data_array, 3, 1);

    data_array[0] = 0x00053902;
    data_array[1] = (y1_MSB << 24) | (y0_LSB << 16) | (y0_MSB << 8) | 0x2b;
    data_array[2] = (y1_LSB);
    dsi_set_cmdq(data_array, 3, 1);

    data_array[0] = 0x002c3909;
    dsi_set_cmdq(data_array, 1, 0);
}

/* return TRUE: need recovery */
/* return FALSE: No need recovery */
static unsigned int lcm_esd_check(void)
{
#ifndef BUILD_LK
    char buffer[3];
    int array[4];

    array[0] = 0x00013700;
    dsi_set_cmdq(array, 1, 1);

    read_reg_v2(0x53, buffer, 1);

    if (buffer[0] != 0x24) {
        LCM_LOGI("[LCM ERROR] [0x53]=0x%02x\n", buffer[0]);
        return TRUE;
    }
    LCM_LOGI("[LCM NORMAL] [0x53]=0x%02x\n", buffer[0]);
    return FALSE;
#else
    return FALSE;
#endif
}

static unsigned int lcm_ata_check(unsigned char *buffer)
{
#ifndef BUILD_LK
    unsigned int ret = 0;
    unsigned int x0 = FRAME_WIDTH / 4;
    unsigned int x1 = FRAME_WIDTH * 3 / 4;

    unsigned char x0_MSB = ((x0 >> 8) & 0xFF);
    unsigned char x0_LSB = (x0 & 0xFF);
    unsigned char x1_MSB = ((x1 >> 8) & 0xFF);
    unsigned char x1_LSB = (x1 & 0xFF);

    unsigned int data_array[3];
    unsigned char read_buf[4];

    LCM_LOGI("ATA check size = 0x%x,0x%x,0x%x,0x%x\n", x0_MSB, x0_LSB, x1_MSB, x1_LSB);
    data_array[0] = 0x0005390A;    /* HS packet */
    data_array[1] = (x1_MSB << 24) | (x0_LSB << 16) | (x0_MSB << 8) | 0x2a;
    data_array[2] = (x1_LSB);
    dsi_set_cmdq(data_array, 3, 1);

    data_array[0] = 0x00043700;    /* read id return two byte,version and id */
    dsi_set_cmdq(data_array, 1, 1);

    read_reg_v2(0x2A, read_buf, 4);

    if ((read_buf[0] == x0_MSB) && (read_buf[1] == x0_LSB)
        && (read_buf[2] == x1_MSB) && (read_buf[3] == x1_LSB))
        ret = 1;
    else
        ret = 0;

    x0 = 0;
    x1 = FRAME_WIDTH - 1;

    x0_MSB = ((x0 >> 8) & 0xFF);
    x0_LSB = (x0 & 0xFF);
    x1_MSB = ((x1 >> 8) & 0xFF);
    x1_LSB = (x1 & 0xFF);

    data_array[0] = 0x0005390A;    /* HS packet */
    data_array[1] = (x1_MSB << 24) | (x0_LSB << 16) | (x0_MSB << 8) | 0x2a;
    data_array[2] = (x1_LSB);
    dsi_set_cmdq(data_array, 3, 1);
    return ret;
#else
    return 0;
#endif
}

LCM_DRIVER ft8716_fhd_dsi_vdo_rt5081_lcm_drv = {
    .name = "ft8716_fhd_dsi_vdo_rt5081_drv",
    .set_util_funcs = lcm_set_util_funcs,
    .get_params = lcm_get_params,
    .init = lcm_init,
    .suspend = lcm_suspend,
    .resume = lcm_resume,
    .compare_id = lcm_compare_id,
    .esd_check = lcm_esd_check,
    .ata_check = lcm_ata_check,
    .update = lcm_update,
};

