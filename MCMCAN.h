#ifndef MCMCAN_H_
#define MCMCAN_H_

/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/
#include "Ifx_Types.h"
#include "IfxCan_Can.h"
#include "IfxCan.h"
#include "IfxCpu_Irq.h"
#include "IfxPort.h"
#include <stdio.h>
#include <string.h>

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/
#define MAXIMUM_CAN_DATA_PAYLOAD 64                          /* Define maximum CAN payload in bytes               */

/*********************************************************************************************************************/
/*--------------------------------------------------Data Structures--------------------------------------------------*/
/*********************************************************************************************************************/
typedef struct
{
    IfxCan_Can_Config canConfig;                        /* CAN module configuration structure                        */
    IfxCan_Can canModule;                               /* CAN module handle                                         */
    IfxCan_Can_Node can0Node0;                          /* CAN node handle data structure                            */
    IfxCan_Can_NodeConfig canNodeConfig;                /* CAN node configuration structure                          */
    IfxCan_Filter canFilter;                            /* CAN filter configuration structure                        */
    IfxCan_Message txMsg;                               /* Transmitted CAN message structure                         */
    IfxCan_Message rxMsg;                               /* Received CAN message structure                            */
    uint8 txData[MAXIMUM_CAN_DATA_PAYLOAD];          /* Transmitted CAN data array                                */
    uint8 rxData[MAXIMUM_CAN_DATA_PAYLOAD];          /* Received CAN data array                                   */
} McmcanType;

/*********************************************************************************************************************/
/*-----------------------------------------------Function Prototypes-------------------------------------------------*/
/*********************************************************************************************************************/
void initMcmcan(void);
void InitCanEnPin(void);
void transmitCanMessage(void);
void CanRx_CheckAndPrint(void);
void switchSleepMode(void);

#endif /* MCMCAN_H_ */
