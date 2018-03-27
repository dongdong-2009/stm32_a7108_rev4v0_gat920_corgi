#include "stm32f10x_lib.h"
#include "platform_config.h"
#include "math.h"
#include "NRF24L01.h"
#include "stdarg.h"
#include "string.h"
#include "stm32_config.h"

#define MAX823WDIGPIO	GPIOC
#define MAX823WDIPIN	GPIO_Pin_2

/*******************************************************************************
* Function Name  : Delay
* Description    : Inserts a delay time.
* Input          : nCount: specifies the delay time length.
* Output         : None
* Return         : None
*******************************************************************************/
void delay_5us(u8 count)
{
	u16 temp16_this;
	temp16_this = TIM4->CNT;
	while (((TIM4->CNT + 20000 - temp16_this) % 20000) < count);
}
void Delay(vu32 nCount)
{
	if(MVB_SYSCLOCK <= 8000000){
		nCount = nCount/8;
	}
	for(; nCount != 0; nCount--);
}
void delay_1ms(u32 nCount)
{
	while (nCount) {
		delay_5us(200);
		nCount--;
#ifdef HARDWAREIWDG
		MAX823_IWDGReloadCounter();										//硬件看门狗喂狗
#endif
#ifdef SOFTWAREIWDG
		IWDG_ReloadCounter();											//软件看门狗喂狗
#endif
	}
}

/**********************************************************************************************************
 @Function			void Max823_IWDGInitEnable(void)
 @Description			配置MAX823 WDI GPIO初始化, 拉高WDI使其使能
 @Input				void
 @Return				void
**********************************************************************************************************/
void MAX823_IWDGInitEnable(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//Configure PC2 as GPIO_Mode_Out_PP : MAX823_WDI
	GPIO_InitStructure.GPIO_Pin = MAX823WDIPIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MAX823WDIGPIO, &GPIO_InitStructure);
	
	//ENABLE MAX823 UP push-pull : MAX823_WDI
	GPIO_SetBits(MAX823WDIGPIO, MAX823WDIPIN);
}

/**********************************************************************************************************
 @Function			void MAX823_IWDGDisable(void)
 @Description			关闭MAX823 WDI GPIO, 开漏WDI使其失能
 @Input				void
 @Return				void
**********************************************************************************************************/
void MAX823_IWDGDisable(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//Configure PC2 as GPIO_Mode_Out_PP : MAX823_WDI
	GPIO_InitStructure.GPIO_Pin = MAX823WDIPIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(MAX823WDIGPIO, &GPIO_InitStructure);
	
	//ENABLE MAX823 UP push-pull : MAX823_WDI
	GPIO_SetBits(MAX823WDIGPIO, MAX823WDIPIN);
}

/**********************************************************************************************************
 @Function			void MAX823_IWDGReloadCounter(void)
 @Description			MAX823 硬件看门狗喂狗
 @Input				void
 @Return				void
**********************************************************************************************************/
void MAX823_IWDGReloadCounter(void)
{
	MAX823WDIGPIO->ODR ^= MAX823WDIPIN;
}

/*******************************************************************************
* Function Name  : EXTI_Configuration
* Description    : Configures EXTI Line9 and Line17(RTC Alarm).
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI_Configuration(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource1);			//PC1 产生EXT1中断
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;				//EXTI中断
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;			//下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;						//使能
	EXTI_Init(&EXTI_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);			//A7108 IRQ  PA0
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;				//EXTI中断
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;			//下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;						//使能
	EXTI_Init(&EXTI_InitStructure);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource4);			//PC4作为软中断
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;				//EXTI中断
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;			//下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;						//使能
	EXTI_Init(&EXTI_InitStructure);
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the used GPIO pins.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
	/*
	GPIO_Mode_AIN 			: analog input
	GPIO_Mode_IN_FLOATING 	: input floating 
	GPIO_Mode_IPD 			: input pull-down 
	GPIO_Mode_IPU 			: input pull-up 
	GPIO_Mode_Out_OD 		: output 开漏输出
	GPIO_Mode_Out_PP 		: output push-pull 推挽输出
	GPIO_Mode_AF_OD 		: alternate function 
	GPIO_Mode_AF_PP 		: alternate function push-pull
	*/

	GPIO_InitTypeDef GPIO_InitStructure;

	// Configure PA as output push-pull : 8
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8| GPIO_Pin_11| GPIO_Pin_12 | GPIO_Pin_15;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Configure PB as output push-pull :9
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8| GPIO_Pin_9;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// PC1/PC4 仅仅用来参生EXT1/EXTI4中断
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// Configure PC13 as output push-pull
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	//configure (PA.0) as input pull-up :A7108 RF中断
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//configure (PB.1 PB.14) as input floating for a7108 CKO
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// 配置 A7108片选  PB12和 clock PB13 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1| GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_15;			  					 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : TIM_Configuration
* Description    : Configures the used Timers.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM_Configuration(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	/* TIM4 configuration */
	TIM_TimeBaseStructure.TIM_Period = 20000; 										//100ms
	TIM_TimeBaseStructure.TIM_Prescaler = (MVB_SYSCLOCK / 200000)-1;						//200kHz
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	
	/* TIM3 configuration */
	TIM_TimeBaseStructure.TIM_Period = 200; 										//1ms
	TIM_TimeBaseStructure.TIM_Prescaler = (MVB_SYSCLOCK / 200000)-1;						//200kHz
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	/* TIM2 configuration */
	TIM_TimeBaseStructure.TIM_Period = 2000; 										//10ms
	TIM_TimeBaseStructure.TIM_Prescaler = (MVB_SYSCLOCK / 200000)-1;						//200kHz
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	/* TIM1 configuration */
	TIM_TimeBaseStructure.TIM_Period = 2000; 										//10ms
	TIM_TimeBaseStructure.TIM_Prescaler = (MVB_SYSCLOCK / 200000)-1;						//200kHz
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	/* TIM1 enable counter */
	TIM_Cmd(TIM1, DISABLE);
	/* TIM2 enable counter */
	TIM_Cmd(TIM2, DISABLE);
	/* TIM3 enable counter */
	TIM_Cmd(TIM3, ENABLE);
	/* TIM4 enable counter */
	TIM_Cmd(TIM4, ENABLE);
	
	/* Clear TIM1 update pending flag */
	TIM_ClearFlag(TIM1, TIM_FLAG_Update);
	/* Clear TIM2 update pending flag */
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	/* Clear TIM3 update pending flag */
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);
	/* Clear TIM4 update pending flag */
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);
	
	/* Enable TIM1 Update interrupt */
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);
	/* Enable TIM2 Update interrupt */
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	/* Enable TIM3 Update interrupt */
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	/* Enable TIM4 Update interrupt */
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
}
