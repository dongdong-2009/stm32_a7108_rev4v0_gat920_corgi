/**
  *********************************************************************************************************
  * @file    socketinitialization.c
  * @author  MoveBroad -- KangYJ
  * @version V1.0
  * @date    
  * @brief   Socket协议初始化
  *********************************************************************************************************
  * @attention
  *			功能:
  *			1. 初始化Socket协议
  *			2. 初始化Socket串口
  *			3. Socket协议串口中断处理函数
  *			4. Socket协议处理
  *			
  *********************************************************************************************************
  */

#include "socketinitialization.h"
#include "socketconfig.h"
#include "socketfunc.h"

#ifdef SOCKET_SERIALPORT_USART1
#define SOCKET_USART		USART1
#endif
#ifdef SOCKET_SERIALPORT_USART2
#define SOCKET_USART		USART2
#endif
#ifdef SOCKET_SERIALPORT_USART3
#define SOCKET_USART		USART3
#endif

extern GPIO_TypeDef* OUTPUT_TYPE[16];
extern u16 OUTPUT_PIN[16];
extern void Delay(vu32 nCount);
#define RS485_Mode_RX()		GPIO_ResetBits( GPIOA, GPIO_Pin_1 )
#define RS485_Mode_TX()		GPIO_SetBits( GPIOA, GPIO_Pin_1 )

#define SOCKETRTCHECKENABLE												//RTC对时使能
#define SOCKETRTCCHECKLED1												//选择开启RTC对时LED1提示
#define SOCKETUSARTTXIRQn												//选择开启USART发送中断(是否使用中断发送或普通发送)

/**
  * 通过判断接收连续2个字符之间的时间差不大于timer来决定是不是一次连续的数据
  * 如果2个字符接收间隔超过timer,则认为不是1次连续数据.也就是超过timer没有接收到任何数据,则表示此次接收完毕
  * 接收到的数据状态
  * [15]  :  0,没有接收到数据;  1,接收到了一批数据.
  * [14:0]:  接收到的数据长度
  */
volatile u16 SOCKET_RX_STA = 0;											//接收数据状态
#ifdef SOCKETRTCHECKENABLE
volatile u8  SOCKET_RTC_CHECK = PACKETTYPE_RTCCHECKINIT;						//RTC对时校验标志  0x80上电对时 0x81运行对时 0x01不对时
#else
volatile u8  SOCKET_RTC_CHECK = PACKETTYPE_FLOWMESSAGE;
#endif

#ifdef SOCKETUSARTTXIRQn
u16 SocketTxDataLangth = 0;												//发送数据长度
volatile u16 TxCounter = 0;												//发送数据地址
#endif

/**********************************************************************************************************
 @Function			void SOCKET_Init(void)
 @Description			初始化Socket协议
 @Input				void
 @Return				void
**********************************************************************************************************/
void SOCKET_Init(void)
{
	socket_dev.PortSerialInit(SOCKET_USART, 115200);							//初始化Socket协议串口
	SOCKET_RX_STA = 0;													//未接收数据状态
	TIM_Cmd(TIM2, DISABLE);												//关闭定时器2
#ifdef SOCKETRTCCHECKLED1
	if (PlatformSockettime == SocketTime_ENABLE) {
		GPIO_SetBits(OUTPUT_TYPE[0], OUTPUT_PIN[0]);
	}
#endif
}

