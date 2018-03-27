#ifndef __CALCULATION_AVGOCCUPANCY_H
#define   __CALCULATION_AVGOCCUPANCY_H

#include "stm32f10x_lib.h"
#include "calculationconfig.h"


void CALCULATION_GetAvgOccupancy(u8 *buf);									//根据地磁发送的数据包计算平均占有时间
u16  CALCULATION_ReadAvgOccupancy(u16 outputid);								//获取该车道平均占有时间值
u32  CALCULATION_ReadAvgOccupancyExtend(u16 outputid);							//获取该车道平均占有时间值(扩增支持数据为4字节int)

#endif
