/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/
#include "Ifx_Types.h"
#include "IfxI2c_I2c.h"
#include "I2C_Read_EC.h"
#include "ASCLIN_UART.h"
#include "IfxPort.h"
#include "Bsp.h"

#include <string.h>
#include <stdio.h>

/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/
IfxI2c_I2c g_i2cHandle;                                         /* I2C handle                                        */
IfxI2c_I2c_Device g_i2cDevEC;                                   /* I2C Slave device handle to EEPROM of MC79411      */

/*********************************************************************************************************************/
/*------------------------------------------------Function Prototypes------------------------------------------------*/
/*********************************************************************************************************************/
void Read_EC_RAM_Data(uint8 RegOff, uint8 *RegRxBuffer, uint8 RegLength);
void Write_Data_To_EC_RAM(uint8 RegOff, uint8 *DataBuffer, uint8 RegLength);

/*********************************************************************************************************************/
/*---------------------------------------------Function Implementations----------------------------------------------*/
/*********************************************************************************************************************/
/* This function initializes the I2C in master mode and configures EC device as an I2C slave                         */
void init_I2C_module(void)
{
    IfxI2c_I2c_Config i2cConfig;                                    /* Create configuration structure                */
    IfxI2c_I2c_initConfig(&i2cConfig, &MODULE_I2C0);                /* Fill structure with default values and Module
                                                                       address                                       */
    const IfxI2c_Pins EC_I2C_PINS =
    {
            &EC_SCL_PIN,                                            /* SCL port pin                                  */
            &EC_SDA_PIN,                                            /* SDA port pin                                  */
            IfxPort_PadDriver_ttlSpeed1                             /* Pad driver mode                               */
    };
    i2cConfig.pins = &EC_I2C_PINS;                                  /* Configure port pins                           */
    i2cConfig.baudrate = I2C_BAUDRATE;                              /* Configure baud rate with 100kHz               */

    IfxI2c_I2c_initModule(&g_i2cHandle, &i2cConfig);                /* Initialize module                             */

    IfxI2c_I2c_deviceConfig i2cDeviceConfig;                        /* Create device configuration                   */
    IfxI2c_I2c_initDeviceConfig(&i2cDeviceConfig, &g_i2cHandle);    /* Fill structure with default values and I2C
                                                                       Handler                                       */
    /* Because it is 7 bit long and bit 0 is R/W bit, the device address has to be shifted by 1 */
    i2cDeviceConfig.deviceAddress = EC_I2C_ADDRESS << 1;
    IfxI2c_I2c_initDevice(&g_i2cDevEC, &i2cDeviceConfig);           /* Initialize the I2C device handle              */
}


/********************************************************************
*  Read EC RAM data from its specific offset.
*
*  @param[in]  RegOff, EC ram data offset
*  @param[out] *RegRxBuffer, pointer to the data read buffer
*  @param[in]  RegLength, data length
*
*  @retval void
********************************************************************/
void Read_EC_RAM_Data(uint8 RegOff, uint8 *RegRxBuffer, uint8 RegLength)
{

    /* Communication via I2C starts by transmitting the address of the specific I2C slave until the slave
     * is ready and confirms the reception via the acknowledge bit (IfxI2c_I2c_Status_nak = not acknowledge).
     * This procedure is done for both the write and read process.
     */
    /* Read data from EC
     * Write EC ram data to I2C device ,and Check whether the reg offset on the slave side matches the host side.
     * If the match is successful, than read the data and save in RegRxbuffer .
     */
    while(IfxI2c_I2c_write(&g_i2cDevEC, &RegOff, 1) == IfxI2c_I2c_Status_nak);
    while(IfxI2c_I2c_read(&g_i2cDevEC, RegRxBuffer, RegLength) == IfxI2c_I2c_Status_nak);
}


