#ifndef BUILD_LK
#include <linux/string.h>
#include <linux/kernel.h>
#endif
#include "lcm_drv.h"

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------
#define FRAME_WIDTH  										(600)
#define FRAME_HEIGHT 										(1280)
#define LCM_ID                       (0x9881)

#define REGFLAG_DELAY               (0XFE)
#define REGFLAG_END_OF_TABLE        (0x100) // END OF REGISTERS MARKER

#define LCM_DSI_CMD_MODE									0
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
    unsigned char cmd;
    unsigned char count;
    unsigned char para_list[64];
};
//extern int mt_dsi_pinctrl_set(unsigned int pin , unsigned int level);
//extern int PMU_db_pos_neg_setting_delay(int ms);
//extern int PMU_db_pos_neg_disable_delay(int ms);

static struct LCM_setting_table lcm_initialization_setting_ST7703[] = 
{
{0xFF,03,{0x98,0x81,0x03}},

{0x01,01,{0x00}},
{0x02,01,{0x00}},
{0x03,01,{0x73}},
{0x04,01,{0x73}},
{0x05,01,{0x00}},
{0x06,01,{0x06}},
{0x07,01,{0x02}},
{0x08,01,{0x00}},
{0x09,01,{0x00}},
{0x0a,01,{0x00}},
{0x0b,01,{0x00}},
{0x0c,01,{0x00}},
{0x0d,01,{0x00}},
{0x0e,01,{0x00}},
{0x0f,01,{0x00}},
{0x10,01,{0x00}},
{0x11,01,{0x00}},
{0x12,01,{0x00}},
{0x13,01,{0x00}},
{0x14,01,{0x00}},
{0x15,01,{0x08}},
{0x16,01,{0x08}},
{0x17,01,{0x00}},
{0x18,01,{0x08}},
{0x19,01,{0x00}},
{0x1a,01,{0x00}},
{0x1b,01,{0x00}},
{0x1c,01,{0x00}},
{0x1d,01,{0x00}},
{0x1e,01,{0xc0}},
{0x1f,01,{0x00}},
{0x20,01,{0x03}},  //03 clk rise}}
{0x21,01,{0x02}},
{0x22,01,{0x00}},
{0x23,01,{0x00}},
{0x24,01,{0x00}},
{0x25,01,{0x00}},
{0x26,01,{0x00}},
{0x27,01,{0x00}},
{0x28,01,{0x33}},
{0x29,01,{0x02}},
{0x2a,01,{0x00}},
{0x2b,01,{0x00}},
{0x2c,01,{0x00}},
{0x2d,01,{0x00}},
{0x2e,01,{0x00}},
{0x2f,01,{0x00}},
{0x30,01,{0x00}},
{0x31,01,{0x00}},
{0x32,01,{0x00}},
{0x33,01,{0x00}},
{0x34,01,{0x00}},
{0x35,01,{0x00}},
{0x36,01,{0x00}},
{0x37,01,{0x00}},
{0x38,01,{0x00}},
{0x39,01,{0x38}},
{0x3A,01,{0x00}},
{0x3B,01,{0x40}},
{0x3C,01,{0x00}},
{0x3D,01,{0x00}},
{0x3E,01,{0x00}},
{0x3F,01,{0x00}},
{0x40,01,{0x38}},
{0x41,01,{0x88}},
{0x42,01,{0x00}},
{0x43,01,{0x00}},
{0x44,01,{0x3F}},   //1F TO 3F_ RESET KEEP LOW ALL GATE ON
{0x45,01,{0x20}},   //LVD
{0x46,01,{0x00}},
{REGFLAG_DELAY,50, {}},

{0x50,01,{0x01}},
{0x51,01,{0x23}},
{0x52,01,{0x45}},
{0x53,01,{0x67}},
{0x54,01,{0x89}},
{0x55,01,{0xab}},
{0x56,01,{0x01}},
{0x57,01,{0x23}},
{0x58,01,{0x45}},
{0x59,01,{0x67}},
{0x5a,01,{0x89}},
{0x5b,01,{0xab}},
{0x5c,01,{0xcd}},
{0x5d,01,{0xef}},
{REGFLAG_DELAY, 50,{}},

{0x5e,01,{0x10}},

{0x5f,01,{0x02}},      //GOUT1_L  VGL
{0x60,01,{0x02}},       //GOUT2_L  VGL
{0x61,01,{0x02}},       //GOUT3_L  VGL}
{0x62,01,{0x09}},      //GOUT4_L  STV4
{0x63,01,{0x08}},       //GOUT5_L  STV3
{0x64,01,{0x0F}},      //GOUT6_L  CLK4
{0x65,01,{0x0E}},       //GOUT7_L  CLK3
{0x66,01,{0x0D}},       //GOUT8_L  CLK2
{0x67,01,{0x0C}},       //GOUT9_L  CLK1
{0x68,01,{0x02}},       //GOUT10_L VGL
{0x69,01,{0x02}},       //GOUT11_L VGL
{0x6a,01,{0x02}},       //GOUT12_L VGL}
{0x6b,01,{0x02}},       //GOUT13_L VGL}
{0x6c,01,{0x02}},       //GOUT14_L VGL
{0x6d,01,{0x02}},       //GOUT15_L VGL
{0x6e,01,{0x02}},       //GOUT16_L VGL
{0x6f,01,{0x02}},      //GOUT17_L VGL
{0x70,01,{0x02}},       //GOUT18_L VGL
{0x71,01,{0x06}},       //GOUT19_L STV1
{0x72,01,{0x07}},       //GOUT20_L STV2}
{0x73,01,{0x02}},       //GOUT21_L VGL}
{0x74,01,{0x02}},      //GOUT22_L VGL


{0x75,01,{0x02}},       //GOUT1_L  VGL
{0x76,01,{0x02}},       //GOUT2_L  VGL
{0x77,01,{0x02}},       //GOUT3_L  VGL
{0x78,01,{0x06}},       //GOUT4_L  STV4
{0x79,01,{0x07}},       //GOUT5_L  STV3
{0x7a,01,{0x0F}},       //GOUT6_L  CLK4
{0x7b,01,{0x0C}},       //GOUT7_L  CLK3}
{0x7c,01,{0x0D}},       //GOUT8_L  CLK2}
{0x7d,01,{0x0E}},      //GOUT9_L  CLK1}
{0x7e,01,{0x02}},       //GOUT10_L VGL
{0x7f,01,{0x02}},       //GOUT11_L VGL
{0x80,01,{0x02}},       //GOUT12_L VGL
{0x81,01,{0x02}},       //GOUT13_L VGL
{0x82,01,{0x02}},       //GOUT14_L VGL
{0x83,01,{0x02}},       //GOUT15_L VGL
{0x84,01,{0x02}},       //GOUT16_L VGL
{0x85,01,{0x02}},       //GOUT17_L VGL
{0x86,01,{0x02}},       //GOUT18_L VGL
{0x87,01,{0x09}},       //GOUT19_L STV1
{0x88,01,{0x08}},       //GOUT20_L STV2
{0x89,01,{0x02}},       //GOUT21_L VGL
{0x8A,01,{0x02}},       //GOUT22_L VGL
                                       
                                       
{0xFF,03,{0x98,0x81,0x04}},    
{0x00,01,{0x00}},
{0x6D,01,{0x08}},
{0x6F,01,{0x05}},     
{0x70,01,{0x00}},     //for 
{0x71,01,{0x00}},     //fo  
{0x66,01,{0xFE}},     //VGH=VSP X4 Pump
{0x82,01,{0x12}},     //VGH_MOD     
{0x84,01,{0x11}},     //VREF_VGH_CLPSEL  15V
{0x85,01,{0x0E}},     //VREF_VGL_CLPSEL -11V
{0x32,01,{0xAC}},     //
{0x8C,01,{0x80}},     //sleep out Vcom dis
{0x3C,01,{0xF5}},     //
{0x3A,01,{0x24}},     //PS_EN OFF       
{0xB5,01,{0x07}},     //
{0x31,01,{0x45}},     //
{0x88,01,{0x33}},     //VSP/VSN LVD Disadble 
{0x89,01,{0xBA}},     //VCI LVD ON
{0x38,01,{0x01}},     //MIPI
{0x39,01,{0x00}},     //MIPI

{0xFF,03,{0x98,0x81,0x01}},    
 
{0x22,01,{0x0A}},               //RGB_SS_GS
{0x31,01,{0x00}},               //c0lumn inversi0n
{0x41,01,{0x24}},               //
{0x53,01,{0x2C}},              //VC0M1}
{0x55,01,{0x2B}},               //VC0M2
{0x50,01,{0x73}},               //VREG10UT 4.8
{0x51,01,{0x73}},               //VREG20UT -4.8
{0x60,01,{0x1D}},               //SDT
{0x61,01,{0x00}},               //CR    
{0x62,01,{0x0D}},               //EQ
{0x63,01,{0x00}},               //PC   

{0xB6,01,{0x09}},             // 600 Resolution     [94(RS1)  0]    [96(RS0) 1]

//============Gamma START=============

//Pos Register
{0xA0,01,{0x00}},
{0xA1,01,{0x1C}},	
{0xA2,01,{0x2E}},	
{0xA3,01,{0x17}},
{0xA4,01,{0x1C}},	
{0xA5,01,{0x31}},	
{0xA6,01,{0x25}},	
{0xA7,01,{0x25}},	
{0xA8,01,{0xA0}},	
{0xA9,01,{0x1C}},	
{0xAA,01,{0x28}},	
{0xAB,01,{0x81}},
{0xAC,01,{0x1C}},	
{0xAD,01,{0x1C}},	
{0xAE,01,{0x51}},	
{0xAF,01,{0x25}},
{0xB0,01,{0x2A}},	
{0xB1,01,{0x4D}},	
{0xB2,01,{0x59}},	
{0xB3,01,{0x23}},	
	
//Neg Register
{0xC0,01,{0x00}},	
{0xC1,01,{0x1C}},	
{0xC2,01,{0x2E}},	
{0xC3,01,{0x17}},	
{0xC4,01,{0x1C}},	
{0xC5,01,{0x31}},	
{0xC6,01,{0x25}},	
{0xC7,01,{0x25}},	
{0xC8,01,{0xA0}},	
{0xC9,01,{0x1C}},	
{0xCA,01,{0x28}},	
{0xCB,01,{0x81}},	
{0xCC,01,{0x1C}},	
{0xCD,01,{0x1C}},	
{0xCE,01,{0x51}},	
{0xCF,01,{0x25}},	
{0xD0,01,{0x2A}},	
{0xD1,01,{0x4D}},	
{0xD2,01,{0x59}},	
{0xD3,01,{0x23}},	
//============ Gamma END===========			
			

//CMD_Page 0			
{0xFF,03,{0x98,0x81,0x00}},
{0x35,01,{0x00}},
{0x11,01,{0x00}},
{REGFLAG_DELAY, 120, {}},
{0x29,01,{0x00}},
{REGFLAG_DELAY, 20, {}},

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
                //MDELAY(1);
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
    
