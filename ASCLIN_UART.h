#ifndef ASCLIN_UART_H_
#define ASCLIN_UART_H_

/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/


/*********************************************************************************************************************/
/*------------------------------------------------Function Prototypes------------------------------------------------*/
/*********************************************************************************************************************/
void init_ASCLIN_UART(void);
void Send_ASCLIN_UART_Message(char *Data, Ifx_SizeT DataSize);

#endif /* ASCLIN_UART_H_ */