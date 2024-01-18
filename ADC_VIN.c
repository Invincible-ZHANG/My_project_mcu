/*
 * ADC_VIN.c
 *
 *  Created on: 2023年10月26日
 *      Author: zijianz1
 */

/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/
#include "ADC_VIN.h"
#include "IfxEvadc_Adc.h"
#include "ASCLIN_UART.h"
#include <string.h>
#include <stdio.h>
#include "Bsp.h"
#include <stdbool.h>

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/
#define GROUPID_1       IfxEvadc_GroupId_1                      /* EVADC group                                      */
#define AN9_CHID        1                                       /* Channel ID for PIN AN9                           */
#define AN10_CHID       2                                       /* Channel ID for PIN AN10                          */

#define ADC_VOLT        5                                       /* EVADC voltage                                    */
#define EVADC_VALUE_MAX 4096                                    /* The maximum value of the measurement results     */

#define EFUSE_PIN       &MODULE_P22, 7                          /* eFuse Pin                                        */

/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/
 /* EVADC handle */
IfxEvadc_Adc         g_evadc;                                   /* EVADC module handle variable                     */
IfxEvadc_Adc_Group   g_adcGroup;                                /* EVADC group handle variable                      */
IfxEvadc_Adc_Channel g_adcChannelAN10;                          /* EVADC channel handle variable                    */
IfxEvadc_Adc_Channel g_adcChannelAN9;                           /* EVADC channel handle variable                    */

Ifx_EVADC_G_RES g_resultAN10;                                   /* Variable to store the result of the measurement  */
Ifx_EVADC_G_RES g_resultAN9;                                    /* Variable to store the result of the measurement  */

/*********************************************************************************************************************/
/*------------------------------------------------Function Prototypes------------------------------------------------*/
/*********************************************************************************************************************/
void initEVADCModule(void);                                     /* Function to initialize the EVADC module          */
void initEVADCGroup(void);                                      /* Function to initialize the EVADC module          */
void initEVADCChannels(void);                                   /* Function to initialize the EVADC module          */
void fillAndStartQueue(void);                                   /* Function to add the channel to a queue           */

/*********************************************************************************************************************/
/*---------------------------------------------Function Implementations----------------------------------------------*/
/*********************************************************************************************************************/

/* Function to initialize the EVADC with default parameters */
void initEVADC(void)
{
    initEVADCModule();      /* Initialize the EVADC module  */
    initEVADCGroup();       /* Initialize the EVADC group   */
    initEVADCChannels();    /* Initialize the channel       */
    fillAndStartQueue();    /* Fill the queue and start it  */
}

/* Function to initialize the EVADC module with default parameters */
void initEVADCModule(void)
{
    /* Create configuration */
    IfxEvadc_Adc_Config adcConfig;
    IfxEvadc_Adc_initModuleConfig(&adcConfig, &MODULE_EVADC);

    /* Initialize module */
    IfxEvadc_Adc_initModule(&g_evadc, &adcConfig);
}

/* Function to initialize the EVADC group with default parameters */
void initEVADCGroup(void)
{
    /* Create and initialize group configuration with default values */
    IfxEvadc_Adc_GroupConfig adcGroupConfig;
    IfxEvadc_Adc_initGroupConfig(&adcGroupConfig, &g_evadc);

    /* Setting user configuration using group 0 */
    adcGroupConfig.groupId = GROUPID_1;
    adcGroupConfig.master = GROUPID_1;

    /* Enable queued source */
    adcGroupConfig.arbiter.requestSlotQueue0Enabled = TRUE;

    /* Enable all gates in "always" mode (no edge detection) */
    adcGroupConfig.queueRequest[0].triggerConfig.gatingMode = IfxEvadc_GatingMode_always;

    /* Initialize the group */
    IfxEvadc_Adc_initGroup(&g_adcGroup, &adcGroupConfig);
}


