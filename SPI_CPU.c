/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/
#include "SPI_CPU.h"
#include "IfxPort.h"
#include "IfxStm.h"
#include "ASCLIN_UART.h"
#include "Bsp.h"
#include <string.h>
#include <stdio.h>
#include "IfxQspi_SpiSlave.h"
#include "MCMCAN.h"
#include "I2C_Read_EC.h"

// #include "IfxGtm_Tom_Timer.h"
// #include "IfxStm_Timer.h"

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/
/* Interrupt Service Routine priorities for Slave SPI communication */
#define ISR_PRIORITY_SPI0_SLAVE_TX 50
#define ISR_PRIORITY_SPI0_SLAVE_RX 51
#define ISR_PRIORITY_SPI0_SLAVE_ER 52
#define ISR_PRIORITY_SPI1_SLAVE_TX 53
#define ISR_PRIORITY_SPI1_SLAVE_RX 54
#define ISR_PRIORITY_SPI1_SLAVE_ER 55
#define ISR_PRIORITY_SPI4_SLAVE_TX 56
#define ISR_PRIORITY_SPI4_SLAVE_RX 57
#define ISR_PRIORITY_SPI4_SLAVE_ER 58

#define DO_NOT_CARE_PIN_PORT NULL_PTR
#define DO_NOT_CARE_PIN_INDEX 0

#define TIMEOUT_LIMIT 1000000

/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/
QspiSlaveComm g_Qspi0Slave; /* Global SPI slave handle for operation in this file            */
QspiSlaveComm g_Qspi1Slave;
QspiSlaveComm g_Qspi2Slave;
QspiSlaveComm g_Qspi3Slave;
QspiSlaveComm g_Qspi4Slave;
QspiSlaveComm *g_QspiLookUptable[5] = {&g_Qspi0Slave, &g_Qspi1Slave, &g_Qspi2Slave, &g_Qspi3Slave, &g_Qspi4Slave};
uint8 g_Spi0SlaveTransTestData[SPI_BUFFER_SIZE] = {0x53, 0x50, 0x49, 0x30}; /* 'S', 'P', 'I', '0' */
uint8 g_Spi1SlaveTransTestData[SPI_BUFFER_SIZE] = {0x53, 0x50, 0x49, 0x31}; /* 'S', 'P', 'I', '1' */
uint8 g_Spi4SlaveTransTestData[SPI_BUFFER_SIZE] = {0x53, 0x50, 0x49, 0x34}; /* 'S', 'P', 'I', '4' */
/* Debug only */
boolean g_NeedPrint_Spi0IsUpdated = FALSE; /* Global flag for SPI Tx ISR debug mseg print */
boolean g_NeedPrint_Spi1IsUpdated = FALSE;
boolean g_NeedPrint_Spi2IsUpdated = FALSE;
boolean g_NeedPrint_Spi3IsUpdated = FALSE;
boolean g_NeedPrint_Spi4IsUpdated = FALSE;
boolean *g_SpiIsUpdated_LookUptable[5] = {&g_NeedPrint_Spi0IsUpdated, &g_NeedPrint_Spi1IsUpdated, &g_NeedPrint_Spi2IsUpdated, &g_NeedPrint_Spi3IsUpdated, &g_NeedPrint_Spi4IsUpdated};
boolean g_StartCPU1Loop = FALSE;

// /* Sleep mode test   */
// IfxStm_Timer g_stmTimer;                        /* STM driver handle                                                */
// float32 g_stmPeriod = 2.0;                      /* Period in seconds at which power modes toggle                    */
// uint8 g_sleep = 0;                              /* Variable used to monitor the current set mode (sleep/run mode)   */

/*********************************************************************************************************************/
/*------------------------------------------------Function Prototypes------------------------------------------------*/
/*********************************************************************************************************************/
void initQSPISlaveHandle(QspiSlaveComm *QspiSlave);
void initQSPISlaveBuffers(QspiSlaveComm *QspiSlave, uint8 *TransferData);
void InitQspiHwResource(QspiSlaveComm *QspiSlave, uint8 SpiIndex);
void InitQspiIntPin(QspiSlaveComm *QspiSlave);
boolean CheckSlaveIsBusy(QspiSlaveComm *QspiSlave);
void UartDbgMsg_ForTxIsr(QspiSlaveComm *QspiSlave, uint8 SpiIndex);
void UartDbgMsg_ForRxIsr(QspiSlaveComm *QspiSlave, uint8 SpiIndex);
// void initStm(void);
// void switchMode(void);

/*********************************************************************************************************************/
/*----------------------------------------------Function Implementations---------------------------------------------*/
/*********************************************************************************************************************/
/* Adding of the interrupt service routines */
IFX_INTERRUPT(Spi0SlaveTxISR, 0, ISR_PRIORITY_SPI0_SLAVE_TX); /* SPI Slave ISR for transmit data          */
IFX_INTERRUPT(Spi0SlaveRxISR, 0, ISR_PRIORITY_SPI0_SLAVE_RX); /* SPI Slave ISR for receive data           */
IFX_INTERRUPT(Spi0SlaveErISR, 0, ISR_PRIORITY_SPI0_SLAVE_ER); /* SPI Slave ISR for error                  */

