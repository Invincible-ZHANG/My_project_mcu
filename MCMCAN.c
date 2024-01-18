/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/
#include "MCMCAN.h"
#include "ASCLIN_UART.h"
#include "I2C_Read_EC.h"
#include "SPI_CPU.h"
#include <string.h>
#include <stdio.h>

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/
#define CAN_MESSAGE_TX_ID0 (uint32)0x12345678
/* Pin to CAN transceiver TJA1043T */
#define CAN_EN_PIN &MODULE_P23, 2
#define CAN_STB_PIN &MODULE_P23, 4

#define ISR_PRIORITY_CAN_RX 88 /* Define the CAN RX interrupt priority              */
#define ISR_PRIORITY_CAN_TX 89 /* Define the CAN RX interrupt priority              */

#define DO_NOT_CARE_BUFFER_INDEX (IfxCan_RxBufferId)0 // why not care the receive data?
#define DO_NOT_CARE_ID2_VALUE (uint32)0

/* Sleep mode test   */

#define ISR_PRIORITY_STM 20 /* STM Interrupt priority for interrupt ISR                         */
#define STM &MODULE_STM0    /* STM0 module is used in this example \
                             */
#define PMSWCR1_CPUSEL 0x1  /* Set the CPU0 as CPU master                                       */
#define PMCSR0_REQSLP 0x2   /* Request sleep mode                                               */

#define BLOCK_SLEEP_MODE 0x1 /* Block sleep mode for STM                                         */

#define A2B_PIN &MODULE_P00, 5

/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/
McmcanType g_mcmcan; /* Structure for handling MCMCAN     */
uint8 g_TxTransTestData[IfxCan_DataLengthCode_8] = {0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10};

/* Definition of CAN messages lengths */
const uint8 g_dlcLookUpTable[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 20, 24, 32, 48, 64};

/* Debug only */
boolean g_NeedPrint_CanNewMseg = FALSE; /* Global flag for SPI Tx ISR debug mseg print */

/* ====================================================================================================================
 * CAN filter configuration:
 * ====================================================================================================================
 * Each filter element is defined based on the following configuration options:
 *  - Filter element number
 *  - Standard/Extended filter element configuration (StdMsgk_S0(k=0-127).SFEC / ExtMsgk_F0(k=0-63).EFEC)
 *  - Standard/Extended filter type (StdMsgk_S0(k=0-127).SFT / ExtMsgk_F1(k=0-63).EFT)
 *  - Standard/Extended filter ID 1 (StdMsgk_S0(k=0-127).SFID1 / ExtMsgk_F0(k=0-63).EFID1)
 *  - Standard/Extended filter ID 2 (StdMsgk_S0(k=0-127).SFID2 / ExtMsgk_F1(k=0-63).EFID2)
 *  - Rx buffer offset to the Rx buffer start address
 * ====================================================================================================================
 * For this example the following standard ID filter configuration is used:
 * ------------------------------------------------------------------------------------------
 * Element # |  Filter element configuration explanation
 * ------------------------------------------------------------------------------------------
 *     0     |  Reject messages with standard IDs in range [0x17-0x19]
 *     1     |  Accept messages with standard IDs in range [0x14-0x1A] and store them in RX FIFO 0
 *     2     |  Accept messages with standard IDs 0x184 or 0x187 and store them in RX FIFO 1
 *     3     |  Accept messages with standard ID 0x189 and store them in RX FIFO 0
 *     4     |  Accept messages according to the standard ID 0x200 and mask 0x39F and store them in RX FIFO 0
 *     5     |  Reject messages according to the standard ID 0x201 and mask 0x39F
 *     6     |  Accept messages with standard ID 0x325 and store them in dedicated RX buffer at index 0x2
 *     7     |  Accept messages with standard ID 0x326 and store them in dedicated RX buffer at index 0x5
 * ====================================================================================================================
 */
