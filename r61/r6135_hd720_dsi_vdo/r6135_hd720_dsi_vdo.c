#include <linux/string.h>
#include <linux/kernel.h>
#include <lcm_drv.h>

// ---------------------------------------------------------------------------
#include <linux/wait.h>
#include <linux/time.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/kernel.h>
#include <linux/module.h>  
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/irq.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#ifdef CONFIG_MTK_LEGACY
#include <mach/mt_gpio.h>
#include <cust_gpio_usage.h>
#include <cust_i2c.h>
#else
#include <mt-plat/mt_gpio.h>
#endif

//  Local Constants
// ---------------------------------------------------------------------------
#define	FRAME_WIDTH		(720)
#define FRAME_HEIGHT		(1280)


#define   LCM_HSYNC_NUM		(20)  
#define   LCM_HBP_NUM		(80)  
#define   LCM_HFP_NUM		(80) 
    
#define   LCM_VSYNC_NUM		(2)  
#define   LCM_VBP_NUM		(13)  
#define   LCM_VFP_NUM		(16)   

#define   LCM_LINE_BYTE		((FRAME_WIDTH+LCM_HSYNC_NUM+LCM_HBP_NUM+LCM_HFP_NUM)*3)

#define REGFLAG_DELAY		0XFE
#define REGFLAG_END_OF_TABLE	0xFF   // END OF REGISTERS MARKER

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

#ifndef BUILD_LK
//static unsigned int lcm_esd_test = FALSE;      ///only for ESD test
#endif
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
#define dsi_set_cmdq_V3(para_tbl,size,force_update)  lcm_util.dsi_set_cmdq_V3(para_tbl,size,force_update)
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)

#define wrtie_cmd(cmd)			       lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)     lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)			       lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)  lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

#define set_gpio_lcd_enp(cmd) \
		lcm_util.set_gpio_lcd_enp_bias(cmd)
#define set_gpio_lcd_enn(cmd) \
		lcm_util.set_gpio_lcd_enn_bias(cmd)

#define   LCM_DSI_CMD_MODE	0

#define GPIO_LCD_BIAS_ENP_PIN	(100|0x80000000)
#define GPIO_LCD_BIAS_ENN_PIN	(108|0x80000000)
#define GPIO_LCM_BL_EN	(63|0x80000000)

static bool is_lcm_suspend = 0;


#if 0
extern int tpd_lcm_resume();
extern int tpd_lcm_suspend();

static void tp_control_func(struct work_struct *work) {
    printk("hx8394f tp_control_func\n\r");
    if (is_lcm_suspend == 1) {
        printk("hx8394f tp suspend\n\r");
        tpd_lcm_suspend();
    }
    else if (is_lcm_suspend == 0) {
        printk("hx8394f tp resume\n\r");
        tpd_lcm_resume();
    }
}
#endif


/*****************************************************************************
** Gate driver function
******************************************************************************/
#include <linux/module.h>
int lcm_gate_write_bytes(unsigned char addr, unsigned char value);

static void lcm_gate_enable(int enable)
{
    #if 0
    int ret;
    unsigned char cmd, data;
    #endif
    
    printk("[Kernel] %s = %d \n", __func__, enable);
    if( TRUE == enable )
    {
    	mt_set_gpio_mode(GPIO_LCD_BIAS_ENP_PIN, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_LCD_BIAS_ENP_PIN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_LCD_BIAS_ENP_PIN, GPIO_OUT_ONE);
        MDELAY(5);
	mt_set_gpio_mode(GPIO_LCD_BIAS_ENN_PIN, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_LCD_BIAS_ENN_PIN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_LCD_BIAS_ENN_PIN, GPIO_OUT_ONE);
        MDELAY(5);
      #if 0
        cmd  = 0x00;
        data = 0x0F;
        ret = lcm_gate_write_bytes(cmd, data);
	if(ret<0)
            printk("lcm gate I2C write cmd=%0x error \n", cmd);

        cmd  = 0x01;
        data = 0x0F;
        ret = lcm_gate_write_bytes(cmd, data);
	if(ret<0)
            printk("lcm gate I2C write cmd=%0x error \n", cmd);
       #endif	
    }
    else
    {
	mt_set_gpio_mode(GPIO_LCD_BIAS_ENN_PIN, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_LCD_BIAS_ENN_PIN, GPIO_DIR_OUT);
	//[SM31][RaymondLin]Disable LCM +-5V voltage when system suspend begin 
	mt_set_gpio_out(GPIO_LCD_BIAS_ENN_PIN, GPIO_OUT_ZERO);
	//[SM31][RaymondLin]Disable LCM +-5V voltage when system suspend end
        MDELAY(5);
    	mt_set_gpio_mode(GPIO_LCD_BIAS_ENP_PIN, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_LCD_BIAS_ENP_PIN, GPIO_DIR_OUT);
	//[SM31][RaymondLin]Disable LCM +-5V voltage when system suspend begin 
	mt_set_gpio_out(GPIO_LCD_BIAS_ENP_PIN, GPIO_OUT_ZERO);
	//[SM31][RaymondLin]Disable LCM +-5V voltage when system suspend end
        MDELAY(5);
    }
}

