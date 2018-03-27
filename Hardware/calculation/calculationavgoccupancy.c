/**
  *********************************************************************************************************
  * @file    calculationavgoccupancy.c
  * @author  MoveBroad -- KangYJ
  * @version V1.0
  * @date    
  * @brief   平均占有时间计算
  *********************************************************************************************************
  * @attention
  *			功能 : 
  *			1.  根据地磁发送的数据包计算平均占有时间							(外部调用)
  *			2.  获取该车道平均占有时间值										(外部调用)
  *
  *********************************************************************************************************
  */

#include "calculationavgoccupancy.h"


/**********************************************************************************************************
 @Function			void CALCULATION_GetAvgOccupancy(u8 *buf)
 @Description			根据地磁发送的数据包计算平均占有时间
 @Input				*buf				: 网关接收到地磁发送的数据包
 @Return				void
**********************************************************************************************************/
void CALCULATION_GetAvgOccupancy(u8 *buf)
{
	u8 i = 0;
	u16 carnumval = 0;
	RF_DataHeader_TypeDef *phead = (RF_DataHeader_TypeDef *)buf;
	
	//如果收到车辆进入车检包
	if ( (phead->type == DATATYPE_CARIN_WITH_MAGVALUE) || (phead->type == DATATYPE_CARIN_WITHOUT_MAGVALUE) ) {
		carnumval = (((~0xffff) | buf[3]) << 8) | buf[4];									//获取地磁发送数据包中的车辆次数
		
		for (i = 0; i < OUTPUT_MAX; i++) {												//遍历ID
			if (CalculationDataPacket[i].OutputID == (phead->addr_dev)) {					//匹配同ID
				
				CalculationDataPacket[i].Occupancy_CarNumState = carnumval;					//计入车辆数作同车匹配
				CalculationDataPacket[i].Occupancy_CarState = 1;							//该地磁车检入标志位置1
				CalculationDataPacket[i].Occupancy_CarUseTime = 0;						//清车检入占有时间累加器
				
			}
		}
	}
	//如果收到车辆离开车检包
	else if ( (phead->type == DATATYPE_CAROUT_WITH_MAGVALUE) || (phead->type == DATATYPE_CAROUT_WITHOUT_MAGVALUE) ) {
		carnumval = (((~0xffff) | buf[3]) << 8) | buf[4];									//获取地磁发送数据包中的车辆次数
		
		for (i = 0; i < OUTPUT_MAX; i++) {												//遍历ID
			if (CalculationDataPacket[i].OutputID == (phead->addr_dev)) {					//匹配同ID
				
				CalculationDataPacket[i].Occupancy_CarState = 0;							//该地磁车检入标志位置0
				//该地磁此次占有时间
				if ( (CalculationDataPacket[i].Occupancy_CarUseTime != 0) &&				//有占有时间
					(carnumval == CalculationDataPacket[i].Occupancy_CarNumState) )			//同车匹配
				{
					CalculationDataPacket[i].Occupancy_CarSumTime += CalculationDataPacket[i].Occupancy_CarUseTime;	//该地磁占有时间累加
					CalculationDataPacket[i].Occupancy_CarSumNum += 1;										//该地磁占有次数累加
				}
				CalculationDataPacket[i].Occupancy_CarUseTime = 0;						//清车检入占有时间累加器
				
			}
		}
	}
}

/**********************************************************************************************************
 @Function			u16 CALCULATION_ReadAvgOccupancy(u16 outputid)
 @Description			获取该车道平均占有时间值
 @Input				u16   outputid		: 		车道ID号
 @Return				u16   AvgOccupancy	: 		该车道AvgOccupancy值
**********************************************************************************************************/
u16 CALCULATION_ReadAvgOccupancy(u16 outputid)
{
	u8 i = 0;
	u16 avgoccupancyval = 0;
	u32 avgoccupancyvaltmp = 0;
	u32 occupancycarsumtime = 0;
	u16 occupancycarsumnum = 0;
	
	for (i = 0; i < OUTPUT_MAX; i++) {
		if (outputid == CalculationDataPacket[i].OutputID) {
			
			occupancycarsumtime = CalculationDataPacket[i].Occupancy_CarSumTime;				//获取该时间段累加占有时间
			CalculationDataPacket[i].Occupancy_CarSumTime = 0;
			occupancycarsumnum = CalculationDataPacket[i].Occupancy_CarSumNum;				//获取该时间段累加占有次数
			CalculationDataPacket[i].Occupancy_CarSumNum = 0;
			
		}
	}
	
	if (occupancycarsumnum != 0) {
		avgoccupancyvaltmp = occupancycarsumtime / occupancycarsumnum;
		if (avgoccupancyvaltmp >= 65535) {
			avgoccupancyval = 65535;
		}
		else {
			avgoccupancyval = avgoccupancyvaltmp;
		}
	}
	
	return avgoccupancyval;
}

/**********************************************************************************************************
 @Function			u32 CALCULATION_ReadAvgOccupancyExtend(u16 outputid)
 @Description			获取该车道平均占有时间值(扩增支持数据为4字节int)
 @Input				u16   outputid		: 		车道ID号
 @Return				u16   AvgOccupancy	: 		该车道AvgOccupancy值
**********************************************************************************************************/
u32 CALCULATION_ReadAvgOccupancyExtend(u16 outputid)
{
	u8 i = 0;
	u32 avgoccupancyval = 0;
	u32 avgoccupancyvaltmp = 0;
	u32 occupancycarsumtime = 0;
	u16 occupancycarsumnum = 0;
	
	for (i = 0; i < OUTPUT_MAX; i++) {
		if (outputid == CalculationDataPacket[i].OutputID) {
			
			occupancycarsumtime = CalculationDataPacket[i].Occupancy_CarSumTime;				//获取该时间段累加占有时间
			CalculationDataPacket[i].Occupancy_CarSumTime = 0;
			occupancycarsumnum = CalculationDataPacket[i].Occupancy_CarSumNum;				//获取该时间段累加占有次数
			CalculationDataPacket[i].Occupancy_CarSumNum = 0;
			
		}
	}
	
	if (occupancycarsumnum != 0) {
		avgoccupancyvaltmp = occupancycarsumtime / occupancycarsumnum;
		if (avgoccupancyvaltmp >= 2147483647) {
			avgoccupancyval = 2147483647;
		}
		else {
			avgoccupancyval = avgoccupancyvaltmp;
		}
	}
	
	return avgoccupancyval;
}

/********************************************** END OF FLEE **********************************************/
