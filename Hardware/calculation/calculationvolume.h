#ifndef __CALCULATION_VOLUME_H
#define   __CALCULATION_VOLUME_H

#include "stm32f10x_lib.h"
#include "calculationconfig.h"

void CALCULATION_GetVolume(u8 *buf);										//根据地磁发送的数据包计算车流量数
u16  CALCULATION_ReadVolume(u16 outputid);									//获取该车道车流量值

#endif