IfxCan_Filter g_filterStandardIdConfiguration[] =
    {
        {0, IfxCan_FilterElementConfiguration_storeInRxFifo0, IfxCan_FilterType_dualId, 0x123, 0x123, DO_NOT_CARE_BUFFER_INDEX},
        {1, IfxCan_FilterElementConfiguration_storeInRxFifo0, IfxCan_FilterType_dualId, 0x456, 0x456, DO_NOT_CARE_BUFFER_INDEX},
        {2, IfxCan_FilterElementConfiguration_storeInRxFifo0, IfxCan_FilterType_dualId, 0x789, 0x789, DO_NOT_CARE_BUFFER_INDEX},
        //    { 3, IfxCan_FilterElementConfiguration_storeInRxFifo0,  IfxCan_FilterType_dualId,   0x189, 0x189,                   DO_NOT_CARE_BUFFER_INDEX },
        //    { 4, IfxCan_FilterElementConfiguration_storeInRxFifo0,  IfxCan_FilterType_classic,  0x200, 0x39F,                   DO_NOT_CARE_BUFFER_INDEX },
        //    { 5, IfxCan_FilterElementConfiguration_rejectId,        IfxCan_FilterType_classic,  0x201, 0x39F,                   DO_NOT_CARE_BUFFER_INDEX },
        //    { 6, IfxCan_FilterElementConfiguration_storeInRxBuffer, IfxCan_FilterType_none,     0x325, DO_NOT_CARE_ID2_VALUE,   IfxCan_RxBufferId_2 },
        //    { 7, IfxCan_FilterElementConfiguration_storeInRxBuffer, IfxCan_FilterType_none,     0x326, DO_NOT_CARE_ID2_VALUE,   IfxCan_RxBufferId_5 }
};

/* ====================================================================================================================
 * For this example the following extended ID filter configuration is used:
 * ------------------------------------------------------------------------------------------
 * Element # |  Filter element configuration explanation
 * ------------------------------------------------------------------------------------------
 *     0     |  Accept messages with extended ID 0x12222222 and store them in dedicated RX buffer at index 0x0
 *     1     |  Accept messages with extended IDs in range [0x19999999-0x1BBBBBBB] and store them in RX FIFO 1
 *     2     |  Accept messages with extended ID 0x1FFABCDE and store them in RX FIFO 0
 *     3     |  Accept messages with extended ID 0x16666666 and store them in RX FIFO 1
 * ====================================================================================================================
 */
IfxCan_Filter g_filterExtendedIdConfiguration[] =
    {
        {0, IfxCan_FilterElementConfiguration_storeInRxFifo0, IfxCan_FilterType_dualId, 0x7654321, 0x7654321, DO_NOT_CARE_BUFFER_INDEX},
        {1, IfxCan_FilterElementConfiguration_storeInRxFifo0, IfxCan_FilterType_dualId, 0x19999999, 0x19999999, DO_NOT_CARE_BUFFER_INDEX},
        {2, IfxCan_FilterElementConfiguration_storeInRxFifo0, IfxCan_FilterType_dualId, 0x2222222, 0x2222222, DO_NOT_CARE_BUFFER_INDEX},
        //    { 3, IfxCan_FilterElementConfiguration_storeInRxFifo1,  IfxCan_FilterType_none,     0x16666666, 0x16666666,             DO_NOT_CARE_BUFFER_INDEX }
};

/*********************************************************************************************************************/
/*---------------------------------------------Function Implementations----------------------------------------------*/
/*********************************************************************************************************************/
/* Macro to define Interrupt Service Routine.
 * This macro:
 * - defines linker section as .intvec_tc<vector number>_<interrupt priority>.
 * - defines compiler specific attribute for the interrupt functions.
 * - defines the Interrupt service routine as ISR function.
 *
 * IFX_INTERRUPT(isr, vectabNum, priority)
 *  - isr: Name of the ISR function.
 *  - vectabNum: Vector table number.
 *  - priority: Interrupt priority. Refer Usage of Interrupt Macro for more details.
 */
IFX_INTERRUPT(canIsrRxHandler, 0, ISR_PRIORITY_CAN_RX);

