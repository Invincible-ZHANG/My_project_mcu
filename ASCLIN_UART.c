/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/
#include "IfxAsclin_Asc.h"
#include "IfxCpu_Irq.h"

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/
#define UART_BAUDRATE           115200                                  /* UART baud rate in bit/s                   */

#define UART_PIN_RX             IfxAsclin0_RXA_P14_1_IN                 /* UART receive port pin                     */
#define UART_PIN_TX             IfxAsclin0_TX_P14_0_OUT                 /* UART transmit port pin                    */

/* Definition of the interrupt priorities */
#define INTPRIO_ASCLIN0_RX      18
#define INTPRIO_ASCLIN0_TX      19

#define UART_RX_BUFFER_SIZE     64                                      /* Definition of the receive buffer size     */
#define UART_TX_BUFFER_SIZE     64                                      /* Definition of the transmit buffer size    */
/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/
static IfxAsclin_Asc g_UartHandle;                  /* Globle UART handle for operation in this file                  */

/* Declaration of the FIFOs parameters */
static uint8 g_ascTxBuffer[UART_TX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];
static uint8 g_ascRxBuffer[UART_RX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];
/*********************************************************************************************************************/
/*---------------------------------------------Function Implementations----------------------------------------------*/
/*********************************************************************************************************************/
/* Adding of the interrupt service routines */
IFX_INTERRUPT(asclin0TxISR, 0, INTPRIO_ASCLIN0_TX);
void asclin0TxISR()
{
    IfxAsclin_Asc_isrTransmit(&g_UartHandle);
}

IFX_INTERRUPT(asclin0RxISR, 0, INTPRIO_ASCLIN0_RX);
void asclin0RxISR()
{
    IfxAsclin_Asc_isrReceive(&g_UartHandle);
}

/********************************************************************
*  This function initializes the ASCLIN UART module
*
*  @param[in] void
*  @param[out] void
*
*  @retval void
********************************************************************/
void init_ASCLIN_UART()
{

    /* Initialize an instance of IfxAsclin_Asc_Config with default values */
    IfxAsclin_Asc_Config ascConfig;
    IfxAsclin_Asc_initModuleConfig(&ascConfig, &MODULE_ASCLIN0);

    /* Set the desired baud rate */
    ascConfig.baudrate.baudrate = UART_BAUDRATE;

    /* ISR priorities and interrupt target */
    ascConfig.interrupt.txPriority = INTPRIO_ASCLIN0_TX;
    ascConfig.interrupt.rxPriority = INTPRIO_ASCLIN0_RX;
    ascConfig.interrupt.typeOfService = IfxCpu_Irq_getTos(IfxCpu_getCoreIndex());

    /* FIFO configuration */
    ascConfig.txBuffer = &g_ascTxBuffer;
    ascConfig.txBufferSize = UART_TX_BUFFER_SIZE;
    ascConfig.rxBuffer = &g_ascRxBuffer;
    ascConfig.rxBufferSize = UART_RX_BUFFER_SIZE;

    /* Pin configuration */
    const IfxAsclin_Asc_Pins pins =
    {
        NULL_PTR,       IfxPort_InputMode_pullUp,     /* CTS pin not used */
        &UART_PIN_RX,   IfxPort_InputMode_pullUp,     /* RX pin           */
        NULL_PTR,       IfxPort_OutputMode_pushPull,  /* RTS pin not used */
        &UART_PIN_TX,   IfxPort_OutputMode_pushPull,  /* TX pin           */
        IfxPort_PadDriver_cmosAutomotiveSpeed1
    };
    ascConfig.pins = &pins;

    /*Initialize global UART handle */
    IfxAsclin_Asc_initModule(&g_UartHandle, &ascConfig); /* Initialize module with above parameters */

}

/********************************************************************
*  Send data via UART
*
*  @param[in] DataSize, length of data buffer
*  @param[in] *Data, pointer to the data buffer
*
*  @retval void
********************************************************************/
void Send_ASCLIN_UART_Message(char *Data, Ifx_SizeT DataSize)
{
    IfxAsclin_Asc_write(&g_UartHandle, Data, &DataSize, TIME_INFINITE);
}
