/**
  *********************************************************************************************************
  * @file    iooutputfunc.c
  * @author  MoveBroad -- KangYJ
  * @version V1.0
  * @date    
  * @brief   
  *********************************************************************************************************
  * @attention
  *			功能 : 
  *					1. IO输出校验数据
  *					2. IO输出定时器补发
  *********************************************************************************************************
  */

#include "iooutputfunc.h"


IOOutputTypedef				IOOutputStruct[IOOutputMAX];					//IOOutput控制结构体

extern GPIO_TypeDef* OUTPUT_TYPE[16];
extern u16 OUTPUT_PIN[16];


/**********************************************************************************************************
 @Function			void IOOutput_Mode0_Check(u16 dici_index, u16 dici_carnum, u8 dici_in)
 @Description			IO输出校验数据 (输出方式0 : 跟随车辆输出)
 @Input				dici_index  	: 地磁ID编号
					dici_carnum 	: 该地磁车辆数状态值
					dici_in	  	: 1.车辆进入
							       0.车辆离开
 @Return				void
**********************************************************************************************************/
void IOOutput_Mode0_Check(u16 dici_index, u16 dici_carnum, u8 dici_in)
{
	u16 carnum = 0;
	
	if (IOOutputStruct[dici_index].CarNumState == 0) {						//初始化ID车辆数
		IOOutputStruct[dici_index].CarNumState = dici_carnum;					//记录车辆数状态值
		if (dici_in == 1) {
			GPIO_SetBits(OUTPUT_TYPE[dici_index],  OUTPUT_PIN[dici_index]);
			IOOutputStruct[dici_index].IOLevel = 1;							//电平置1
			IOOutputStruct[dici_index].IOHighUseTime = 0;					//高电平时间清0
		}
		else {
			GPIO_ResetBits(OUTPUT_TYPE[dici_index],  OUTPUT_PIN[dici_index]);
			IOOutputStruct[dici_index].IOLevel = 0;							//电平置0
			IOOutputStruct[dici_index].IOHighUseTime = 0;					//高电平时间清0
		}
	}
	else {															//已初始化完成
		if (dici_carnum > IOOutputStruct[dici_index].CarNumState) {				//计算车辆数值
			if ((dici_carnum - IOOutputStruct[dici_index].CarNumState) < IOOutputNUM) {
				carnum = dici_carnum - IOOutputStruct[dici_index].CarNumState;
			}
		}
		else if (dici_carnum < IOOutputStruct[dici_index].CarNumState) {
			if ((65535 - IOOutputStruct[dici_index].CarNumState + dici_carnum) < IOOutputNUM) {
				carnum = 65535 - IOOutputStruct[dici_index].CarNumState + dici_carnum;
			}
		}
		IOOutputStruct[dici_index].CarNumState = dici_carnum;					//记录车辆数状态值
		
		/* 判断该地磁是否正在补发数据 */
		if (IOOutputStruct[dici_index].SupplyingFlag == 0) {					//补发结束
			if (carnum == 0) {											//无需补充车辆数
				if (dici_in == 1) {
					GPIO_SetBits(OUTPUT_TYPE[dici_index],  OUTPUT_PIN[dici_index]);
					IOOutputStruct[dici_index].IOLevel = 1;					//电平置1
					IOOutputStruct[dici_index].IOHighUseTime = 0;			//高电平时间清0
				}
				else {
					GPIO_ResetBits(OUTPUT_TYPE[dici_index],  OUTPUT_PIN[dici_index]);
					IOOutputStruct[dici_index].IOLevel = 0;					//电平置0
					IOOutputStruct[dici_index].IOHighUseTime = 0;			//高电平时间清0
				}
			}
			else {													//需要补充车辆数
				IOOutputStruct[dici_index].SupplyCarNum = carnum;				//获取需要补发的车辆数
				IOOutputStruct[dici_index].SupplyedIOLevel = dici_in;			//获取补充车辆数结束后输出电平
				IOOutputStruct[dici_index].SupplyingFlag = 1;				//标志车辆数正在补发中
			}
		}
		else {														//正在补发
			IOOutputStruct[dici_index].SupplyingCarNum += carnum;				//获取正在补发中收到的车辆数
			if (IOOutputStruct[dici_index].SupplyingCarNum >= IOOutputNUM) {		//正在补发中收到的车辆数超过需补发最大车辆数
				IOOutputStruct[dici_index].SupplyingCarNum = IOOutputNUM;
			}
			IOOutputStruct[dici_index].SupplyedIOLevel = dici_in;				//获取补充车辆数结束后输出电平
		}
	}
}

/**********************************************************************************************************
 @Function			void IOOutput_Mode0_Supplying(void)
 @Description			IO输出定时器补发 1ms执行一次 (输出方式0 : 跟随车辆输出)
 @Input				void
 @Return				void
**********************************************************************************************************/
void IOOutput_Mode0_Supplying(void)
{
	u8 i = 0;
	
	for (i = 0; i < IOOutputMAX; i++) {
		if (IOOutputStruct[i].OutputID != 0) {								//ID不为0处理
			
			/* 高电平超时拉低 */
			if (IOOutputStruct[i].IOLevel == 1) {
				IOOutputStruct[i].IOHighUseTime += 1;
				if (IOOutputStruct[i].IOHighUseTime >= (param_recv.output_high_max * 1000)) {
					GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
					IOOutputStruct[i].IOLevel = 0;						//电平置0
					IOOutputStruct[i].IOHighUseTime = 0;					//高电平时间清0
				}
			}
			
			/* 补发车辆数 */
			if (IOOutputStruct[i].SupplyingFlag != 0) {
				
				if (IOOutputStruct[i].SupplyUseTime == 0) {
					GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
					IOOutputStruct[i].IOLevel = 0;						//电平置0
					IOOutputStruct[i].IOHighUseTime = 0;					//高电平时间清0
				}
				else if (IOOutputStruct[i].SupplyUseTime == (param_recv.output_low_min * 20)) {
					GPIO_SetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
					IOOutputStruct[i].IOLevel = 1;						//电平置1
					IOOutputStruct[i].IOHighUseTime = 0;					//高电平时间清0
				}
				
				IOOutputStruct[i].SupplyUseTime += 1;
				
				if (IOOutputStruct[i].SupplyUseTime >= ((param_recv.output_low_min * 20) + (param_recv.vi_output_high_time * 20) + 1)) {
					IOOutputStruct[i].SupplyUseTime = 0;
					IOOutputStruct[i].SupplyCarNum -= 1;					//补发车辆数减一
					if (IOOutputStruct[i].SupplyCarNum == 0) {				//补发车辆数完成
						IOOutputStruct[i].SupplyCarNum = 0;				//清空补发车辆数
						IOOutputStruct[i].SupplyingFlag = 0;				//补发结束置0
						if (IOOutputStruct[i].SupplyedIOLevel == 0) {		//补发结束电平值
							GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
							IOOutputStruct[i].IOLevel = 0;				//电平置0
							IOOutputStruct[i].IOHighUseTime = 0;			//高电平时间清0
						}
						else {
							GPIO_SetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
							IOOutputStruct[i].IOLevel = 1;				//电平置1
							IOOutputStruct[i].IOHighUseTime = 0;			//高电平时间清0
						}
						
						if (IOOutputStruct[i].SupplyingCarNum != 0) {		//补发中收到车辆数
							IOOutputStruct[i].SupplyCarNum += IOOutputStruct[i].SupplyingCarNum;					//获取需要补发的车辆数
							IOOutputStruct[i].SupplyingCarNum = 0;
							IOOutputStruct[i].SupplyingFlag = 1;			//标志车辆数正在补发中
						}
					}
					else if (IOOutputStruct[i].SupplyCarNum > IOOutputNUM) {	//超过需补发车辆数
						IOOutputStruct[i].SupplyCarNum = IOOutputNUM;		//补发最大补发数
					}
				}
			}
		}
	}
}


/**********************************************************************************************************
 @Function			void IOOutput_Mode1_Check(u16 dici_index, u16 dici_carnum, u8 dici_in)
 @Description			IO输出校验数据 (输出方式1 : 车辆进入输出固定时长(记数))
 @Input				dici_index  	: 地磁ID编号
					dici_carnum 	: 该地磁车辆数状态值
					dici_in	  	: 1.车辆进入
							       0.车辆离开
 @Return				void
**********************************************************************************************************/
void IOOutput_Mode1_Check(u16 dici_index, u16 dici_carnum, u8 dici_in)
{
	u16 carnum = 0;
	
	if (IOOutputStruct[dici_index].CarNumState == 0) {						//初始化ID车辆数
		IOOutputStruct[dici_index].CarNumState = dici_carnum;					//记录车辆数状态值
		if (dici_in == 1) {
			GPIO_SetBits(OUTPUT_TYPE[dici_index],  OUTPUT_PIN[dici_index]);
			IOOutputStruct[dici_index].IOLevel = 1;							//电平置1
			IOOutputStruct[dici_index].IOHighUseTime = 0;					//高电平时间清0
		}
		else {
			GPIO_ResetBits(OUTPUT_TYPE[dici_index],  OUTPUT_PIN[dici_index]);
			IOOutputStruct[dici_index].IOLevel = 0;							//电平置0
			IOOutputStruct[dici_index].IOHighUseTime = 0;					//高电平时间清0
		}
	}
	else {															//已初始化完成
		if (dici_carnum > IOOutputStruct[dici_index].CarNumState) {				//计算车辆数值
			if ((dici_carnum - IOOutputStruct[dici_index].CarNumState) < IOOutputNUM) {
				carnum = dici_carnum - IOOutputStruct[dici_index].CarNumState;
			}
		}
		else if (dici_carnum < IOOutputStruct[dici_index].CarNumState) {
			if ((65535 - IOOutputStruct[dici_index].CarNumState + dici_carnum) < IOOutputNUM) {
				carnum = 65535 - IOOutputStruct[dici_index].CarNumState + dici_carnum;
			}
		}
		IOOutputStruct[dici_index].CarNumState = dici_carnum;					//记录车辆数状态值
		
		/* 判断该地磁是否正在补发数据 */
		if (IOOutputStruct[dici_index].SupplyingFlag == 0) {					//补发结束
			if (carnum != 0) {											//需要补充车辆数
				IOOutputStruct[dici_index].SupplyCarNum = carnum;				//获取需要补发的车辆数
				IOOutputStruct[dici_index].SupplyedIOLevel = 0;				//补充车辆数结束后输出低电平
				IOOutputStruct[dici_index].SupplyingFlag = 1;				//标志车辆数正在补发中
			}
		}
		else {														//正在补发
			IOOutputStruct[dici_index].SupplyingCarNum += carnum;				//获取正在补发中收到的车辆数
			if (IOOutputStruct[dici_index].SupplyingCarNum >= IOOutputNUM) {		//正在补发中收到的车辆数超过需补发最大车辆数
				IOOutputStruct[dici_index].SupplyingCarNum = IOOutputNUM;
			}
			IOOutputStruct[dici_index].SupplyedIOLevel = 0;					//补充车辆数结束后输出低电平
		}
	}
}

