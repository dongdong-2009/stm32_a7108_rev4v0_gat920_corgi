/**
  *********************************************************************************************************
  * @file    usrserial.c
  * @author  MoveBroad -- KangYJ
  * @version V1.0
  * @date    
  * @brief   USR串口通信协议
  *********************************************************************************************************
  * @attention
  *		
  *			
  *********************************************************************************************************
  */

#include "usrserial.h"


#define USRSerialPort		USART3										//USR串口

volatile u8  USRReceiveBuf[USR_CACHE_SIZE];									//USR接收数据缓存区
volatile u16 USRReceiveLength;											//USR接收数据长度    [15] : 0,没有接收到数据; 1,接收到了一帧数据


extern void Delay(vu32 nCount);
#define RS485_Mode_RX()		GPIO_ResetBits( GPIOA, GPIO_Pin_1 )
#define RS485_Mode_TX()		GPIO_SetBits( GPIOA, GPIO_Pin_1 )

/**********************************************************************************************************
 @Function			void USR_PortSerialEnable(u8 xRxEnable, u8 xTxEnable)
 @Description			使能/失能 发送/接收 串口中断
 @Input				xRxEnable : 1 ENABLE
							  0 DISABLE
					xTxEnable : 1 ENBALE
							: 0 DISABLE
 @Return				void
**********************************************************************************************************/
void USR_PortSerialEnable(u8 xRxEnable, u8 xTxEnable)
{
	USR_ENTER_CRITICAL_SECTION();
	
	if (xRxEnable) {													//使能串口接收中断
		while (USART_GetFlagStatus(USRSerialPort, USART_FLAG_TC) == 0);			//等待发送完最后一个字节成再开启读
		if (USRSerialPort == USART1) {
			GPIO_ResetBits(GPIO_RS485_CTL, GPIO_RS485_CTL_PIN);
		}
		USART_ITConfig(USRSerialPort, USART_IT_RXNE, ENABLE);
	}
	else {															//失能串口接收中断
		USART_ITConfig(USRSerialPort, USART_IT_RXNE, DISABLE);
	}
	
	if (xTxEnable) {													//使能串口发送中断
		USART_ITConfig(USRSerialPort, USART_IT_TXE, ENABLE);
		if (USRSerialPort == USART1) {
			GPIO_SetBits(GPIO_RS485_CTL, GPIO_RS485_CTL_PIN);
		}
	}
	else {															//失能串口发送中断
		USART_ITConfig(USRSerialPort, USART_IT_TXE, DISABLE);
	}
	
	USR_EXIT_CRITICAL_SECTION();
}

/**********************************************************************************************************
 @Function			USRErrorCode USR_PortSerialInit(USART_TypeDef* Port, u32 BaudRate, u8 DataBits, USRParity Parity)
 @Description			初始化USR串口
 @Input				Port			: 串口号
					BaudRate		: 波特率
					DataBits		: 数据位 7/8
					Parity		: 校验位 GAT_PAR_NONE/GAT_PAR_ODD/GAT_PAR_EVEN
 @Return				USRErrorCode	: 错误信息
**********************************************************************************************************/
USRErrorCode USR_PortSerialInit(USART_TypeDef* Port, u32 BaudRate, u8 DataBits, USRParity Parity)
{
	USRErrorCode Initialized = USR_ENOERR;
	
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	/* USART Clock / GPIO Config */
	if (Port == USART1) {
		/* Config USART1 and GPIOA Clock */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
		
		/* USART1 GPIO Config */
		/* Config USART1 TX ( PA.9 ) */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		/* Config USART1 RX ( PA.10 ) */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		/* Config USART1_RS485 DE ( PA.1 ) */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}
	else if (Port == USART2) {
		/* Config USART2 and GPIOA Clock */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		
		/* USART2 GPIO Config */
		/* Config USART2 TX ( PA.2 ) */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		/* Config USART2 RX ( PA.3 ) */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}
	else if (Port == USART3) {
		/* Config USART3 and GPIOB Clock */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		
		/* USART3 GPIO Config */
		/* Config USART3 TX ( PB.10 ) */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		/* Config USART3 RX ( PB.11 ) */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
	}
	else {
		Initialized = USR_EPORTINITERR;
	}
	
	/* USART MODE Config */
	USART_InitStructure.USART_BaudRate = BaudRate;
	
	switch (DataBits)
	{
	case 8:
		if (Parity == USR_PAR_NONE)
			USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		else
			USART_InitStructure.USART_WordLength = USART_WordLength_9b;
		break;
	case 7:
		break;
	default:
		Initialized = USR_EPORTINITERR;
	}
	
	switch (Parity)
	{
	case USR_PAR_NONE:
		USART_InitStructure.USART_Parity = USART_Parity_No;
		break;
	case USR_PAR_ODD:
		USART_InitStructure.USART_Parity = USART_Parity_Odd;
		break;
	case USR_PAR_EVEN:
		USART_InitStructure.USART_Parity = USART_Parity_Even;
		break;
	}
	
	if (Initialized == USR_ENOERR) {
		USR_ENTER_CRITICAL_SECTION();
		
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(Port, &USART_InitStructure);
		USART_Cmd(Port, ENABLE);
		
		USR_EXIT_CRITICAL_SECTION();
	}
	
	return Initialized;
}