/* Interrupt Service Routine (ISR) called once the RX interrupt has been generated.
 * Reads the received CAN message and increments a counter confirming to the source node that the previous message
 * has been received.
 */
void canIsrRxHandler(void)
{
    /* Clear the "RX FIFO 0 new message" interrupt flag */
    IfxCan_Node_clearInterruptFlag(g_mcmcan.can0Node0.node, IfxCan_Interrupt_rxFifo0NewMessage);

    /* Received message content should be updated with the data stored in the RX FIFO 0 */
    g_mcmcan.rxMsg.readFromRxFifo0 = TRUE;

    /* Read the received CAN message */
    IfxCan_Can_readMessage(&g_mcmcan.can0Node0, &g_mcmcan.rxMsg, (uint32 *)&g_mcmcan.rxData[0]);

    /* Debug only */
    g_NeedPrint_CanNewMseg = TRUE;
}

/* Function to initialize MCMCAN module and nodes related for this application use case */
void initMcmcan(void)
{
    uint8 currentFilterElement;

    /* ==========================================================================================
     * CAN module configuration and initialization:
     * ==========================================================================================
     *  - load default CAN module configuration into configuration structure
     *  - initialize CAN module with the default configuration
     * ==========================================================================================
     */
    IfxCan_Can_initModuleConfig(&g_mcmcan.canConfig, &MODULE_CAN0);
    IfxCan_Can_initModule(&g_mcmcan.canModule, &g_mcmcan.canConfig);

    /* ==========================================================================================
     * CAN0 node0 configuration and initialization:
     * ==========================================================================================
     */
    IfxCan_Can_initNodeConfig(&g_mcmcan.canNodeConfig, &g_mcmcan.canModule);

    g_mcmcan.canNodeConfig.nodeId = IfxCan_NodeId_0;

    g_mcmcan.canNodeConfig.clockSource = IfxCan_ClockSource_both;

    g_mcmcan.canNodeConfig.frame.type = IfxCan_FrameType_transmitAndReceive;
    g_mcmcan.canNodeConfig.frame.mode = IfxCan_FrameMode_fdLongAndFast;

    g_mcmcan.canNodeConfig.baudRate.baudrate = 500000; // 500KBaud

    g_mcmcan.canNodeConfig.txConfig.txMode = IfxCan_TxMode_dedicatedBuffers;
    g_mcmcan.canNodeConfig.txConfig.dedicatedTxBuffersNumber = 255;
    g_mcmcan.canNodeConfig.txConfig.txBufferDataFieldSize = IfxCan_DataFieldSize_64;

    g_mcmcan.canNodeConfig.rxConfig.rxMode = IfxCan_RxMode_fifo0;
    g_mcmcan.canNodeConfig.rxConfig.rxFifo0DataFieldSize = IfxCan_DataFieldSize_64;
    g_mcmcan.canNodeConfig.rxConfig.rxFifo0Size = 64;

    g_mcmcan.canNodeConfig.filterConfig.standardListSize = 64;
    g_mcmcan.canNodeConfig.filterConfig.extendedListSize = 64;
    g_mcmcan.canNodeConfig.filterConfig.messageIdLength = IfxCan_MessageIdLength_both;
    g_mcmcan.canNodeConfig.filterConfig.standardFilterForNonMatchingFrames = IfxCan_NonMatchingFrame_reject;
    g_mcmcan.canNodeConfig.filterConfig.extendedFilterForNonMatchingFrames = IfxCan_NonMatchingFrame_reject;

    // transmit interrupt
    g_mcmcan.canNodeConfig.interruptConfig.rxFifo0NewMessageEnabled = TRUE;
    g_mcmcan.canNodeConfig.interruptConfig.rxf0n.priority = ISR_PRIORITY_CAN_RX;
    g_mcmcan.canNodeConfig.interruptConfig.rxf0n.interruptLine = IfxCan_InterruptLine_1;
    g_mcmcan.canNodeConfig.interruptConfig.rxf0n.typeOfService = IfxSrc_Tos_cpu0;

    IFX_CONST IfxCan_Can_Pins can0Node0_pins = {
        &IfxCan_TXD00_P20_8_OUT, IfxPort_OutputMode_pushPull, // CAN0_NODE0_TX
        &IfxCan_RXD00B_P20_7_IN, IfxPort_InputMode_pullUp,    // CAN0_NODE0_RX
        IfxPort_PadDriver_cmosAutomotiveSpeed4};
    g_mcmcan.canNodeConfig.pins = &can0Node0_pins;

    IfxCan_Can_initNode(&g_mcmcan.can0Node0, &g_mcmcan.canNodeConfig);

    /* ------------------------------------------------------------------------------------------
     * Initialization of standard and extended ID filter elements
     * ==========================================================================================
     */
    for (currentFilterElement = 0; currentFilterElement < sizeof(g_filterStandardIdConfiguration) / sizeof(g_filterStandardIdConfiguration[0]); currentFilterElement++)
    {
        IfxCan_Can_setStandardFilter(&g_mcmcan.can0Node0, &g_filterStandardIdConfiguration[currentFilterElement]);
    }
    for (currentFilterElement = 0; currentFilterElement < sizeof(g_filterExtendedIdConfiguration) / sizeof(g_filterExtendedIdConfiguration[0]); currentFilterElement++)
    {
        IfxCan_Can_setExtendedFilter(&g_mcmcan.can0Node0, &g_filterExtendedIdConfiguration[currentFilterElement]);
    }
}

