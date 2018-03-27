#ifndef __GAT_SERIAL_H
#define   __GAT_SERIAL_H

#include "stm32f10x_lib.h"
#include "platform_config.h"
#include "gatconfig.h"


#define GAT_ENTER_CRITICAL_SECTION()		GAT_EnterCriticalSection()
#define GAT_EXIT_CRITICAL_SECTION()		GAT_ExitCriticalSection()

typedef enum
{
	GAT_PAR_NONE,																/* !< No parity.   */
	GAT_PAR_ODD,																/* !< Odd parity.  */
	GAT_PAR_EVEN																/* !< Even parity. */
}GATParity;

extern volatile u8  GATReceiveBuf[GAT_CACHE_SIZE];									//GAT接收数据缓存区
extern volatile u8  GATSendBuf[GAT_CACHE_SIZE];										//GAT发送数据缓存区
extern volatile u16 GATReceiveLength;												//GAT接收数据长度
extern volatile u16 GATSendLength;													//GAT发送数据长度


void GAT_PortSerialEnable(u8 xRxEnable, u8 xTxEnable);									//使能/失能 发送/接收 串口中断
u8   GAT_PortSerialInit(USART_TypeDef* Port, u32 BaudRate, u8 DataBits, GATParity Parity);	//初始化串口
u8   GAT_PortSerialPutByte(u8 ucByte);												//串口发送 1Byte数据
u8   GAT_PortSerialGetByte(u8 *pucByte);											//串口接收 1Byte数据

void GAT_UARTx_IRQ(USART_TypeDef* USARTx);											//GAT协议串口中断处理函数
void GAT_FrameCBByteReceived(void);												//GAT协议串口中断接收处理函数
void GAT_FrameCBTransmitterEmpty(void);												//GAT协议串口中断发送处理函数

void GAT_EnterCriticalSection(void);												//进入关键操作,关闭中断,等待中断结束返回
void GAT_ExitCriticalSection(void);												//退出关键操作,开启中断

#endif