void Spi0SlaveTxISR()
{
    IfxCpu_enableInterrupts();
    IfxQspi_SpiSlave_isrTransmit(&g_Qspi0Slave.SpiSlaveHandle);
}

void Spi0SlaveRxISR()
{
    IfxCpu_enableInterrupts();
    IfxQspi_SpiSlave_isrReceive(&g_Qspi0Slave.SpiSlaveHandle);
    /* Debug only */
    *g_SpiIsUpdated_LookUptable[0] = TRUE;
}

void Spi0SlaveErISR()
{
    IfxCpu_enableInterrupts();
    IfxQspi_SpiSlave_isrError(&g_Qspi0Slave.SpiSlaveHandle);
}

IFX_INTERRUPT(Spi1SlaveTxISR, 0, ISR_PRIORITY_SPI1_SLAVE_TX); /* SPI Slave ISR for transmit data          */
IFX_INTERRUPT(Spi1SlaveRxISR, 0, ISR_PRIORITY_SPI1_SLAVE_RX); /* SPI Slave ISR for receive data           */
IFX_INTERRUPT(Spi1SlaveErISR, 0, ISR_PRIORITY_SPI1_SLAVE_ER); /* SPI Slave ISR for error                  */

void Spi1SlaveTxISR()
{
    IfxCpu_enableInterrupts();
    IfxQspi_SpiSlave_isrTransmit(&g_Qspi1Slave.SpiSlaveHandle);
}

void Spi1SlaveRxISR()
{
    IfxCpu_enableInterrupts();
    IfxQspi_SpiSlave_isrReceive(&g_Qspi1Slave.SpiSlaveHandle);
    /* Debug only */
    *g_SpiIsUpdated_LookUptable[1] = TRUE;
}

void Spi1SlaveErISR()
{
    IfxCpu_enableInterrupts();
    IfxQspi_SpiSlave_isrError(&g_Qspi1Slave.SpiSlaveHandle);
}

IFX_INTERRUPT(Spi4SlaveTxISR, 0, ISR_PRIORITY_SPI4_SLAVE_TX); /* SPI Slave ISR for transmit data          */
IFX_INTERRUPT(Spi4SlaveRxISR, 0, ISR_PRIORITY_SPI4_SLAVE_RX); /* SPI Slave ISR for receive data           */
IFX_INTERRUPT(Spi4SlaveErISR, 0, ISR_PRIORITY_SPI4_SLAVE_ER); /* SPI Slave ISR for error                  */

void Spi4SlaveTxISR()
{
    IfxCpu_enableInterrupts();
    IfxQspi_SpiSlave_isrTransmit(&g_Qspi4Slave.SpiSlaveHandle);
}

void Spi4SlaveRxISR()
{
    IfxCpu_enableInterrupts();
    IfxQspi_SpiSlave_isrReceive(&g_Qspi4Slave.SpiSlaveHandle);
    /* Debug only */
    *g_SpiIsUpdated_LookUptable[4] = TRUE;
}

void Spi4SlaveErISR()
{
    IfxCpu_enableInterrupts();
    IfxQspi_SpiSlave_isrError(&g_Qspi4Slave.SpiSlaveHandle);
}

// /* Sleep mode test   */
// IFX_INTERRUPT(stmIsr, 0, ISR_PRIORITY_STM);

// /* STM ISR to switch between run mode and sleep mode every two seconds */
// void stmIsr(void)
// {
//     /* Enabling interrupts as ISR disables it */
//     IfxCpu_enableInterrupts();

//     /* Clear the timer event */
//     IfxStm_Timer_acknowledgeTimerIrq(&g_stmTimer);

//     /* Switch between run mode and sleep mode   */
//     switchMode();
// }

/********************************************************************
 *  This function initializes the QSPI module in Slave mode.
 *
 *  @param[in] void
 *  @param[out] QspiSlaveComm *QspiSlave
 *
 *  @retval void
 ********************************************************************/
void initQSPISlaveHandle(QspiSlaveComm *QspiSlave)
{
    IfxQspi_SpiSlave_Config spiSlaveConfig;                                   /* Define the Slave configuration           */
    IfxQspi_SpiSlave_initModuleConfig(&spiSlaveConfig, (*QspiSlave).SpiPort); /* Initialize it with default values        */

    spiSlaveConfig.pins = (*QspiSlave).SpiSlavepin; /* Assign Slave port pins                   */
    /* Set the ISR priorities and the service provider */
    spiSlaveConfig.base.txPriority = (*QspiSlave).ISR_PRIORITY_SLAVE_TX;
    spiSlaveConfig.base.rxPriority = (*QspiSlave).ISR_PRIORITY_SLAVE_RX;
    spiSlaveConfig.base.erPriority = (*QspiSlave).ISR_PRIORITY_SLAVE_ER;
    spiSlaveConfig.base.isrProvider = IfxSrc_Tos_cpu0;

    /* Initialize QSPI Slave module */
    IfxQspi_SpiSlave_initModule(&(*QspiSlave).SpiSlaveHandle, &spiSlaveConfig);
}

