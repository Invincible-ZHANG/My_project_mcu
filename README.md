# MCU项目代码优化
## 实习项目代码优化，没有实际跑，只是重构了一下代码，并非实际发布版本

===========================

MCU配置：
|           | 数量     | 频率       | 闪存大小   | SRAM大小   |
|-----------|---------|------------|------------|------------|
| CPU       | 4 TriCore | 300 MHz  | -          | -          |
|           |           |            | 10 MB      | 1376 KB    |
|           |           |            | 512 KB DataFlash | - |



## 项目介绍
	本项目是基于英飞凌TC387的MCU开发，主要完成了SOC与EC的通信，以及ADC检测，电源管理等。

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
这里写一下为什么上海版本不能使用电源管理：
因为使用电源管理进行高低压检测后，会拉eFuse的状态，从而是SOC掉电，可参见位号图Q5，以及英飞凌的用户手册，可看EVADC的参考电压的提供为SOC供电12V，从而造成，eFuse掉电后，ADC也掉电，从而失去了对电压检测的功能，所以这个问题的出现，是硬件设计问题，需要进行手动飞线，或者改板，JWIPC已经通知手动改板，上海因为没有硬件部门，所以直接选择提供没有电源管理的版本来规避这个问题，如果硬件问题在之后的板子上已经解决，则不太需要因为这个区分两个版本。




2023/11/21：
1.将AO的板子升级为A1的板子，很多硬件接口发生了变化，所以从新修改了UART和I2C的功能，一下是新增的功能。

	UART：较上一个版本没有发生任何的变化；

	I2C：增加了状态转换的功能：
		1.S3->S0
		2.S5->S0
		3.增加了PHY 和EC Power button pin控制：
			1）MCU直接拉PWRBTN，EC检测到PWRBTN的下降沿之后唤醒
			2）PHY网络功能
2.增加 GPIO初始化文件，保证eFuse会被开机的时候上电拉起来，并将A2B功能、SOC eFuse、PHY初始化设置。

3.新增ADC功能，进行电压检测。

	因为IPCM和EC连在同一个I2C上，所以必须增加ADC检测，保证电压的控制使I2C能够确保连接在EC上，所以加入了判断函数在ADC文件里，ADC实现了电源管理等功能，电源管理功能会和P22，8前压检测功能相斥，需要硬件飞线短路某个MOS管，已经通知*****，*****所需要的版本不需要电源管理功能及欠压检测功能，所以需要两个version。

2023/11/27:
1.增加CAN和SPI相关配置。



## 之前各版本的Release Note
ISSUE BY: zijian
-------------------------------------------------------------------------------
Release A1 TC387 MCU Firmware Version : 02.01  
-------------------------------------------------------------------------------

FILENAME: [SHANGHAI_A1_0201_TC387_11_20.hex], DATE: [2023/11/20]

RELEASE NOTES:
* Version for SHANGHAI
	
BUG FIXES:
* Compared to the 01.02 version, the High and Low Voltage power management function has been removed, and the undervoltage detection will affect the power-up state of the efuse due to hardware-related issues. The Shanghai-specific version canceled this feature.


KNOWN ISSUES:
* NULL


ISSUE BY: zijian
-------------------------------------------------------------------------------
Release A1 TC387 MCU Firmware Version : 01.02  
-------------------------------------------------------------------------------

FILENAME: [A1_TC387_11_17.hex], DATE: [2023/11/17]

RELEASE NOTES:
* New version
	
BUG FIXES:
* Added High and Low Voltage Power Management Strategy. Detect the voltage value of AN9.
* Added the CAN data return function to return the received CAN data.


KNOWN ISSUES:
* NULL



ISSUE BY: zijian
-------------------------------------------------------------------------------
Release A1 TC387 MCU Firmware Version : 01.01  
-------------------------------------------------------------------------------

FILENAME: [A1_TC387_11_8.hex], DATE: [2023/11/8]

RELEASE NOTES:
* New version
	
BUG FIXES:
* Fix the MCU to SOC GPIO ERROR   &MODULE_P21 ,3
* Remove CANFD , change CMD(S5->S0) in 8 bits


KNOWN ISSUES:
* NULL




ISSUE BY: zijian
-------------------------------------------------------------------------------
Release A1 TC387 MCU Firmware Version : 01.00  
-------------------------------------------------------------------------------

FILENAME: [A1_TC387_11_7.hex], DATE: [2023/11/7]

RELEASE NOTES:
* Last Version is a test version(2023/11/6),this is a first release version Fireware(2023/11/7 01.00).
	
BUG FIXES:
* Add A2B control pin.Initialize the pin is Mode_outputPushPull and high status.
* Add PHY control,The PHY exits sleep by being controlled by the MCU P00_8, which sends a high pulse to the 88Q2122.(About 10 ms)
* S3->S0 by pulling power button about 30ms,No longer via EC
* S5->S0 by pulling power button about 120ms,No longer via EC
* Adapt all functions from A0 to A1(SPI,UART,CAN,I2C).

KNOWN ISSUES:
* NULL




