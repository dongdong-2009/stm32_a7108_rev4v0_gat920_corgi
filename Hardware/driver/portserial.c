/*
 * FreeModbus Libary: MSP430 Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial.c,v 1.3 2006/11/19 03:57:49 wolti Exp $
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"
#include "platform_config.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"


/* ----------------------- Static variables ---------------------------------*/
UCHAR           ucGIEWasEnabled = FALSE;
UCHAR           ucCriticalNesting = 0x00;
//USART1:485
//USART2:232
//USART3:TTL 232

/* ----------------------- Start implementation -----------------------------*/
void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
	USART_TypeDef* USART_now = USART2;
	
	if (PlatformModbusUsart == Modbus_USART1) {								//根据SN选择Modbus连接串口1
		USART_now = USART1;
	}
	else {															//根据SN选择Modbus连接串口2
		USART_now = USART2;
	}
	
	ENTER_CRITICAL_SECTION(  );
	if( xRxEnable )
	{
		while(USART_GetFlagStatus(USART_now, USART_FLAG_TC) == 0);  //等待发送完最后一个字节成再开启读
#ifdef MB_SERIALPORT_USART1
		if (PlatformModbusUsart == Modbus_USART1) {							//根据SN选择Modbus连接串口1
			GPIO_ResetBits(GPIO_RS485_CTL,  GPIO_RS485_CTL_PIN); 		 //For 485DE : 0
			USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
		}
#endif
#ifdef MB_SERIALPORT_USART2
		if (PlatformModbusUsart == Modbus_USART2) {							//根据SN选择Modbus连接串口2
			USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
		}
#endif
#ifdef MB_SERIALPORT_USART3
		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
#endif
	}
	else
	{
#ifdef MB_SERIALPORT_USART1
		if (PlatformModbusUsart == Modbus_USART1) {							//根据SN选择Modbus连接串口1
			USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
		}
#endif
#ifdef MB_SERIALPORT_USART2
		if (PlatformModbusUsart == Modbus_USART2) {							//根据SN选择Modbus连接串口2
			USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
		}
#endif
#ifdef MB_SERIALPORT_USART3
		USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
#endif
	}
	if( xTxEnable )
	{
#ifdef MB_SERIALPORT_USART1
		if (PlatformModbusUsart == Modbus_USART1) {							//根据SN选择Modbus连接串口1
			USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
			GPIO_SetBits(GPIO_RS485_CTL,  GPIO_RS485_CTL_PIN);
		}
#endif
#ifdef MB_SERIALPORT_USART2
		if (PlatformModbusUsart == Modbus_USART2) {							//根据SN选择Modbus连接串口2
			USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
		}
#endif
#ifdef MB_SERIALPORT_USART3
		USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
#endif
	}
	else
	{
#ifdef MB_SERIALPORT_USART1
		if (PlatformModbusUsart == Modbus_USART1) {							//根据SN选择Modbus连接串口1
			USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
		}
#endif
#ifdef MB_SERIALPORT_USART2
		if (PlatformModbusUsart == Modbus_USART2) {							//根据SN选择Modbus连接串口2
			USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
		}
#endif
#ifdef MB_SERIALPORT_USART3
		USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
#endif
	}
	EXIT_CRITICAL_SECTION(  );
}

BOOL
xMBPortSerialInit( UCHAR ucPort, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
	BOOL    bInitialized = TRUE;

	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

#ifdef MB_SERIALPORT_USART1
	if (PlatformModbusUsart == Modbus_USART1) {								//根据SN选择Modbus连接串口1
		/* Enable  USART1*/
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	}
#endif
#ifdef MB_SERIALPORT_USART2
	if (PlatformModbusUsart == Modbus_USART2) {								//根据SN选择Modbus连接串口2
		/* Enable  USART2 clocks */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	}
#endif
#ifdef MB_SERIALPORT_USART3  
  	/*Enable USART3 clocks */
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
#endif

#ifdef MB_SERIALPORT_USART1
	if (PlatformModbusUsart == Modbus_USART1) {								//根据SN选择Modbus连接串口1
		 //USART1
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_InitStructure);

		// Configure RS485 contrel pin: CTL 
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	}
#endif
#ifdef MB_SERIALPORT_USART2
	if (PlatformModbusUsart == Modbus_USART2) {								//根据SN选择Modbus连接串口2
		//USART2
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_InitStructure);
	}
