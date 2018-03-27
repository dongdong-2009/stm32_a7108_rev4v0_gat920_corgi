#ifndef __USR_SERIAL_H
#define   __USR_SERIAL_H

#include "stm32f10x_lib.h"
#include "platform_config.h"
#include "usrconfig.h"


#define USR_ENTER_CRITICAL_SECTION()		USR_EnterCriticalSection()
#define USR_EXIT_CRITICAL_SECTION()		USR_ExitCriticalSection()

#define USR_CACHE_SIZE		64													//数据缓存区大小

typedef enum
{
	USR_PAR_NONE,																/* !< No parity.   */
	USR_PAR_ODD,																/* !< Odd parity.  */
	USR_PAR_EVEN																/* !< Even parity. */
}USRParity;

extern volatile u8  USRReceiveBuf[USR_CACHE_SIZE];										//USR接收数据缓存区
extern volatile u16 USRReceiveLength;													//接收数据长度

void USR_PortSerialEnable(u8 xRxEnable, u8 xTxEnable);										//使能/失能 发送/接收 串口中断
USRErrorCode USR_PortSerialInit(USART_TypeDef* Port, u32 BaudRate, u8 DataBits, USRParity Parity);	//初始化串口
void USR_PortSerialPutByte(u8 ucByte);													//串口发送 1Byte 数据
void USR_PortSerialGetByte(u8 *pucByte);												//串口接收 1Byte 数据
void USR_PortSerialSend(u8 *data_arr, u32 length);										//串口发送数据

void USR_UARTx_IRQ(USART_TypeDef* USARTx);												//USR串口中断处理函数
void USR_FrameCBByteReceived(void);													//USR串口中断接收处理函数
void USR_FrameCBTransmitterEmpty(void);													//USR串口中断发送处理函数

void USR_EnterCriticalSection(void);												//进入关键操作,关闭中断,等待中断结束返回
void USR_ExitCriticalSection(void);												//退出关键操作,开启中断

#endif