/********************************************************************
*  Write data to EC RAM specific offset.
*
*  @param[in]  RegOff, EC ram data offset
*  @param[out] *DataBuffer, pointer to the data write buffer
*  @param[in]  RegLength, data length
*
*  @retval void
********************************************************************/
void Write_Data_To_EC_RAM(uint8 RegOff, uint8 *DataBuffer, uint8 RegLength)
{
    uint8 I2cTxBufferLength = RegLength + 1;
    uint8 I2cTxBuffer[I2cTxBufferLength];

    /* TxBuffer data format :
     * I2cTxBuffer[DataBufferLength+1] = {RegOff, DataBuffer[]};
     */
    I2cTxBuffer[0] = RegOff;
    for (uint8 i = 1; i < I2cTxBufferLength; i++)
    {
        I2cTxBuffer[i] = DataBuffer[i-1];
    }

    /* Communication via I2C starts by transmitting the address of the specific I2C slave until the slave
     * is ready and confirms the reception via the acknowledge bit (IfxI2c_I2c_Status_nak = not acknowledge).
     * This procedure is done for both the write and read process.
     */
    /* Write data to EC */
    while(IfxI2c_I2c_write(&g_i2cDevEC, &I2cTxBuffer[0], I2cTxBufferLength) == IfxI2c_I2c_Status_nak);
}

/************************************************
*  Read EC RAM data and print by UART
*
*  @param[in] void
*  @param[out] void
*
*  @retval void
************************************************/
void EC_Ram_Operation()
{
    
    Ifx_TickTime TicksFor1s;                    /* Variable to store the number of ticks to wait for 1 second delay */
    uint32 WaitTime;
    /* Initialize the time variable */
    TicksFor1s = IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, 1000);

    /*Debug Only end*/

    uint8 RegRxBuffer_CHIPID[LENGTH_OF_CHIPID] = {0};
    uint8 RegRxBuffer_PWR_STATUS[LENGTH_OF_PWR_STATUS] = {0};
    uint8 RegRxBuffer_HEARTBEAT[LENGTH_OF_HEARTBEAT] = {0};
    uint8 RegRxBuffer_HEARTBEAT_Wait1s[LENGTH_OF_HEARTBEAT] = {0};
    uint8 RegRxBuffer_SYSTEMMODE[LENGTH_OF_SYSTEMMODE] = {0};

    uint8 RegTxBuffer_PWR_STATUS[LENGTH_OF_PWR_STATUS] = {0x0};

    /* Read EC RAM offset 0x00 - CHIP ID */
    Read_EC_RAM_Data(RAMOFFS_OF_CHIPID, &RegRxBuffer_CHIPID[0], LENGTH_OF_CHIPID);
    /* Read EC RAM offset 0x01 - Power Status */
    Read_EC_RAM_Data(RAMOFFS_OF_PWR_STATUS, &RegRxBuffer_PWR_STATUS[0], LENGTH_OF_PWR_STATUS);
    /* Read EC RAM offset 0x02 - Heart Beat */
    Read_EC_RAM_Data(RAMOFFS_OF_HEARTBEAT, &RegRxBuffer_HEARTBEAT[0], LENGTH_OF_HEARTBEAT);
    wait(TicksFor1s);
    /* Read EC RAM offset 0x02 - Heart Beat, Avoid System is died */
    Read_EC_RAM_Data(RAMOFFS_OF_HEARTBEAT, &RegRxBuffer_HEARTBEAT_Wait1s[0], LENGTH_OF_HEARTBEAT);
    /* Read EC RAM offset 0x03 - System Mode */
    Read_EC_RAM_Data(RAMOFFS_OF_SYSTEMMODE, &RegRxBuffer_SYSTEMMODE[0], LENGTH_OF_SYSTEMMODE);

    /* Debug printf info only -- START */
    char UartTxOutput[1024];                         /* Debug string buffer for UART */
    Ifx_SizeT BufferLen;                             /* Debug string buffer length for UART */
    char temp[16] = {'\0'};

    strcpy(UartTxOutput, "EC RAM Read - RAM[0x00]_CHIPID = { ");
    sprintf(temp, "0x%02X };\n\r", RegRxBuffer_CHIPID[0]);   //Format the RegRxBuffer_CHIPID[0] to temp
    strcat(UartTxOutput, temp);      // Add temp to the end of UartTxOutput

    switch (RegRxBuffer_PWR_STATUS[0])
    {
        case SOC_S0:
            strcat(UartTxOutput, "EC RAM Read - RAM[0x01]_PWR_STATUS = { S0 };\n\r");
            break;
        case SOC_S1:
            strcat(UartTxOutput, "EC RAM Read - RAM[0x01]_PWR_STATUS = { S1 };\n\r");
            break;
        case SOC_S3:
            strcat(UartTxOutput, "EC RAM Read - RAM[0x01]_PWR_STATUS = { S3 };\n\r");
            break;
        case SOC_S4:
            strcat(UartTxOutput, "EC RAM Read - RAM[0x01]_PWR_STATUS = { S4 };\n\r");
            break;
        case SOC_S5:
            strcat(UartTxOutput, "EC RAM Read - RAM[0x01]_PWR_STATUS = { S5 };\n\r");
            break;
        case SOC_G3:
            strcat(UartTxOutput, "EC RAM Read - RAM[0x01]_PWR_STATUS = { G3 };\n\r");
            break;
        default:
            strcat(UartTxOutput, "EC RAM Read - RAM[0x01]_PWR_STATUS = { Unknown };\n\r");
            break;
    }

    strcat(UartTxOutput, "EC RAM Read - RAM[0x02]_HEARTBEAT = { ");
    sprintf(temp, "0x%02X };\n\r", RegRxBuffer_HEARTBEAT[0]);
    strcat(UartTxOutput, temp);

    strcat(UartTxOutput, "EC RAM Read - RAM[0x02]_HEARTBEAT after 1s waiting = { ");
    sprintf(temp, "0x%02X };\n\r", RegRxBuffer_HEARTBEAT_Wait1s[0]);
    strcat(UartTxOutput, temp);

    switch (RegRxBuffer_SYSTEMMODE[0])
    {
        case NORMAL_MODE:
            strcat(UartTxOutput, "EC RAM Read - RAM[0x03]_SYSTEMMODE = { NORMAL_MODE };\n\r\n\r");
            break;
        case SILENT_MODE:
            strcat(UartTxOutput, "EC RAM Read - RAM[0x03]_SYSTEMMODE = { SILENT_MODE };\n\r\n\r");
            break;
        default:
            strcat(UartTxOutput, "EC RAM Read - RAM[0x03]_SYSTEMMODE = { Unknown };\n\r\n\r");
            break;
    }

    BufferLen = (Ifx_SizeT)strlen(UartTxOutput);
    Send_ASCLIN_UART_Message(UartTxOutput, BufferLen);
    /* Debug printf info only -- END */

}


