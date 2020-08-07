#ifndef BUILD_LK
#include <linux/string.h>
#include <linux/kernel.h>
#endif
#include "lcm_drv.h"

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------
	#define FRAME_WIDTH  (720)
	#define FRAME_HEIGHT (1440)
#define LCM_ID                       (0x9881)

#define REGFLAG_DELAY               (0XFED)
#define REGFLAG_END_OF_TABLE        (0x100) // END OF REGISTERS MARKER

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util;

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------
#define dsi_set_cmdq_V3(para_tbl,size,force_update)        lcm_util.dsi_set_cmdq_V3(para_tbl,size,force_update)
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)    lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)       lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)                                      lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)                  lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)											lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)               lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

#define   LCM_DSI_CMD_MODE							0
 struct LCM_setting_table {
    unsigned  cmd;
    unsigned char count;
    unsigned char para_list[64];
};
//extern int mt_dsi_pinctrl_set(unsigned int pin , unsigned int level);
//extern int PMU_db_pos_neg_setting_delay(int ms);
//extern int PMU_db_pos_neg_disable_delay(int ms);

static struct LCM_setting_table lcm_initialization_setting_ST7703[] = 
{
{0xFF,3,{0x98,0x81,0x03}},
{0x01,1,{0x00}},
{0x02,1,{0x00}},
{0x03,1,{0x73}},
{0x04,1,{0x73}},
{0x05,1,{0x00}},
{0x06,1,{0x06}},
{0x07,1,{0x02}},
{0x08,1,{0x00}},
{0x09,1,{0x01}},
{0x0a,1,{0x01}},
{0x0b,1,{0x01}},
{0x0c,1,{0x01}},
{0x0d,1,{0x01}},
{0x0e,1,{0x01}},
{0x0f,1,{0x00}},
{0x10,1,{0x00}},
{0x11,1,{0x00}},
{0x12,1,{0x00}},
{0x13,1,{0x01}},
{0x14,1,{0x00}},
{0x15,1,{0x08}},
{0x16,1,{0x08}},
{0x17,1,{0x00}},
{0x18,1,{0x08}},
{0x19,1,{0x00}},
{0x1a,1,{0x00}},
{0x1b,1,{0x00}},
{0x1c,1,{0x00}},
{0x1d,1,{0x00}},
{0x1e,1,{0xC0}},
{0x1f,1,{0x80}},
{0x20,1,{0x03}},
{0x21,1,{0x04}},
{0x22,1,{0x00}},
{0x23,1,{0x00}},
{0x24,1,{0x00}},
{0x25,1,{0x00}},
{0x26,1,{0x00}},
{0x27,1,{0x00}},
{0x28,1,{0x33}},
{0x29,1,{0x02}}, //03
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
{0x36,1,{0x03}},
{0x37,1,{0x00}},
{0x38,1,{0x00}},
{0x39,1,{0x38}},   //35
{0x3a,1,{0x00}},   //01
{0x3b,1,{0x40}},
{0x3c,1,{0x00}},
{0x3d,1,{0x00}},   //01
{0x3e,1,{0x00}},
{0x3f,1,{0x00}},
{0x40,1,{0x38}},
{0x41,1,{0x88}},
{0x42,1,{0x00}},
{0x43,1,{0x00}}, //01
{0x44,1,{0x3F}},
{0x45,1,{0x20}},
{0x46,1,{0x00}},
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
{0x5E,1,{0x10}},
{0x5F,1,{0x09}},
{0x60,1,{0x08}},
{0x61,1,{0x0F}},
{0x62,1,{0x0E}},
{0x63,1,{0x0D}},
{0x64,1,{0x0C}},
{0x65,1,{0x02}},
{0x66,1,{0x02}},
{0x67,1,{0x02}},
{0x68,1,{0x02}},
{0x69,1,{0x02}},
{0x6A,1,{0x02}},
{0x6B,1,{0x02}},
{0x6C,1,{0x02}},
{0x6D,1,{0x02}},
{0x6E,1,{0x02}},
{0x6F,1,{0x02}},
{0x70,1,{0x02}},
{0x71,1,{0x06}},
{0x72,1,{0x07}},
{0x73,1,{0x02}},
{0x74,1,{0x02}},
{0x75,1,{0x06}},
{0x76,1,{0x07}},
{0x77,1,{0x0E}},
{0x78,1,{0x0F}},
{0x79,1,{0x0C}},
{0x7A,1,{0x0D}},
{0x7B,1,{0x02}},
{0x7C,1,{0x02}},
{0x7D,1,{0x02}},
{0x7E,1,{0x02}},
{0x7F,1,{0x02}},
{0x80,1,{0x02}},
{0x81,1,{0x02}},
{0x82,1,{0x02}},
{0x83,1,{0x02}},
{0x84,1,{0x02}},
{0x85,1,{0x02}},
{0x86,1,{0x02}},
{0x87,1,{0x09}},
{0x88,1,{0x08}},
{0x89,1,{0x02}},
{0x8A,1,{0x02}}, 

{0xFF,3,{0x98,0x81,0x04}},
{0x6D,1,{0x08}},
{0x6F,1,{0x05}},
{0x70,1,{0x00}},             
{0x71,1,{0x00}},         
{0x66,1,{0xFE}},      
{0x82,1,{0x12}},     //VREF_VGH_MOD_CLPSEL 15V
{0x84,1,{0x12}},     //VREF_VGH_CLPSEL 15V
{0x85,1,{0x12}},     //VREF_VGL_CLPSEL -11V
{0x32,1,{0xAC}},
{0x8C,1,{0x80}},            
{0x3C,1,{0xF5}},
{0x3A,1,{0x24}},
{0xB5,1,{0x07}},
{0x31,1,{0x45}},
{0x88,1,{0x33}},
{0x89,1,{0xBA}},     //VCI LVD ON

{0xFF,3,{0x98,0x81,0x01}},
{0x22,1,{0x0A}},		
{0x31,1,{0x00}},
{0x41,1,{0x24}},			
{0x50,1,{0x61}},     //VREG10UT 5.1
{0x51,1,{0x61}}, //8c    //VREG20UT -5.1
{0x53,1,{0x29}}, //74    //VC0M1
{0x55,1,{0x29}}, //8a    //VC0M2
{0x60,1,{0x20}},     //SDT=2.5us
{0x61,1,{0x00}}, 
{0x62,1,{0x0D}},     //EQ
{0x63,1,{0x00}},
{0x2E,1,{0xF0}},
{0x2F,1,{0x00}},

{0xA0,1,{0x00}},
{0xA1,1,{0x1B}},
{0xA2,1,{0x2D}},
{0xA3,1,{0x16}},
{0xA4,1,{0x1D}},
{0xA5,1,{0x32}},
{0xA6,1,{0x26}},
{0xA7,1,{0x25}},
{0xA8,1,{0xA2}},
{0xA9,1,{0x1C}},
{0xAA,1,{0x27}},
{0xAB,1,{0x83}},
{0xAC,1,{0x1C}},
{0xAD,1,{0x1C}},
{0xAE,1,{0x52}},
{0xAF,1,{0x27}},
{0xB0,1,{0x2B}},
{0xB1,1,{0x51}},
{0xB2,1,{0x5C}},
{0xB3,1,{0x23}},
                                               
{0xC0,1,{0x00}},
{0xC1,1,{0x1B}},
{0xC2,1,{0x2D}},
{0xC3,1,{0x16}},
{0xC4,1,{0x1D}},
{0xC5,1,{0x32}},
{0xC6,1,{0x26}},
{0xC7,1,{0x25}},
{0xC8,1,{0xA2}},
{0xC9,1,{0x1C}},
{0xCA,1,{0x27}},                   
{0xCB,1,{0x83}},
{0xCC,1,{0x1C}},
{0xCD,1,{0x1C}},
{0xCE,1,{0x52}},
{0xCF,1,{0x27}},
{0xD0,1,{0x2B}},
{0xD1,1,{0x51}},
{0xD2,1,{0x5C}},
{0xD3,1,{0x23}},
             

//CMD_Page 0
{0xFF,3,{0x98,0x81,0x00}},
{0x11,1,{0x00}}, 
{REGFLAG_DELAY,120,{}},
{0x29,1,{0x00}}, 
{REGFLAG_DELAY,20,{}}
};

