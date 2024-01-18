/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/
#include "GPIO_Init.h"

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/
#define Do_NOT_CARE 0 /* UART baud rate in bit/s                   */

/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/
GPIO_Init_Table A1_Gpio_Table[] =
    {
        {&MODULE_P00, 5, IfxPort_Mode_outputPushPullGeneral, IfxPort_State_high}, // A2B
        {&MODULE_P00, 6, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P00, 8, IfxPort_Mode_outputPushPullGeneral, IfxPort_State_low}, // PHY
        {&MODULE_P00, 10, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},

        {&MODULE_P02, 7, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},

        {&MODULE_P10, 1, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P10, 2, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P10, 4, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P10, 5, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P10, 6, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},

        {&MODULE_P11, 10, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P11, 13, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P11, 14, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},

        {&MODULE_P13, 1, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P13, 2, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},

        {&MODULE_P14, 0, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P14, 1, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P14, 2, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P14, 3, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P14, 4, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P14, 5, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},

        {&MODULE_P15, 2, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P15, 3, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P15, 6, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P15, 7, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},

        {&MODULE_P20, 0, IfxPort_Mode_outputPushPullGeneral, IfxPort_State_high}, // KB_PWRBTN_EC
        {&MODULE_P20, 2, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P20, 3, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P20, 7, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P20, 8, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P20, 11, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P20, 12, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P20, 13, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P20, 14, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},

        {&MODULE_P21, 0, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P21, 1, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P21, 2, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P21, 3, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P21, 5, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},

        {&MODULE_P22, 0, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P22, 1, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P22, 6, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},

        {&MODULE_P22, 8, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P22, 9, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},

        {&MODULE_P23, 2, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P23, 3, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P23, 4, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},

        {&MODULE_P33, 0, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P33, 4, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P33, 10, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P33, 11, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},

        {&MODULE_P40, 2, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P40, 3, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P40, 10, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P40, 11, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P40, 12, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P40, 13, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P40, 14, IfxPort_Mode_inputNoPullDevice, Do_NOT_CARE},
        {&MODULE_P22, 7, IfxPort_Mode_outputPushPullGeneral, IfxPort_State_low}, // SOC eFuse
};

/*********************************************************************************************************************/
/*---------------------------------------------Function Implementations----------------------------------------------*/
/*********************************************************************************************************************/
/********************************************************************
 *  This function initializes project GPIO list
 *
 *  @param[in] void
 *  @param[out] void
 *
 *  @retval void
 ********************************************************************/
void Init_GPIO_Table()
{
    for (uint8 i = 0; i < sizeof(A1_Gpio_Table) / sizeof(GPIO_Init_Table); i++)
    {
        IfxPort_setPinMode(A1_Gpio_Table[i].port, A1_Gpio_Table[i].pinIndex, A1_Gpio_Table[i].mode);
        if (A1_Gpio_Table[i].mode > IfxPort_Mode_inputPullUp) // All modes > IfxPort_Mode_inputPullUp are output
        {
            IfxPort_setPinState(A1_Gpio_Table[i].port, A1_Gpio_Table[i].pinIndex, A1_Gpio_Table[i].action);
        }
    }
}