/**********************************************************************************************************
 @Function			void IOOutput_Mode1_Supplying(void)
 @Description			IO输出定时器补发 1ms执行一次 (输出方式1 : 车辆进入输出固定时长(记数))
 @Input				void
 @Return				void
**********************************************************************************************************/
void IOOutput_Mode1_Supplying(void)
{
	u8 i = 0;
	
	for (i = 0; i < IOOutputMAX; i++) {
		if (IOOutputStruct[i].OutputID != 0) {								//ID不为0处理
			
			/* 补发车辆数 */
			if (IOOutputStruct[i].SupplyingFlag != 0) {
				
				if (IOOutputStruct[i].SupplyUseTime == 0) {
					GPIO_SetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
					IOOutputStruct[i].IOLevel = 1;						//电平置1
					IOOutputStruct[i].IOHighUseTime = 0;					//高电平时间清0
				}
				else if (IOOutputStruct[i].SupplyUseTime == (param_recv.vi_output_high_time * 20)) {
					GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
					IOOutputStruct[i].IOLevel = 0;						//电平置0
					IOOutputStruct[i].IOHighUseTime = 0;					//高电平时间清0
				}
				
				IOOutputStruct[i].SupplyUseTime += 1;
				
				if (IOOutputStruct[i].SupplyUseTime >= ((param_recv.vi_output_high_time * 20) + (param_recv.output_low_min * 20) + 1)) {
					IOOutputStruct[i].SupplyUseTime = 0;
					IOOutputStruct[i].SupplyCarNum -= 1;					//补发车辆数减一
					if (IOOutputStruct[i].SupplyCarNum == 0) {				//补发车辆数完成
						IOOutputStruct[i].SupplyCarNum = 0;				//清空补发车辆数
						IOOutputStruct[i].SupplyingFlag = 0;				//补发结束置0
						GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
						IOOutputStruct[i].IOLevel = 0;					//电平置0
						IOOutputStruct[i].IOHighUseTime = 0;				//高电平时间清0
						
						if (IOOutputStruct[i].SupplyingCarNum != 0) {		//补发中收到车辆数
							IOOutputStruct[i].SupplyCarNum += IOOutputStruct[i].SupplyingCarNum;					//获取需要补发的车辆数
							IOOutputStruct[i].SupplyingCarNum = 0;
							IOOutputStruct[i].SupplyingFlag = 1;			//标志车辆数正在补发中
						}
					}
					else if (IOOutputStruct[i].SupplyCarNum > IOOutputNUM) {	//超过需补发车辆数
						IOOutputStruct[i].SupplyCarNum = IOOutputNUM;		//补发最大补发数
					}
				}
			}
		}
	}
}


/**********************************************************************************************************
 @Function			void IOOutput_Mode2_Check(u16 dici_index, u16 dici_carnum, u8 dici_in)
 @Description			IO输出校验数据 (输出方式2 : 车辆离开输出固定时长(记数))
 @Input				dici_index  	: 地磁ID编号
					dici_carnum 	: 该地磁车辆数状态值
					dici_in	  	: 1.车辆进入
							       0.车辆离开
 @Return				void
**********************************************************************************************************/
void IOOutput_Mode2_Check(u16 dici_index, u16 dici_carnum, u8 dici_in)
{
	u16 carnum = 0;
	
	if (IOOutputStruct[dici_index].CarNumState == 0) {						//初始化ID车辆数
		IOOutputStruct[dici_index].CarNumState = dici_carnum;					//记录车辆数状态值
		if (dici_in == 1) {
			GPIO_SetBits(OUTPUT_TYPE[dici_index],  OUTPUT_PIN[dici_index]);
			IOOutputStruct[dici_index].IOLevel = 1;							//电平置1
			IOOutputStruct[dici_index].IOHighUseTime = 0;					//高电平时间清0
		}
		else {
			GPIO_ResetBits(OUTPUT_TYPE[dici_index],  OUTPUT_PIN[dici_index]);
			IOOutputStruct[dici_index].IOLevel = 0;							//电平置0
			IOOutputStruct[dici_index].IOHighUseTime = 0;					//高电平时间清0
		}
	}
	else {															//已初始化完成
		if (dici_carnum > IOOutputStruct[dici_index].CarNumState) {				//计算车辆数值
			if ((dici_carnum - IOOutputStruct[dici_index].CarNumState) < IOOutputNUM) {
				carnum = dici_carnum - IOOutputStruct[dici_index].CarNumState;
			}
		}
		else if (dici_carnum < IOOutputStruct[dici_index].CarNumState) {
			if ((65535 - IOOutputStruct[dici_index].CarNumState + dici_carnum) < IOOutputNUM) {
				carnum = 65535 - IOOutputStruct[dici_index].CarNumState + dici_carnum;
			}
		}
		IOOutputStruct[dici_index].CarNumState = dici_carnum;					//记录车辆数状态值
		
		/* 判断该地磁是否正在补发数据 */
		if (IOOutputStruct[dici_index].SupplyingFlag == 0) {					//补发结束
			if (carnum != 0) {											//需要补充车辆数
				IOOutputStruct[dici_index].SupplyCarNum = carnum;				//获取需要补发的车辆数
				IOOutputStruct[dici_index].SupplyedIOLevel = 0;				//补充车辆数结束后输出低电平
				IOOutputStruct[dici_index].SupplyingFlag = 1;				//标志车辆数正在补发中
			}
		}
		else {														//正在补发
			IOOutputStruct[dici_index].SupplyingCarNum += carnum;				//获取正在补发中收到的车辆数
			if (IOOutputStruct[dici_index].SupplyingCarNum >= IOOutputNUM) {		//正在补发中收到的车辆数超过需补发最大车辆数
				IOOutputStruct[dici_index].SupplyingCarNum = IOOutputNUM;
			}
			IOOutputStruct[dici_index].SupplyedIOLevel = 0;					//补充车辆数结束后输出低电平
		}
	}
}

/**********************************************************************************************************
 @Function			void IOOutput_Mode2_Supplying(void)
 @Description			IO输出定时器补发 1ms执行一次 (输出方式2 : 车辆离开输出固定时长(记数))
 @Input				void
 @Return				void
**********************************************************************************************************/
void IOOutput_Mode2_Supplying(void)
{
	u8 i = 0;
	
	for (i = 0; i < IOOutputMAX; i++) {
		if (IOOutputStruct[i].OutputID != 0) {								//ID不为0处理
			
			/* 补发车辆数 */
			if (IOOutputStruct[i].SupplyingFlag != 0) {
				
				if (IOOutputStruct[i].SupplyUseTime == 0) {
					GPIO_SetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
					IOOutputStruct[i].IOLevel = 1;						//电平置1
					IOOutputStruct[i].IOHighUseTime = 0;					//高电平时间清0
				}
				else if (IOOutputStruct[i].SupplyUseTime == (param_recv.vo_output_high_time * 20)) {
					GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
					IOOutputStruct[i].IOLevel = 0;						//电平置0
					IOOutputStruct[i].IOHighUseTime = 0;					//高电平时间清0
				}
				
				IOOutputStruct[i].SupplyUseTime += 1;
				
				if (IOOutputStruct[i].SupplyUseTime >= ((param_recv.vo_output_high_time * 20) + (param_recv.output_low_min * 20) + 1)) {
					IOOutputStruct[i].SupplyUseTime = 0;
					IOOutputStruct[i].SupplyCarNum -= 1;					//补发车辆数减一
					if (IOOutputStruct[i].SupplyCarNum == 0) {				//补发车辆数完成
						IOOutputStruct[i].SupplyCarNum = 0;				//清空补发车辆数
						IOOutputStruct[i].SupplyingFlag = 0;				//补发结束置0
						GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
						IOOutputStruct[i].IOLevel = 0;					//电平置0
						IOOutputStruct[i].IOHighUseTime = 0;				//高电平时间清0
						
						if (IOOutputStruct[i].SupplyingCarNum != 0) {		//补发中收到车辆数
							IOOutputStruct[i].SupplyCarNum += IOOutputStruct[i].SupplyingCarNum;					//获取需要补发的车辆数
							IOOutputStruct[i].SupplyingCarNum = 0;
							IOOutputStruct[i].SupplyingFlag = 1;			//标志车辆数正在补发中
						}
					}
					else if (IOOutputStruct[i].SupplyCarNum > IOOutputNUM) {	//超过需补发车辆数
						IOOutputStruct[i].SupplyCarNum = IOOutputNUM;		//补发最大补发数
					}
				}
			}
		}
	}
}


