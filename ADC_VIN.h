/*
 * ADC_VIN.h
 *
 *  Created on: 2023年10月26日
 *      Author: zijianz1
 */

#ifndef ADC_VIN_H_
#define ADC_VIN_H_

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/

/**
  * Defines the type used for voltage checking.
  *
  * CHECK_ABOVE - Indicates checking whether the voltage is continuously above a certain threshold.
  * CHECK_BELOW - Indicates checking whether the voltage is continuously lower than a certain threshold.
  */
typedef enum {
    CHECK_ABOVE,  // For checking voltage above threshold
    CHECK_BELOW   // For checking voltage below threshold
} CheckType;

/*********************************************************************************************************************/
/*------------------------------------------------Function Prototypes------------------------------------------------*/
/*********************************************************************************************************************/
// Initializes the EVADC module.
void initEVADC(void);

// Manages the power based on ADC readings.
void powerManagement(void);

// Monitors I2C SDA voltage levels.
void judgeI2cSdaVal(void);

// Detects and handles undervoltage situations.
void undervoltageDetection(void);


#endif /* ADC_VIN_H_ */
