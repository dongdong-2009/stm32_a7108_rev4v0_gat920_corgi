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
#include "platform_config.h"
void SetupClock (void)	;
ErrorStatus HSEStartUpStatus;


// SYSCLK 8M
// HCLK 8M
// PCLK1 4M
// PCLK2 8M
// PLL = SYSCLK

void SetupClock (void)
{
	/* RCC system reset(for debug purpose) */
	RCC_DeInit();

	/* Enable HSE */
	RCC_HSEConfig(RCC_HSE_ON);

	/* Wait till HSE is ready */
	HSEStartUpStatus = RCC_WaitForHSEStartUp();

	if (HSEStartUpStatus == SUCCESS)
	{
		/// HCLK = SYSCLK 
		RCC_HCLKConfig(RCC_SYSCLK_Div1); 

		// PCLK2 = HCLK 
		RCC_PCLK2Config(RCC_HCLK_Div1); 

		// PCLK1 = HCLK/2 
		RCC_PCLK1Config(RCC_HCLK_Div2);

		// Flash 2 wait state 
		FLASH_SetLatency(FLASH_Latency_2);
		
		// Enable Prefetch Buffer 
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

		// PLLCLK = 8MHz * 9 = 72 MHz 
		//RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
		if(MVB_SYSCLOCK > 8000000) {
			RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

			// Enable PLL 
			RCC_PLLCmd(ENABLE);

			// Wait till PLL is ready 
			while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
			{
			}

			// Select PLL as system clock source 
			RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
				
			// Wait till PLL is used as system clock source 
			while(RCC_GetSYSCLKSource() != 0x08)
			{
			}
		} else {
			// Select PLL as system clock source 
			RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);
		}
	}

	/* Enable GPIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	/* Enable TIM clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
}

/*******************************************************************************
* Function Name  : SYSCLKConfig_STOP
* Description    : Configures system clock after wake-up from STOP: enable HSE, PLL
*                  and select PLL as system clock source.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SYSCLKConfig_STOP(void)
{
	/* Enable HSE */
	RCC_HSEConfig(RCC_HSE_ON);

	/* Wait till HSE is ready */
	HSEStartUpStatus = RCC_WaitForHSEStartUp();

	if(HSEStartUpStatus == SUCCESS)
	{
		if (MVB_SYSCLOCK > 8000000) {
			/* Enable PLL */ 
			RCC_PLLCmd(ENABLE);

			/* Wait till PLL is ready */
			while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
			{
			}

			/* Select PLL as system clock source */
			RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

			/* Wait till PLL is used as system clock source */
			while(RCC_GetSYSCLKSource() != 0x08)
			{
			}
		} else {
			/* Select HSE as system clock source */
			RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);
		}
	}
}