/**********************************************************************************************************
 @Function			void IOOutput_Mode3_Check(u16 dici_index, u16 dici_carnum, u8 dici_in)
 @Description			IO输出校验数据 (输出方式3 : 车辆进入,离开时都输出固定时长(记数))
 @Input				dici_index  	: 地磁ID编号
					dici_carnum 	: 该地磁车辆数状态值
					dici_in	  	: 1.车辆进入
							       0.车辆离开
 @Return				void
**********************************************************************************************************/
void IOOutput_Mode3_Check(u16 dici_index, u16 dici_carnum, u8 dici_in)
{
	u16 carnum = 0;
	
	if (IOOutputStruct[dici_index].CarNumState == 0) {						//初始化ID车辆数
		IOOutputStruct[dici_index].CarNumState = dici_carnum;					//记录车辆数状态值
		if (dici_in == 1) {
			GPIO_SetBits(OUTPUT_TYPE[dici_index],  OUTPUT_PIN[dici_index]);
			IOOutputStruct[dici_index].IOLevel = 1;							//电平置1
			IOOutputStruct[dici_index].IOHighUseTime = 0;					//高电平时间清0
		}
		else {
			GPIO_ResetBits(OUTPUT_TYPE[dici_index],  OUTPUT_PIN[dici_index]);
			IOOutputStruct[dici_index].IOLevel = 0;							//电平置0
			IOOutputStruct[dici_index].IOHighUseTime = 0;					//高电平时间清0
		}
	}
	else {															//已初始化完成
		if (dici_carnum > IOOutputStruct[dici_index].CarNumState) {				//计算车辆数值
			if ((dici_carnum - IOOutputStruct[dici_index].CarNumState) < IOOutputNUM) {
				carnum = dici_carnum - IOOutputStruct[dici_index].CarNumState;
			}
		}
		else if (dici_carnum < IOOutputStruct[dici_index].CarNumState) {
			if ((65535 - IOOutputStruct[dici_index].CarNumState + dici_carnum) < IOOutputNUM) {
				carnum = 65535 - IOOutputStruct[dici_index].CarNumState + dici_carnum;
			}
		}
		IOOutputStruct[dici_index].CarNumState = dici_carnum;					//记录车辆数状态值
		
		/* 判断该地磁是否正在补发数据 */
		if (IOOutputStruct[dici_index].SupplyingFlag == 0) {					//补发结束
			if (carnum != 0) {											//需要补充车辆数
				if (dici_in == 1) {
					IOOutputStruct[dici_index].SupplyCarNum = (carnum * 2 - 1);	//获取需要补发的车辆数
					IOOutputStruct[dici_index].CarInOrOut = 1;				//车检入
					IOOutputStruct[dici_index].CarEvenOrOdd = 0;				//奇数包
				}
				else {
					IOOutputStruct[dici_index].SupplyCarNum = (carnum * 2);	//获取需要补发的车辆数
					IOOutputStruct[dici_index].CarInOrOut = 0;				//车检出
					IOOutputStruct[dici_index].CarEvenOrOdd = 1;				//偶数包
				}
				IOOutputStruct[dici_index].SupplyedIOLevel = 0;				//补充车辆数结束后输出低电平
				IOOutputStruct[dici_index].SupplyingFlag = 1;				//标志车辆数正在补发中
			}
			else {													//车检出carnum不加
				IOOutputStruct[dici_index].SupplyCarNum = 1;					//获取需要补发的车辆数
				IOOutputStruct[dici_index].CarInOrOut = 0;					//车检出
				IOOutputStruct[dici_index].CarEvenOrOdd = 0;					//奇数包
				IOOutputStruct[dici_index].SupplyedIOLevel = 0;				//补充车辆数结束后输出低电平
				IOOutputStruct[dici_index].SupplyingFlag = 1;				//标志车辆数正在补发中
			}
		}
		else {																	//正在补发
			if (carnum != 0) {														//需要补充车辆数
				if (dici_in == 1) {
					IOOutputStruct[dici_index].SupplyingCarNum += (carnum * 2 - 1);			//获取正在补发中收到的车辆数
					if (IOOutputStruct[dici_index].SupplyingCarNum >= (IOOutputNUM * 2)) {
						IOOutputStruct[dici_index].SupplyingCarNum -= (carnum * 2 - 1);
					}
					else {
						IOOutputStruct[dici_index].SupplyingCarInOrOut = 1;				//车检入
					}
				}
				else {
					IOOutputStruct[dici_index].SupplyingCarNum += (carnum * 2);				//获取正在补发中收到的车辆数
					if (IOOutputStruct[dici_index].SupplyingCarNum >= (IOOutputNUM * 2)) {
						IOOutputStruct[dici_index].SupplyingCarNum -= (carnum * 2);
					}
					else {
						IOOutputStruct[dici_index].SupplyingCarInOrOut = 0;				//车检出
					}
				}
			}
			else {																//车检出carnum不加
				IOOutputStruct[dici_index].SupplyingCarNum += 1;
				if (IOOutputStruct[dici_index].SupplyingCarNum >= (IOOutputNUM * 2)) {
					IOOutputStruct[dici_index].SupplyingCarNum -= 1;
				}
				else {
					IOOutputStruct[dici_index].SupplyingCarInOrOut = 0;					//车检出
				}
			}
			
			IOOutputStruct[dici_index].SupplyedIOLevel = 0;								//补充车辆数结束后输出低电平
		}
	}
}