struct LCM_setting_table {
	unsigned int cmd;
	unsigned char count;
	unsigned char para_list[64];
};


static struct LCM_setting_table lcm_init_setting[] = 
{
  /*
  Note :

  Data ID will depends on the following rule.
	
	count of parameters > 1	=> Data ID = 0x39
	count of parameters = 1	=> Data ID = 0x15
	count of parameters = 0	=> Data ID = 0x05

  Structure Format :

  {DCS command, count of parameters, {parameter list}}
  {REGFLAG_DELAY, milliseconds of time, {}},

  ...

  Setting ending by predefined flag
	
  {REGFLAG_END_OF_TABLE, 0x00, {}}
  */
#if 1
 //#Manufacture Command
  {0xB0,1,{ 0x00 }},
  {0xCC,1,{ 0x04 }},
  {0xE3,1,{ 0x01 }}, 
  //#DSI control
 //[SM31][RaymondLin]Fix LCM horizontal white line issue begin 
  {0xB6,2,{ 0x62, 0x2C }},
 //[SM31][RaymondLin]Fix LCM horizontal white line issue end 
  //#display setting
  {0xC0,6,{ 0x23,0xB2,0x0F,0x10,0xC2,0x7F }},
  //#display h-timing setting
  {0xC1,22,{0x0B,0x6F,0x01,0x80,0x00,0x00,0x00,0x00,0x7F,0x03,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08 }},
  {0xC3,1,{ 0x75 }},
  //#display V-timing setting
  {0xC5,28,{0x06,0x06,0x40,0x43,0x00,0x00,0x03,0x01,0x80,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0A,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x11,0x11,0x0E }},
  //#panel drive setting
  {0xC6,2,{ 0x01,0x02 }},
  //#GOUT pin assignment
  {0xC8,58,{0x09,0x13,0x11,0x00,0x00,0x26,0x24,0x22,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x0A,0x13,0x11,0x00,0x00,0x27,0x25,0x23,0x21,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x89,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x02,0x00,0x00,0x00,0x03,0x00 }},
  //#Gamma setting
 {0xCA,38,{0x1B,0x29,0x32,0x41,0x4C,0x56,0x6C,0x7D,0x8B,0x97,0x4A,0x56,0x63,0x76,0x7F,0x8C,0x9B,0xA8,0xBF,0x1B,0x29,0x32,0x41,0x4C,0x56,0x6C,0x7D,0x8B,0x97,0x4A,0x56,0x63,0x76,0x7F,0x8C,0x9B,0xA8,0xBF }},
 //#power setting for VCI1/2/3, DC2/3
 //[SM31][Raymond]fix HW test case fail in -20 degree -begin
{0xD0,6,{ 0x01,0x4B,0x41,0x00,0x35,0x99 }},
 //[SM31][Raymond]fix HW test case fail in -20 degree -end
//#power setting for VCL,VCLLVL
{0xD1,1,{ 0x03 }},
//#power setting for external booster
{0xD2,2,{ 0x8E,0x0B }},
//#register write control
{0xE5,1,{ 0x02 }},
//#VOCMDC setting
 //[SM31][Raymond]fix HW test case fail in -20 degree -begin
{0xD4,2,{ 0x00,0xC5 }},
 //[SM31][Raymond]fix HW test case fail in -20 degree -end
//#VPLVL/VNLVL setting
{0xD5,2,{ 0x24,0x24 }},
  
{0x35,1,{ 0x00 }},
//[SM31][RaymondLin]LCM Reverse 180 degree begin 
{0x36,1,{ 0x03 }},
//[SM31][RaymondLin]LCM Reverse 180 degree end
{0x51,1,{ 0xFF }},
{0x53,1,{ 0x2C }},
{0x55,1,{ 0x01 }},
#endif
  //Sleep Out
  {0x11, 0, {}},
  {REGFLAG_DELAY, 120, {}}, 

  //Display ON
  {0x29, 0, {}},
  {REGFLAG_DELAY, 20, {}}, 

  // Setting ending by predefined flag
  {REGFLAG_END_OF_TABLE, 0x00, {}}
};