#endif
#ifdef MB_SERIALPORT_USART3
	// Configure USART3_Tx as alternate function push-pull 
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);

 	 //Configure USART3_Rx as input floating 
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	  //不可以选 GPIO_Mode_IN_FLOATING，TTL直接输出容易引起误中断
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
#endif

 	USART_InitStructure.USART_BaudRate = ulBaudRate;
	
	switch ( eParity )
	{
	case MB_PAR_NONE:
		USART_InitStructure.USART_Parity = USART_Parity_No;
		break;
	case MB_PAR_ODD:
		USART_InitStructure.USART_Parity = USART_Parity_Odd;
		break;
	case MB_PAR_EVEN:
		USART_InitStructure.USART_Parity = USART_Parity_Even;
		break;
	}
	switch ( ucDataBits )
	{
	case 8:
		if(eParity==MB_PAR_NONE)
			USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		else
			USART_InitStructure.USART_WordLength = USART_WordLength_9b;
		break;
	case 7:
		break;
	default:
		bInitialized = FALSE;
	}
	if( bInitialized )
	{
		ENTER_CRITICAL_SECTION(  );
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

#ifdef MB_SERIALPORT_USART1
		if (PlatformModbusUsart == Modbus_USART1) {							//根据SN选择Modbus连接串口1
			USART_Init(USART1, &USART_InitStructure);
			USART_Cmd(USART1, ENABLE);
		}
#endif
#ifdef MB_SERIALPORT_USART2
		if (PlatformModbusUsart == Modbus_USART2) {							//根据SN选择Modbus连接串口2
			USART_Init(USART2, &USART_InitStructure);
			USART_Cmd(USART2, ENABLE);
		}
#endif
#ifdef MB_SERIALPORT_USART3
		USART_Init(USART3, &USART_InitStructure);
		USART_Cmd(USART3, ENABLE);
#endif
		
		EXIT_CRITICAL_SECTION(  );
	}
	return bInitialized;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
	USART_TypeDef* USART_now = USART2;
	
	if (PlatformModbusUsart == Modbus_USART1) {								//根据SN选择Modbus连接串口1
		USART_now = USART1;
	}
	else {															//根据SN选择Modbus连接串口2
		USART_now = USART2;
	}
	
	USART_SendData(USART_now, ucByte);
	while(USART_GetFlagStatus(USART_now, USART_FLAG_TXE) == 0);  //等待发送寄存器空
	return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
	USART_TypeDef* USART_now = USART2;
	
	if (PlatformModbusUsart == Modbus_USART1) {								//根据SN选择Modbus连接串口1
		USART_now = USART1;
	}
	else {															//根据SN选择Modbus连接串口2
		USART_now = USART2;
	}
	
	*pucByte = (u8)USART_ReceiveData(USART_now);								//获取接收BUFFER的数据
	return TRUE;
}

void UARTx_IRQ(USART_TypeDef* USARTx)
{
	if(USART_GetITStatus(USARTx, USART_IT_TXE))
	{
	    	pxMBFrameCBTransmitterEmpty(  );
		USART_ClearITPendingBit(USARTx, USART_IT_TXE);
	}
	else if(USART_GetITStatus(USARTx, USART_IT_RXNE))
	{
		pxMBFrameCBByteReceived(  );
		USART_ClearITPendingBit(USARTx, USART_IT_RXNE);
	}
}
void
EnterCriticalSection( void )
{
  __disable_irq();
}

void
ExitCriticalSection( void )
{
  __enable_irq();
}
