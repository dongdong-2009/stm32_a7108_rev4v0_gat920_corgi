/******************************************************************************/
/* SERIAL.C: Low Level Serial Routines                                        */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2007 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#include <stm32f10x_lib.h>              /* STM32F10x Library Definitions      */

NVIC_InitTypeDef NVIC_InitStructure;
void SetNVIC(void);
void SetNVIC(void)
{
	#ifdef  VECT_TAB_RAM  
	/* Set the Vector Table base location at 0x20000000 */ 
	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
	#else  /* VECT_TAB_FLASH  */
	/* Set the Vector Table base location at 0x08000000 */ 
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
	#endif

	/* Configure one bit for preemption priority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQChannel;					//TIM4
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;				//抢占优先级 2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;					//子优先级为2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQChannel;					//TIM3
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;				//抢占优先级 2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;					//子优先级为1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQChannel;					//TIM2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;				//抢占优先级 2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					//子优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQChannel;				//TIM1
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;				//抢占优先级 2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;					//子优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQChannel;					//中断响应 PA0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;				//抢占优先级 2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;					//子优先级为1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;						//使能
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQChannel;					//中断响应 PC1
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;				//抢占优先级 1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;					//子优先级为1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;						//使能
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQChannel;					//中断响应 PC4
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;				//抢占优先级 3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;					//子优先级为1
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;						//使能
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable USART1_IRQChannel interrupt IRQ channel */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable USART2_IRQChannel interrupt IRQ channel */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable USART3_IRQChannel interrupt IRQ channel */
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
