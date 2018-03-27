/**
  *********************************************************************************************************
  * @file    gatserial.c
  * @author  MoveBroad -- KangYJ
  * @version V1.0
  * @date    
  * @brief   
  *********************************************************************************************************
  * @attention
  *			功能 : 
  *			1.  使能/失能 发送/接收 串口中断
  *			2.  初始化串口
  *			3.  串口发送 1Byte数据
  *			4.  串口接收 1Byte数据
  *			5.  GAT协议串口中断处理函数
  *			6.  GAT协议串口中断接收处理函数
  *			7.  GAT协议串口中断发送处理函数
  *			8.  进入关键操作,关闭中断,等待中断结束返回
  *			9.  退出关键操作,开启中断
  *********************************************************************************************************
  */

#include "gatserial.h"


volatile u8  GATReceiveBuf[GAT_CACHE_SIZE];									//GAT接收数据缓存区
volatile u8  GATSendBuf[GAT_CACHE_SIZE];									//GAT发送数据缓存区
volatile u16 GATReceiveLength;											//GAT接收数据长度    [15] : 0,没有接收到数据; 1,接收到了一帧数据
volatile u16 GATSendLength;												//GAT发送数据长度    [15] : 0,数据正在发送;   1,数据发送完成
volatile u16 GATTxCounter;												//GAT发送数据指针

/**********************************************************************************************************
 @Function			void GAT_PortSerialEnable(u8 xRxEnable, u8 xTxEnable)
 @Description			使能/失能 发送/接收 串口中断
 @Input				xRxEnable : 1 ENABLE
							  0 DISABLE
					xTxEnable : 1 ENBALE
							: 0 DISABLE
 @Return				void
**********************************************************************************************************/
void GAT_PortSerialEnable(u8 xRxEnable, u8 xTxEnable)
{
	USART_TypeDef* GAT920_USART = USART1;
	
	if (PlatformGat920Usart == Gat920_USART1) {								//根据SN选择Gat920连接串口1
		GAT920_USART = USART1;
	}
	else {															//根据SN选择Gat920连接串口2
		GAT920_USART = USART2;
	}
	
	GAT_ENTER_CRITICAL_SECTION();
	
	if (xRxEnable) {													//使能串口接收中断
		while (USART_GetFlagStatus(GAT920_USART, USART_FLAG_TC) == 0);			//等待发送完最后一个字节成再开启读
#ifdef GAT920_SERIALPORT_USART1
		if (PlatformGat920Usart == Gat920_USART1) {							//根据SN选择Gat920连接串口1
			GPIO_ResetBits(GPIO_RS485_CTL, GPIO_RS485_CTL_PIN);
			USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
		}
#endif
#ifdef GAT920_SERIALPORT_USART2
		if (PlatformGat920Usart == Gat920_USART2) {							//根据SN选择Gat920连接串口2
			USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
		}
#endif
#ifdef GAT920_SERIALPORT_USART3
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
#endif
	}
	else {															//失能串口接收中断
		USART_ITConfig(GAT920_USART, USART_IT_RXNE, DISABLE);
	}
	
	if (xTxEnable) {													//使能串口发送中断
#ifdef GAT920_SERIALPORT_USART1
		if (PlatformGat920Usart == Gat920_USART1) {							//根据SN选择Gat920连接串口1
			USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
			GPIO_SetBits(GPIO_RS485_CTL, GPIO_RS485_CTL_PIN);
		}
#endif
#ifdef GAT920_SERIALPORT_USART2
		if (PlatformGat920Usart == Gat920_USART2) {							//根据SN选择Gat920连接串口2
			USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
		}
#endif
#ifdef GAT920_SERIALPORT_USART3
		USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
#endif
	}
	else {
		USART_ITConfig(GAT920_USART, USART_IT_TXE, DISABLE);					//失能串口发送中断
	}
	
	GAT_EXIT_CRITICAL_SECTION();
}