/**********************************************************************************************************
 @Function			void IOOutput_Mode3_Supplying(void)
 @Description			IO输出定时器补发 1ms执行一次 (输出方式3 : 车辆进入,离开时都输出固定时长(记数))
 @Input				void
 @Return				void
**********************************************************************************************************/
void IOOutput_Mode3_Supplying(void)
{
	u8 i = 0;
	
	for (i = 0; i < IOOutputMAX; i++) {
		if (IOOutputStruct[i].OutputID != 0) {												//ID不为0处理
			
			/* 补发车辆数 */
			if (IOOutputStruct[i].SupplyingFlag != 0) {
				
				if (IOOutputStruct[i].CarInOrOut == 1) {									//最后一包为车检入
					if (IOOutputStruct[i].CarEvenOrOdd == 0) {								//奇数包
						if ((IOOutputStruct[i].SupplyCarNum % 2) == 1) {
							if (IOOutputStruct[i].SupplyUseTime == 0) {
								GPIO_SetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
								IOOutputStruct[i].IOLevel = 1;							//电平置1
								IOOutputStruct[i].IOHighUseTime = 0;						//高电平时间清0
							}
							else if (IOOutputStruct[i].SupplyUseTime == (param_recv.vi_output_high_time * 20)) {
								GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
								IOOutputStruct[i].IOLevel = 0;							//电平置0
								IOOutputStruct[i].IOHighUseTime = 0;						//高电平时间清0
							}
							
							IOOutputStruct[i].SupplyUseTime += 1;
							
							if (IOOutputStruct[i].SupplyUseTime >= ((param_recv.vi_output_high_time * 20) + (param_recv.output_low_min * 20) + 1)) {
								IOOutputStruct[i].SupplyUseTime = 0;
								IOOutputStruct[i].SupplyCarNum -= 1;						//补发车辆数减一
								if (IOOutputStruct[i].SupplyCarNum == 0) {					//补发车辆数完成
									IOOutputStruct[i].SupplyCarNum = 0;					//清空补发车辆数
									IOOutputStruct[i].SupplyingFlag = 0;					//补发结束置0
									GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
									IOOutputStruct[i].IOLevel = 0;						//电平置0
									IOOutputStruct[i].IOHighUseTime = 0;					//高电平时间清0
									
									if (IOOutputStruct[i].SupplyingCarNum != 0) {			//补发中收到车辆数
										IOOutputStruct[i].SupplyCarNum = IOOutputStruct[i].SupplyingCarNum;		//获取需要补发的车辆数
										IOOutputStruct[i].CarInOrOut = IOOutputStruct[i].SupplyingCarInOrOut;
										if (IOOutputStruct[i].SupplyingCarNum % 2 == 1) {
											IOOutputStruct[i].CarEvenOrOdd = 0;
										}
										else {
											IOOutputStruct[i].CarEvenOrOdd = 1;
										}
										IOOutputStruct[i].SupplyingCarNum = 0;
										IOOutputStruct[i].SupplyingFlag = 1;				//标志车辆数正在补发中
									}
								}
								else if (IOOutputStruct[i].SupplyCarNum > (IOOutputNUM * 2)) {	//超过需补发车辆数
									IOOutputStruct[i].SupplyCarNum = (IOOutputNUM * 2);		//补发最大补发数
								}
							}
						}
						else {
							if (IOOutputStruct[i].SupplyUseTime == 0) {
								GPIO_SetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
								IOOutputStruct[i].IOLevel = 1;							//电平置1
								IOOutputStruct[i].IOHighUseTime = 0;						//高电平时间清0
							}
							else if (IOOutputStruct[i].SupplyUseTime == (param_recv.vo_output_high_time * 20)) {
								GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
								IOOutputStruct[i].IOLevel = 0;							//电平置0
								IOOutputStruct[i].IOHighUseTime = 0;						//高电平时间清0
							}
							
							IOOutputStruct[i].SupplyUseTime += 1;
							
							if (IOOutputStruct[i].SupplyUseTime >= ((param_recv.vo_output_high_time * 20) + (param_recv.output_low_min * 20) + 1)) {
								IOOutputStruct[i].SupplyUseTime = 0;
								IOOutputStruct[i].SupplyCarNum -= 1;						//补发车辆数减一
								if (IOOutputStruct[i].SupplyCarNum == 0) {					//补发车辆数完成
									IOOutputStruct[i].SupplyCarNum = 0;					//清空补发车辆数
									IOOutputStruct[i].SupplyingFlag = 0;					//补发结束置0
									GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
									IOOutputStruct[i].IOLevel = 0;						//电平置0
									IOOutputStruct[i].IOHighUseTime = 0;					//高电平时间清0
									
									if (IOOutputStruct[i].SupplyingCarNum != 0) {			//补发中收到车辆数
										IOOutputStruct[i].SupplyCarNum = IOOutputStruct[i].SupplyingCarNum;		//获取需要补发的车辆数
										IOOutputStruct[i].CarInOrOut = IOOutputStruct[i].SupplyingCarInOrOut;
										if (IOOutputStruct[i].SupplyingCarNum % 2 == 1) {
											IOOutputStruct[i].CarEvenOrOdd = 0;
										}
										else {
											IOOutputStruct[i].CarEvenOrOdd = 1;
										}
										IOOutputStruct[i].SupplyingCarNum = 0;
										IOOutputStruct[i].SupplyingFlag = 1;				//标志车辆数正在补发中
									}
								}
								else if (IOOutputStruct[i].SupplyCarNum > (IOOutputNUM * 2)) {	//超过需补发车辆数
									IOOutputStruct[i].SupplyCarNum = (IOOutputNUM * 2);		//补发最大补发数
								}
							}
						}
					}
					else {															//偶数包
						if ((IOOutputStruct[i].SupplyCarNum % 2) == 1) {
							if (IOOutputStruct[i].SupplyUseTime == 0) {
								GPIO_SetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
								IOOutputStruct[i].IOLevel = 1;							//电平置1
								IOOutputStruct[i].IOHighUseTime = 0;						//高电平时间清0
							}
							else if (IOOutputStruct[i].SupplyUseTime == (param_recv.vi_output_high_time * 20)) {
								GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
								IOOutputStruct[i].IOLevel = 0;							//电平置0
								IOOutputStruct[i].IOHighUseTime = 0;						//高电平时间清0
							}
							
							IOOutputStruct[i].SupplyUseTime += 1;
							
							if (IOOutputStruct[i].SupplyUseTime >= ((param_recv.vi_output_high_time * 20) + (param_recv.output_low_min * 20) + 1)) {
								IOOutputStruct[i].SupplyUseTime = 0;
								IOOutputStruct[i].SupplyCarNum -= 1;						//补发车辆数减一
								if (IOOutputStruct[i].SupplyCarNum == 0) {					//补发车辆数完成
									IOOutputStruct[i].SupplyCarNum = 0;					//清空补发车辆数
									IOOutputStruct[i].SupplyingFlag = 0;					//补发结束置0
									GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
									IOOutputStruct[i].IOLevel = 0;						//电平置0
									IOOutputStruct[i].IOHighUseTime = 0;					//高电平时间清0
									
									if (IOOutputStruct[i].SupplyingCarNum != 0) {			//补发中收到车辆数
										IOOutputStruct[i].SupplyCarNum = IOOutputStruct[i].SupplyingCarNum;		//获取需要补发的车辆数
										IOOutputStruct[i].CarInOrOut = IOOutputStruct[i].SupplyingCarInOrOut;
										if (IOOutputStruct[i].SupplyingCarNum % 2 == 1) {
											IOOutputStruct[i].CarEvenOrOdd = 0;
										}
										else {
											IOOutputStruct[i].CarEvenOrOdd = 1;
										}
										IOOutputStruct[i].SupplyingCarNum = 0;
										IOOutputStruct[i].SupplyingFlag = 1;				//标志车辆数正在补发中
									}
								}
								else if (IOOutputStruct[i].SupplyCarNum > (IOOutputNUM * 2)) {	//超过需补发车辆数
									IOOutputStruct[i].SupplyCarNum = (IOOutputNUM * 2);		//补发最大补发数
								}
							}
						}
						else {
							if (IOOutputStruct[i].SupplyUseTime == 0) {
								GPIO_SetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
								IOOutputStruct[i].IOLevel = 1;							//电平置1
								IOOutputStruct[i].IOHighUseTime = 0;						//高电平时间清0
							}
							else if (IOOutputStruct[i].SupplyUseTime == (param_recv.vo_output_high_time * 20)) {
								GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
								IOOutputStruct[i].IOLevel = 0;							//电平置0
								IOOutputStruct[i].IOHighUseTime = 0;						//高电平时间清0
							}
							
							IOOutputStruct[i].SupplyUseTime += 1;
							
							if (IOOutputStruct[i].SupplyUseTime >= ((param_recv.vo_output_high_time * 20) + (param_recv.output_low_min * 20) + 1)) {
								IOOutputStruct[i].SupplyUseTime = 0;
								IOOutputStruct[i].SupplyCarNum -= 1;						//补发车辆数减一
								if (IOOutputStruct[i].SupplyCarNum == 0) {					//补发车辆数完成
									IOOutputStruct[i].SupplyCarNum = 0;					//清空补发车辆数
									IOOutputStruct[i].SupplyingFlag = 0;					//补发结束置0
									GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
									IOOutputStruct[i].IOLevel = 0;						//电平置0
									IOOutputStruct[i].IOHighUseTime = 0;					//高电平时间清0
									
									if (IOOutputStruct[i].SupplyingCarNum != 0) {			//补发中收到车辆数
										IOOutputStruct[i].SupplyCarNum = IOOutputStruct[i].SupplyingCarNum;		//获取需要补发的车辆数
										IOOutputStruct[i].CarInOrOut = IOOutputStruct[i].SupplyingCarInOrOut;
										if (IOOutputStruct[i].SupplyingCarNum % 2 == 1) {
											IOOutputStruct[i].CarEvenOrOdd = 0;
										}
										else {
											IOOutputStruct[i].CarEvenOrOdd = 1;
										}
										IOOutputStruct[i].SupplyingCarNum = 0;
										IOOutputStruct[i].SupplyingFlag = 1;				//标志车辆数正在补发中
									}
								}
								else if (IOOutputStruct[i].SupplyCarNum > (IOOutputNUM * 2)) {	//超过需补发车辆数
									IOOutputStruct[i].SupplyCarNum = (IOOutputNUM * 2);		//补发最大补发数
								}
							}
						}
					}
				}
				else {													//最后一包为车检出
					if (IOOutputStruct[i].CarEvenOrOdd == 0) {					//奇数包
						if ((IOOutputStruct[i].SupplyCarNum % 2) == 1) {
							if (IOOutputStruct[i].SupplyUseTime == 0) {
								GPIO_SetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
								IOOutputStruct[i].IOLevel = 1;							//电平置1
								IOOutputStruct[i].IOHighUseTime = 0;						//高电平时间清0
							}
							else if (IOOutputStruct[i].SupplyUseTime == (param_recv.vo_output_high_time * 20)) {
								GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
								IOOutputStruct[i].IOLevel = 0;							//电平置0
								IOOutputStruct[i].IOHighUseTime = 0;						//高电平时间清0
							}
							
							IOOutputStruct[i].SupplyUseTime += 1;
							
							if (IOOutputStruct[i].SupplyUseTime >= ((param_recv.vo_output_high_time * 20) + (param_recv.output_low_min * 20) + 1)) {
								IOOutputStruct[i].SupplyUseTime = 0;
								IOOutputStruct[i].SupplyCarNum -= 1;						//补发车辆数减一
								if (IOOutputStruct[i].SupplyCarNum == 0) {					//补发车辆数完成
									IOOutputStruct[i].SupplyCarNum = 0;					//清空补发车辆数
									IOOutputStruct[i].SupplyingFlag = 0;					//补发结束置0
									GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
									IOOutputStruct[i].IOLevel = 0;						//电平置0
									IOOutputStruct[i].IOHighUseTime = 0;					//高电平时间清0
									
									if (IOOutputStruct[i].SupplyingCarNum != 0) {			//补发中收到车辆数
										IOOutputStruct[i].SupplyCarNum = IOOutputStruct[i].SupplyingCarNum;		//获取需要补发的车辆数
										IOOutputStruct[i].CarInOrOut = IOOutputStruct[i].SupplyingCarInOrOut;
										if (IOOutputStruct[i].SupplyingCarNum % 2 == 1) {
											IOOutputStruct[i].CarEvenOrOdd = 0;
										}
										else {
											IOOutputStruct[i].CarEvenOrOdd = 1;
										}
										IOOutputStruct[i].SupplyingCarNum = 0;
										IOOutputStruct[i].SupplyingFlag = 1;				//标志车辆数正在补发中
									}
								}
								else if (IOOutputStruct[i].SupplyCarNum > (IOOutputNUM * 2)) {	//超过需补发车辆数
									IOOutputStruct[i].SupplyCarNum = (IOOutputNUM * 2);		//补发最大补发数
								}
							}
						}
						else {
							if (IOOutputStruct[i].SupplyUseTime == 0) {
								GPIO_SetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
								IOOutputStruct[i].IOLevel = 1;							//电平置1
								IOOutputStruct[i].IOHighUseTime = 0;						//高电平时间清0
							}
							else if (IOOutputStruct[i].SupplyUseTime == (param_recv.vi_output_high_time * 20)) {
								GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
								IOOutputStruct[i].IOLevel = 0;							//电平置0
								IOOutputStruct[i].IOHighUseTime = 0;						//高电平时间清0
							}
							
							IOOutputStruct[i].SupplyUseTime += 1;
							
							if (IOOutputStruct[i].SupplyUseTime >= ((param_recv.vi_output_high_time * 20) + (param_recv.output_low_min * 20) + 1)) {
								IOOutputStruct[i].SupplyUseTime = 0;
								IOOutputStruct[i].SupplyCarNum -= 1;						//补发车辆数减一
								if (IOOutputStruct[i].SupplyCarNum == 0) {					//补发车辆数完成
									IOOutputStruct[i].SupplyCarNum = 0;					//清空补发车辆数
									IOOutputStruct[i].SupplyingFlag = 0;					//补发结束置0
									GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
									IOOutputStruct[i].IOLevel = 0;						//电平置0
									IOOutputStruct[i].IOHighUseTime = 0;					//高电平时间清0
									
									if (IOOutputStruct[i].SupplyingCarNum != 0) {			//补发中收到车辆数
										IOOutputStruct[i].SupplyCarNum = IOOutputStruct[i].SupplyingCarNum;		//获取需要补发的车辆数
										IOOutputStruct[i].CarInOrOut = IOOutputStruct[i].SupplyingCarInOrOut;
										if (IOOutputStruct[i].SupplyingCarNum % 2 == 1) {
											IOOutputStruct[i].CarEvenOrOdd = 0;
										}
										else {
											IOOutputStruct[i].CarEvenOrOdd = 1;
										}
										IOOutputStruct[i].SupplyingCarNum = 0;
										IOOutputStruct[i].SupplyingFlag = 1;				//标志车辆数正在补发中
									}
								}
								else if (IOOutputStruct[i].SupplyCarNum > (IOOutputNUM * 2)) {	//超过需补发车辆数
									IOOutputStruct[i].SupplyCarNum = (IOOutputNUM * 2);		//补发最大补发数
								}
							}
						}
					}
					else {												//偶数包
						if ((IOOutputStruct[i].SupplyCarNum % 2) == 1) {
							if (IOOutputStruct[i].SupplyUseTime == 0) {
								GPIO_SetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
								IOOutputStruct[i].IOLevel = 1;							//电平置1
								IOOutputStruct[i].IOHighUseTime = 0;						//高电平时间清0
							}
							else if (IOOutputStruct[i].SupplyUseTime == (param_recv.vo_output_high_time * 20)) {
								GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
								IOOutputStruct[i].IOLevel = 0;							//电平置0
								IOOutputStruct[i].IOHighUseTime = 0;						//高电平时间清0
							}
							
							IOOutputStruct[i].SupplyUseTime += 1;
							
							if (IOOutputStruct[i].SupplyUseTime >= ((param_recv.vo_output_high_time * 20) + (param_recv.output_low_min * 20) + 1)) {
								IOOutputStruct[i].SupplyUseTime = 0;
								IOOutputStruct[i].SupplyCarNum -= 1;						//补发车辆数减一
								if (IOOutputStruct[i].SupplyCarNum == 0) {					//补发车辆数完成
									IOOutputStruct[i].SupplyCarNum = 0;					//清空补发车辆数
									IOOutputStruct[i].SupplyingFlag = 0;					//补发结束置0
									GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
									IOOutputStruct[i].IOLevel = 0;						//电平置0
									IOOutputStruct[i].IOHighUseTime = 0;					//高电平时间清0
									
									if (IOOutputStruct[i].SupplyingCarNum != 0) {			//补发中收到车辆数
										IOOutputStruct[i].SupplyCarNum = IOOutputStruct[i].SupplyingCarNum;		//获取需要补发的车辆数
										IOOutputStruct[i].CarInOrOut = IOOutputStruct[i].SupplyingCarInOrOut;
										if (IOOutputStruct[i].SupplyingCarNum % 2 == 1) {
											IOOutputStruct[i].CarEvenOrOdd = 0;
										}
										else {
											IOOutputStruct[i].CarEvenOrOdd = 1;
										}
										IOOutputStruct[i].SupplyingCarNum = 0;
										IOOutputStruct[i].SupplyingFlag = 1;				//标志车辆数正在补发中
									}
								}
								else if (IOOutputStruct[i].SupplyCarNum > (IOOutputNUM * 2)) {	//超过需补发车辆数
									IOOutputStruct[i].SupplyCarNum = (IOOutputNUM * 2);		//补发最大补发数
								}
							}
						}
						else {
							if (IOOutputStruct[i].SupplyUseTime == 0) {
								GPIO_SetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
								IOOutputStruct[i].IOLevel = 1;							//电平置1
								IOOutputStruct[i].IOHighUseTime = 0;						//高电平时间清0
							}
							else if (IOOutputStruct[i].SupplyUseTime == (param_recv.vi_output_high_time * 20)) {
								GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
								IOOutputStruct[i].IOLevel = 0;							//电平置0
								IOOutputStruct[i].IOHighUseTime = 0;						//高电平时间清0
							}
							
							IOOutputStruct[i].SupplyUseTime += 1;
							
							if (IOOutputStruct[i].SupplyUseTime >= ((param_recv.vi_output_high_time * 20) + (param_recv.output_low_min * 20) + 1)) {
								IOOutputStruct[i].SupplyUseTime = 0;
								IOOutputStruct[i].SupplyCarNum -= 1;						//补发车辆数减一
								if (IOOutputStruct[i].SupplyCarNum == 0) {					//补发车辆数完成
									IOOutputStruct[i].SupplyCarNum = 0;					//清空补发车辆数
									IOOutputStruct[i].SupplyingFlag = 0;					//补发结束置0
									GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
									IOOutputStruct[i].IOLevel = 0;						//电平置0
									IOOutputStruct[i].IOHighUseTime = 0;					//高电平时间清0
									
									if (IOOutputStruct[i].SupplyingCarNum != 0) {			//补发中收到车辆数
										IOOutputStruct[i].SupplyCarNum = IOOutputStruct[i].SupplyingCarNum;		//获取需要补发的车辆数
										IOOutputStruct[i].CarInOrOut = IOOutputStruct[i].SupplyingCarInOrOut;
										if (IOOutputStruct[i].SupplyingCarNum % 2 == 1) {
											IOOutputStruct[i].CarEvenOrOdd = 0;
										}
										else {
											IOOutputStruct[i].CarEvenOrOdd = 1;
										}
										IOOutputStruct[i].SupplyingCarNum = 0;
										IOOutputStruct[i].SupplyingFlag = 1;				//标志车辆数正在补发中
									}
								}
								else if (IOOutputStruct[i].SupplyCarNum > (IOOutputNUM * 2)) {	//超过需补发车辆数
									IOOutputStruct[i].SupplyCarNum = (IOOutputNUM * 2);		//补发最大补发数
								}
							}
						}
					}
				}
			}
		}
	}
}