/**********************************************************************************************************
 @Function			void SOCKET_UARTx_IRQ(USART_TypeDef* USARTx)
 @Description			Socket协议串口中断处理函数
 @Input				串口号
 @Return				void
**********************************************************************************************************/
void SOCKET_UARTx_IRQ(USART_TypeDef* USARTx)
{
	u8 recvivedata;
	
	if (USART_GetITStatus(USARTx, USART_IT_TXE)) {							//发送中断处理
#ifdef SOCKETUSARTTXIRQn
		USART_SendData(USARTx, (u8) SocketSendBuf[TxCounter++]);				//发送数据
		if (TxCounter == SocketTxDataLangth) {								//发送数据完成
			USART_ITConfig(SOCKET_USART, USART_IT_TXE, DISABLE);				//关闭发送中断
			if (SOCKET_USART == USART1) {									//485发送结束转为接收模式
				Delay(0x1fff);
				RS485_Mode_RX();
			}
		}
#endif
		USART_ClearITPendingBit(USARTx, USART_IT_TXE);
	}
	else if (USART_GetITStatus(USARTx, USART_IT_RXNE)) {						//接收中断处理
		
		recvivedata = USART_ReceiveData(USARTx);
		if ((SOCKET_RX_STA & (1<<15)) == 0) 								//接收完的一批数据,还没有被处理,则不再接收其他数据
		{
			if (SOCKET_RX_STA < SOCKET_RECVIVE_SIZE)						//还可以接收数据
			{
				TIM_SetCounter(TIM2, 0);									//计数器清空
				if (SOCKET_RX_STA == 0) {								//使能定时器2的中断
					TIM_Cmd(TIM2, ENABLE);								//使能定时器2
				}
				SocketReceiveBuf[SOCKET_RX_STA++] = recvivedata;				//记录接收到的值
			}
			else {
				SOCKET_RX_STA |= 1<<15;									//强制标记接收完成
			}
		}
		USART_ClearITPendingBit(USARTx, USART_IT_RXNE);
	}
}

/**********************************************************************************************************
 @Function			void SOCKET_Implement(u16 sendtime)
 @Description			Socket协议处理
 @Input				统计时长
 @Return				void
**********************************************************************************************************/
void SOCKET_Implement(u16 sendtime)
{
	static u16 socketsendtime = 0;										//socket发送时间计时器
	static u16 socketsendlength = 0;										//socket发送数据长度
	static u8 receiveflag = 0;
	static u16 receivesendtime = 0;
	static u8 sendnum = 0;
	u32 rtctime = 0;
	
	socketsendtime += 1;
	if ((socketsendtime >= sendtime) && (receiveflag == 0)) {					//到达发送时间
		socketsendtime = 0;												//计时器清0
		SOCKET_RX_STA = 0;												//未接收数据状态
		socketsendlength = socket_dev.PackagedStorage();						//将数据打包存入缓存区
#ifdef SOCKETUSARTTXIRQn
		SocketTxDataLangth = socketsendlength;								//获取发送数据长度
		TxCounter = 0;													//发送地址归0
		if (SOCKET_USART == USART1) {										//485发送
			RS485_Mode_TX();
			Delay(0x1fff);
		}
		USART_ITConfig(SOCKET_USART, USART_IT_TXE, ENABLE);					//开发送中断,并进入发送中断发送数据
#else
		SOCKET_USARTSend(SOCKET_USART, (u8 *)SocketSendBuf, socketsendlength);	//发送数据
#endif
		receiveflag = 1;
	}
	if (receiveflag == 1) {
		receivesendtime += 1;
	}
	if (receivesendtime >= SOCKET_AGAINSENDTIME) {							//到达检测时间
		receivesendtime = 0;
		if (SOCKET_RX_STA & 0X8000) {										//接收到期待的应答结果
			if ((SocketReceiveBuf[8] == PACKETTYPE_RTCCHECK) || 
			    (SocketReceiveBuf[8] == PACKETTYPE_RTCCHECKINIT)) {			//接受到对时数据
				rtctime |= SocketReceiveBuf[13] << 24;
				rtctime |= SocketReceiveBuf[14] << 16;
				rtctime |= SocketReceiveBuf[15] << 8;
				rtctime |= SocketReceiveBuf[16];
				rtctime += (8 * 3600);									//获取时间数据
				RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
				PWR_BackupAccessCmd(ENABLE);
				RTC_WaitForLastTask();
				RTC_SetCounter(rtctime);									//写入RTC寄存器值
				RTC_WaitForLastTask();
				SOCKET_RTC_CHECK = 0;									//对时结束置0
#ifdef SOCKETRTCCHECKLED1
				if (PlatformSockettime == SocketTime_ENABLE) {
					GPIO_ResetBits(OUTPUT_TYPE[0], OUTPUT_PIN[0]);
				}
#endif
			}
			if ((SocketReceiveBuf[8] == PACKETTYPE_RTCCHECK) ||				//接收数据正确
			    (SocketReceiveBuf[8] == PACKETTYPE_RTCCHECKINIT) ||
			    (SocketReceiveBuf[8] == PACKETTYPE_FLOWMESSAGE)) {
				sendnum = 0;
				receiveflag = 0;
				receivesendtime = 0;
			}
			else {
				SOCKET_RX_STA = 0;
			}
		}
		else {
#ifdef SOCKETUSARTTXIRQn
			SocketTxDataLangth = socketsendlength;							//获取发送数据长度
			TxCounter = 0;												//发送地址归0
			if (SOCKET_USART == USART1) {									//485发送
				RS485_Mode_TX();
				Delay(0x1fff);
			}
			USART_ITConfig(SOCKET_USART, USART_IT_TXE, ENABLE);				//开发送中断,并进入发送中断发送数据
#else
			SOCKET_USARTSend(SOCKET_USART, (u8 *)SocketSendBuf, socketsendlength);//发送数据
#endif
			sendnum += 1;
			if (sendnum >= 3) {
				sendnum = 0;
				receiveflag = 0;
				receivesendtime = 0;
			}
		}
	}

	if (PlatformSockettime == SocketTime_ENABLE) {
#ifdef SOCKETRTCHECKENABLE
		rtctime = RTC_GetCounter();										//获取当前时间值
		if ((rtctime % 86400) == 0) {										//判断是否到达凌晨0点
			SOCKET_RTC_CHECK = PACKETTYPE_RTCCHECK;							//开启RTC运行对时置0x81
#ifdef SOCKETRTCCHECKLED1
			GPIO_SetBits(OUTPUT_TYPE[0], OUTPUT_PIN[0]);
#endif
		}
#endif
	}
}

