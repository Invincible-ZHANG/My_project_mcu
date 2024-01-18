#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"
// #include "SPI_CPU.h"
// #include "ASCLIN_UART.h"
// #include <string.h>
// #include <stdio.h>
// #include "Bsp.h"

extern IfxCpu_syncEvent g_cpuSyncEvent;

void core1_main(void)
{
    IfxCpu_enableInterrupts();
    
    /* !!WATCHDOG1 IS DISABLED HERE!!
     * Enable the watchdog and service it periodically if it is required
     */
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    
    /* Wait for CPU sync event */
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 1);

    // char UartTxOutput[128];                         /* Debug string buffer for UART */
    // Ifx_SizeT BufferLen;                            /* Debug string buffer length for UART */

    // Ifx_TickTime TicksFor1s;                    /* Variable to store the number of ticks to wait for 1 second delay */
    // TicksFor1s = IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, 1000);

    // int countTimer = 0;
    // char temp[16] = {'\0'};

    // wait(TicksFor1s* 5);
    
    while(1)
    {
        // while(!CheckIfCPU1ShouldStart())
        // {;}
        // CheckQspiIntPinAndReTrans(SPI1);
        // strcpy(UartTxOutput, "Core1 : Debug time : { ");
        // sprintf(temp, "%d  s };\n\r", countTimer);
        // strcat(UartTxOutput, temp);
        // strcat(UartTxOutput, "Core1 : CAN transform successfully_infineon!!!!!!!\n\r\n\r\n\r\n\r");
        // BufferLen = (Ifx_SizeT)strlen(UartTxOutput);
        // Send_ASCLIN_UART_Message(UartTxOutput, BufferLen);
        // countTimer++;
        // wait(TicksFor1s);
        

    }
}