/************************************************
 *  QSPI Slave SW buffer initialization
 *
 *  @param[in] void
 *  @param[out] QspiSlaveComm *QspiSlave
 *
 *  @retval void
 ************************************************/
void initQSPISlaveBuffers(QspiSlaveComm *QspiSlave, uint8 *TransferData)
{
    for (uint8 i = 0; i < SPI_BUFFER_SIZE; i++)
    {
        (*QspiSlave).SpiSlaveTxBuffer[i] = TransferData[i]; /* Init TX Slave Buffer                    */
        (*QspiSlave).SpiSlaveRxBuffer[i] = 0;               /* Clear RX Slave Buffer                   */
    }
}

/************************************************
 *  QSPI Slave HW resource initialization
 *
 *  @param[in] void
 *  @param[out] QspiSlaveComm *QspiSlave
 *
 *  @retval void
 ************************************************/
void InitQspiHwResource(QspiSlaveComm *QspiSlave, uint8 SpiIndex)
{
    IFX_CONST IfxQspi_SpiSlave_Pins qspi0SlavePins = {
        &IfxQspi0_SCLKA_P20_11_IN, IfxPort_InputMode_pullDown,  /* SCLK Pin                       (CLK)     */
        &IfxQspi0_MTSRA_P20_14_IN, IfxPort_InputMode_pullDown,  /* MasterTransmitSlaveReceive pin (MOSI)    */
        &IfxQspi0_MRST_P20_12_OUT, IfxPort_OutputMode_pushPull, /* MasterReceiveSlaveTransmit pin (MISO)    */
        &IfxQspi0_SLSIA_P20_13_IN, IfxPort_InputMode_pullDown,  /* SlaveSelect Pin                (CS)      */
        IfxPort_PadDriver_cmosAutomotiveSpeed3                  /* Pad driver mode                          */
    };

    IFX_CONST IfxQspi_SpiSlave_Pins qspi1SlavePins = {
        &IfxQspi1_SCLKA_P10_2_IN, IfxPort_InputMode_pullDown,  /* SCLK Pin                       (CLK)     */
        &IfxQspi1_MTSRC_P10_4_IN, IfxPort_InputMode_pullDown,  /* MasterTransmitSlaveReceive pin (MOSI)    */
        &IfxQspi1_MRST_P10_1_OUT, IfxPort_OutputMode_pushPull, /* MasterReceiveSlaveTransmit pin (MISO)    */
        &IfxQspi1_SLSIA_P11_10_IN, IfxPort_InputMode_pullDown, /* SlaveSelect Pin                (CS)      */
        IfxPort_PadDriver_cmosAutomotiveSpeed3                 /* Pad driver mode                          */
    };

    IFX_CONST IfxQspi_SpiSlave_Pins qspi4SlavePins = {
        &IfxQspi4_SCLKB_P22_3_IN, IfxPort_InputMode_pullDown,  /* SCLK Pin                       (CLK)     */
        &IfxQspi4_MTSRB_P22_0_IN, IfxPort_InputMode_pullDown,  /* MasterTransmitSlaveReceive pin (MOSI)    */
        &IfxQspi4_MRST_P22_1_OUT, IfxPort_OutputMode_pushPull, /* MasterReceiveSlaveTransmit pin (MISO)    */
        &IfxQspi4_SLSIB_P22_2_IN, IfxPort_InputMode_pullDown,  /* SlaveSelect Pin                (CS)      */
        IfxPort_PadDriver_cmosAutomotiveSpeed3                 /* Pad driver mode                          */
    };

    switch (SpiIndex)
    {
    case SPI0:
        (*QspiSlave).SpiPort = &MODULE_QSPI0;
        (*QspiSlave).SpiSlavepin = &qspi0SlavePins;
        (*QspiSlave).ISR_PRIORITY_SLAVE_TX = ISR_PRIORITY_SPI0_SLAVE_TX;
        (*QspiSlave).ISR_PRIORITY_SLAVE_RX = ISR_PRIORITY_SPI0_SLAVE_RX;
        (*QspiSlave).ISR_PRIORITY_SLAVE_ER = ISR_PRIORITY_SPI0_SLAVE_ER;
        (*QspiSlave).SlaveTransTestData = g_Spi0SlaveTransTestData;
        (*QspiSlave).port = DO_NOT_CARE_PIN_PORT;
        //(*QspiSlave).pinIndex = 2;
        (*QspiSlave).pinIndex = DO_NOT_CARE_PIN_INDEX; // Only P23_3 is GPIO-IN in SOC side.
        break;
    case SPI1:
        (*QspiSlave).SpiPort = &MODULE_QSPI1;
        (*QspiSlave).SpiSlavepin = &qspi1SlavePins;
        (*QspiSlave).ISR_PRIORITY_SLAVE_TX = ISR_PRIORITY_SPI1_SLAVE_TX;
        (*QspiSlave).ISR_PRIORITY_SLAVE_RX = ISR_PRIORITY_SPI1_SLAVE_RX;
        (*QspiSlave).ISR_PRIORITY_SLAVE_ER = ISR_PRIORITY_SPI1_SLAVE_ER;
        (*QspiSlave).SlaveTransTestData = g_Spi1SlaveTransTestData;
        (*QspiSlave).port = &MODULE_P21;
        (*QspiSlave).pinIndex = 3;
        break;
    case SPI4:
        (*QspiSlave).SpiPort = &MODULE_QSPI4;
        (*QspiSlave).SpiSlavepin = &qspi4SlavePins;
        (*QspiSlave).ISR_PRIORITY_SLAVE_TX = ISR_PRIORITY_SPI4_SLAVE_TX;
        (*QspiSlave).ISR_PRIORITY_SLAVE_RX = ISR_PRIORITY_SPI4_SLAVE_RX;
        (*QspiSlave).ISR_PRIORITY_SLAVE_ER = ISR_PRIORITY_SPI4_SLAVE_ER;
        (*QspiSlave).SlaveTransTestData = g_Spi4SlaveTransTestData;
        (*QspiSlave).port = DO_NOT_CARE_PIN_PORT;
        (*QspiSlave).pinIndex = DO_NOT_CARE_PIN_INDEX;
        break;
    default:
    {
        /* Debug Print Only --Start*/
        char UartTxOutput[128]; /* Debug string buffer for UART */
        Ifx_SizeT BufferLen;    /* Debug string buffer length for UART */

        strcpy(UartTxOutput, "SPI-Error: invalid SPI Slave!\n\r\n\r");
        BufferLen = (Ifx_SizeT)strlen(UartTxOutput);
        Send_ASCLIN_UART_Message(UartTxOutput, BufferLen);
        /* Debug Print Only --End*/
    }
    break;
    }
}

