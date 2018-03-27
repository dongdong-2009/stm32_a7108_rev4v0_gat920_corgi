#ifndef __IOOUTPUT_FUNC_H
#define   __IOOUTPUT_FUNC_H

#include "stm32f10x_lib.h"
#include "iooutputconfig.h"


void IOOutput_Mode0_Check(u16 dici_index, u16 dici_carnum, u8 dici_in);			//IO输出校验数据	(输出方式0 : 跟随车辆输出)
void IOOutput_Mode0_Supplying(void);										//IO输出定时器补发	(输出方式0 : 跟随车辆输出)
void IOOutput_Mode1_Check(u16 dici_index, u16 dici_carnum, u8 dici_in);			//IO输出校验数据	(输出方式1 : 车辆进入输出固定时长(记数))
void IOOutput_Mode1_Supplying(void);										//IO输出定时器补发	(输出方式1 : 车辆进入输出固定时长(记数))
void IOOutput_Mode2_Check(u16 dici_index, u16 dici_carnum, u8 dici_in);			//IO输出校验数据	(输出方式2 : 车辆离开输出固定时长(记数))
void IOOutput_Mode2_Supplying(void);										//IO输出定时器补发	(输出方式2 : 车辆离开输出固定时长(记数))
void IOOutput_Mode3_Check(u16 dici_index, u16 dici_carnum, u8 dici_in);			//IO输出校验数据	(输出方式3 : 车辆进入,离开时都输出固定时长(记数))
void IOOutput_Mode3_Supplying(void);										//IO输出定时器补发	(输出方式3 : 车辆进入,离开时都输出固定时长(记数))
void IOOutput_Mode4_Check(u16 dici_index, u16 dici_carnum, u8 dici_in);			//IO输出校验数据	(输出方式4 : 车辆进入输出固定时长(不记数))
void IOOutput_Mode4_Supplying(void);										//IO输出定时器补发	(输出方式4 : 车辆进入输出固定时长(不记数))
void IOOutput_Mode5_Check(u16 dici_index, u16 dici_carnum, u8 dici_in);			//IO输出校验数据	(输出方式5 : 车辆离开输出固定时长(不记数))
void IOOutput_Mode5_Supplying(void);										//IO输出定时器补发	(输出方式5 : 车辆离开输出固定时长(不记数))
void IOOutput_Mode6_Check(u16 dici_index, u16 dici_carnum, u8 dici_in);			//IO输出校验数据	(输出方式6 : 车辆进入,离开时都输出固定时长(不记数))
void IOOutput_Mode6_Supplying(void);										//IO输出定时器补发	(输出方式6 : 车辆进入,离开时都输出固定时长(不记数))

void IOOutput_GetOutputID(u16 *outputid);									//读取output_ID输出端口号到IOOutput控制数据包

#endif