/* Function to initialize both TX and RX messages with the default data values.
 * After initialization of the messages, the TX message will be transmitted.
 */
void transmitCanMessage(void)
{
    IfxCan_Can_initMessage(&g_mcmcan.txMsg);
    g_mcmcan.txMsg.messageId = CAN_MESSAGE_TX_ID0;
    g_mcmcan.txMsg.bufferNumber = 0;
    g_mcmcan.txMsg.dataLengthCode = IfxCan_DataLengthCode_8;          // 8 bytes
    g_mcmcan.txMsg.frameMode = IfxCan_FrameMode_standard;             // Classic CAN
    g_mcmcan.txMsg.messageIdLength = IfxCan_MessageIdLength_extended; // Extended Frame

    for (uint8 i = 0; i < IfxCan_DataLengthCode_8; i++)
    {
        g_mcmcan.txData[i] = g_TxTransTestData[i]; /* Init TX Buffer                    */
    }

    /* Send the CAN message with the previously defined TX message configuration and content */
    while (IfxCan_Status_notSentBusy ==
           IfxCan_Can_sendMessage(&g_mcmcan.can0Node0, &g_mcmcan.txMsg, (uint32 *)&g_mcmcan.txData[0]))
    {
    }
}

/************************************************
 *  Init Pins connected to CAN transceiver TJA1043T
 *
 *  @param[in] void
 *  @param[out] void
 *
 *  @retval void
 ************************************************/
void InitCanEnPin(void)
{
    /* Init CAN_EN pin on CAN transceiver, active high */
    /* Init STB_N pin on CAN transceiver, active low */
    IfxPort_setPinMode(CAN_EN_PIN, IfxPort_Mode_outputPushPullGeneral);
    IfxPort_setPinMode(CAN_STB_PIN, IfxPort_Mode_outputPushPullGeneral);

    IfxPort_setPinState(CAN_EN_PIN, IfxPort_State_high);
    IfxPort_setPinState(CAN_STB_PIN, IfxPort_State_high);
}

/* Debug only */
/************************************************
 *  Check global update flag and print Rx value
 *
 *  @param[in] void
 *  @param[out] void
 *
 *  @retval void
 ************************************************/