//[SM31][RaymondLin] LCM into deep standby mode when system suspend - begin 
static struct LCM_setting_table lcm_suspend_setting[] = {
	{0x28, 0, {} },// Display Off
	{REGFLAG_DELAY, 20, {} },
	{0x10, 0, {} },// Dleep In
	{0xB0, 1, {0x00} },
	{0xB1, 1, {0x01} },//Deep standby In
	{REGFLAG_DELAY, 80, {} },
};
//[SM31][RaymondLin] LCM into deep standby mode when system suspend - end
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

    #if (LCM_DSI_CMD_MODE)
        params->dsi.mode   = CMD_MODE;
    #else
        params->dsi.mode   = BURST_VDO_MODE; //SYNC_PULSE_VDO_MODE;//BURST_VDO_MODE; 
    #endif
	
    // DSI
    /* Command mode setting */
    //1 Three lane or Four lane
    params->dsi.LANE_NUM		= LCM_FOUR_LANE;
    //The following defined the fomat for data coming from LCD engine.
    params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

    // Video mode setting		
    params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

    params->dsi.vertical_sync_active	= LCM_VSYNC_NUM;
    params->dsi.vertical_backporch	= LCM_VBP_NUM;
    params->dsi.vertical_frontporch	= LCM_VFP_NUM;
    params->dsi.vertical_active_line	= FRAME_HEIGHT; 

    params->dsi.horizontal_sync_active	= LCM_HSYNC_NUM;
    params->dsi.horizontal_backporch	= LCM_HBP_NUM;
    params->dsi.horizontal_frontporch	= LCM_HFP_NUM;
    params->dsi.horizontal_active_pixel	= FRAME_WIDTH;

    //params->dsi.LPX=8; 

    // Bit rate calculation
    params->dsi.ssc_range 	= 4;
    params->dsi.ssc_disable	= 1;
    //params->dsi.PLL_CLOCK	= 231;// 240;
//[SM31][RaymondLin]Fix LCM horizontal white line issue begin 	
    params->dsi.PLL_CLOCK = 220;//240;
//[SM31][RaymondLin]Fix LCM horizontal white line issue end	
}

static void lcm_init(void)
{
    lcm_gate_enable(1);

    SET_RESET_PIN(1);
    MDELAY(2); 
    SET_RESET_PIN(0);
    MDELAY(5); 
    SET_RESET_PIN(1);
    MDELAY(5); 

    push_table(lcm_init_setting, sizeof(lcm_init_setting) / sizeof(struct LCM_setting_table), 1);
    //dsi_set_cmdq_V3(lcm_init_setting,sizeof(lcm_init_setting)/sizeof(lcm_init_setting[0]),1);
    printk("lcm init in kernel is done \n");
}



static void lcm_suspend(void)
{
//[SM31][RaymondLin] LCM into deep standby mode when system suspend - begin
    //unsigned int data_array[16];
//[SM31][RaymondLin] LCM into deep standby mode when system suspend - end	
printk("[Kernel] %s ++\n", __func__);
//[SM31][RaymondLin]Disable BL_EN pin when system suspend -begin
	mt_set_gpio_mode(GPIO_LCM_BL_EN, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_LCM_BL_EN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_LCM_BL_EN, GPIO_OUT_ZERO);
//[SM31][RaymondLin]Disable BL_EN pin when system suspend -begin
//[SM31][RaymondLin] LCM into deep standby mode when system suspend - begin
push_table(lcm_suspend_setting, sizeof(lcm_suspend_setting) / sizeof(struct LCM_setting_table), 1);
MDELAY(10);
    
//[SM31][RaymondLin] LCM into deep standby mode when system suspend - end
    is_lcm_suspend = 1;
    lcm_gate_enable(0);
printk("[Kernel] %s --\n", __func__);	
}


static void lcm_resume(void)
{
    printk("[Kernel] %s ++\n",__func__);
	lcm_init();

    is_lcm_suspend = 0;

//[SM31][RaymondLin]Enable BL_EN pin when system resume -begin
	mt_set_gpio_mode(GPIO_LCM_BL_EN, GPIO_MODE_00);
	mt_set_gpio_dir(GPIO_LCM_BL_EN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_LCM_BL_EN, GPIO_OUT_ONE);
//[SM31][RaymondLin]Enable BL_EN pin when system suspend -begin	
    printk("[Kernel] %s --\n",__func__);
}
         
#if (LCM_DSI_CMD_MODE)
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
     return 1;
}

#if 0
static unsigned int lcm_esd_check(void)
{
  #ifndef BUILD_LK
	char  buffer[3];
	int   array[4];

	if(lcm_esd_test)
	{
		lcm_esd_test = FALSE;
		return TRUE;
	}

	array[0] = 0x00013700;
	dsi_set_cmdq(array, 1, 1);

	read_reg_v2(0x36, buffer, 1);
	if(buffer[0]==0x90)
	{
		return FALSE;
	}
	else
	{			 
		return TRUE;
	}
#else
	return FALSE;
#endif

}

static unsigned int lcm_esd_recover(void)
{
	lcm_init();
	lcm_resume();

	return TRUE;
}
#endif


LCM_DRIVER r6135_hd720_dsi_vdo_lcm_drv = 
{
    .name			= "r6135_hd720_dsi_vdo",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = lcm_compare_id,
	//.esd_check = lcm_esd_check,
	//.esd_recover = lcm_esd_recover,
#if (LCM_DSI_CMD_MODE)
    .update         = lcm_update,
#endif
    };