#if 0
static struct LCM_setting_table lcm_sleep_out_setting[] = {
    // Sleep Out
	{0x11, 0, {0x00}},
	{REGFLAG_DELAY, 150, {}},

    // Display ON
	{0x29, 0, {0x00}},
	{REGFLAG_DELAY, 50, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_sleep_mode_in_setting[] = {
	// Display off sequence
	{0x28, 0, {0x00}},
	{REGFLAG_DELAY, 50, {}},

       // Sleep Mode In
	{0x10, 0, {0x00}},
	{REGFLAG_DELAY, 150, {}},
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
                MDELAY(1);
        }
    }
	
}

// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

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
    params->dbi.te_mode				= LCM_DBI_TE_MODE_DISABLED;
    params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;



    params->dsi.mode   = SYNC_PULSE_VDO_MODE; //SYNC_EVENT_VDO_MODE;


    // DSI
    /* Command mode setting */
    params->dsi.LANE_NUM                = LCM_FOUR_LANE;
    
    //The following defined the fomat for data coming from LCD engine.
    params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
    params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST; 
    params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
    params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;
    // Highly depends on LCD driver capability.
    // Not support in MT6573
    params->dsi.packet_size=256;
    // Video mode setting       
    params->dsi.intermediat_buffer_num = 2;
    params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
    params->physical_width = 65;
    params->physical_height = 129;
	
	params->dsi.vertical_sync_active				= 8;
	params->dsi.vertical_backporch					= 20;
	params->dsi.vertical_frontporch					= 20; 
	params->dsi.vertical_active_line				= FRAME_HEIGHT; 

	params->dsi.horizontal_sync_active				= 20;	//2;
	params->dsi.horizontal_backporch				= 100;//120;	//60;	//42;
	params->dsi.horizontal_frontporch				= 100;
	params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
	params->dsi.HS_TRAIL = 20; 
  