/**********************************************************************************************************
 @Function			u8 GAT_PortSerialInit(USART_TypeDef* Port, u32 BaudRate, u8 DataBits, GATParity Parity)
 @Description			初始化GAT串口
 @Input				Port		: 串口号
					BaudRate	: 波特率
					DataBits	: 数据位 7/8
					Parity	: 校验位 GAT_PAR_NONE/GAT_PAR_ODD/GAT_PAR_EVEN
 @Return				GAT_TRUE  : 正确
					GAT_FALSE : 错误
**********************************************************************************************************/
u8 GAT_PortSerialInit(USART_TypeDef* Port, u32 BaudRate, u8 DataBits, GATParity Parity)
{
	u8 Initialized = GAT_TRUE;
	
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	USART_TypeDef* GAT920_USART = USART1;
	
	if (PlatformGat920Usart == Gat920_USART1) {								//根据SN选择Gat920连接串口1
		GAT920_USART = USART1;
	}
	else {															//根据SN选择Gat920连接串口2
		GAT920_USART = USART2;
	}
	
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
		Initialized = GAT_FALSE;
	}
	
	/* USART MODE Config */
	USART_InitStructure.USART_BaudRate = BaudRate;
	
	switch (DataBits)
	{
	case 8:
		if (Parity == GAT_PAR_NONE)
			USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		else
			USART_InitStructure.USART_WordLength = USART_WordLength_9b;
		break;
	case 7:
		break;
	default:
		Initialized = GAT_FALSE;
	}
	
	switch (Parity)
	{
	case GAT_PAR_NONE:
		USART_InitStructure.USART_Parity = USART_Parity_No;
		break;
	case GAT_PAR_ODD:
		USART_InitStructure.USART_Parity = USART_Parity_Odd;
		break;
	case GAT_PAR_EVEN:
		USART_InitStructure.USART_Parity = USART_Parity_Even;
		break;
	}
	
	if (Initialized == GAT_TRUE) {
		GAT_ENTER_CRITICAL_SECTION();
		
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
		USART_Init(GAT920_USART, &USART_InitStructure);
		USART_Cmd(GAT920_USART, ENABLE);
		
		GAT_EXIT_CRITICAL_SECTION();
	}
	
	return Initialized;
}

/**********************************************************************************************************
 @Function			u8 GAT_PortSerialPutByte(u8 ucByte)
 @Description			串口发送 1Byte数据
 @Input				ucByte	: 1Byte数据
 @Return				GAT_TRUE  : 正确
					GAT_FALSE : 错误
**********************************************************************************************************/
u8 GAT_PortSerialPutByte(u8 ucByte)
{
	USART_TypeDef* GAT920_USART = USART1;
	
	if (PlatformGat920Usart == Gat920_USART1) {								//根据SN选择Gat920连接串口1
		GAT920_USART = USART1;
	}
	else {															//根据SN选择Gat920连接串口2
		GAT920_USART = USART2;
	}
	
	USART_SendData(GAT920_USART, ucByte);
	while (USART_GetFlagStatus(GAT920_USART, USART_FLAG_TXE) == 0);				//等待发送寄存器空
	return GAT_TRUE;
}

/**********************************************************************************************************
 @Function			u8 GAT_PortSerialGetByte(u8 *pucByte)
 @Description			串口接收 1Byte数据
 @Input				pucByte	: 接收1Byte数据地址
 @Return				GAT_TRUE  : 正确
					GAT_FALSE : 错误
**********************************************************************************************************/
u8 GAT_PortSerialGetByte(u8 *pucByte)
{
	USART_TypeDef* GAT920_USART = USART1;
	
	if (PlatformGat920Usart == Gat920_USART1) {								//根据SN选择Gat920连接串口1
		GAT920_USART = USART1;
	}
	else {															//根据SN选择Gat920连接串口2
		GAT920_USART = USART2;
	}
	
	*pucByte = (u8)USART_ReceiveData(GAT920_USART);							//获取接收到的数据
	return GAT_TRUE;
}