/************************************************
 *  Init SPI comm INT pin. Active high
 *
 *  @param[in] void
 *  @param[out] QspiSlaveComm *QspiSlave
 *
 *  @retval void
 ************************************************/
void InitQspiIntPin(QspiSlaveComm *QspiSlave)
{
    IfxPort_setPinMode((*QspiSlave).port, (*QspiSlave).pinIndex, IfxPort_Mode_outputPushPullGeneral);
    IfxPort_setPinState((*QspiSlave).port, (*QspiSlave).pinIndex, IfxPort_State_low);
}

/************************************************
 *  Qspi slave initialization
 *
 *  @param[in] uint8 SpiIndex
 *  @param[out] void
 *
 *  @retval void
 ************************************************/
void initQspiSlave(uint8 SpiIndex)
{
    InitQspiHwResource(g_QspiLookUptable[SpiIndex], SpiIndex);
    /* Initialize the Slave and its buffer */
    initQSPISlaveHandle(g_QspiLookUptable[SpiIndex]);
    initQSPISlaveBuffers(g_QspiLookUptable[SpiIndex], (*g_QspiLookUptable[SpiIndex]).SlaveTransTestData);
    if (SpiIndex == SPI1)
    {
        /* Initialize P23_3 as GPIO. Used for SPI communication INT to notify host that data is ready */
        InitQspiIntPin(g_QspiLookUptable[SpiIndex]);
    }
}

/************************************************
 *  start the data transfer
 *
 *  @param[in] SpiIndex
 *  @param[out] void
 *
 *  @retval void
 ************************************************/
void SpiTransferData(uint8 SpiIndex)
{
    while (CheckSlaveIsBusy(g_QspiLookUptable[SpiIndex]))
    {
    }
    /* Instruct the SPI Slave to receive a data stream of defined length */
    IfxQspi_SpiSlave_exchange(&(*g_QspiLookUptable[SpiIndex]).SpiSlaveHandle, &(*g_QspiLookUptable[SpiIndex]).SpiSlaveTxBuffer[0], &(*g_QspiLookUptable[SpiIndex]).SpiSlaveRxBuffer[0], SPI_BUFFER_SIZE);

    /* Toggle INT to notify host to read data. */
    if (SpiIndex == SPI1)
    {
        IfxPort_setPinState((*g_QspiLookUptable[SpiIndex]).port, (*g_QspiLookUptable[SpiIndex]).pinIndex, IfxPort_State_high);
        // if (!g_StartCPU1Loop) g_StartCPU1Loop = TRUE;
    }

    /* Debug Print Only --Start*/
    char UartTxOutput[128]; /* Debug string buffer for UART */
    Ifx_SizeT BufferLen;    /* Debug string buffer length for UART */
    char temp[16] = {'\0'};

    strcpy(UartTxOutput, "SPI");
    sprintf(temp, "%d", SpiIndex);
    strcat(UartTxOutput, temp);

    strcat(UartTxOutput, "-Slave: Tx data is ready to be exchanged. Pull High Interrupt Pin!\n\r\n\r");
    BufferLen = (Ifx_SizeT)strlen(UartTxOutput);
    Send_ASCLIN_UART_Message(UartTxOutput, BufferLen);
    /* Debug Print Only --End*/
}