/**********************************************************************************************************
 @Function			void USR_PortSerialPutByte(u8 ucByte)
 @Description			串口发送 1Byte 数据
 @Input				ucByte	: 1Byte数据
 @Return				void
**********************************************************************************************************/
void USR_PortSerialPutByte(u8 ucByte)
{
	USART_SendData(USRSerialPort, ucByte);
	while (USART_GetFlagStatus(USRSerialPort, USART_FLAG_TXE) == 0);				//等待发送寄存器空
}

/**********************************************************************************************************
 @Function			void USR_PortSerialGetByte(u8 *pucByte)
 @Description			串口接收 1Byte 数据
 @Input				pucByte	: 接收1Byte数据地址
 @Return				void
**********************************************************************************************************/
void USR_PortSerialGetByte(u8 *pucByte)
{
	*pucByte = (u8)USART_ReceiveData(USRSerialPort);							//获取接收到的数据
}

/**********************************************************************************************************
 @Function			void USR_PortSerialSend(u8 *data_arr, u32 length)
 @Description			串口发送数据
 @Input				data_arr	: 数据地址
					length	: 数据长度
 @Return				void
**********************************************************************************************************/
void USR_PortSerialSend(u8 *data_arr, u32 length)
{
	if (USRSerialPort == USART1) {
		RS485_Mode_TX();
		Delay(0x1fff);
		while (length > 0) {
			USR_PortSerialPutByte(*data_arr++);
			length--;
		}
		Delay(0x1fff);
		RS485_Mode_RX();
	}
	else {
		while (length > 0) {
			USR_PortSerialPutByte(*data_arr++);
			length--;
		}
	}
}

/**********************************************************************************************************
 @Function			void USR_UARTx_IRQ(USART_TypeDef* USARTx)
 @Description			USR串口中断处理函数
 @Input				USARTx : 串口号
 @Return				void
**********************************************************************************************************/
void USR_UARTx_IRQ(USART_TypeDef* USARTx)
{
	if (USART_GetITStatus(USARTx, USART_IT_TXE)) {							//发送中断处理
		if (USARTx == USRSerialPort) {
			USR_FrameCBTransmitterEmpty();								//发送中断处理函数
		}
		USART_ClearITPendingBit(USARTx, USART_IT_TXE);						//清发送中断标志位
	}
	else if (USART_GetITStatus(USARTx, USART_IT_RXNE)) {						//接收中断处理
		if (USARTx == USRSerialPort) {
			USR_FrameCBByteReceived();									//接收中断处理函数
		}
		USART_ClearITPendingBit(USARTx, USART_IT_RXNE);						//清接收中断标志位
	}
}

/**********************************************************************************************************
 @Function			void GAT_FrameCBByteReceived(void)
 @Description			USR串口中断接收处理函数
 @Input				void
 @Return				void
**********************************************************************************************************/
void USR_FrameCBByteReceived(void)
{
	u8 recvivedata;
	
	USR_PortSerialGetByte((u8 *) &recvivedata);								//获取串口接收到数据
	if ((USRReceiveLength & (1<<15)) == 0)									//接收完的一批数据,还没有被处理,则不再接收其他数据
	{
		if (USRReceiveLength < USR_CACHE_SIZE) {							//还可以接收数据
			TIM_SetCounter(TIM2, 0);										//定时计数器清空
			if (USRReceiveLength == 0) {									//使能定时器2的中断
				TIM_Cmd(TIM2, ENABLE);									//使能定时器2
			}
			USRReceiveBuf[USRReceiveLength++] = recvivedata;					//记录接收到的值
		}
		else {
			USRReceiveLength |= 1<<15;									//强制标记接收完成
		}
	}
}

/**********************************************************************************************************
 @Function			void GAT_FrameCBTransmitterEmpty(void)
 @Description			USR串口中断发送处理函数
 @Input				void
 @Return				void
**********************************************************************************************************/
void USR_FrameCBTransmitterEmpty(void)
{
	
}

/**********************************************************************************************************
 @Function			void USR_EnterCriticalSection(void)
 @Description			进入关键操作,关闭中断,等待中断结束返回
 @Input				void
 @Return				void
**********************************************************************************************************/
void USR_EnterCriticalSection(void)
{
	__disable_irq();
}

/**********************************************************************************************************
 @Function			void USR_ExitCriticalSection(void)
 @Description			退出关键操作,开启中断
 @Input				void
 @Return				void
**********************************************************************************************************/
void USR_ExitCriticalSection(void)
{
	__enable_irq();
}

/********************************************** END OF FLEE **********************************************/
