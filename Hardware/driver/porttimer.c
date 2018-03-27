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
 * File: $Id: porttimer.c,v 1.3 2007/06/12 06:42:01 wolti Exp $
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/
/* Timer ticks are counted in multiples of 50us. Therefore 20000 ticks are
 * one second.
 */
#define MB_TIMER_TICKS          ( 20000L )


/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit( USHORT usTim1Timeout50us )
{
  /* SysTick event each 50us*usTim1Timeout50us  with input clock equal to 9MHz (HCLK/8)         */
    //SysTick_SetReload(3600*usTim1Timeout50us);   // 1us = 72
  	//SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);   //HCLK=72M,SYSTIK = 72M  

   //SysTick_SetReload(450*usTim1Timeout50us);	//	1us = 9 ,ok
   //SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); 	//  HCLK=72M,SYSTIK = 9M  

  	//SysTick_SetReload(400*usTim1Timeout50us);	//	1us = 8,ok
  	//SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK); 	//  HCLK=8M,SYSTIK = 8M  

  	SysTick_SetReload(50*usTim1Timeout50us);	//	1us = 1, ok
  	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8); 	//  HCLK=8M,SYSTIK = 1M  
  
   SysTick_ITConfig(ENABLE);             /* Enable SysTick interrupt           */

  return 1;
}

void
vMBPortTimersEnable( void )
{
	SysTick_CounterCmd(SysTick_Counter_Clear);
	SysTick_CounterCmd(SysTick_Counter_Enable);

}

void
vMBPortTimersDisable( void )
{
	SysTick_CounterCmd(SysTick_Counter_Disable);
}


void prvvTIMERExpiredISR( void )
{
	( void )pxMBPortCBTimerExpired();
}