/* Debug only */
/************************************************
 *  Check global update flag and print Rx/Tx value
 *
 *  @param[in] uint8 SpiIndex
 *  @param[out] void
 *
 *  @retval void
 ************************************************/
void SpiUpd_CheckAndPrint(uint8 SpiIndex)
{
    if (*g_SpiIsUpdated_LookUptable[SpiIndex])
    {
        UartDbgMsg_ForTxIsr(g_QspiLookUptable[SpiIndex], SpiIndex);
        UartDbgMsg_ForRxIsr(g_QspiLookUptable[SpiIndex], SpiIndex);
        *g_SpiIsUpdated_LookUptable[SpiIndex] = FALSE;
        if (SpiIndex == SPI1)
        {
            IfxPort_setPinState((*g_QspiLookUptable[SpiIndex]).port, (*g_QspiLookUptable[SpiIndex]).pinIndex, IfxPort_State_low);
        }
    }
}

/************************************************
 *  Check INT pin status. If it's low, wait 10s to
 *  start next transfer.
 *
 *  @param[in] uint8 SpiIndex
 *  @param[out] void
 *
 *  @retval void
 ************************************************/
void CheckQspiIntPinAndReTrans(uint8 SpiIndex)
{
    if (!IfxPort_getPinState((*g_QspiLookUptable[SpiIndex]).port, (*g_QspiLookUptable[SpiIndex]).pinIndex))
    {
        Ifx_TickTime TicksFor1s; /* Variable to store the number of ticks to wait for 1 second delay */
        TicksFor1s = IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, 1000);

        wait(TicksFor1s * 5);
        initQSPISlaveBuffers(g_QspiLookUptable[SpiIndex], (*g_QspiLookUptable[SpiIndex]).SlaveTransTestData);
        SpiTransferData(SpiIndex);
    }
}

/************************************************
 *  Global flag to control when the loop in CPU1
 *  should be started
 *
 *  @param[in] void
 *  @param[out] void
 *
 *  @retval void
 ************************************************/
boolean CheckIfCPU1ShouldStart(void)
{
    if (g_StartCPU1Loop)
        return TRUE;
    else
        return FALSE;
}

/************************************************
 *  Check if slave is busy now
 *
 *  @param[in] void
 *  @param[out] QspiSlaveComm *QspiSlave
 *
 *  @retval TRUE -  Slave is busy
 *  @retval FALSE - Slave is idle
 ************************************************/
boolean CheckSlaveIsBusy(QspiSlaveComm *QspiSlave)
{
    if (IfxQspi_SpiSlave_getStatus(&(*QspiSlave).SpiSlaveHandle) == SpiIf_Status_busy)
    {
        return TRUE;
    }
    else
        return FALSE;
}

/************************************************
 *  Print Tx buffer data
 *
 *  @param[in] void
 *  @param[out] QspiSlaveComm *QspiSlave
 *
 *  @retval void
 ************************************************/
void UartDbgMsg_ForTxIsr(QspiSlaveComm *QspiSlave, uint8 SpiIndex)
{
    char UartTxOutput[1024]; /* Debug string buffer for UART */
    Ifx_SizeT BufferLen;     /* Debug string buffer length for UART */
    char temp[16] = {'\0'};

    if (SpiIndex > 4)
    {
        strcpy(UartTxOutput, "SPI Index error - No such SPI device!");
        BufferLen = (Ifx_SizeT)strlen(UartTxOutput);
        Send_ASCLIN_UART_Message(UartTxOutput, BufferLen);
        return;
    }

    strcpy(UartTxOutput, "SPI");
    sprintf(temp, "%d", SpiIndex);
    strcat(UartTxOutput, temp);

    strcat(UartTxOutput, "-Slave: Tx data = { ");

    for (uint8 i = 0; i < SPI_BUFFER_SIZE; i++)
    {
        sprintf(temp, "0x%02X ", (*QspiSlave).SpiSlaveTxBuffer[i]);
        strcat(UartTxOutput, temp);
        /* Clear Tx buffer */
        (*QspiSlave).SpiSlaveTxBuffer[i] = 0;
    }

    strcat(UartTxOutput, "};\n\r\n\r");
    BufferLen = (Ifx_SizeT)strlen(UartTxOutput);
    Send_ASCLIN_UART_Message(UartTxOutput, BufferLen);
}

/************************************************
 *  Print Rx buffer data
 *
 *  @param[in] void
 *  @param[out] QspiSlaveComm *QspiSlave
 *
 *  @retval void
 ************************************************/