/************************************************
*  Execute command to transfer SOC system power 
*  state from S5 to S0
*
*  @param[in] void
*  @param[out] void
*
*  @retval void
************************************************/
void PowerTrans_S5_To_S0()
{
    uint8 RegRxBuffer_PWR_STATUS[LENGTH_OF_PWR_STATUS] = {0};
    // uint8 RegTxBuffer_PWR_STATUS[LENGTH_OF_PWR_STATUS] = {0x0};
    char UartTxOutput[128];                         /* Debug string buffer for UART */
    Ifx_SizeT BufferLen;                            /* Debug string buffer length for UART */

    /*EC POWER BOTTEN WAIT TIME*/
    Ifx_TickTime TicksFor10ms;                    /* Variable to store the number of ticks to wait for 1 second delay */
    TicksFor10ms = IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, 10);


    /* First check if system is in S5 */
    /* Read EC RAM offset 0x01 - Power Status */
    Read_EC_RAM_Data(RAMOFFS_OF_PWR_STATUS, &RegRxBuffer_PWR_STATUS[0], LENGTH_OF_PWR_STATUS);
    switch (RegRxBuffer_PWR_STATUS[0])
    {
        case SOC_S5:
            strcpy(UartTxOutput, "EC RAM Read System Power Status = { S5 }, Will power on system right now!\n\r\n\r");
            // RegTxBuffer_PWR_STATUS[0] = CMD_SWITCH_SOC_S5_TO_S0;            /* Send command to power on */
            // Write_Data_To_EC_RAM(RAMOFFS_OF_PWR_STATUS, &RegTxBuffer_PWR_STATUS[0], LENGTH_OF_PWR_STATUS);
            IfxPort_setPinState(EC_PD0_GP1_PIN, IfxPort_State_high);

            IfxPort_setPinState(EC_PWRBTN_PIN, IfxPort_State_low);
            wait(TicksFor10ms * 12);
            IfxPort_setPinState(EC_PWRBTN_PIN, IfxPort_State_high);
            break;
        case SOC_S1:
            strcpy(UartTxOutput, "EC RAM Read System Power Status = { S1 }, Will do nothing!\n\r\n\r");
            break;
        case SOC_S3:
            strcpy(UartTxOutput, "EC RAM Read System Power Status = { S3 }, Will do nothing!\n\r\n\r");
            break;
        case SOC_S4:
            strcpy(UartTxOutput, "EC RAM Read System Power Status = { S4 }, Will do nothing!\n\r\n\r");
            break;
        case SOC_S0:
            strcpy(UartTxOutput, "EC RAM Read System Power Status = { S0 }, Will do nothing!\n\r\n\r");
            break;
        case SOC_G3:
            strcpy(UartTxOutput, "EC RAM Read System Power Status = { G3 }, Will do nothing!\n\r\n\r");
            break;
        default:
            strcat(UartTxOutput, "System power status unknown, EC sleep, S0 - > S5 Ready!!!\n\r\n\r");
            break;
    }
    BufferLen = (Ifx_SizeT)strlen(UartTxOutput);
    Send_ASCLIN_UART_Message(UartTxOutput, BufferLen);
}


