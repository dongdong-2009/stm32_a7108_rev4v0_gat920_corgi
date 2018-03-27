#ifndef __CALCULATION_AVGHEADTIME_H
#define   __CALCULATION_AVGHEADTIME_H

#include "stm32f10x_lib.h"
#include "calculationconfig.h"


void CALCULATION_GetAvgHeadTime(u8 *buf);									//根据地磁发送的数据包计算平均车头时距
u16  CALCULATION_ReadAvgHeadTime(u16 outputid);								//获取该车道平均车头时距
u32  CALCULATION_ReadAvgHeadTimeExtend(u16 outputid);							//获取该车道平均车头时距(扩增支持数据为4字节int)

#endif
