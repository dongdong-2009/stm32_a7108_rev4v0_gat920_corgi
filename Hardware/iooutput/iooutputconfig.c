/**
  *********************************************************************************************************
  * @file    iooutputconfig.c
  * @author  MoveBroad -- KangYJ
  * @version V1.0
  * @date    
  * @brief   IO输出配置文件
  *********************************************************************************************************
  * @attention
  *		
  *			
  *********************************************************************************************************
  */

#include "iooutputconfig.h"
#include "iooutputfunc.h"


/* IOOutput外部接口控制例化各函数 */
struct _m_iooutput_dev iooutput_dev = 
{
	IOOutput_Mode0_Check,												//IO输出校验数据	(输出方式0 : 跟随车辆输出)
	IOOutput_Mode0_Supplying,											//IO输出定时器补发	(输出方式0 : 跟随车辆输出)
	IOOutput_Mode1_Check,												//IO输出校验数据	(输出方式1 : 车辆进入输出固定时长(记数))
	IOOutput_Mode1_Supplying,											//IO输出定时器补发	(输出方式1 : 车辆进入输出固定时长(记数))
	IOOutput_Mode2_Check,												//IO输出校验数据	(输出方式2 : 车辆离开输出固定时长(记数))
	IOOutput_Mode2_Supplying,											//IO输出定时器补发	(输出方式2 : 车辆离开输出固定时长(记数))
	IOOutput_Mode3_Check,												//IO输出校验数据	(输出方式3 : 车辆进入,离开时都输出固定时长(记数))
	IOOutput_Mode3_Supplying,											//IO输出定时器补发	(输出方式3 : 车辆进入,离开时都输出固定时长(记数))
	IOOutput_Mode4_Check,												//IO输出校验数据	(输出方式4 : 车辆进入输出固定时长(不记数))
	IOOutput_Mode4_Supplying,											//IO输出定时器补发	(输出方式4 : 车辆进入输出固定时长(不记数))
	IOOutput_Mode5_Check,												//IO输出校验数据	(输出方式5 : 车辆离开输出固定时长(不记数))
	IOOutput_Mode5_Supplying,											//IO输出定时器补发	(输出方式5 : 车辆离开输出固定时长(不记数))
	IOOutput_Mode6_Check,												//IO输出校验数据	(输出方式6 : 车辆进入,离开时都输出固定时长(不记数))
	IOOutput_Mode6_Supplying,											//IO输出定时器补发	(输出方式6 : 车辆进入,离开时都输出固定时长(不记数))
	
	IOOutput_GetOutputID,												//读取output_ID输出端口号到IOOutput控制数据包
};

/********************************************** END OF FLEE **********************************************/
