WHAT IS THIS?
===================
 This is a LCM Collection for:
  * MTK Kernels
  * LCMs are from others kernels sources, aren't extracted by me!!!

INSTRUCTIONS!
=============
If you wanna use one of the LCMs that is situated here you need a kernel for the SOC of your device, for example MT6580, then you will need to search a kernel for that device, clone it. And see which LCM use your phone. If you know that, you only need to place LCM at **drivers/misc/mediatek/lcm**, then you will need to add the LCM function, sometimes they are at these files:
 * mt65xx_lcm_list.c
 * mt65xx_lcm_list.h

You only need to add the lines that has a +, for know the name of the LCM Driver you are gonna add you need to check at the .c file of your LCM normally you will find like this:
        LCM_DRIVER  = {
            .name			= "",

Example of MT6757CD:
====================
 * mt65xx_lcm_list.c
        LCM_DRIVER *lcm_driver_list[] = {
        +#if defined(HCT_RM67120_DSI_VDO_HD_GVO)
        +        &hct_rm67120_dsi_vdo_hd_gvo,
        +#endif
        #if defined(TD4310_TFT60_HXGD_LFHD)
                &td4310_tft60_hxgd_lfhd_lcm_drv,
        #endif
 * mt65xx_lcm_list.h
        #include <lcm_drv.h>
        +extern LCM_DRIVER hct_rm67120_dsi_vdo_hd_gvo;
        extern LCM_DRIVER otm1282a_hd720_dsi_vdo_60hz_lcm_drv;

If your device has different SOC, LCM functions can be situated only at mt65xx_lcm_list.h:

Example of a kernel that only have it at mt65xx_lcm_list.h
===========================================================
 * mt65xx_lcm_list.h
        #define __MT65XX_LCM_LIST_H__
        +extern LCM_DRIVER hct_nt35521_dsi_vdo_hd_cmi_55_rx;
        extern LCM_DRIVER hct_rm68200_dsi_vdo_hd_auo_50_ykl;

        LCM_DRIVER *lcm_driver_list[] = {
        +#if defined(HCT_NT35521_DSI_VDO_HD_CMI_55_RX)
        +    &hct_nt35521_dsi_vdo_hd_cmi_55_rx,
        +#endif
        #if defined(HCT_RM68200_DSI_VDO_HD_AUO_50_YKL)
            &hct_rm68200_dsi_vdo_hd_auo_50_ykl,
        #endif

If the video driver of your kernel use for example set_backlight_cmdq, you will need to modify your LCM driver, for make it work with your kernel.

Example of how to fix this issue:
=================================
 * First of all take a look to another LCM of your kernel.

 * Second see how they use set_backlight_cmdq function.

 * Third implement it:
        - static void lcm_setbacklight(unsigned int level)
        + static void lcm_setbacklight_cmdq(void *handle, unsigned int level)
        
        -    .set_backlight = lcm_setbacklight,
        +    .set_backlight_cmdq = lcm_setbacklight_cmdq,

Examples of defconfig modifications for make LCM work:
=====================================================
 * First of all, try to extract defconfig, normally is situated on a file situated at /proc named config.gz. If your device doesn't have this file at /proc, I recommend you to use a defconfig of your kernel, you will need to add it at "arch/arm/configs" if your device is arm, and if your device is arm64 your will need to add it at "arch/arm64/configs".

 * Second, you need to set the correct LCM name, the one that is being used by your device:
        -CONFIG_CUSTOM_KERNEL_LCM="td4310_fhd_dsi_vdo_chuangwei_malata"
        +CONFIG_CUSTOM_KERNEL_LCM="hct_rm67120_dsi_vdo_hd_gvo"

 * Third, you need to see which is the HEIGHT and WIDTH of your device, and set the correct values.
        -CONFIG_LCM_HEIGHT="1920"
        +CONFIG_LCM_HEIGHT="1280"
        -CONFIG_LCM_WIDTH="1080"
        +CONFIG_LCM_WIDTH="720"

 * Fourth, you need to extract the .dts from your stock kernel. And you will need to add it at "arch/arm/boot/dts/mediatek" if your device is arm, and if your device is arm64 you will need to add it at "arch/arm64/boot/dts/mediatek", when you have finished making this, you will need to add the correct dts name at your defconfig.
        -CONFIG_BUILD_ARM64_APPENDED_DTB_IMAGE_NAMES="mediatek/N1"
        +CONFIG_BUILD_ARM64_APPENDED_DTB_IMAGE_NAMES="mediatek/MIX"


#If any of these LCMs worked for you, give the corrects credits.