void CanRx_CheckAndPrint(void)
{
    char UartTxOutput[1024]; /* Debug string buffer for UART */
    Ifx_SizeT BufferLen;     /* Debug string buffer length for UART */
    char temp[16] = {'\0'};

    if (g_NeedPrint_CanNewMseg)
    {

        g_NeedPrint_CanNewMseg = FALSE;
        strcpy(UartTxOutput, "Can Rx: MessageId = { ");
        sprintf(temp, "0x%lX };\n\r", g_mcmcan.rxMsg.messageId);
        strcat(UartTxOutput, temp);

        while (IfxCan_Status_notSentBusy ==
               IfxCan_Can_sendMessage(&g_mcmcan.can0Node0, &g_mcmcan.txMsg, (uint32 *)&g_mcmcan.rxData[0]))
        {
        }

        switch (g_mcmcan.rxMsg.messageIdLength)
        {
        case IfxCan_MessageIdLength_standard:
            strcat(UartTxOutput, "Can Rx: MessageIdLength = {standard};\n\r");
            break;
        case IfxCan_MessageIdLength_extended:
            strcat(UartTxOutput, "Can Rx: MessageIdLength = {extended};\n\r");
            break;
        case IfxCan_MessageIdLength_both:
            strcat(UartTxOutput, "Can Rx: MessageIdLength = {both};\n\r");
            break;
        default:
            strcat(UartTxOutput, "Can Rx: MessageIdLength = {unknown};\n\r");
            break;
        }

        strcat(UartTxOutput, "Can Rx: dataLengthCode = { ");
        sprintf(temp, "%d };\n\r", g_dlcLookUpTable[g_mcmcan.rxMsg.dataLengthCode]);
        strcat(UartTxOutput, temp);

        switch (g_mcmcan.rxMsg.frameMode)
        {
        case IfxCan_FrameMode_standard:
            strcat(UartTxOutput, "Can Rx: frameMode = {standard};\n\r");
            break;
        case IfxCan_FrameMode_fdLong:
            strcat(UartTxOutput, "Can Rx: frameMode = {fdLong};\n\r");
            break;
        case IfxCan_FrameMode_fdLongAndFast:
            strcat(UartTxOutput, "Can Rx: frameMode = {fdLongAndFast};\n\r");
            break;
        default:
            strcat(UartTxOutput, "Can Rx: frameMode = {unknown};\n\r");
            break;
        }

        strcat(UartTxOutput, "Can Rx: Data = { ");
        for (uint8 i = 0; i < g_dlcLookUpTable[g_mcmcan.rxMsg.dataLengthCode]; i++)
        {
            sprintf(temp, "0x%02X ", g_mcmcan.rxData[i]);
            strcat(UartTxOutput, temp);
        }
        strcat(UartTxOutput, "};\n\r\n\r");
        BufferLen = (Ifx_SizeT)strlen(UartTxOutput);
        Send_ASCLIN_UART_Message(UartTxOutput, BufferLen);

        /* Special command for S0->S5, S5->S0 check */
        if (g_mcmcan.rxMsg.messageId == 0x123 && g_mcmcan.rxMsg.messageIdLength == IfxCan_MessageIdLength_standard && *((uint64 *)g_mcmcan.rxData) == 0x7766554433221100)
        {
            strcpy(UartTxOutput, "Can Rx: S0 -> S5 Command match, start power trans\n\r\n\r");
            BufferLen = (Ifx_SizeT)strlen(UartTxOutput);
            Send_ASCLIN_UART_Message(UartTxOutput, BufferLen);
            PowerTrans_S0_To_S5(SPI1);
        }
        // else if (g_mcmcan.rxMsg.messageId == 0x7654321 && g_mcmcan.rxMsg.messageIdLength == IfxCan_MessageIdLength_extended \
        // && *((uint64 *)g_mcmcan.rxData) == 0x8899AABBCCDDEEFF && *((uint64 *)g_mcmcan.rxData+1) == 0x0011223344556677 \
        // && *((uint64 *)g_mcmcan.rxData+2) == 0x7766554433221100 && *((uint64 *)g_mcmcan.rxData+3) == 0xFFEEDDCCBBAA9988 \
        // )
        // {
        //     strcpy(UartTxOutput, "Can Rx: S5 -> S0 Command match, start power trans\n\r\n\r");
        //     BufferLen = (Ifx_SizeT)strlen(UartTxOutput);
        //     Send_ASCLIN_UART_Message(UartTxOutput, BufferLen);
        //     PowerTrans_S5_To_S0();
        // }
        if (g_mcmcan.rxMsg.messageId == 0x123 && g_mcmcan.rxMsg.messageIdLength == IfxCan_MessageIdLength_standard && *((uint64 *)g_mcmcan.rxData) == 0x0011223344)
        {
            strcpy(UartTxOutput, "Can Rx: S5 -> S0 Command match, start power trans\n\r\n\r");
            BufferLen = (Ifx_SizeT)strlen(UartTxOutput);
            Send_ASCLIN_UART_Message(UartTxOutput, BufferLen);
            PowerTrans_S5_To_S0();
        }

        /* Special command for S0->S3, S3->S0 check */
        if (g_mcmcan.rxMsg.messageId == 0x123 && g_mcmcan.rxMsg.messageIdLength == IfxCan_MessageIdLength_standard && *((uint64 *)g_mcmcan.rxData) == 0xFF0033)
        {
            strcpy(UartTxOutput, "Can Rx: S0 -> S3 Command match, start power trans\n\r\n\r");
            BufferLen = (Ifx_SizeT)strlen(UartTxOutput);
            Send_ASCLIN_UART_Message(UartTxOutput, BufferLen);
            // g_NeedPrint_CanNewMseg = FALSE;
            PowerTrans_S0_To_S3(SPI1);
        }

        if (g_mcmcan.rxMsg.messageId == 0x123 && g_mcmcan.rxMsg.messageIdLength == IfxCan_MessageIdLength_standard && *((uint64 *)g_mcmcan.rxData) == 0xFF3300)
        {
            strcpy(UartTxOutput, "Can Rx: S3 -> S0 Command match, start power trans\n\r\n\r");
            BufferLen = (Ifx_SizeT)strlen(UartTxOutput);
            Send_ASCLIN_UART_Message(UartTxOutput, BufferLen);
            // g_NeedPrint_CanNewMseg = FALSE;
            PowerTrans_S3_To_S0();
        }

        // close the A2B power
        if (g_mcmcan.rxMsg.messageId == 0x123 && g_mcmcan.rxMsg.messageIdLength == IfxCan_MessageIdLength_standard && *((uint64 *)g_mcmcan.rxData) == 0xBB22AA)
        {
            strcpy(UartTxOutput, "A2B has closed!!!\n\r\n\r");
            BufferLen = (Ifx_SizeT)strlen(UartTxOutput);
            Send_ASCLIN_UART_Message(UartTxOutput, BufferLen);

            IfxPort_setPinState(A2B_PIN, IfxPort_State_low);
        }

        // g_NeedPrint_CanNewMseg = FALSE;
    }
}