void UartDbgMsg_ForRxIsr(QspiSlaveComm *QspiSlave, uint8 SpiIndex)
{
    char UartTxOutput[1024]; /* Debug string buffer for UART */
    Ifx_SizeT BufferLen;     /* Debug string buffer length for UART */
    char temp[16] = {'\0'};

    if (SpiIndex > 4)
    {
        strcpy(UartTxOutput, "SPI Index error - No such SPI device!");
        BufferLen = (Ifx_SizeT)strlen(UartTxOutput);
        Send_ASCLIN_UART_Message(UartTxOutput, BufferLen);
        return;
    }

    strcpy(UartTxOutput, "SPI");
    sprintf(temp, "%d", SpiIndex);
    strcat(UartTxOutput, temp);

    strcat(UartTxOutput, "-Slave: Rx data = { ");

    for (uint8 i = 0; i < SPI_BUFFER_SIZE; i++)
    {
        sprintf(temp, "0x%02X ", (*QspiSlave).SpiSlaveRxBuffer[i]);
        strcat(UartTxOutput, temp);
        /* Clear Rx buffer */
        (*QspiSlave).SpiSlaveRxBuffer[i] = 0;
    }

    strcat(UartTxOutput, "};\n\r\n\r");
    BufferLen = (Ifx_SizeT)strlen(UartTxOutput);
    Send_ASCLIN_UART_Message(UartTxOutput, BufferLen);
}

/************************************************
 *  Clear SPI TX BUFFER
 *
 *  @param[in] uint8 SpiIndex - Which SPI used to send CMD
 *  @param[out] void
 *
 *  @retval void
 ************************************************/

// void ClearSpiTransmitBuffer(uint8 SpiIndex)
// {
//     //Ensure that SpiIndex is valid
//     if (SpiIndex > 4)
//     {
//         return;
//     }

//     while ((*g_QspiLookUptable[SpiIndex]).SpiSlaveTxBuffer[0] != 0)
//     {
//         (*g_QspiLookUptable[SpiIndex]).SpiSlaveTxBuffer[0] = 0;

//     }
//     IfxQspi_SpiSlave_exchange(&(*g_QspiLookUptable[SpiIndex]).SpiSlaveHandle, &(*g_QspiLookUptable[SpiIndex]).SpiSlaveTxBuffer[0], &(*g_QspiLookUptable[SpiIndex]).SpiSlaveRxBuffer[0], SPI_BUFFER_SIZE);
//     SpiUpd_CheckAndPrint(SPI1);
// }

/************************************************
 *  Execute command to transfer SOC system power
 *  state from S0 to S3
 *
 *  @param[in] uint8 SpiIndex - Which SPI used to send CMD
 *  @param[out] void
 *
 *  @retval void
 ************************************************/