/**********************************************************************************************************
 @Function			void IOOutput_Mode4_Check(u16 dici_index, u16 dici_carnum, u8 dici_in)
 @Description			IO输出校验数据 (输出方式4 : 车辆进入输出固定时长(不记数))
 @Input				dici_index  	: 地磁ID编号
					dici_carnum 	: 该地磁车辆数状态值
					dici_in	  	: 1.车辆进入
							       0.车辆离开
 @Return				void
**********************************************************************************************************/
void IOOutput_Mode4_Check(u16 dici_index, u16 dici_carnum, u8 dici_in)
{
	u16 carnum = 0;
	
	if (IOOutputStruct[dici_index].CarNumState == 0) {						//初始化ID车辆数
		IOOutputStruct[dici_index].CarNumState = dici_carnum;					//记录车辆数状态值
		if (dici_in == 1) {
			GPIO_SetBits(OUTPUT_TYPE[dici_index],  OUTPUT_PIN[dici_index]);
			IOOutputStruct[dici_index].IOLevel = 1;							//电平置1
			IOOutputStruct[dici_index].IOHighUseTime = 0;					//高电平时间清0
		}
		else {
			GPIO_ResetBits(OUTPUT_TYPE[dici_index],  OUTPUT_PIN[dici_index]);
			IOOutputStruct[dici_index].IOLevel = 0;							//电平置0
			IOOutputStruct[dici_index].IOHighUseTime = 0;					//高电平时间清0
		}
	}
	else {															//已初始化完成
		if (dici_carnum > IOOutputStruct[dici_index].CarNumState) {				//计算车辆数值
			if ((dici_carnum - IOOutputStruct[dici_index].CarNumState) < IOOutputNUM) {
				carnum = dici_carnum - IOOutputStruct[dici_index].CarNumState;
			}
		}
		else if (dici_carnum < IOOutputStruct[dici_index].CarNumState) {
			if ((65535 - IOOutputStruct[dici_index].CarNumState + dici_carnum) < IOOutputNUM) {
				carnum = 65535 - IOOutputStruct[dici_index].CarNumState + dici_carnum;
			}
		}
		IOOutputStruct[dici_index].CarNumState = dici_carnum;					//记录车辆数状态值
		
		/* 判断该地磁是否正在补发数据 */
		if (IOOutputStruct[dici_index].SupplyingFlag == 0) {					//补发结束
			if (carnum != 0) {											//需要补充车辆数
				IOOutputStruct[dici_index].SupplyCarNum = 1;					//获取需要补发的车辆数
				IOOutputStruct[dici_index].SupplyedIOLevel = 0;				//补充车辆数结束后输出低电平
				IOOutputStruct[dici_index].SupplyingFlag = 1;				//标志车辆数正在补发中
			}
		}
		else {														//正在补发
			IOOutputStruct[dici_index].SupplyingCarNum = 1;					//获取正在补发中收到的车辆数
			if (IOOutputStruct[dici_index].SupplyingCarNum >= IOOutputNUM) {		//正在补发中收到的车辆数超过需补发最大车辆数
				IOOutputStruct[dici_index].SupplyingCarNum = IOOutputNUM;
			}
			IOOutputStruct[dici_index].SupplyedIOLevel = 0;					//补充车辆数结束后输出低电平
		}
	}
}

/**********************************************************************************************************
 @Function			void IOOutput_Mode4_Supplying(void)
 @Description			IO输出定时器补发 1ms执行一次 (输出方式4 : 车辆进入输出固定时长(不记数))
 @Input				void
 @Return				void
**********************************************************************************************************/
void IOOutput_Mode4_Supplying(void)
{
	u8 i = 0;
	
	for (i = 0; i < IOOutputMAX; i++) {
		if (IOOutputStruct[i].OutputID != 0) {								//ID不为0处理
			
			/* 补发车辆数 */
			if (IOOutputStruct[i].SupplyingFlag != 0) {
				
				if (IOOutputStruct[i].SupplyUseTime == 0) {
					GPIO_SetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
					IOOutputStruct[i].IOLevel = 1;						//电平置1
					IOOutputStruct[i].IOHighUseTime = 0;					//高电平时间清0
				}
				else if (IOOutputStruct[i].SupplyUseTime == (param_recv.vi_output_high_time * 20)) {
					GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
					IOOutputStruct[i].IOLevel = 0;						//电平置0
					IOOutputStruct[i].IOHighUseTime = 0;					//高电平时间清0
				}
				
				IOOutputStruct[i].SupplyUseTime += 1;
				
				if (IOOutputStruct[i].SupplyUseTime >= ((param_recv.vi_output_high_time * 20) + (param_recv.output_low_min * 20) + 1)) {
					IOOutputStruct[i].SupplyUseTime = 0;
					IOOutputStruct[i].SupplyCarNum -= 1;					//补发车辆数减一
					if (IOOutputStruct[i].SupplyCarNum == 0) {				//补发车辆数完成
						IOOutputStruct[i].SupplyCarNum = 0;				//清空补发车辆数
						
						if (IOOutputStruct[i].SupplyingCarNum != 0) {		//补发中收到车辆数
							IOOutputStruct[i].SupplyCarNum = IOOutputStruct[i].SupplyingCarNum;					//获取需要补发的车辆数
							IOOutputStruct[i].SupplyingCarNum = 0;
							IOOutputStruct[i].SupplyingFlag = 1;			//标志车辆数正在补发中
						}
						else {
							IOOutputStruct[i].SupplyingFlag = 0;			//补发结束置0
							GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
							IOOutputStruct[i].IOLevel = 0;				//电平置0
							IOOutputStruct[i].IOHighUseTime = 0;			//高电平时间清0
						}
					}
					else if (IOOutputStruct[i].SupplyCarNum > IOOutputNUM) {	//超过需补发车辆数
						IOOutputStruct[i].SupplyCarNum = IOOutputNUM;		//补发最大补发数
					}
				}
			}
		}
	}
}