/**********************************************************************************************************
 @Function			void GAT_UARTx_IRQ(USART_TypeDef* USARTx)
 @Description			GAT协议串口中断处理函数
 @Input				USARTx : 串口号
 @Return				void
**********************************************************************************************************/
void GAT_UARTx_IRQ(USART_TypeDef* USARTx)
{
	USART_TypeDef* GAT920_USART = USART1;
	
	if (PlatformGat920Usart == Gat920_USART1) {								//根据SN选择Gat920连接串口1
		GAT920_USART = USART1;
	}
	else {															//根据SN选择Gat920连接串口2
		GAT920_USART = USART2;
	}
	
	if (USART_GetITStatus(USARTx, USART_IT_TXE)) {							//发送中断处理
		if (USARTx == GAT920_USART) {
			GAT_FrameCBTransmitterEmpty();								//发送中断处理函数
		}
		USART_ClearITPendingBit(USARTx, USART_IT_TXE);						//清发送中断标志位
	}
	else if (USART_GetITStatus(USARTx, USART_IT_RXNE)) {						//接收中断处理
		if (USARTx == GAT920_USART) {
			GAT_FrameCBByteReceived();									//接收中断处理函数
		}
		USART_ClearITPendingBit(USARTx, USART_IT_RXNE);						//清接收中断标志位
	}
}

/**********************************************************************************************************
 @Function			void GAT_FrameCBByteReceived(void)
 @Description			GAT协议串口中断接收处理函数
 @Input				void
 @Return				void
**********************************************************************************************************/
void GAT_FrameCBByteReceived(void)
{
	u8 recvivedata;
	
	GAT_PortSerialGetByte((u8 *) &recvivedata);								//获取串口接收到数据
	if ((GATReceiveLength & (1<<15)) == 0)									//接收完的一批数据,还没有被处理,则不再接收其他数据
	{
		if (GATReceiveLength < GAT_CACHE_SIZE) {							//还可以接收数据
			TIM_SetCounter(TIM1, 0);										//定时计数器清空
			if (GATReceiveLength == 0) {									//使能定时器1的中断
				TIM_Cmd(TIM1, ENABLE);									//使能定时器1
			}
			GATReceiveBuf[GATReceiveLength++] = recvivedata;					//记录接收到的值
		}
		else {
			GATReceiveLength |= 1<<15;									//强制标记接收完成
		}
	}
}

/**********************************************************************************************************
 @Function			void GAT_FrameCBTransmitterEmpty(void)
 @Description			GAT协议串口中断发送处理函数
 @Input				void
 @Return				void
**********************************************************************************************************/
void GAT_FrameCBTransmitterEmpty(void)
{
	USART_TypeDef* GAT920_USART = USART1;
	
	if (PlatformGat920Usart == Gat920_USART1) {								//根据SN选择Gat920连接串口1
		GAT920_USART = USART1;
	}
	else {															//根据SN选择Gat920连接串口2
		GAT920_USART = USART2;
	}
	
	GAT_PortSerialPutByte(GATSendBuf[GATTxCounter++]);						//发送数据
	if ((GATTxCounter == GATSendLength) || (GATTxCounter >= GAT_CACHE_SIZE)) {		//发送数据完毕
		USART_ITConfig(GAT920_USART, USART_IT_TXE, DISABLE);					//关闭发送中断
		GATTxCounter = 0;
		GATSendLength |= 1<<15;											//标志发送数据完毕
	}
}

/**********************************************************************************************************
 @Function			void GAT_EnterCriticalSection(void)
 @Description			进入关键操作,关闭中断,等待中断结束返回
 @Input				void
 @Return				void
**********************************************************************************************************/
void GAT_EnterCriticalSection(void)
{
	__disable_irq();
}

/**********************************************************************************************************
 @Function			void GAT_ExitCriticalSection(void)
 @Description			退出关键操作,开启中断
 @Input				void
 @Return				void
**********************************************************************************************************/
void GAT_ExitCriticalSection(void)
{
	__enable_irq();
}

/********************************************** END OF FLEE **********************************************/