void PowerTrans_S0_To_S3(uint8 SpiIndex)
{
    uint8 RegRxBuffer_PWR_STATUS[LENGTH_OF_PWR_STATUS] = {0};
    // uint8 RegTxBuffer_PWR_STATUS[LENGTH_OF_PWR_STATUS] = {0x0};
    char UartTxOutput[128]; /* Debug string buffer for UART */
    Ifx_SizeT BufferLen;    /* Debug string buffer length for UART */

    // TIMEOUT
    int startTime = 0;

    Read_EC_RAM_Data(RAMOFFS_OF_PWR_STATUS, &RegRxBuffer_PWR_STATUS[0], LENGTH_OF_PWR_STATUS);

    switch (RegRxBuffer_PWR_STATUS[0])
    {
    case SOC_S0:
        strcpy(UartTxOutput, "EC RAM Read System Power Status = { S0 }, Will trans to S3!\n\r\n\r");
        BufferLen = (Ifx_SizeT)strlen(UartTxOutput);
        Send_ASCLIN_UART_Message(UartTxOutput, BufferLen);
        char UartTxOutput1[128]; /* Debug string buffer for UART */
        char UartTxOutput2[128]; /* Debug string buffer for UART */

        Ifx_TickTime TicksFor100ms; /* Variable to store the number of ticks to wait for 1 second delay */
        TicksFor100ms = IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, 100);
        int countTimer = 0;
        char temp[16] = {'\0'};

        uint8 spiSlaveTransTestData[SPI_BUFFER_SIZE] = {0x53, 0x30, 0x53, 0x33};
        initQSPISlaveBuffers(g_QspiLookUptable[SpiIndex], spiSlaveTransTestData);

        while (1)
        {
            if (CheckSlaveIsBusy(g_QspiLookUptable[SpiIndex]))
            {
                strcpy(UartTxOutput1, "SPI1-Slave is busy , so wait time is: { ");
                sprintf(temp, "%d  ms };\n\r\n\r", countTimer);
                strcat(UartTxOutput1, temp);
                BufferLen = (Ifx_SizeT)strlen(UartTxOutput1);
                Send_ASCLIN_UART_Message(UartTxOutput1, BufferLen);
                countTimer = countTimer + 100;
                wait(TicksFor100ms);
                if (countTimer > 300)
                {
                    strcpy(UartTxOutput2, "!ERROR:SPI1-SLave is Busy, SPI cannot transform CMD. \n\r ");
                    strcat(UartTxOutput2, "Cannot change the statu.Pls,try again later.\n\r\n\r\n\r\n\r");
                    BufferLen = (Ifx_SizeT)strlen(UartTxOutput2);
                    Send_ASCLIN_UART_Message(UartTxOutput2, BufferLen);
                    break;
                }
            }
            else
            {
                SpiTransferData(SpiIndex);
                while (1)
                {
                    if (startTime > TIMEOUT_LIMIT)
                    {
                        Send_ASCLIN_UART_Message("ERRPR:SPIRX_WAIT_TIMEOUT!!!!\n\r", 30);
                        // ClearSpiTransmitBuffer(SPI1);
                        // Send_ASCLIN_UART_Message("CLEAR_SLAVE_TX_BUFFER!\n\r", 30);
                        break;
                    }
                    if ((*g_QspiLookUptable[SpiIndex]).SpiSlaveRxBuffer[0] != 0)
                    {
                        while (CheckSlaveIsBusy(g_QspiLookUptable[SpiIndex]))
                        {
                        }

                        strcpy(UartTxOutput2, "S0 -> S3 .Already Sleep mode. \r\n ");
                        strcat(UartTxOutput2, "Pls awake me when you need it.\n\r\n\r\n\r\n\r");
                        BufferLen = (Ifx_SizeT)strlen(UartTxOutput2);
                        Send_ASCLIN_UART_Message(UartTxOutput2, BufferLen);

                        // wait(TicksFor100ms * 10);
                        switchSleepMode();
                        break;
                    }
                    startTime++;
                }
                break;
            }
        }
        break;
    case SOC_S1:
        strcpy(UartTxOutput, "EC RAM Read System Power Status = { S1 }, Will do nothing!\n\r\n\r");
        BufferLen = (Ifx_SizeT)strlen(UartTxOutput);
        Send_ASCLIN_UART_Message(UartTxOutput, BufferLen);
        break;
    case SOC_S3:
        strcpy(UartTxOutput, "EC RAM Read System Power Status = { S3 }, Will do nothing!\n\r\n\r");
        BufferLen = (Ifx_SizeT)strlen(UartTxOutput);
        Send_ASCLIN_UART_Message(UartTxOutput, BufferLen);
        break;
    case SOC_S4:
        strcpy(UartTxOutput, "EC RAM Read System Power Status = { S4 }, Will do nothing!\n\r\n\r");
        BufferLen = (Ifx_SizeT)strlen(UartTxOutput);
        Send_ASCLIN_UART_Message(UartTxOutput, BufferLen);
        break;
    case SOC_S5:
        strcpy(UartTxOutput, "EC RAM Read System Power Status = { S5 }, Will do nothing!\n\r\n\r");
        BufferLen = (Ifx_SizeT)strlen(UartTxOutput);
        Send_ASCLIN_UART_Message(UartTxOutput, BufferLen);
        break;

    case SOC_G3:
        strcpy(UartTxOutput, "EC RAM Read System Power Status = { G3 }, Will do nothing!\n\r\n\r");
        BufferLen = (Ifx_SizeT)strlen(UartTxOutput);
        Send_ASCLIN_UART_Message(UartTxOutput, BufferLen);
        break;
    default:
        strcat(UartTxOutput, "System power status unknown, EC sleep, S0 - > S5 Ready!!!\n\r\n\r");
        BufferLen = (Ifx_SizeT)strlen(UartTxOutput);
        Send_ASCLIN_UART_Message(UartTxOutput, BufferLen);
        break;
    }
}

/************************************************
 *  Execute command to transfer SOC system power
 *  state from S0 to S5
 *
 *  @param[in] uint8 SpiIndex - Which SPI used to send CMD
 *  @param[out] void
 *
 *  @retval void
 ************************************************/