/* Function to initialize the EVADC channel */
void initEVADCChannels(void)
{
    /* Create channel configuration */
    IfxEvadc_Adc_ChannelConfig adcChannelConfigAN10;

    /* Initialize the configuration with default values */
    IfxEvadc_Adc_initChannelConfig(&adcChannelConfigAN10, &g_adcGroup);

    /* Select the channel ID and the respective result register */
    adcChannelConfigAN10.channelId = (IfxEvadc_ChannelId)(AN10_CHID);
    adcChannelConfigAN10.resultRegister = (IfxEvadc_ChannelResult)(AN10_CHID);

    /* Initialize the channel */
    IfxEvadc_Adc_initChannel(&g_adcChannelAN10, &adcChannelConfigAN10);

    /* Create channel configuration */
    IfxEvadc_Adc_ChannelConfig adcChannelConfigAN9;

    /* Initialize the configuration with default values */
    IfxEvadc_Adc_initChannelConfig(&adcChannelConfigAN9, &g_adcGroup);

    /* Select the channel ID and the respective result register */
    adcChannelConfigAN9.channelId = (IfxEvadc_ChannelId)(AN9_CHID);
    adcChannelConfigAN9.resultRegister = (IfxEvadc_ChannelResult)(AN9_CHID);

    /* Initialize the channel */
    IfxEvadc_Adc_initChannel(&g_adcChannelAN9, &adcChannelConfigAN9);

}

/* Function to add the channel to a queue */
void fillAndStartQueue(void)
{
    /* Add channel to queue with refill option enabled */
    IfxEvadc_Adc_addToQueue(&g_adcChannelAN10, IfxEvadc_RequestSource_queue0, IFXEVADC_QUEUE_REFILL);

    IfxEvadc_Adc_addToQueue(&g_adcChannelAN9, IfxEvadc_RequestSource_queue0, IFXEVADC_QUEUE_REFILL);

    /* Start the queue */
    IfxEvadc_Adc_startQueue(&g_adcGroup, IfxEvadc_RequestSource_queue0);
}

/**
 * Reads the result from an EVADC channel with error handling.
 * It retries reading multiple times if the result is not valid and returns a success or failure status.
 */

bool readEVADC(IfxEvadc_Adc_Channel *channel, Ifx_EVADC_G_RES *result) {
    Ifx_EVADC_G_RES conversionResult;
    int retryCount = 0;
    const int maxRetries = 5;

    do {
        conversionResult = IfxEvadc_Adc_getResult(channel);
        if (!conversionResult.B.VF) {
            retryCount++;
            if (retryCount >= maxRetries) {
                // Handle error: Log and/or take corrective action
                return false;
            }
        }
    } while (!conversionResult.B.VF);

    *result = conversionResult;
    return true;
}


/**
 * Retrieves the voltage value from a specified ADC channel.
 *
 * @param channel A pointer to IfxEvadc_Adc_Channel, indicating the ADC channel to read.
 * @return The calculated voltage value in Volts (V).
 *
 * This function first calls readEVADC to get raw data from the specified ADC channel,
 * and then calculates the actual voltage value based on predefined voltage and maximum ADC value.
 */
float getVoltageValue(IfxEvadc_Adc_Channel *channel) {
    Ifx_EVADC_G_RES result;
    readEVADC(channel, &result);
    return (float)(ADC_VOLT * result.B.RESULT) / (float)EVADC_VALUE_MAX;
}


/**
 * Monitors the voltage on a specific ADC channel and breaks the loop if it exceeds a threshold.
 * Continuously checks the voltage and stops when it goes above 1.3 volts.
 */

void judgeI2cSdaVal(void) {   
    float valueInt;
    while (1) {
        valueInt = getVoltageValue(&g_adcChannelAN10);

        if (valueInt > 1.3) {
            break;
        }   
    }
}

/**
 * Checks if the voltage value of a specific ADC channel is consistently above or below a threshold multiple times.
 *
 * @param channel A pointer to IfxEvadc_Adc_Channel, indicating the ADC channel to read.
 * @param threshold The voltage threshold for comparison.
 * @param times The number of consecutive checks.
 * @param delayMs The delay time between each check, in milliseconds.
 * @param checkType The type of check, either above (CHECK_ABOVE) or below (CHECK_BELOW) the threshold.
 * @return Returns true if the voltage value is consistently above or below the threshold for the specified number of times; otherwise, returns false.
 */
bool checkVoltageThreshold(IfxEvadc_Adc_Channel *channel, float threshold, int times, float delayMs, CheckType checkType) {
    float value;
    for (int i = 0; i < times; i++) {
        if (i > 0) {
            wait(delayMs);  // Wait for the specified time between consecutive checks
        }
        value = getVoltageValue(channel);  // Get the current voltage value

        // Determine if the voltage value meets the condition based on the check type
        if ((checkType == CHECK_ABOVE && value < threshold) || (checkType == CHECK_BELOW && value > threshold)) {
            return false;  // Return false if the condition is not met
        }
    }
    return true;  // Return true if the condition is consistently met
}

