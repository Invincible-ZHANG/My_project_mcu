/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/
#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "SPI_CPU.h"
#include "Bsp.h"
#include "GPIO_Init.h"
#include "IfxAsclin_Asc.h"
#include "IfxCpu_Irq.h"
#include "ASCLIN_UART.h"
#include "I2C_Read_EC.h"
#include "MCMCAN.h"
#include "ADC_VIN.h"

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/

// #define EFUSE_PIN       &MODULE_P22, 7                          /* eFuse                 */
#define LOG_OUTPUT_INTERVAL 1000000                                /* Debug log interval time*/

// #define EC_PD1_GP2       &MODULE_P33, 9

/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/
IFX_ALIGN(4) IfxCpu_syncEvent g_cpuSyncEvent = 0;

/*********************************************************************************************************************/
/*------------------------------------------------Function Prototypes------------------------------------------------*/
/*********************************************************************************************************************/
void EC_Task_Test(void);
void SPI_Task_Test(void);
void Mcmcan_Task_Test(void);

/*********************************************************************************************************************/
/*-------------------------------------------------Main Function-----------------------------------------------------*/
/*********************************************************************************************************************/
void core0_main(void)
{
    IfxCpu_enableInterrupts();
    
    /* !!WATCHDOG0 AND SAFETY WATCHDOG ARE DISABLED HERE!!
     * Enable the watchdogs and service them periodically if it is required
     */
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

    /* Wait for CPU sync event */
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 1);


/*********************************************************************************************************************/
/*-------------------------------------------------Peripheral init---------------------------------------------------*/
/*********************************************************************************************************************/
    /* Init GPIO table */
    Init_GPIO_Table();

    /* Init UART first for debug purpose */
    /**************************************** 
    * ASCLIN-0                              *
    * RX : IfxAsclin0_RXA_P14_1_IN          *
    * TX : IfxAsclin0_TX_P14_0_OUT          *
    ****************************************/
    init_ASCLIN_UART();                         /* Initialize the module */
    IfxCpu_enableInterrupts();                  /* Enable interrupts after initialization */

    /**************************************** 
    * QSPI-0 as slave                       *
    * SCLK : IfxQspi0_SCLKA_P20_11_IN       *
    * MTSR : IfxQspi0_MTSRA_P20_14_IN       *
    * MRST : IfxQspi0_MRST_P20_12_OUT       *
    * SLSI : IfxQspi0_SLSIA_P20_13_IN       *
    * QSPI-1 as slave                       *
    * SCLK : IfxQspi1_SCLKA_P10_2_IN        *
    * MTSR : IfxQspi1_MTSRC_P10_4_IN        *
    * MRST : IfxQspi1_MRST_P10_1_OUT        *
    * SLSI : IfxQspi1_SLSIA_P11_10_IN       *
    * QSPI-4 as slave                       *
    * SCLK : IfxQspi1_SCLKA_P10_2_IN        *
    * MTSR : IfxQspi1_MTSRC_P10_4_IN        *
    * MRST : IfxQspi1_MRST_P10_1_OUT        *
    * SLSI : IfxQspi1_SLSIA_P11_10_IN       *
    ****************************************/
    initQspiSlave(SPI0);
    initQspiSlave(SPI1);
    initQspiSlave(SPI4);

    /**************************************** 
    * Mcmcan 0 Node 2                       *
    * *************************************** 
    * RX : IfxCan_RXD02A_P15_1_IN           *
    * TX : IfxCan_TXD02_P15_0_OUT           *
    ****************************************/
    InitCanEnPin();
    initMcmcan();

    /****************************************
     * Function to initialize the EVADC
     * EVADC G2 CH0
     ****************************************
     * AN16
     ***************************************/
    initEVADC();
    
    /* Init I2C */
    /**************************************** 
    * I2C-0                                 *
    * SCL : IfxI2c0_SCL_P13_1_INOUT         *
    * SDA : IfxI2c0_SDA_P13_2_INOUT         *
    ****************************************/	
    judgeI2cSdaVal();
    init_I2C_module();

/*********************************************************************************************************************/
/*----------------------------------------------Peripheral Operation-------------------------------------------------*/
/*********************************************************************************************************************/
    /* Start EC R/W task */
    EC_Task_Test();

    /* Start data transfer via QSPI */
    SPI_Task_Test();

    /* Start data transfer via Mcmcan */
    Mcmcan_Task_Test();    

    // char UartTxOutput[128];                         /* Debug string buffer for UART */
    // Ifx_SizeT BufferLen;                            /* Debug string buffer length for UART */

    Ifx_TickTime TicksFor100ms;                    /* Variable to store the number of ticks to wait for 1 second delay */
    TicksFor100ms = IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, 100);

    // // int countTimer = 0;
    // char temp[16] = {'\0'};
    // // char efuse[16] = {'\0'};
//    int logCounter = 0;

    // int res;

    while(1)
    {
        /* Debug only -- Start */
        SpiUpd_CheckAndPrint(SPI0);
        SpiUpd_CheckAndPrint(SPI1);
        SpiUpd_CheckAndPrint(SPI4);
        CanRx_CheckAndPrint();

        powerManagement();
        wait(TicksFor100ms *2);
        undervoltageDetection();
        
        // int value_efuse;    
        // value_efuse = IfxPort_getPinState (EFUSE_PIN);
        // /*Print Log via UART*/
        // strcpy(UartTxOutput, "Core0 : Debug time : { ");
        // sprintf(temp, "%d  s };\n\r", countTimer);
        // sprintf(efuse, "%d  };\n\r", value_efuse);
        // strcat(UartTxOutput, temp);
        // strcat(UartTxOutput, efuse);
        // strcat(UartTxOutput, "Core0 : CAN transform successfully_infineon!!!!!!!\n\r\n\r\n\r\n\r");
        // BufferLen = (Ifx_SizeT)strlen(UartTxOutput);
        // Send_ASCLIN_UART_Message(UartTxOutput, BufferLen);
        // countTimer++;
        // wait(TicksFor1s);

        // EC_Task_Test();
        
        // 控制日志输出的频率
        // if (logCounter % LOG_OUTPUT_INTERVAL == 0)
        // {
        //     Send_ASCLIN_UART_Message("MCU is running...\n\r", 25);
        // }
        // logCounter++;

   

        // res = IfxPort_getPinState (EC_PD1_GP2) ;
        // strcpy(UartTxOutput, "Core0 : Debug time : { ");
        // sprintf(temp, "%d   };\n\r", res);
        // strcat(UartTxOutput, temp);
        // BufferLen = (Ifx_SizeT)strlen(UartTxOutput);
        // Send_ASCLIN_UART_Message(UartTxOutput, BufferLen);
        /* Debug only -- End */
    }

}


void EC_Task_Test(void)
{
    EC_Ram_Operation();
}

void SPI_Task_Test()
{
    SpiTransferData(SPI0);
    // SpiTransferData(SPI1);
    // SpiTransferData(SPI4);
}

void Mcmcan_Task_Test()
{
    transmitCanMessage();
}
