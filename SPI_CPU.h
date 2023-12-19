#ifndef SPI_CPU_H_
#define SPI_CPU_H_

/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/
#include "Ifx_Types.h"
#include "IfxQspi_SpiSlave.h"
#include "IfxPort.h"

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/
#define SPI_BUFFER_SIZE     4                       /* Buffers size                  */
#define SPI0     0                       /* Index for SPI port in SPI lookup table                  */
#define SPI1     1
#define SPI2     2
#define SPI3     3
#define SPI4     4

/*********************************************************************************************************************/
/*-------------------------------------------------Data Structures---------------------------------------------------*/
/*********************************************************************************************************************/
typedef struct
{
    char                   SpiSlaveTxBuffer[SPI_BUFFER_SIZE];       /* QSPI Slave Transmit buffer    */
    char                   SpiSlaveRxBuffer[SPI_BUFFER_SIZE];       /* QSPI Slave Receive buffer     */
    IfxQspi_SpiSlave       SpiSlaveHandle;                          /* QSPI Slave handle             */
    Ifx_QSPI               *SpiPort;                                /* QSPI port module              */
    IFX_CONST IfxQspi_SpiSlave_Pins  *SpiSlavepin;                  /* QSPI pin define               */
    uint8                  ISR_PRIORITY_SLAVE_TX;                   /* QSPI TX ISR prio define       */
    uint8                  ISR_PRIORITY_SLAVE_RX;                   /* QSPI RX ISR prio define       */
    uint8                  ISR_PRIORITY_SLAVE_ER;                   /* QSPI ER ISR prio define       */
    uint8                  *SlaveTransTestData;                     /* Tx test data sent by QSPI     */
    Ifx_P                  *port;                                   /* (Optional) Interupt pin to SOC. Notice data is ready */
    uint8                  pinIndex;                                /* (Optional) Interupt pin to SOC. Notice data is ready */
} QspiSlaveComm;

/*********************************************************************************************************************/
/*------------------------------------------------Function Prototypes------------------------------------------------*/
/*********************************************************************************************************************/
void initQspiSlave(uint8 SpiIndex);
void SpiTransferData(uint8 SpiIndex);
void PowerTrans_S0_To_S3(uint8 SpiIndex);
void PowerTrans_S0_To_S5(uint8 SpiIndex);

/* Debug only */
void CheckQspiIntPinAndReTrans(uint8 SpiIndex);
boolean CheckIfCPU1ShouldStart(void);
void SpiUpd_CheckAndPrint(uint8 SpiIndex);
void setQspiStatuSleep(void);

#endif /* SPI_CPU_H_ */