/**********************************************************************************************************
 @Function			void IOOutput_Mode5_Check(u16 dici_index, u16 dici_carnum, u8 dici_in)
 @Description			IO输出校验数据 (输出方式5 : 车辆离开输出固定时长(不记数))
 @Input				dici_index  	: 地磁ID编号
					dici_carnum 	: 该地磁车辆数状态值
					dici_in	  	: 1.车辆进入
							       0.车辆离开
 @Return				void
**********************************************************************************************************/
void IOOutput_Mode5_Check(u16 dici_index, u16 dici_carnum, u8 dici_in)
{
	u16 carnum = 0;
	
	if (IOOutputStruct[dici_index].CarNumState == 0) {						//初始化ID车辆数
		IOOutputStruct[dici_index].CarNumState = dici_carnum;					//记录车辆数状态值
		if (dici_in == 1) {
			GPIO_SetBits(OUTPUT_TYPE[dici_index],  OUTPUT_PIN[dici_index]);
			IOOutputStruct[dici_index].IOLevel = 1;							//电平置1
			IOOutputStruct[dici_index].IOHighUseTime = 0;					//高电平时间清0
		}
		else {
			GPIO_ResetBits(OUTPUT_TYPE[dici_index],  OUTPUT_PIN[dici_index]);
			IOOutputStruct[dici_index].IOLevel = 0;							//电平置0
			IOOutputStruct[dici_index].IOHighUseTime = 0;					//高电平时间清0
		}
	}
	else {															//已初始化完成
		if (dici_carnum > IOOutputStruct[dici_index].CarNumState) {				//计算车辆数值
			if ((dici_carnum - IOOutputStruct[dici_index].CarNumState) < IOOutputNUM) {
				carnum = dici_carnum - IOOutputStruct[dici_index].CarNumState;
			}
		}
		else if (dici_carnum < IOOutputStruct[dici_index].CarNumState) {
			if ((65535 - IOOutputStruct[dici_index].CarNumState + dici_carnum) < IOOutputNUM) {
				carnum = 65535 - IOOutputStruct[dici_index].CarNumState + dici_carnum;
			}
		}
		IOOutputStruct[dici_index].CarNumState = dici_carnum;					//记录车辆数状态值
		
		/* 判断该地磁是否正在补发数据 */
		if (IOOutputStruct[dici_index].SupplyingFlag == 0) {					//补发结束
			if (carnum != 0) {											//需要补充车辆数
				IOOutputStruct[dici_index].SupplyCarNum = 1;					//获取需要补发的车辆数
				IOOutputStruct[dici_index].SupplyedIOLevel = 0;				//补充车辆数结束后输出低电平
				IOOutputStruct[dici_index].SupplyingFlag = 1;				//标志车辆数正在补发中
			}
		}
		else {														//正在补发
			IOOutputStruct[dici_index].SupplyingCarNum = 1;					//获取正在补发中收到的车辆数
			if (IOOutputStruct[dici_index].SupplyingCarNum >= IOOutputNUM) {		//正在补发中收到的车辆数超过需补发最大车辆数
				IOOutputStruct[dici_index].SupplyingCarNum = IOOutputNUM;
			}
			IOOutputStruct[dici_index].SupplyedIOLevel = 0;					//补充车辆数结束后输出低电平
		}
	}
}

/**********************************************************************************************************
 @Function			void IOOutput_Mode5_Supplying(void)
 @Description			IO输出定时器补发 1ms执行一次 (输出方式5 : 车辆离开输出固定时长(不记数))
 @Input				void
 @Return				void
**********************************************************************************************************/
void IOOutput_Mode5_Supplying(void)
{
	u8 i = 0;
	
	for (i = 0; i < IOOutputMAX; i++) {
		if (IOOutputStruct[i].OutputID != 0) {								//ID不为0处理
			
			/* 补发车辆数 */
			if (IOOutputStruct[i].SupplyingFlag != 0) {
				
				if (IOOutputStruct[i].SupplyUseTime == 0) {
					GPIO_SetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
					IOOutputStruct[i].IOLevel = 1;						//电平置1
					IOOutputStruct[i].IOHighUseTime = 0;					//高电平时间清0
				}
				else if (IOOutputStruct[i].SupplyUseTime == (param_recv.vo_output_high_time * 20)) {
					GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
					IOOutputStruct[i].IOLevel = 0;						//电平置0
					IOOutputStruct[i].IOHighUseTime = 0;					//高电平时间清0
				}
				
				IOOutputStruct[i].SupplyUseTime += 1;
				
				if (IOOutputStruct[i].SupplyUseTime >= ((param_recv.vo_output_high_time * 20) + (param_recv.output_low_min * 20) + 1)) {
					IOOutputStruct[i].SupplyUseTime = 0;
					IOOutputStruct[i].SupplyCarNum -= 1;					//补发车辆数减一
					if (IOOutputStruct[i].SupplyCarNum == 0) {				//补发车辆数完成
						IOOutputStruct[i].SupplyCarNum = 0;				//清空补发车辆数
						
						if (IOOutputStruct[i].SupplyingCarNum != 0) {		//补发中收到车辆数
							IOOutputStruct[i].SupplyCarNum = IOOutputStruct[i].SupplyingCarNum;					//获取需要补发的车辆数
							IOOutputStruct[i].SupplyingCarNum = 0;
							IOOutputStruct[i].SupplyingFlag = 1;			//标志车辆数正在补发中
						}
						else {
							IOOutputStruct[i].SupplyingFlag = 0;			//补发结束置0
							GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
							IOOutputStruct[i].IOLevel = 0;				//电平置0
							IOOutputStruct[i].IOHighUseTime = 0;			//高电平时间清0
						}
					}
					else if (IOOutputStruct[i].SupplyCarNum > IOOutputNUM) {	//超过需补发车辆数
						IOOutputStruct[i].SupplyCarNum = IOOutputNUM;		//补发最大补发数
					}
				}
			}
		}
	}
}


/**********************************************************************************************************
 @Function			void IOOutput_Mode6_Check(u16 dici_index, u16 dici_carnum, u8 dici_in)
 @Description			IO输出校验数据 (输出方式6 : 车辆进入,离开时都输出固定时长(不记数))
 @Input				dici_index  	: 地磁ID编号
					dici_carnum 	: 该地磁车辆数状态值
					dici_in	  	: 1.车辆进入
							       0.车辆离开
 @Return				void
**********************************************************************************************************/
void IOOutput_Mode6_Check(u16 dici_index, u16 dici_carnum, u8 dici_in)
{
	u16 carnum = 0;
	
	if (IOOutputStruct[dici_index].CarNumState == 0) {						//初始化ID车辆数
		IOOutputStruct[dici_index].CarNumState = dici_carnum;					//记录车辆数状态值
		if (dici_in == 1) {
			GPIO_SetBits(OUTPUT_TYPE[dici_index],  OUTPUT_PIN[dici_index]);
			IOOutputStruct[dici_index].IOLevel = 1;							//电平置1
			IOOutputStruct[dici_index].IOHighUseTime = 0;					//高电平时间清0
		}
		else {
			GPIO_ResetBits(OUTPUT_TYPE[dici_index],  OUTPUT_PIN[dici_index]);
			IOOutputStruct[dici_index].IOLevel = 0;							//电平置0
			IOOutputStruct[dici_index].IOHighUseTime = 0;					//高电平时间清0
		}
	}
	else {															//已初始化完成
		if (dici_carnum > IOOutputStruct[dici_index].CarNumState) {				//计算车辆数值
			if ((dici_carnum - IOOutputStruct[dici_index].CarNumState) < IOOutputNUM) {
				carnum = dici_carnum - IOOutputStruct[dici_index].CarNumState;
			}
		}
		else if (dici_carnum < IOOutputStruct[dici_index].CarNumState) {
			if ((65535 - IOOutputStruct[dici_index].CarNumState + dici_carnum) < IOOutputNUM) {
				carnum = 65535 - IOOutputStruct[dici_index].CarNumState + dici_carnum;
			}
		}
		IOOutputStruct[dici_index].CarNumState = dici_carnum;					//记录车辆数状态值
		
		/* 判断该地磁是否正在补发数据 */
		if (IOOutputStruct[dici_index].SupplyingFlag == 0) {					//补发结束
			if (carnum != 0) {											//需要补充车辆数
				if (dici_in == 1) {
					IOOutputStruct[dici_index].SupplyCarNum = 1;				//获取需要补发的车辆数
					IOOutputStruct[dici_index].CarInOrOut = 1;				//车检入
					IOOutputStruct[dici_index].CarEvenOrOdd = 0;				//奇数包
				}
				else {
					IOOutputStruct[dici_index].SupplyCarNum = 1;				//获取需要补发的车辆数
					IOOutputStruct[dici_index].CarInOrOut = 0;				//车检出
					IOOutputStruct[dici_index].CarEvenOrOdd = 1;				//偶数包
				}
				IOOutputStruct[dici_index].SupplyedIOLevel = 0;				//补充车辆数结束后输出低电平
				IOOutputStruct[dici_index].SupplyingFlag = 1;				//标志车辆数正在补发中
			}
			else {													//车检出carnum不加
				IOOutputStruct[dici_index].SupplyCarNum = 1;					//获取需要补发的车辆数
				IOOutputStruct[dici_index].CarInOrOut = 0;					//车检出
				IOOutputStruct[dici_index].CarEvenOrOdd = 0;					//奇数包
				IOOutputStruct[dici_index].SupplyedIOLevel = 0;				//补充车辆数结束后输出低电平
				IOOutputStruct[dici_index].SupplyingFlag = 1;				//标志车辆数正在补发中
			}
		}
		else {																	//正在补发
			if (carnum != 0) {														//需要补充车辆数
				if (dici_in == 1) {
					IOOutputStruct[dici_index].SupplyingCarNum = 1;						//获取正在补发中收到的车辆数
					if (IOOutputStruct[dici_index].SupplyingCarNum >= (IOOutputNUM * 2)) {
						IOOutputStruct[dici_index].SupplyingCarNum -= (carnum * 2 - 1);
					}
					else {
						IOOutputStruct[dici_index].SupplyingCarInOrOut = 1;				//车检入
					}
				}
				else {
					IOOutputStruct[dici_index].SupplyingCarNum = 1;						//获取正在补发中收到的车辆数
					if (IOOutputStruct[dici_index].SupplyingCarNum >= (IOOutputNUM * 2)) {
						IOOutputStruct[dici_index].SupplyingCarNum -= (carnum * 2);
					}
					else {
						IOOutputStruct[dici_index].SupplyingCarInOrOut = 0;				//车检出
					}
				}
			}
			else {																//车检出carnum不加
				IOOutputStruct[dici_index].SupplyingCarNum = 1;
				if (IOOutputStruct[dici_index].SupplyingCarNum >= (IOOutputNUM * 2)) {
					IOOutputStruct[dici_index].SupplyingCarNum -= 1;
				}
				else {
					IOOutputStruct[dici_index].SupplyingCarInOrOut = 0;					//车检出
				}
			}
			
			IOOutputStruct[dici_index].SupplyedIOLevel = 0;								//补充车辆数结束后输出低电平
		}
	}
}