// zhangxiaofei add for test
	params->dsi.PLL_CLOCK = 234;//
	
	params->dsi.ssc_disable =1;	
	params->dsi.HS_TRAIL =20;
	params->dsi.clk_lp_per_line_enable = 1;
	params->dsi.esd_check_enable = 1;
	params->dsi.customization_esd_check_enable = 1;
	params->dsi.lcm_esd_check_table[0].cmd          = 0x0a;
	params->dsi.lcm_esd_check_table[0].count        = 1;
	params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9C;
	params->dsi.noncont_clock = 1;
}

static unsigned int lcm_ata_check(unsigned char *buffer)
{
#ifndef BUILD_LK

	return 1;
#endif
}

static void lcm_init(void)
{

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

	MDELAY(10);
    SET_RESET_PIN(1);
    MDELAY(10);
    SET_RESET_PIN(0);
	MDELAY(10);            //1ms
    SET_RESET_PIN(1);
	MDELAY(30);
 
	
	push_table(lcm_initialization_setting_ST7703, sizeof(lcm_initialization_setting_ST7703) / sizeof(struct LCM_setting_table), 1);
}

static void lcm_suspend(void)
{

    unsigned int data_array[16];


    data_array[0]=0x00280500;
    dsi_set_cmdq(data_array, 1, 1);
	MDELAY(10);
    data_array[0]=0x00100500;

    dsi_set_cmdq(data_array, 1, 1);
    MDELAY(150);
    //SET_RESET_PIN(1);
    //MDELAY(20);
	SET_RESET_PIN(0);     //reset down
	  //MDELAY(10); // 1ms
	  //SET_RESET_PIN(1);
	MDELAY(15);
#ifdef GPIO_LCM_LDO_2V8_EN_PIN
    lcm_util.set_gpio_mode(GPIO_LCM_LDO_2V8_EN_PIN, GPIO_MODE_00);
    lcm_util.set_gpio_dir(GPIO_LCM_LDO_2V8_EN_PIN, GPIO_DIR_OUT);
    lcm_util.set_gpio_out(GPIO_LCM_LDO_2V8_EN_PIN, GPIO_OUT_ZERO);
#else   
    /* 2V8 off*/
    mt_dsi_pinctrl_set(LCM_POWER_DP_NO, 0);
#endif
	MDELAY(10);
#ifdef GPIO_LCM_LDO_1V8_EN_PIN
    lcm_util.set_gpio_mode(GPIO_LCM_LDO_1V8_EN_PIN, GPIO_MODE_00);
    lcm_util.set_gpio_dir(GPIO_LCM_LDO_1V8_EN_PIN, GPIO_DIR_OUT);
    lcm_util.set_gpio_out(GPIO_LCM_LDO_1V8_EN_PIN, GPIO_OUT_ZERO);  //shm
#else   
    mt_dsi_pinctrl_set(LCM_POWER_DM_NO, 0);
#endif
    MDELAY(5);
}

