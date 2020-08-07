#ifndef BUILD_LK
#include <linux/string.h>
#include <linux/kernel.h>
#endif
#include "lcm_drv.h"

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------
	#define FRAME_WIDTH  (720)
	#define FRAME_HEIGHT (1520)
#define LCM_ID                       (0x991)

#define REGFLAG_DELAY               (0XFED)
#define REGFLAG_END_OF_TABLE        (0x100) // END OF REGISTERS MARKER


#define LCM_DSI_CMD_MODE                                    0

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
extern int mt_dsi_pinctrl_set(unsigned int pin , unsigned int level);
extern int PMU_db_pos_neg_setting_delay(int ms);
extern int PMU_db_pos_neg_disable_delay(int ms);

static struct LCM_setting_table lcm_initialization_setting_ST7703[] = 
{

{0x0A, 1, {0x08}},
{0xF0,2,{0x5A,0x5A}},
{0xF1,2,{0xA5,0xA5}},
{0xC2, 1, {0x00}},
{0xB0,16,{0x21,0x54,0x76,0x54,0x66,0x66,0x33,0x33,0x0c,0x03,0x03,0x8c,0x03,0x03,0x0F,0x00}},
{0xB1, 16, {0x11,0xD4,0x02,0x86,0x00,0x01,0x01,0x82,0x01,0x01,0x53,0x00,0x00,0x00,0x00,0x00}},
{0xB2,16,{0x67,0x2A,0x05,0x8A,0x65,0x02,0x08,0x20,0x30,0x91,0x22,0x33,0x44,0x00,0x18,0xA1}},
{0xB3,16,{0x01,0x00,0x00,0x33,0x00,0x26,0x26,0xC0,0x3F,0xAA,0x33,0xC3,0xAA,0x30,0xC3,0xAA}},
{0xB6,16,{0x0a,0x02,0x14,0x15,0x1b,0x02,0x02,0x02,0x02,0x13,0x11,0x02,0x02,0x0F,0x0D,0x05}},
{0xB4,16,{0x0b,0x02,0x14,0x15,0x1b,0x02,0x02,0x02,0x02,0x12,0x10,0x02,0x02,0x0E,0x0C,0x04}},
{0xB9, 2, {0x1e,0x1E}},
{0xBB, 16, {0x00,0x00,0x00,0x00,0x02,0xFF,0xFC,0x0B,0x13,0x01,0x73,0x44,0x44,0x00,0x00,0x00}},
{0xBC, 10, {0x61,0x03,0xff,0xDE,0x72,0xE0,0x2E,0x04,0x88,0x3e}},
{0xBD, 16, {0x6E,0x0E,0x65,0x65,0x15,0x15,0x50,0x32,0x14,0x66,0x23,0x02,0x00,0x00,0x00,0x00}},
{0xBE, 5, {0x60,0x60,0x50,0x60,0x77}},
{0xC1, 16, {0x70,0x7c,0x0c,0x7c,0x04,0x0C,0x10,0x04,0x2A,0x31,0x00,0x07,0x10,0x10,0x00,0x00}},
{0xC3, 8, {0x00,0x00,0xFF,0x00,0xFF,0x00,0x00,0x0d}},
{0xC4, 8, {0xB4,0xA3,0xee,0x41,0x04,0x2F,0x00,0x00}},
{0xC5, 12, {0x07,0x1F,0x42,0x26,0x52,0x44,0x14,0x1A,0x04,0x00,0x0A,0x08}},
{0xC6, 16, {0x81,0x01,0x67,0x01,0x33,0xA0,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},
//####Gamma
{0xC7, 16, {0x7C,0x5F,0x4D,0x63,0x50,0x3B,0x2C,0x33,0x20,0x3D,0x3F,0x31,0x4D,0x47,0x55,0x47}},
{0xC8, 5, {0x34,0x5F,0x37,0x26,0x0C}},
{0xC9, 16, {0x7C,0x5F,0x4D,0x63,0x50,0x3B,0x2C,0x33,0x20,0x3D,0x3F,0x31,0x4D,0x47,0x55,0x47}},
{0xCA, 5, {0x34,0x5F,0x37,0x26,0x0C}},
{0xCB, 11, {0x00,0x00,0x00,0x01,0x6C,0x00,0x33,0x00,0x17,0xFF,0xEF}},
{0xF0, 2, {0xB4,0x4B}},
{0xD0, 8, {0x80,0x0D,0xFF,0x0F,0x63,0x2B,0x08,0x08}},
{0xD2, 10, {0x43,0x0C,0x00,0x01,0x80,0x26,0x04,0x00,0x16,0x42}},
{0xd5, 1, {0x0f}},
{0x35, 1, {0x00}},
{0xF0, 2, {0xA5,0xA5}},
{0xF1,2,{0x5A,0x5A}},
{0x11,1,{0x00}},
{REGFLAG_DELAY,120,{}},
{0x29,1,{0x00}},
{REGFLAG_DELAY,20,{}},
{REGFLAG_END_OF_TABLE, 0x00, {}},
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

	params->dsi.vertical_sync_active = 4;
	params->dsi.vertical_backporch = 12;
	params->dsi.vertical_frontporch = 124;
	params->dsi.vertical_active_line = FRAME_HEIGHT;

	params->dsi.horizontal_sync_active = 4;
	params->dsi.horizontal_backporch = 36;//12->30-36
	params->dsi.horizontal_frontporch = 36;//16-30-36
	params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
	params->dsi.HS_TRAIL = 20; 
  

// zhangxiaofei add for test
	params->dsi.PLL_CLOCK = 280;//
	
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


	PMU_db_pos_neg_setting_delay(12);
    MDELAY(20);
    SET_RESET_PIN(1);
    MDELAY(10);
    SET_RESET_PIN(0);
    MDELAY(20);
    SET_RESET_PIN(1);
    MDELAY(120);
 
	
	push_table(lcm_initialization_setting_ST7703, sizeof(lcm_initialization_setting_ST7703) / sizeof(struct LCM_setting_table), 1);
}

static void lcm_suspend(void)
{

    unsigned int data_array[16];


    data_array[0]=0x00280500;
    dsi_set_cmdq(data_array, 1, 1);
    MDELAY(20);
    data_array[0]=0x00100500;

    dsi_set_cmdq(data_array, 1, 1);
    MDELAY(150);
    //SET_RESET_PIN(1);
    //MDELAY(20);
    //SET_RESET_PIN(0);
	  //MDELAY(10); // 1ms
	  //SET_RESET_PIN(1);
	  //MDELAY(20); 
	PMU_db_pos_neg_disable_delay(10);
//    MDELAY(5);
}

static void lcm_resume(void)
{

     lcm_init();

    /// please notice: the max return packet size is 1
    /// if you want to change it, you can refer to the following marked code
    /// but read_reg currently only support read no more than 4 bytes....
    /// if you need to read more, please let BinHan knows.
    /*
            unsigned int data_array[16];
            unsigned int max_return_size = 1;
            
            data_array[0]= 0x00003700 | (max_return_size << 16);    
            
            dsi_set_cmdq(&data_array, 1, 1);
    


	unsigned int data_array[16];

    MDELAY(100);
    data_array[0]=0x00290500;
    dsi_set_cmdq(data_array,1,1);
    MDELAY(10);
*/
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


LCM_DRIVER hct_icnl991_dsi_vdo_hdp_ivo_62_kl = 
{
    .name			= "hct_icnl991_dsi_vdo_hdp_ivo_62_kl",
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