void PowerTrans_S0_To_S5(uint8 SpiIndex)
{
    uint8 RegRxBuffer_PWR_STATUS[LENGTH_OF_PWR_STATUS] = {0};
    // uint8 RegTxBuffer_PWR_STATUS[LENGTH_OF_PWR_STATUS] = {0x0};
    char UartTxOutput[128]; /* Debug string buffer for UART */
    Ifx_SizeT BufferLen;    /* Debug string buffer length for UART */

    // TIMEOUT
    Ifx_TickTime startTime = IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, 0);

    Read_EC_RAM_Data(RAMOFFS_OF_PWR_STATUS, &RegRxBuffer_PWR_STATUS[0], LENGTH_OF_PWR_STATUS);

    switch (RegRxBuffer_PWR_STATUS[0])
    {
    case SOC_S0:
        strcpy(UartTxOutput, "EC RAM Read System Power Status = { S0 }, Will trans to S5!\n\r\n\r");
        BufferLen = (Ifx_SizeT)strlen(UartTxOutput);
        Send_ASCLIN_UART_Message(UartTxOutput, BufferLen);
        char UartTxOutput1[128]; /* Debug string buffer for UART */
        char UartTxOutput2[128]; /* Debug string buffer for UART */

        Ifx_TickTime TicksFor100ms; /* Variable to store the number of ticks to wait for 1 second delay */
        TicksFor100ms = IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, 100);
        int countTimer = 0;
        char temp[16] = {'\0'};

        uint8 spiSlaveTransTestData[SPI_BUFFER_SIZE] = {0x53, 0x30, 0x53, 0x35};
        initQSPISlaveBuffers(g_QspiLookUptable[SpiIndex], spiSlaveTransTestData);

        while (1)
        {
            if (CheckSlaveIsBusy(g_QspiLookUptable[SpiIndex]))
            {
                strcpy(UartTxOutput1, "SPI1-Slave is busy , so wait time is: { ");
                sprintf(temp, "%d  ms };\n\r\n\r", countTimer);
                strcat(UartTxOutput1, temp);
                BufferLen = (Ifx_SizeT)strlen(UartTxOutput1);
                Send_ASCLIN_UART_Message(UartTxOutput1, BufferLen);
                countTimer = countTimer + 100;
                wait(TicksFor100ms);
                if (countTimer > 300)
                {
                    strcpy(UartTxOutput2, "!ERROR:SPI1-SLave is Busy, SPI cannot transform CMD. \n\r ");
                    strcat(UartTxOutput2, "Cannot change the statu.Pls,try again later.\n\r\n\r\n\r\n\r");
                    BufferLen = (Ifx_SizeT)strlen(UartTxOutput2);
                    Send_ASCLIN_UART_Message(UartTxOutput2, BufferLen);
                    break;
                }
            }
            else
            {
                SpiTransferData(SpiIndex);
                while (1)
                {
                    if (startTime > TIMEOUT_LIMIT)
                    {
                        Send_ASCLIN_UART_Message("ERRPR:SPIRX_WAIT_TIMEOUT!!!!\n\r", 30);
                        // ClearSpiTransmitBuffer(SPI1);
                        // Send_ASCLIN_UART_Message("CLEAR_SLAVE_TX_BUFFER!\n\r", 30);
                        break;
                    }
                    if ((*g_QspiLookUptable[SpiIndex]).SpiSlaveRxBuffer[0] != 0)
                    {
                        while (CheckSlaveIsBusy(g_QspiLookUptable[SpiIndex]))
                        {
                        }

                        strcpy(UartTxOutput2, "S0 -> S3 .Already Sleep mode. \r\n ");
                        strcat(UartTxOutput2, "Pls awake me when you need it.\n\r\n\r\n\r\n\r");
                        BufferLen = (Ifx_SizeT)strlen(UartTxOutput2);
                        Send_ASCLIN_UART_Message(UartTxOutput2, BufferLen);

                        // wait(TicksFor100ms * 10);
                        switchSleepMode();
                        break;
                    }
                    startTime++;
                }
                break;
            }
        }
        break;
    case SOC_S1:
        strcpy(UartTxOutput, "EC RAM Read System Power Status = { S1 }, Will do nothing!\n\r\n\r");
        BufferLen = (Ifx_SizeT)strlen(UartTxOutput);
        Send_ASCLIN_UART_Message(UartTxOutput, BufferLen);
        break;
    case SOC_S3:
        strcpy(UartTxOutput, "EC RAM Read System Power Status = { S3 }, Will do nothing!\n\r\n\r");
        BufferLen = (Ifx_SizeT)strlen(UartTxOutput);
        Send_ASCLIN_UART_Message(UartTxOutput, BufferLen);
        break;
    case SOC_S4:
        strcpy(UartTxOutput, "EC RAM Read System Power Status = { S4 }, Will do nothing!\n\r\n\r");
        BufferLen = (Ifx_SizeT)strlen(UartTxOutput);
        Send_ASCLIN_UART_Message(UartTxOutput, BufferLen);
        break;
    case SOC_S5:
        strcpy(UartTxOutput, "EC RAM Read System Power Status = { S5 }, Will do nothing!\n\r\n\r");
        BufferLen = (Ifx_SizeT)strlen(UartTxOutput);
        Send_ASCLIN_UART_Message(UartTxOutput, BufferLen);
        break;

    case SOC_G3:
        strcpy(UartTxOutput, "EC RAM Read System Power Status = { G3 }, Will do nothing!\n\r\n\r");
        BufferLen = (Ifx_SizeT)strlen(UartTxOutput);
        Send_ASCLIN_UART_Message(UartTxOutput, BufferLen);
        break;
    default:
        strcat(UartTxOutput, "System power status unknown, EC sleep, S0 - > S5 Ready!!!\n\r\n\r");
        BufferLen = (Ifx_SizeT)strlen(UartTxOutput);
        Send_ASCLIN_UART_Message(UartTxOutput, BufferLen);
        break;
    }
}

#define ALLOW_SLEEP_MODE 0x0 /* Allow sleep mode for SPI                                         */

void setQspiStatuSleep(void)
{
    g_Qspi1Slave.SpiPort->CLC.B.EDIS = ALLOW_SLEEP_MODE;
}