/**
 * Sets the state of the eFuse.
 * 
 * @param state Indicates the desired state of the eFuse. true to turn on, false to turn off.
 */
void setEfuseState(bool state) {
    if (state) {
        IfxPort_setPinState(EFUSE_PIN, IfxPort_State_low); // Turn on the eFuse
    } else {
        IfxPort_setPinState(EFUSE_PIN, IfxPort_State_high);  // Turn off the eFuse
    }
}

/**
 * Logs an action related to the eFuse.
 * 
 * @param action A string describing the action performed on the eFuse, e.g., "SOCPower off (High Voltage)".
 */
void logEfuseAction(const char* action) {
    char logBuffer[128];
    Ifx_SizeT bufferLen;
    bufferLen = (Ifx_SizeT)strlen(logBuffer);
//    Ifx_SizeT bufferLen = snprintf(logBuffer, strlen(logBuffer), "Efuse Action: %s\n\r", action);
    Send_ASCLIN_UART_Message(logBuffer, bufferLen); // Send log information via UART
}


void powerManagement(void) {
    const float highThreshold = 1.65;  // High voltage threshold
    const float lowThreshold = 0.85;   // Low voltage threshold
    const float restartHighThreshold = 1.6; // High voltage restart threshold
    const float restartLowThreshold = 0.9; // Low voltage restart threshold
    const int checkTimes = 3;           // Number of times to check
    const float delayMs = 10;           // Delay between checks (milliseconds)

    // Check if voltage is consistently above the high threshold
    if (checkVoltageThreshold(&g_adcChannelAN9, highThreshold, checkTimes, delayMs, CHECK_ABOVE)) {
        // Voltage is above high threshold, turn off eFuse
        setEfuseState(false);
        logEfuseAction("SOCPower off (High Voltage)");
        
        // Loop until voltage is below the high voltage restart threshold
        while (!checkVoltageThreshold(&g_adcChannelAN9, restartHighThreshold, checkTimes, delayMs, CHECK_BELOW)) {
            // Waiting for voltage to drop
        }
        // Voltage dropped below restart threshold, restart eFuse
        setEfuseState(true);
        logEfuseAction("SOC restart (Voltage Below High Restart Threshold)");
    } 
    // Check if voltage is consistently below the low threshold
    else if (checkVoltageThreshold(&g_adcChannelAN9, lowThreshold, checkTimes, delayMs, CHECK_BELOW)) {
        // Voltage is below low threshold, turn off eFuse
        setEfuseState(false);
        logEfuseAction("SOCPower off (Low Voltage)");

        // Loop until voltage is above the low voltage restart threshold
        while (!checkVoltageThreshold(&g_adcChannelAN9, restartLowThreshold, checkTimes, delayMs, CHECK_ABOVE)) {
            // Waiting for voltage to rise
        }
        // Voltage rose above restart threshold, restart eFuse
        setEfuseState(true);
        logEfuseAction("SOC restart (Voltage Above Low Restart Threshold)");
    }
}


/**
 * Checks for undervoltage conditions and shuts down the system if detected.
 * If the eFuse pin is off and undervoltage is detected on a specific port, it turns off the system.
 */

void undervoltageDetection(void)
{
    Ifx_TickTime TicksFor10ms;                    /* Variable to store the number of ticks to wait for 1 second delay */
    TicksFor10ms = IfxStm_getTicksFromMilliseconds(BSP_DEFAULT_TIMER, 10);

    char UartTxOutput[128];                         /* Debug string buffer for UART */
    Ifx_SizeT BufferLen;                            /* Debug string buffer length for UART */

    if(IfxPort_getPinState (EFUSE_PIN) == FALSE)
    {
        if(IfxPort_getPinState (&MODULE_P22, 8) == FALSE)
            {
                IfxPort_setPinState(EFUSE_PIN, IfxPort_State_high);
                strcpy(UartTxOutput, "Undervoltage Detection SHUT DOWN right now . \n\r\n\r\n\r\n\r");
                BufferLen = (Ifx_SizeT)strlen(UartTxOutput);
                Send_ASCLIN_UART_Message(UartTxOutput, BufferLen);
            }
    }
}

       