void switchSleepMode(void)
{
    /* Clear safety EndInit protection */
    IfxScuWdt_clearSafetyEndinitInline(IfxScuWdt_getSafetyWatchdogPasswordInline());
    /* Clear EndInit protection */
    IfxScuWdt_clearCpuEndinit(IfxScuWdt_getCpuWatchdogPassword());

    // setQspiStatuSleep();

    g_mcmcan.can0Node0.can->CLC.B.EDIS = BLOCK_SLEEP_MODE;

    SCU_PMSWCR1.B.CPUSEL = PMSWCR1_CPUSEL; /* Set the CPU0 as CPU master to trigger a power down mode      */

    SCU_PMCSR0.B.REQSLP = PMCSR0_REQSLP; /* Request System Sleep Mode CPU0                               */

    SCU_PMCSR1.B.REQSLP = PMCSR0_REQSLP;

    SCU_PMCSR2.B.REQSLP = PMCSR0_REQSLP;

    SCU_PMCSR3.B.REQSLP = PMCSR0_REQSLP;

    /* Set safety EndInit protection */
    IfxScuWdt_setSafetyEndinitInline(IfxScuWdt_getSafetyWatchdogPasswordInline());
    /* Set EndInit protection */
    IfxScuWdt_setCpuEndinit(IfxScuWdt_getCpuWatchdogPassword());
}
