#ifndef BUILD_LK
#include <linux/string.h>
#include <linux/kernel.h>
#endif
#include "lcm_drv.h"

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------
#define FRAME_WIDTH  										(720)
#define FRAME_HEIGHT 										(1520)
#define LCM_ID 0x38

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

{0x11,1,{0x00}},			                                              
{REGFLAG_DELAY,120,{}},  
{0xB9,3,{0xF1,0x12,0x83}},
{0xBA,27,{0x32,0x81,0x05,0xF9,0x0E,0x0E,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x44,0x25,0x00,0x90,0x0A,0x00,0x00,0x00,0x4F,0xD1,0x03,0x02,0x37}},
{0xB8,1,{0x26}},
{0xBF,3,{0x02,0x10,0x00}},
{0xB3,10,{0x07,0x0B,0x28,0x28,0x03,0xFF,0x00,0x00,0x00,0x00}},
{0xC0,9,{0x73,0x73,0x50,0x50,0x00,0x00,0x08,0x70,0x00}},
{0xBC,1,{0x46}},
{0xCC,1,{0x0B}},
{0xB4,1,{0x80}},
{0xB1,1,{0x85}},
{0xB2,3,{0x04,0x12,0xF0}},
{0xE3,14,{0x07,0x07,0x0B,0x0B,0x07,0x0B,0x00,0x00,0x00,0x00,0xFF,0x80,0xC0,0x17}},
{0xC1,15,{0x53,0x80,0x1E,0x1E,0x77,0xF1,0xFF,0xFF,0xEC,0xEC,0x7F,0x7F}},
{0xC6,4,{0x01,0x40,0xFF,0xFF}},
{0xB5,2,{0x09,0x09}},
//{0xB6,2,{0x5B,0x5B}},
{0xE9,63,{0xC2,0x10,0x05,0x05,0xF1,0x08,0xE0,0x12,0x31,0x23,0x37,0x82,0x08,0xE0,0x37,0x00,0x00,0x81,0x00,0x00,0x00,0x00,0x00,0x81,0x00,0x00,0x00,0x00,0xF8,0xAB,0x02,0x46,0x08,0x88,0x88,0x84,0x88,0x88,0x88,0xF8,0xAB,0x13,0x57,0x18,0x88,0x88,0x85,0x88,0x88,0x88,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
{0xEA,61,{0x96,0x12,0x01,0x01,0x00,0x3D,0x00,0x00,0x00,0x00,0x00,0x00,0x8F,0xAB,0x75,0x31,0x58,0x88,0x88,0x81,0x88,0x88,0x88,0x8F,0xAB,0x64,0x20,0x48,0x88,0x88,0x80,0x88,0x88,0x88,0x23,0x14,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x30,0x08,0xE0,0x00,0x00,0x00,0x00}},
{0xE0,34,{0x00,0x10,0x1C,0x29,0x3B,0x3F,0x4D,0x3F,0x06,0x0C,0x0E,0x12,0x13,0x11,0x13,0x12,0x18,0x00,0x10,0x1C,0x29,0x3B,0x3F,0x4D,0x3F,0x06,0x0C,0x0E,0x12,0x13,0x11,0x13,0x12,0x18}},

		                                             			       									
                                                     
{0x29, 1,{0x00}},                                                             
{REGFLAG_DELAY, 50, {0}},		                                                  

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
        params->physical_width = 68;
        params->physical_height = 136;
	params->dsi.packet_size=256;
	params->dsi.vertical_sync_active				=  4;//4 2 
	params->dsi.vertical_backporch					= 11;//50; 16
	params->dsi.vertical_frontporch					= 8;//50; 8
	params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				= 10; // 20
		params->dsi.horizontal_backporch				= 35; //20
		params->dsi.horizontal_frontporch				= 15; //20
	params->dsi.horizontal_active_pixel				= FRAME_WIDTH;


	params->dsi.PLL_CLOCK = 295;//270--->257--->295
	params->dsi.noncont_clock=1;
	params->dsi.noncont_clock_period=1;
	params->dsi.ssc_disable= 1;	
	params->dsi.esd_check_enable = 1;
	params->dsi.customization_esd_check_enable = 1;
        params->dsi.lcm_esd_check_table[0].cmd          = 0x68;
	params->dsi.lcm_esd_check_table[0].count = 1;
        params->dsi.lcm_esd_check_table[0].para_list[0] = 0xC0;
        params->dsi.lcm_esd_check_table[1].cmd          = 0x09;
        params->dsi.lcm_esd_check_table[1].count        = 3;
        params->dsi.lcm_esd_check_table[1].para_list[0] = 0x80;
        params->dsi.lcm_esd_check_table[1].para_list[1] = 0x73;
        params->dsi.lcm_esd_check_table[1].para_list[2] = 0x04;		
        params->dsi.lcm_esd_check_table[2].cmd          = 0xAF;
        params->dsi.lcm_esd_check_table[2].count        = 1;
        params->dsi.lcm_esd_check_table[2].para_list[0] = 0xFD;
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
      data_array[0]=0x00e01500;
    dsi_set_cmdq(data_array, 1, 1);
  MDELAY(10);
        data_array[0]=0x93e11500;
    dsi_set_cmdq(data_array, 1, 1);
    MDELAY(10);
        data_array[0]=0x65e21500;
    dsi_set_cmdq(data_array, 1, 1);
    MDELAY(10);
        data_array[0]=0xf8e31500;
    dsi_set_cmdq(data_array, 1, 1);
    MDELAY(10);
        data_array[0]=0x01e11500;
    dsi_set_cmdq(data_array, 1, 1);
    MDELAY(10);
        data_array[0]=0x00191500;
    dsi_set_cmdq(data_array, 1, 1);
    MDELAY(10);
        data_array[0]=0x001c1500;
    dsi_set_cmdq(data_array, 1, 1);
    MDELAY(10);  
          data_array[0]=0x00e01500;
    dsi_set_cmdq(data_array, 1, 1);
    MDELAY(10);

    data_array[0]=0x00280500;
    dsi_set_cmdq(data_array, 1, 1);
	MDELAY(10);
    data_array[0]=0x00100500;

    dsi_set_cmdq(data_array, 1, 1);
	MDELAY(100);
    //SET_RESET_PIN(1);
    //MDELAY(20);
	SET_RESET_PIN(0);     //reset down
	  //MDELAY(10); // 1ms
	  //SET_RESET_PIN(1);
    /* 2V8 off*/
	MDELAY(10);
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
	unsigned int id=0;
	unsigned int id1=0;
	unsigned int id2=0;
       
       
        
    //Do reset here
    SET_RESET_PIN(1);
    MDELAY(10);
    SET_RESET_PIN(0);
	MDELAY(10);
    SET_RESET_PIN(1);
	MDELAY(120);
   
    array[0]=0x00023700;
    dsi_set_cmdq(array, 1, 1);
	MDELAY(10);
	read_reg_v2(0xda, buffer, 2);
    
	id = buffer[0];
	read_reg_v2(0xdb, buffer, 2);
	id1 = buffer[0];
	read_reg_v2(0xdc, buffer, 2);
	id2 = buffer[0];   
       // id = (id_midd &lt;&lt; 8) | id_low;

    #ifdef BUILD_LK
	printf("st7703 id = 0x%08x\n",  id);
	#else

	printk("st7703 id = 0x%08x\n",  id);
	#endif
	//return 1;	
	return (LCM_ID == id)?1:0;
	 

}


LCM_DRIVER hct_st7703_dsi_vdo_hdp_sc_57_gz = 
{
    .name			= "hct_st7703_dsi_vdo_hdp_sc_57_gz",
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
