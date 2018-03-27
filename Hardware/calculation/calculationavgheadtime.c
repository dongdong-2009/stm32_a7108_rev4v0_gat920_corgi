/**
  *********************************************************************************************************
  * @file    calculationavgheadtime.c
  * @author  MoveBroad -- KangYJ
  * @version V1.0
  * @date    
  * @brief   平均车头时距计算
  *********************************************************************************************************
  * @attention
  *			功能 : 
  *			1.  根据地磁发送的数据包计算平均车头时距							(外部调用)
  *			2.  获取该车道平均车头时距										(外部调用)
  *
  *********************************************************************************************************
  */

#include "calculationavgheadtime.h"


/**********************************************************************************************************
 @Function			void CALCULATION_GetAvgHeadTime(u8 *buf)
 @Description			根据地磁发送的数据包计算平均车头时距(车检包触发)
 @Input				*buf				: 网关接收到地磁发送的数据包
 @Return				void
**********************************************************************************************************/
void CALCULATION_GetAvgHeadTime(u8 *buf)
{
	u8 i = 0;
	u16 carnumval = 0;
	u16 addval = 0;
	RF_DataHeader_TypeDef *phead = (RF_DataHeader_TypeDef *)buf;
	
	//如果收到车辆进入车检包
	if ( (phead->type == DATATYPE_CAROUT_WITH_MAGVALUE) || (phead->type == DATATYPE_CAROUT_WITHOUT_MAGVALUE) ) {
		carnumval = (((~0xffff) | buf[3]) << 8) | buf[4];													//获取地磁发送数据包中的车辆次数
		
		for (i = 0; i < OUTPUT_MAX; i++) {																//遍历ID
			if (CalculationDataPacket[i].OutputID == (phead->addr_dev)) {									//匹配同ID
				
				//车辆数计算累计
				if (CalculationDataPacket[i].Headtime_CarNumState == 0) {									//未收到过车检包
					CalculationDataPacket[i].Headtime_CarNumState = carnumval;								//记入CarNumState
					CalculationDataPacket[i].Headtime_CarSumNum += 1;
				}
				else {																			//已收到过车检包
					if (carnumval > CalculationDataPacket[i].Headtime_CarNumState) {
						addval = carnumval - CalculationDataPacket[i].Headtime_CarNumState;					//增量计算
						CalculationDataPacket[i].Headtime_CarSumNum += addval;								//增量累加
						CalculationDataPacket[i].Headtime_CarNumState = carnumval;							//状态赋值
					}
					else if (carnumval < CalculationDataPacket[i].Headtime_CarNumState) {
						addval = (0xffff - CalculationDataPacket[i].Headtime_CarNumState) + carnumval;			//增量计算
						CalculationDataPacket[i].Headtime_CarSumNum += addval;								//增量累加
						CalculationDataPacket[i].Headtime_CarNumState = carnumval;							//状态赋值
					}
				}
				
				//车头时距计算累计
				if (CalculationDataPacket[i].Headtime_CarState == 0) {										//计时器未开启
					CalculationDataPacket[i].Headtime_CarState = 1;										//开启计时器
					CalculationDataPacket[i].Headtime_CarUseTime = 0;										//清计时器数据
				}
				else {																			//计时器已开启
					if (CalculationDataPacket[i].Headtime_CarUseTime != 0) {								//累加时间
						if (addval > 0) {															//有车增加
							CalculationDataPacket[i].Headtime_CarSumTime += CalculationDataPacket[i].Headtime_CarUseTime;
						}
					}
					CalculationDataPacket[i].Headtime_CarUseTime = 0;										//清计时器数据
				}
				
			}
		}
	}
}

/**********************************************************************************************************
 @Function			u16 CALCULATION_ReadAvgHeadTime(u16 outputid)
 @Description			获取该车道平均车头时距(统计时间到触发)
 @Input				u16   outputid		: 		车道ID号
 @Return				u16   AvgHeadTime	: 		该车道AvgHeadTime值
**********************************************************************************************************/
u16 CALCULATION_ReadAvgHeadTime(u16 outputid)
{
	u8 i = 0;
	u16 avgheadtimeval = 0;
	u32 avgheadtimevaltmp = 0;
	u32 headtimecarsumtime = 0;
	u16 headtimecarsumnum = 0;
	
	for (i = 0; i < OUTPUT_MAX; i++) {										//获取统计时间内车辆数与车间距时间
		if (outputid == CalculationDataPacket[i].OutputID) {
			headtimecarsumnum = CalculationDataPacket[i].Headtime_CarSumNum;
			CalculationDataPacket[i].Headtime_CarSumNum = 0;
			headtimecarsumtime = CalculationDataPacket[i].Headtime_CarSumTime;
			CalculationDataPacket[i].Headtime_CarSumTime = 0;
		}
	}
	
	if (headtimecarsumnum != 0) {											//该时间段内有车
		avgheadtimevaltmp = headtimecarsumtime / headtimecarsumnum;
		if (avgheadtimevaltmp >= 65535) {
			avgheadtimeval = 65535;
		}
		else {
			avgheadtimeval = avgheadtimevaltmp;
		}
	}
	else {															//该时间段内无车
		avgheadtimeval = 0;
	}
	
	return avgheadtimeval;
}

/**********************************************************************************************************
 @Function			u32 CALCULATION_ReadAvgHeadTimeExtend(u16 outputid)
 @Description			获取该车道平均车头时距(扩增支持数据为4字节int)(统计时间到触发)
 @Input				u16   outputid		: 		车道ID号
 @Return				u16   AvgHeadTime	: 		该车道AvgHeadTime值
**********************************************************************************************************/
u32 CALCULATION_ReadAvgHeadTimeExtend(u16 outputid)
{
	u8 i = 0;
	u32 avgheadtimeval = 0;
	u32 headtimecarsumtime = 0;
	u32 avgheadtimevaltmp = 0;
	u16 headtimecarsumnum = 0;
	
	for (i = 0; i < OUTPUT_MAX; i++) {										//获取统计时间内车辆数与车间距时间
		if (outputid == CalculationDataPacket[i].OutputID) {
			headtimecarsumnum = CalculationDataPacket[i].Headtime_CarSumNum;
			CalculationDataPacket[i].Headtime_CarSumNum = 0;
			headtimecarsumtime = CalculationDataPacket[i].Headtime_CarSumTime;
			CalculationDataPacket[i].Headtime_CarSumTime = 0;
		}
	}
	
	if (headtimecarsumnum != 0) {											//该时间段内有车
		avgheadtimevaltmp = headtimecarsumtime / headtimecarsumnum;
		if (avgheadtimevaltmp >= 2147483647) {
			avgheadtimeval = 2147483647;
		}
		else {
			avgheadtimeval = avgheadtimevaltmp;
		}
	}
	else {															//该时间段内无车
		avgheadtimeval = 0;
	}
	
	return avgheadtimeval;
}

/********************************************** END OF FLEE **********************************************/