static void lcm_resume(void)
{
     lcm_init();
}

#if 0
static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
    unsigned int x0 = x;
    unsigned int y0 = y;
    unsigned int x1 = x0 + width - 1;
    unsigned int y1 = y0 + height - 1;
    
    unsigned char x0_MSB = ((x0>>8)&0xFF);
    unsigned char x0_LSB = (x0&0xFF);
    unsigned char x1_MSB = ((x1>>8)&0xFF);
    unsigned char x1_LSB = (x1&0xFF);
    unsigned char y0_MSB = ((y0>>8)&0xFF);
    unsigned char y0_LSB = (y0&0xFF);
    unsigned char y1_MSB = ((y1>>8)&0xFF);
    unsigned char y1_LSB = (y1&0xFF);
    
    unsigned int data_array[16];
    

    data_array[0]= 0x00053902;
    data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
    data_array[2]= (x1_LSB);
    dsi_set_cmdq(data_array, 3, 1);

    data_array[0]= 0x00053902;
    data_array[1]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
    data_array[2]= (y1_LSB);
    dsi_set_cmdq(data_array, 3, 1);
    
    data_array[0]= 0x002c3909;
    dsi_set_cmdq(data_array, 1, 0);
}
#endif 

static unsigned int lcm_compare_id(void)
{
	int array[4];
	char buffer[5];
	unsigned int id_high;
	unsigned int id_low;
	unsigned int id=0;
       
       
       
        
    //Do reset here
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
    array[0]=0x00023700;
    dsi_set_cmdq(array, 1, 1);
    
    read_reg_v2(0xf0, buffer,1);
    id_high = buffer[0]; ///////////////////////0x98
    read_reg_v2(0xf1, buffer,1);  
	  id_low = buffer[0]; 
       // id = (id_midd &lt;&lt; 8) | id_low;
	id = (id_high << 8) | id_low;

	#if defined(BUILD_LK)
			printf("ILI9881 %s id_high = 0x%04x, id_low = 0x%04x\n,id=0x%x\n", __func__, id_high, id_low,id);
	#else
			printk("ILI9881 %s id_high = 0x%04x, id_low = 0x%04x\n,id=0x%x\n", __func__, id_high, id_low,id);
	#endif
	//return 1;	
	return (LCM_ID == id)?1:0;
	 

}


LCM_DRIVER hct_ili9881d_dsi_vdo_hdp_ivo_57_hlt = 
{
    .name			= "hct_ili9881d_dsi_vdo_hdp_ivo_57_hlt",
	.set_util_funcs = lcm_set_util_funcs,
	.compare_id     = lcm_compare_id,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.ata_check	= lcm_ata_check,
#if defined(LCM_DSI_CMD_MODE)
    //.update         = lcm_update,
#endif
    };
