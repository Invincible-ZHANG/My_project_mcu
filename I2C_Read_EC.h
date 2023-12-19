#ifndef I2C_READ_EC_H_
#define I2C_READ_EC_H_

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/
/* I2C HW configuration to control EC */
#define EC_SCL_PIN                  IfxI2c0_SCL_P13_1_INOUT     /* SCL PIN                                           */
#define EC_SDA_PIN                  IfxI2c0_SDA_P13_2_INOUT     /* SDA PIN       */
#define EC_PWRBTN_PIN               &MODULE_P20,0
#define EC_PD0_GP1_PIN               &MODULE_P20,3
                                     
#define I2C_BAUDRATE                100000                      /* 100 kHz baud rate                                 */
#define EC_I2C_ADDRESS              0x68                        /* 7 bit slave device address of EC is 0x68.         */

/*PHY*/
#define PHY_PIN                     &MODULE_P00,8

/* EC RAM define :
 * Offset   Definition              Data length     Attribute   Default
 * 0x00     Chip ID                 1               RO          0xEC
 * 0x01     Power Status            1               R/W         0x01
 * 0x02     Heart Beat              1               RO          0xFF
 * 0x03     System Mode             1               R/W         0x00 (0x00 - Normal mode, 0x01 - Silent Mode)
 * ... ...
 */
#define RAMOFFS_OF_CHIPID           0x00                        /* RAM offset of Chip ID register                    */
#define LENGTH_OF_CHIPID            1                           /* Length of the ChipID register                     */
#define RAMOFFS_OF_PWR_STATUS       0x01                        /* RAM offset of Power status register               */
#define LENGTH_OF_PWR_STATUS        1                           /* Length of Power status register                   */
#define RAMOFFS_OF_HEARTBEAT        0x02                        /* RAM offset of HeartBeat register                  */
#define LENGTH_OF_HEARTBEAT         1                           /* Length of HeartBeat register                      */
#define RAMOFFS_OF_SYSTEMMODE       0x03                        /* RAM offset of SystemMode register                 */
#define LENGTH_OF_SYSTEMMODE        1                           /* Length of SystemMode register                     */

/* EC RAM 0x01, SOC power status R/W define :
 * bit[7] = 1 means EC is busy;
 */
#define SOC_S0                      0x01                        /* Read, SOC system power state is S0                               */
#define SOC_S1                      0x02                        /* Read, SOC system power state is S1                               */
#define SOC_S3                      0x03                        /* Read, SOC system power state is S3                               */
#define SOC_S4                      0x04                        /* Read, SOC system power state is S4                               */
#define SOC_S5                      0x05                        /* Read, SOC system power state is S5                               */
#define SOC_G3                      0x06                        /* Read, SOC system power state is G3                               */
#define CMD_SWITCH_SOC_G3_TO_S5     0x80                        /* Write, switch SOC system power state from G3 to S5               */
#define CMD_SWITCH_SOC_S5_TO_S0     0x81                        /* Write, switch SOC system power state from S5 to S0               */
#define CMD_SWITCH_SOC_S0_TO_S3     0x82                        /* Write, switch SOC system power state from S0 to S3               */
#define CMD_SWITCH_SOC_S3_TO_S0     0x83                        /* Write, switch SOC system power state from S3 to S0               */
#define CMD_SWITCH_SOC_S0_TO_S5     0x84                        /* Write, switch SOC system power state from S0 to S5               */
#define CMD_SWITCH_SOC_S5_TO_G3     0x85                        /* Write, switch SOC system power state from S5 to G3               */

/* Macros for EC RAM 0x03, SOC system mode R/W define :
 * bit[7] = 1 means EC is busy;
 */
#define NORMAL_MODE                 0x00
#define SILENT_MODE                 0x01


/*********************************************************************************************************************/
/*------------------------------------------------Function Prototypes------------------------------------------------*/
/*********************************************************************************************************************/
void init_I2C_module(void);
void EC_Ram_Operation(void);
// void PowerTrans_S0_To_S5(void);
void PowerTrans_S5_To_S0(void);
void PowerTrans_S3_To_S0(void);
void Read_EC_RAM_Data(uint8 RegOff, uint8 *RegRxBuffer, uint8 RegLength);

#endif /* I2C_READ_EC_H_ */
