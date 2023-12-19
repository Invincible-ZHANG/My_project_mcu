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
    init_ASCLIN_UART();                         /* Initialize the module */
    IfxCpu_enableInterrupts();                  /* Enable interrupts after initialization */

    /* Init QSPI first for debug purpose */
    initQspiSlave(SPI0);
    initQspiSlave(SPI1);
    initQspiSlave(SPI4);

    /* Init CAN first for debug purpose */
    InitCanEnPin();
    initMcmcan();

    /* Function to initialize the EVADC */
    initEVADC();
    
    /* Init I2C */	
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

    Ifx_TickTime TicksFor100ms;                    /* Variable to store the number of ticks to wait for 1 second delay */
    TicksFor100ms = IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, 100);

    while(1)
    {
        SpiUpd_CheckAndPrint(SPI0);
        SpiUpd_CheckAndPrint(SPI1);
        SpiUpd_CheckAndPrint(SPI4);
        CanRx_CheckAndPrint();

        powerManagement();
        wait(TicksFor100ms *2);
        undervoltageDetection();
    }

}


void EC_Task_Test(void)
{
    EC_Ram_Operation();
}

void SPI_Task_Test()
{
    SpiTransferData(SPI0);
}

void Mcmcan_Task_Test()
{
    transmitCanMessage();
}
