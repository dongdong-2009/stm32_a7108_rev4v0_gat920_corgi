#ifndef __CALCULATION_AVGSPEED_H
#define   __CALCULATION_AVGSPEED_H

#include "stm32f10x_lib.h"
#include "calculationconfig.h"


#define		SPEEDDISTANCEINTERVAL		3								//两地磁间隔距离3M
#define		SPEEDOVERTIME				5000								//超时阀值5000ms
#define		SPEEDLANNUMMAX				16								//配置车道数最大16路车道


void  CALCULATION_GetAvgSpeed(u8 *buf);										//根据地磁发送的数据包计算平均速度
float CALCULATION_ReadAvgSpeed(u16 outputid);								//获取该车道平均速度值


#endif