/***************************************************************
 @Function			void USART_SendByte(USART_TypeDef* USARTx, uint8_t data)
 @Description			USART 
 @Input				data : 8bit
 @Return				void
***************************************************************/
void SOCKET_USARTSendByte(USART_TypeDef* USARTx, u8 data)
{
	USART_SendData(USARTx, (u8) data);
	
	while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET);
}

/**********************************************************************************************************
 @Function			void SOCKET_Implement(u16 sendtime)
 @Description			Socket数据串口发送
 @Input				统计时长
 @Return				void
**********************************************************************************************************/
u32 SOCKET_USARTSend(USART_TypeDef* USARTx, u8 *data_arr, u32 length)
{
	u32 i = 0;
	
	if (USARTx == USART1) {
		RS485_Mode_TX();
		Delay(0x1fff);
		while (length > 0) {
			SOCKET_USARTSendByte(USARTx, *data_arr++);
			i++;
			length--;
		}
		Delay(0x1fff);
		RS485_Mode_RX();
	}
	else {
		while (length > 0) {
			SOCKET_USARTSendByte(USARTx, *data_arr++);
			i++;
			length--;
		}
	}
	
	return i;
}

/**********************************************************************************************************
 @Function			void SOCKET_PortSerialInit(USART_TypeDef* USARTx, u32 BaudRate)
 @Description			初始化Socket串口
 @Input				串口号
					波特率
 @Return				void
**********************************************************************************************************/
void SOCKET_PortSerialInit(USART_TypeDef* USARTx, u32 BaudRate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
#ifdef SOCKET_SERIALPORT_USART1
	/* Enable  USART1 clocks */
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
#endif
#ifdef SOCKET_SERIALPORT_USART2
  	/* Enable  USART2 clocks */
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
#endif
#ifdef SOCKET_SERIALPORT_USART3  
  	/* Enable  USART3 clocks */
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
#endif
	
	USART_DeInit(USARTx);
#ifdef SOCKET_SERIALPORT_USART1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);  
#endif
#ifdef SOCKET_SERIALPORT_USART2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
#endif
#ifdef SOCKET_SERIALPORT_USART3
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);

  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif
	
	USART_InitStructure.USART_BaudRate = BaudRate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	
#ifdef SOCKET_SERIALPORT_USART1
	USART_Init(USART1, &USART_InitStructure);
	USART_Cmd(USART1, ENABLE);
#endif
#ifdef SOCKET_SERIALPORT_USART2
	USART_Init(USART2, &USART_InitStructure);
	USART_Cmd(USART2, ENABLE);
#endif
#ifdef SOCKET_SERIALPORT_USART3
	USART_Init(USART3, &USART_InitStructure);
	USART_Cmd(USART3, ENABLE);
#endif

	USART_ITConfig(USARTx, USART_IT_RXNE, ENABLE);							//开启接收中断
	USART_ITConfig(USARTx, USART_IT_TXE, DISABLE);							//关闭发送中断
}

/********************************************** END OF FLEE **********************************************/