        params->physical_width = 65;
        params->physical_height = 116;
    // enable tearing-free
    params->dbi.te_mode				= LCM_DBI_TE_MODE_DISABLED;
    params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

#if (LCM_DSI_CMD_MODE)
	params->dsi.mode = CMD_MODE;
#else
	params->dsi.mode   = SYNC_EVENT_VDO_MODE;//SYNC_EVENT_VDO_MODE;//BURST_VDO_MODE;////SYNC_PULSE_VDO_MODE
#endif

    // DSI
    /* Command mode setting */
		params->dsi.LANE_NUM				= LCM_THREE_LANE;//LCM_FOUR_LANE;
    
    //The following defined the fomat for data coming from LCD engine.
    params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
    params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST; 
    params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
    params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;
    // Highly depends on LCD driver capability.
    // Not support in MT6573
    // Video mode setting       
    params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
	
#if (LCM_DSI_CMD_MODE)
	params->dsi.intermediat_buffer_num = 0;//because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage
	params->dsi.word_count=FRAME_WIDTH*3;	//DSI CMD mode need set these two bellow params, different to 6577
#else
	params->dsi.intermediat_buffer_num = 0;	//because DSI/DPI HW design change, this parameters should be 0 when video mode in MT658X; or memory leakage
#endif
        params->physical_width = 69;
        params->physical_height = 138;
	params->dsi.packet_size=256;
	params->dsi.vertical_sync_active				=  10;//4 2 
	params->dsi.vertical_backporch					= 20;
	params->dsi.vertical_frontporch					= 20; 
	params->dsi.vertical_active_line				= FRAME_HEIGHT; 

