



# Infineon_TC387_Codebase


===========================

## 项目介绍
	本项目是基于英飞凌TC387的MCU开发，主要完成了SOC与EC的通信，以及ADC检测，电源管理等（PerDV）。


### 目录结构描述

```
.
│  .cproject
│  .project
│  ADC_VIN.c                        //ADC 功能
│  ADC_VIN.h
│  ASCLIN_UART.c                    //UART 
│  ASCLIN_UART.h
│  Cpu0_Main.c						//主程序跑在CORE0
│  Cpu1_Main.c
│  Cpu2_Main.c
│  Cpu3_Main.c
│  git_codebase.launch
│  GPIO_Init.c                      //初始化所有GPIO，保证eFuse上电
│  GPIO_Init.h
│  I2C_Read_EC.c					//I2C
│  I2C_Read_EC.h
│  Lcf_Gnuc_Tricore_Tc.lsl
│  Lcf_Tasking_Tricore_Tc.lsl
│  list.txt							//文件目录
│  MCMCAN.c							//CAN功能
│  MCMCAN.h
│  README.md						//阅读指引
│  SPI_CPU.c						//SPI功能
│  SPI_CPU.h
│  TC387_CodeBase_Master.launch
│  
├─.ads
├─.metadata
├─.settings
├─Configurations
└─Libraries

```

===
### 存在的问题
这里写一下为什么SH版本不能使用电源管理：
因为使用电源管理进行高低压检测后，会拉eFuse的状态，从而是SOC掉电，可参见位号图Q5，以及英飞凌的用户手册，可看EVADC的参考电压的提供为SOC供电12V，从而造成，eFuse掉电后，ADC也掉电，从而失去了对电压检测的功能，所以这个问题的出现，是硬件设计问题，需要进行手动飞线，或者改板，XXXXX已经通知手动改板，SH因为没有硬件部门，所以直接选择提供没有电源管理的版本来规避这个问题，如果硬件问题在之后的板子上已经解决，则不太需要因为这个区分两个版本。








