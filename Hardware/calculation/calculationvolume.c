/**
  *********************************************************************************************************
  * @file    calculationvolume.c
  * @author  MoveBroad -- KangYJ
  * @version V1.0
  * @date    
  * @brief   车流量计算
  *********************************************************************************************************
  * @attention
  *			功能 : 
  *			1.  根据地磁发送的数据包计算车流量数								(外部调用)
  *			2.  获取该车道车流量值											(外部调用)
  *
  *********************************************************************************************************
  */

#include "calculationvolume.h"


/**********************************************************************************************************
 @Function			void CALCULATION_GetVolume(u8 *buf)
 @Description			根据地磁发送的数据包计算车流量数
 @Input				*buf				: 网关接收到地磁发送的数据包
 @Return				void
**********************************************************************************************************/
void CALCULATION_GetVolume(u8 *buf)
{
	u8 i = 0;
	u16 carnumval = 0;
	u16 addval = 0;
	RF_DataHeader_TypeDef *phead = (RF_DataHeader_TypeDef *)buf;
	
	//如果收到车辆离开车检包
	if ( (phead->type == DATATYPE_CAROUT_WITH_MAGVALUE) || (phead->type == DATATYPE_CAROUT_WITHOUT_MAGVALUE) )
	{
		carnumval = (((~0xffff) | buf[3]) << 8) | buf[4];						//获取地磁发送数据包中的车辆次数
		for (i = 0; i < OUTPUT_MAX; i++) {									//遍历ID
			if (CalculationDataPacket[i].OutputID == (phead->addr_dev)) {		//匹配同ID
				if (CalculationDataPacket[i].CarNumState == 0) {				//初次收到车辆次数
					CalculationDataPacket[i].CarNumState = carnumval;			//填入状态值
					CalculationDataPacket[i].CarNumProcess += 1;
				}
				else {
					if (carnumval > CalculationDataPacket[i].CarNumState) {
						addval = carnumval - CalculationDataPacket[i].CarNumState;			//增量计算
						CalculationDataPacket[i].CarNumProcess += addval;					//增量累加
						CalculationDataPacket[i].CarNumState = carnumval;					//状态赋值
					}
					else if (carnumval < CalculationDataPacket[i].CarNumState) {
						addval = (0xffff - CalculationDataPacket[i].CarNumState) + carnumval;	//增量计算
						CalculationDataPacket[i].CarNumProcess += addval;					//增量累加
						CalculationDataPacket[i].CarNumState = carnumval;					//状态赋值
					}
				}
			}
		}
	}
}

/**********************************************************************************************************
 @Function			u16 CALCULATION_ReadVolume(u16 outputid)
 @Description			获取该车道车流量值
 @Input				u16 outputid   : 		车道ID号
 @Return				u16 Volume	: 		该车道Volume值
**********************************************************************************************************/
u16 CALCULATION_ReadVolume(u16 outputid)
{
	u8 i = 0;
	u16 volumeval = 0;
	
	for (i = 0; i < OUTPUT_MAX; i++) {
		if (outputid == CalculationDataPacket[i].OutputID) {
			volumeval = CalculationDataPacket[i].CarNumProcess;
			CalculationDataPacket[i].CarNumProcess = 0;
		}
	}
	
	return volumeval;
}

/********************************************** END OF FLEE **********************************************/