/************************************************
*  Execute command to transfer SOC system power 
*  state from S3 to S0
*
*  @param[in] void
*  @param[out] void
*
*  @retval void
************************************************/
void PowerTrans_S3_To_S0()
{
    uint8 RegRxBuffer_PWR_STATUS[LENGTH_OF_PWR_STATUS] = {0};
    // uint8 RegTxBuffer_PWR_STATUS[LENGTH_OF_PWR_STATUS] = {0x0};
    char UartTxOutput[128];                         /* Debug string buffer for UART */
    Ifx_SizeT BufferLen;                            /* Debug string buffer length for UART */

    /*EC POWER BOTTEN WAIT TIME*/
    Ifx_TickTime TicksFor10ms;                    /* Variable to store the number of ticks to wait for 1 second delay */
    TicksFor10ms = IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, 10);


    /* First check if system is in S5 */
    /* Read EC RAM offset 0x01 - Power Status */
    Read_EC_RAM_Data(RAMOFFS_OF_PWR_STATUS, &RegRxBuffer_PWR_STATUS[0], LENGTH_OF_PWR_STATUS);
    switch (RegRxBuffer_PWR_STATUS[0])
    {
        case SOC_S5:
            strcpy(UartTxOutput, "EC RAM Read System Power Status = { S5 }, Will do nothing!\n\r\n\r");
            break;
        case SOC_S1:
            strcpy(UartTxOutput, "EC RAM Read System Power Status = { S1 }, Will do nothing!\n\r\n\r");
            break;
        case SOC_S3:
            strcpy(UartTxOutput, "EC RAM Read System Power Status = { S3 }, Will power on system right now!\n\r\n\r");
            // RegTxBuffer_PWR_STATUS[0] = CMD_SWITCH_SOC_S3_TO_S0;            /* Send command to power on */
            // Write_Data_To_EC_RAM(RAMOFFS_OF_PWR_STATUS, &RegTxBuffer_PWR_STATUS[0], LENGTH_OF_PWR_STATUS);
            IfxPort_setPinState(EC_PWRBTN_PIN, IfxPort_State_low);
            wait(TicksFor10ms * 12);
            IfxPort_setPinState(EC_PWRBTN_PIN, IfxPort_State_high);

            IfxPort_setPinState(PHY_PIN, IfxPort_State_high);
            wait(TicksFor10ms);
            IfxPort_setPinState(PHY_PIN, IfxPort_State_low);
            break;
        case SOC_S4:
            strcpy(UartTxOutput, "EC RAM Read System Power Status = { S4 }, Will do nothing!\n\r\n\r");
            break;
        case SOC_S0:
            strcpy(UartTxOutput, "EC RAM Read System Power Status = { S0 }, Will do nothing!\n\r\n\r");
            break;
        case SOC_G3:
            strcpy(UartTxOutput, "EC RAM Read System Power Status = { G3 }, Will do nothing!\n\r\n\r");
            break;
        default:
            strcat(UartTxOutput, "System power status unknown, EC sleep, S3 - > S0 Ready!!!\n\r\n\r");
            IfxPort_setPinState(EC_PWRBTN_PIN, IfxPort_State_low);
            wait(TicksFor10ms * 12);
            IfxPort_setPinState(EC_PWRBTN_PIN, IfxPort_State_high);

            IfxPort_setPinState(PHY_PIN, IfxPort_State_high);
            wait(TicksFor10ms);
            IfxPort_setPinState(PHY_PIN, IfxPort_State_low);
            break;
    }
    BufferLen = (Ifx_SizeT)strlen(UartTxOutput);
    Send_ASCLIN_UART_Message(UartTxOutput, BufferLen);
}