        params->dsi.horizontal_sync_active                = 20; // 20 
        params->dsi.horizontal_backporch                = 70; //20 
        params->dsi.horizontal_frontporch                = 80; //20 
	params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
params->dsi.ssc_disable=1;

    params->dsi.PLL_CLOCK = 257;//270--->257 
	params->dsi.noncont_clock=1;
	params->dsi.noncont_clock_period=1;
	params->dsi.ssc_disable= 1;	
	params->dsi.esd_check_enable = 1;
	params->dsi.customization_esd_check_enable = 1;
    params->dsi.lcm_esd_check_table[0].cmd = 0x09; 
	params->dsi.lcm_esd_check_table[0].count = 1;
    params->dsi.lcm_esd_check_table[0].para_list[0] = 0x80; 
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
	MDELAY(120);
 
	
	push_table(lcm_initialization_setting_ST7703, sizeof(lcm_initialization_setting_ST7703) / sizeof(struct LCM_setting_table), 1);
}

static void lcm_suspend(void)
{

    unsigned int data_array[16];
  MDELAY(10);

    data_array[0]=0x00280500;
    dsi_set_cmdq(data_array, 1, 1);
    MDELAY(20);
    data_array[0]=0x00100500;

    dsi_set_cmdq(data_array, 1, 1);
	MDELAY(120);
    //SET_RESET_PIN(1);
    //MDELAY(20);
	SET_RESET_PIN(0);     //reset down
	  //MDELAY(10); // 1ms
	  //SET_RESET_PIN(1);
    /* 2V8 off*/
    MDELAY(20); 
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


LCM_DRIVER hct_ili9881d_dsi_vdo_hdp_ivo_57_dzx = 
{
    .name			= "hct_ili9881d_dsi_vdo_hdp_ivo_57_dzx",
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