/**********************************************************************************************************
 @Function			void IOOutput_Mode6_Supplying(void)
 @Description			IO输出定时器补发 1ms执行一次 (输出方式6 : 车辆进入,离开时都输出固定时长(不记数))
 @Input				void
 @Return				void
**********************************************************************************************************/
void IOOutput_Mode6_Supplying(void)
{
	u8 i = 0;
	
	for (i = 0; i < IOOutputMAX; i++) {
		if (IOOutputStruct[i].OutputID != 0) {												//ID不为0处理
			
			/* 补发车辆数 */
			if (IOOutputStruct[i].SupplyingFlag != 0) {
				
				if (IOOutputStruct[i].CarInOrOut == 1) {									//最后一包为车检入
					if (IOOutputStruct[i].CarEvenOrOdd == 0) {								//奇数包
						if ((IOOutputStruct[i].SupplyCarNum % 2) == 1) {
							if (IOOutputStruct[i].SupplyUseTime == 0) {
								GPIO_SetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
								IOOutputStruct[i].IOLevel = 1;							//电平置1
								IOOutputStruct[i].IOHighUseTime = 0;						//高电平时间清0
							}
							else if (IOOutputStruct[i].SupplyUseTime == (param_recv.vi_output_high_time * 20)) {
								GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
								IOOutputStruct[i].IOLevel = 0;							//电平置0
								IOOutputStruct[i].IOHighUseTime = 0;						//高电平时间清0
							}
							
							IOOutputStruct[i].SupplyUseTime += 1;
							
							if (IOOutputStruct[i].SupplyUseTime >= ((param_recv.vi_output_high_time * 20) + (param_recv.output_low_min * 20) + 1)) {
								IOOutputStruct[i].SupplyUseTime = 0;
								IOOutputStruct[i].SupplyCarNum -= 1;						//补发车辆数减一
								if (IOOutputStruct[i].SupplyCarNum == 0) {					//补发车辆数完成
									IOOutputStruct[i].SupplyCarNum = 0;					//清空补发车辆数
									
									if (IOOutputStruct[i].SupplyingCarNum != 0) {			//补发中收到车辆数
										IOOutputStruct[i].SupplyCarNum = IOOutputStruct[i].SupplyingCarNum;		//获取需要补发的车辆数
										IOOutputStruct[i].CarInOrOut = IOOutputStruct[i].SupplyingCarInOrOut;
										if (IOOutputStruct[i].SupplyingCarNum % 2 == 1) {
											IOOutputStruct[i].CarEvenOrOdd = 0;
										}
										else {
											IOOutputStruct[i].CarEvenOrOdd = 1;
										}
										IOOutputStruct[i].SupplyingCarNum = 0;
										IOOutputStruct[i].SupplyingFlag = 1;				//标志车辆数正在补发中
									}
									else {
										IOOutputStruct[i].SupplyingFlag = 0;				//补发结束置0
										GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
										IOOutputStruct[i].IOLevel = 0;					//电平置0
										IOOutputStruct[i].IOHighUseTime = 0;				//高电平时间清0
									}
								}
								else if (IOOutputStruct[i].SupplyCarNum > (IOOutputNUM * 2)) {	//超过需补发车辆数
									IOOutputStruct[i].SupplyCarNum = (IOOutputNUM * 2);		//补发最大补发数
								}
							}
						}
						else {
							if (IOOutputStruct[i].SupplyUseTime == 0) {
								GPIO_SetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
								IOOutputStruct[i].IOLevel = 1;							//电平置1
								IOOutputStruct[i].IOHighUseTime = 0;						//高电平时间清0
							}
							else if (IOOutputStruct[i].SupplyUseTime == (param_recv.vo_output_high_time * 20)) {
								GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
								IOOutputStruct[i].IOLevel = 0;							//电平置0
								IOOutputStruct[i].IOHighUseTime = 0;						//高电平时间清0
							}
							
							IOOutputStruct[i].SupplyUseTime += 1;
							
							if (IOOutputStruct[i].SupplyUseTime >= ((param_recv.vo_output_high_time * 20) + (param_recv.output_low_min * 20) + 1)) {
								IOOutputStruct[i].SupplyUseTime = 0;
								IOOutputStruct[i].SupplyCarNum -= 1;						//补发车辆数减一
								if (IOOutputStruct[i].SupplyCarNum == 0) {					//补发车辆数完成
									IOOutputStruct[i].SupplyCarNum = 0;					//清空补发车辆数
									
									if (IOOutputStruct[i].SupplyingCarNum != 0) {			//补发中收到车辆数
										IOOutputStruct[i].SupplyCarNum = IOOutputStruct[i].SupplyingCarNum;		//获取需要补发的车辆数
										IOOutputStruct[i].CarInOrOut = IOOutputStruct[i].SupplyingCarInOrOut;
										if (IOOutputStruct[i].SupplyingCarNum % 2 == 1) {
											IOOutputStruct[i].CarEvenOrOdd = 0;
										}
										else {
											IOOutputStruct[i].CarEvenOrOdd = 1;
										}
										IOOutputStruct[i].SupplyingCarNum = 0;
										IOOutputStruct[i].SupplyingFlag = 1;				//标志车辆数正在补发中
									}
									else {
										IOOutputStruct[i].SupplyingFlag = 0;				//补发结束置0
										GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
										IOOutputStruct[i].IOLevel = 0;					//电平置0
										IOOutputStruct[i].IOHighUseTime = 0;				//高电平时间清0
									}
								}
								else if (IOOutputStruct[i].SupplyCarNum > (IOOutputNUM * 2)) {	//超过需补发车辆数
									IOOutputStruct[i].SupplyCarNum = (IOOutputNUM * 2);		//补发最大补发数
								}
							}
						}
					}
					else {															//偶数包
						if ((IOOutputStruct[i].SupplyCarNum % 2) == 1) {
							if (IOOutputStruct[i].SupplyUseTime == 0) {
								GPIO_SetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
								IOOutputStruct[i].IOLevel = 1;							//电平置1
								IOOutputStruct[i].IOHighUseTime = 0;						//高电平时间清0
							}
							else if (IOOutputStruct[i].SupplyUseTime == (param_recv.vi_output_high_time * 20)) {
								GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
								IOOutputStruct[i].IOLevel = 0;							//电平置0
								IOOutputStruct[i].IOHighUseTime = 0;						//高电平时间清0
							}
							
							IOOutputStruct[i].SupplyUseTime += 1;
							
							if (IOOutputStruct[i].SupplyUseTime >= ((param_recv.vi_output_high_time * 20) + (param_recv.output_low_min * 20) + 1)) {
								IOOutputStruct[i].SupplyUseTime = 0;
								IOOutputStruct[i].SupplyCarNum -= 1;						//补发车辆数减一
								if (IOOutputStruct[i].SupplyCarNum == 0) {					//补发车辆数完成
									IOOutputStruct[i].SupplyCarNum = 0;					//清空补发车辆数
									
									if (IOOutputStruct[i].SupplyingCarNum != 0) {			//补发中收到车辆数
										IOOutputStruct[i].SupplyCarNum = IOOutputStruct[i].SupplyingCarNum;		//获取需要补发的车辆数
										IOOutputStruct[i].CarInOrOut = IOOutputStruct[i].SupplyingCarInOrOut;
										if (IOOutputStruct[i].SupplyingCarNum % 2 == 1) {
											IOOutputStruct[i].CarEvenOrOdd = 0;
										}
										else {
											IOOutputStruct[i].CarEvenOrOdd = 1;
										}
										IOOutputStruct[i].SupplyingCarNum = 0;
										IOOutputStruct[i].SupplyingFlag = 1;				//标志车辆数正在补发中
									}
									else {
										IOOutputStruct[i].SupplyingFlag = 0;				//补发结束置0
										GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
										IOOutputStruct[i].IOLevel = 0;					//电平置0
										IOOutputStruct[i].IOHighUseTime = 0;				//高电平时间清0
									}
								}
								else if (IOOutputStruct[i].SupplyCarNum > (IOOutputNUM * 2)) {	//超过需补发车辆数
									IOOutputStruct[i].SupplyCarNum = (IOOutputNUM * 2);		//补发最大补发数
								}
							}
						}
						else {
							if (IOOutputStruct[i].SupplyUseTime == 0) {
								GPIO_SetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
								IOOutputStruct[i].IOLevel = 1;							//电平置1
								IOOutputStruct[i].IOHighUseTime = 0;						//高电平时间清0
							}
							else if (IOOutputStruct[i].SupplyUseTime == (param_recv.vo_output_high_time * 20)) {
								GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
								IOOutputStruct[i].IOLevel = 0;							//电平置0
								IOOutputStruct[i].IOHighUseTime = 0;						//高电平时间清0
							}
							
							IOOutputStruct[i].SupplyUseTime += 1;
							
							if (IOOutputStruct[i].SupplyUseTime >= ((param_recv.vo_output_high_time * 20) + (param_recv.output_low_min * 20) + 1)) {
								IOOutputStruct[i].SupplyUseTime = 0;
								IOOutputStruct[i].SupplyCarNum -= 1;						//补发车辆数减一
								if (IOOutputStruct[i].SupplyCarNum == 0) {					//补发车辆数完成
									IOOutputStruct[i].SupplyCarNum = 0;					//清空补发车辆数
									
									if (IOOutputStruct[i].SupplyingCarNum != 0) {			//补发中收到车辆数
										IOOutputStruct[i].SupplyCarNum = IOOutputStruct[i].SupplyingCarNum;		//获取需要补发的车辆数
										IOOutputStruct[i].CarInOrOut = IOOutputStruct[i].SupplyingCarInOrOut;
										if (IOOutputStruct[i].SupplyingCarNum % 2 == 1) {
											IOOutputStruct[i].CarEvenOrOdd = 0;
										}
										else {
											IOOutputStruct[i].CarEvenOrOdd = 1;
										}
										IOOutputStruct[i].SupplyingCarNum = 0;
										IOOutputStruct[i].SupplyingFlag = 1;				//标志车辆数正在补发中
									}
									else {
										IOOutputStruct[i].SupplyingFlag = 0;				//补发结束置0
										GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
										IOOutputStruct[i].IOLevel = 0;					//电平置0
										IOOutputStruct[i].IOHighUseTime = 0;				//高电平时间清0
									}
								}
								else if (IOOutputStruct[i].SupplyCarNum > (IOOutputNUM * 2)) {	//超过需补发车辆数
									IOOutputStruct[i].SupplyCarNum = (IOOutputNUM * 2);		//补发最大补发数
								}
							}
						}
					}
				}
				else {													//最后一包为车检出
					if (IOOutputStruct[i].CarEvenOrOdd == 0) {					//奇数包
						if ((IOOutputStruct[i].SupplyCarNum % 2) == 1) {
							if (IOOutputStruct[i].SupplyUseTime == 0) {
								GPIO_SetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
								IOOutputStruct[i].IOLevel = 1;							//电平置1
								IOOutputStruct[i].IOHighUseTime = 0;						//高电平时间清0
							}
							else if (IOOutputStruct[i].SupplyUseTime == (param_recv.vo_output_high_time * 20)) {
								GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
								IOOutputStruct[i].IOLevel = 0;							//电平置0
								IOOutputStruct[i].IOHighUseTime = 0;						//高电平时间清0
							}
							
							IOOutputStruct[i].SupplyUseTime += 1;
							
							if (IOOutputStruct[i].SupplyUseTime >= ((param_recv.vo_output_high_time * 20) + (param_recv.output_low_min * 20) + 1)) {
								IOOutputStruct[i].SupplyUseTime = 0;
								IOOutputStruct[i].SupplyCarNum -= 1;						//补发车辆数减一
								if (IOOutputStruct[i].SupplyCarNum == 0) {					//补发车辆数完成
									IOOutputStruct[i].SupplyCarNum = 0;					//清空补发车辆数
									
									if (IOOutputStruct[i].SupplyingCarNum != 0) {			//补发中收到车辆数
										IOOutputStruct[i].SupplyCarNum = IOOutputStruct[i].SupplyingCarNum;		//获取需要补发的车辆数
										IOOutputStruct[i].CarInOrOut = IOOutputStruct[i].SupplyingCarInOrOut;
										if (IOOutputStruct[i].SupplyingCarNum % 2 == 1) {
											IOOutputStruct[i].CarEvenOrOdd = 0;
										}
										else {
											IOOutputStruct[i].CarEvenOrOdd = 1;
										}
										IOOutputStruct[i].SupplyingCarNum = 0;
										IOOutputStruct[i].SupplyingFlag = 1;				//标志车辆数正在补发中
									}
									else {
										IOOutputStruct[i].SupplyingFlag = 0;				//补发结束置0
										GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
										IOOutputStruct[i].IOLevel = 0;					//电平置0
										IOOutputStruct[i].IOHighUseTime = 0;				//高电平时间清0
									}
								}
								else if (IOOutputStruct[i].SupplyCarNum > (IOOutputNUM * 2)) {	//超过需补发车辆数
									IOOutputStruct[i].SupplyCarNum = (IOOutputNUM * 2);		//补发最大补发数
								}
							}
						}
						else {
							if (IOOutputStruct[i].SupplyUseTime == 0) {
								GPIO_SetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
								IOOutputStruct[i].IOLevel = 1;							//电平置1
								IOOutputStruct[i].IOHighUseTime = 0;						//高电平时间清0
							}
							else if (IOOutputStruct[i].SupplyUseTime == (param_recv.vi_output_high_time * 20)) {
								GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
								IOOutputStruct[i].IOLevel = 0;							//电平置0
								IOOutputStruct[i].IOHighUseTime = 0;						//高电平时间清0
							}
							
							IOOutputStruct[i].SupplyUseTime += 1;
							
							if (IOOutputStruct[i].SupplyUseTime >= ((param_recv.vi_output_high_time * 20) + (param_recv.output_low_min * 20) + 1)) {
								IOOutputStruct[i].SupplyUseTime = 0;
								IOOutputStruct[i].SupplyCarNum -= 1;						//补发车辆数减一
								if (IOOutputStruct[i].SupplyCarNum == 0) {					//补发车辆数完成
									IOOutputStruct[i].SupplyCarNum = 0;					//清空补发车辆数
									
									if (IOOutputStruct[i].SupplyingCarNum != 0) {			//补发中收到车辆数
										IOOutputStruct[i].SupplyCarNum = IOOutputStruct[i].SupplyingCarNum;		//获取需要补发的车辆数
										IOOutputStruct[i].CarInOrOut = IOOutputStruct[i].SupplyingCarInOrOut;
										if (IOOutputStruct[i].SupplyingCarNum % 2 == 1) {
											IOOutputStruct[i].CarEvenOrOdd = 0;
										}
										else {
											IOOutputStruct[i].CarEvenOrOdd = 1;
										}
										IOOutputStruct[i].SupplyingCarNum = 0;
										IOOutputStruct[i].SupplyingFlag = 1;				//标志车辆数正在补发中
									}
									else {
										IOOutputStruct[i].SupplyingFlag = 0;				//补发结束置0
										GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
										IOOutputStruct[i].IOLevel = 0;					//电平置0
										IOOutputStruct[i].IOHighUseTime = 0;				//高电平时间清0
									}
								}
								else if (IOOutputStruct[i].SupplyCarNum > (IOOutputNUM * 2)) {	//超过需补发车辆数
									IOOutputStruct[i].SupplyCarNum = (IOOutputNUM * 2);		//补发最大补发数
								}
							}
						}
					}
					else {												//偶数包
						if ((IOOutputStruct[i].SupplyCarNum % 2) == 1) {
							if (IOOutputStruct[i].SupplyUseTime == 0) {
								GPIO_SetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
								IOOutputStruct[i].IOLevel = 1;							//电平置1
								IOOutputStruct[i].IOHighUseTime = 0;						//高电平时间清0
							}
							else if (IOOutputStruct[i].SupplyUseTime == (param_recv.vo_output_high_time * 20)) {
								GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
								IOOutputStruct[i].IOLevel = 0;							//电平置0
								IOOutputStruct[i].IOHighUseTime = 0;						//高电平时间清0
							}
							
							IOOutputStruct[i].SupplyUseTime += 1;
							
							if (IOOutputStruct[i].SupplyUseTime >= ((param_recv.vo_output_high_time * 20) + (param_recv.output_low_min * 20) + 1)) {
								IOOutputStruct[i].SupplyUseTime = 0;
								IOOutputStruct[i].SupplyCarNum -= 1;						//补发车辆数减一
								if (IOOutputStruct[i].SupplyCarNum == 0) {					//补发车辆数完成
									IOOutputStruct[i].SupplyCarNum = 0;					//清空补发车辆数
									
									if (IOOutputStruct[i].SupplyingCarNum != 0) {			//补发中收到车辆数
										IOOutputStruct[i].SupplyCarNum = IOOutputStruct[i].SupplyingCarNum;		//获取需要补发的车辆数
										IOOutputStruct[i].CarInOrOut = IOOutputStruct[i].SupplyingCarInOrOut;
										if (IOOutputStruct[i].SupplyingCarNum % 2 == 1) {
											IOOutputStruct[i].CarEvenOrOdd = 0;
										}
										else {
											IOOutputStruct[i].CarEvenOrOdd = 1;
										}
										IOOutputStruct[i].SupplyingCarNum = 0;
										IOOutputStruct[i].SupplyingFlag = 1;				//标志车辆数正在补发中
									}
									else {
										IOOutputStruct[i].SupplyingFlag = 0;				//补发结束置0
										GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
										IOOutputStruct[i].IOLevel = 0;					//电平置0
										IOOutputStruct[i].IOHighUseTime = 0;				//高电平时间清0
									}
								}
								else if (IOOutputStruct[i].SupplyCarNum > (IOOutputNUM * 2)) {	//超过需补发车辆数
									IOOutputStruct[i].SupplyCarNum = (IOOutputNUM * 2);		//补发最大补发数
								}
							}
						}
						else {
							if (IOOutputStruct[i].SupplyUseTime == 0) {
								GPIO_SetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
								IOOutputStruct[i].IOLevel = 1;							//电平置1
								IOOutputStruct[i].IOHighUseTime = 0;						//高电平时间清0
							}
							else if (IOOutputStruct[i].SupplyUseTime == (param_recv.vi_output_high_time * 20)) {
								GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
								IOOutputStruct[i].IOLevel = 0;							//电平置0
								IOOutputStruct[i].IOHighUseTime = 0;						//高电平时间清0
							}
							
							IOOutputStruct[i].SupplyUseTime += 1;
							
							if (IOOutputStruct[i].SupplyUseTime >= ((param_recv.vi_output_high_time * 20) + (param_recv.output_low_min * 20) + 1)) {
								IOOutputStruct[i].SupplyUseTime = 0;
								IOOutputStruct[i].SupplyCarNum -= 1;						//补发车辆数减一
								if (IOOutputStruct[i].SupplyCarNum == 0) {					//补发车辆数完成
									IOOutputStruct[i].SupplyCarNum = 0;					//清空补发车辆数
									
									if (IOOutputStruct[i].SupplyingCarNum != 0) {			//补发中收到车辆数
										IOOutputStruct[i].SupplyCarNum = IOOutputStruct[i].SupplyingCarNum;		//获取需要补发的车辆数
										IOOutputStruct[i].CarInOrOut = IOOutputStruct[i].SupplyingCarInOrOut;
										if (IOOutputStruct[i].SupplyingCarNum % 2 == 1) {
											IOOutputStruct[i].CarEvenOrOdd = 0;
										}
										else {
											IOOutputStruct[i].CarEvenOrOdd = 1;
										}
										IOOutputStruct[i].SupplyingCarNum = 0;
										IOOutputStruct[i].SupplyingFlag = 1;				//标志车辆数正在补发中
									}
									else {
										IOOutputStruct[i].SupplyingFlag = 0;				//补发结束置0
										GPIO_ResetBits(OUTPUT_TYPE[i],  OUTPUT_PIN[i]);
										IOOutputStruct[i].IOLevel = 0;					//电平置0
										IOOutputStruct[i].IOHighUseTime = 0;				//高电平时间清0
									}
								}
								else if (IOOutputStruct[i].SupplyCarNum > (IOOutputNUM * 2)) {	//超过需补发车辆数
									IOOutputStruct[i].SupplyCarNum = (IOOutputNUM * 2);		//补发最大补发数
								}
							}
						}
					}
				}
			}
		}
	}
}


/**********************************************************************************************************
 @Function			void IOOutput_GetOutputID(u16 *outputid)
 @Description			读取output_ID输出端口号到IOOutput控制数据包
 @Input				outputid
 @Return				void
**********************************************************************************************************/
void IOOutput_GetOutputID(u16 *outputid)
{
	u8 i = 0;
	
	for (i = 0; i < IOOutputMAX; i++) {
		IOOutputStruct[i].OutputID = outputid[i];
		IOOutputStruct[i].CarNumState = 0;
	}
}

/********************************************** END OF FLEE **********************************************/
