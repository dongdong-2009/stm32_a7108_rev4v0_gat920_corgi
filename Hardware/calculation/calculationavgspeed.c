/**
  *********************************************************************************************************
  * @file    calculationavgspeed.c
  * @author  MoveBroad -- KangYJ
  * @version V1.0
  * @date    
  * @brief   平均速度计算
  *********************************************************************************************************
  * @attention
  *			功能 : 
  *			1.  根据地磁发送的数据包计算平均速度								(外部调用)
  *			2.  获取该车道平均速度值											(外部调用)
  *
  *********************************************************************************************************
  */

#include "calculationavgspeed.h"


/**********************************************************************************************************
 @Function			void CALCULATION_GetAvgSpeed(u8 *buf)
 @Description			根据地磁发送的数据包计算平均速度
 @Input				*buf				: 网关接收到地磁发送的数据包
 @Return				void
**********************************************************************************************************/
void CALCULATION_GetAvgSpeed(u8 *buf)
{
	u8 i = 0;
	RF_DataHeader_TypeDef *phead = (RF_DataHeader_TypeDef *)buf;
	
	//如果收到车辆进入车检包
	if ( (phead->type == DATATYPE_CARIN_WITH_MAGVALUE) || (phead->type == DATATYPE_CARIN_WITHOUT_MAGVALUE) ) {
		for (i = 0; i < OUTPUT_MAX; i++) {															//遍历ID
			if (CalculationDataPacket[i].OutputID == (phead->addr_dev)) {								//匹配同ID

				if (i < SPEEDLANNUMMAX) {														//A地磁车检入
					CalculationDataPacket[i].Speed_CarinState = 1;									//A地磁车检入标志位置1
					CalculationDataPacket[i].Speed_CarinUseTime = 0;									//A地磁清车检入使用时间累加器
				}
				else if ((i >= SPEEDLANNUMMAX) && (i < (2 * SPEEDLANNUMMAX))) {							//B地磁车检入
					CalculationDataPacket[i - SPEEDLANNUMMAX].Speed_CarinState = 0;						//A地磁车检入标志位置0
					//A地磁车检入使用时间
					if ( (CalculationDataPacket[i - SPEEDLANNUMMAX].Speed_CarinUseTime != 0) && 			//有使用时间
						(CalculationDataPacket[i - SPEEDLANNUMMAX].Speed_CarinUseTime < SPEEDOVERTIME) ) 	//使用时间小于阀值
					{
						CalculationDataPacket[i - SPEEDLANNUMMAX].Speed_CarSumTime += CalculationDataPacket[i - SPEEDLANNUMMAX].Speed_CarinUseTime;	//A地磁累加时间
						CalculationDataPacket[i].Speed_CarSumTime += CalculationDataPacket[i - SPEEDLANNUMMAX].Speed_CarinUseTime;				//B地磁累加时间
						CalculationDataPacket[i - SPEEDLANNUMMAX].Speed_CarNum += 1;													//A地磁累加次数
						CalculationDataPacket[i].Speed_CarNum += 1;																	//B地磁累加次数
					}
					CalculationDataPacket[i - SPEEDLANNUMMAX].Speed_CarinUseTime = 0;					//A地磁清车检入使用时间累加器
				}

			}
		}
	}
	//如果收到车辆离开车检包
	else if ( (phead->type == DATATYPE_CAROUT_WITH_MAGVALUE) || (phead->type == DATATYPE_CAROUT_WITHOUT_MAGVALUE) ) {
		for (i = 0; i < OUTPUT_MAX; i++) {															//遍历ID
			if (CalculationDataPacket[i].OutputID == (phead->addr_dev)) {								//匹配同ID

				if (i < SPEEDLANNUMMAX) {														//A地磁车检出
					CalculationDataPacket[i].Speed_CaroutState = 1;									//A地磁车检出标志位置1
					CalculationDataPacket[i].Speed_CaroutUseTime = 0;									//A地磁清车检出使用时间累加器
				}
				else if ((i >= SPEEDLANNUMMAX) && (i < (2 * SPEEDLANNUMMAX))) {							//B地磁车检出
					CalculationDataPacket[i - SPEEDLANNUMMAX].Speed_CaroutState = 0;						//A地磁车检出标志位置0
					//A地磁车检出使用时间
					if ( (CalculationDataPacket[i - SPEEDLANNUMMAX].Speed_CaroutUseTime != 0) && 			//有使用时间
						(CalculationDataPacket[i - SPEEDLANNUMMAX].Speed_CaroutUseTime < SPEEDOVERTIME) ) 	//使用时间小于阀值
					{
						CalculationDataPacket[i - SPEEDLANNUMMAX].Speed_CarSumTime += CalculationDataPacket[i - SPEEDLANNUMMAX].Speed_CaroutUseTime;	//A地磁累加时间
						CalculationDataPacket[i].Speed_CarSumTime += CalculationDataPacket[i - SPEEDLANNUMMAX].Speed_CaroutUseTime;				//B地磁累加时间
						CalculationDataPacket[i - SPEEDLANNUMMAX].Speed_CarNum += 1;													//A地磁累加次数
						CalculationDataPacket[i].Speed_CarNum += 1;																	//B地磁累加次数
					}
					CalculationDataPacket[i - SPEEDLANNUMMAX].Speed_CaroutUseTime = 0;					//A地磁清车检入使用时间累加器
				}

			}
		}
	}
}

/**********************************************************************************************************
 @Function			float CALCULATION_ReadAvgSpeed(u16 outputid)
 @Description			获取该车道平均速度值
 @Input				u16   outputid		: 		车道ID号
 @Return				float AvgSpeed		: 		该车道AvgSpeed值
**********************************************************************************************************/
float CALCULATION_ReadAvgSpeed(u16 outputid)
{
	u8 i = 0;
	float avgspeedval = 0;
	u32 speedcarsumtime = 0;
	u16 speedcarnum = 0;
	
	for (i = 0; i < OUTPUT_MAX; i++) {
		if (outputid == CalculationDataPacket[i].OutputID) {
			speedcarsumtime = CalculationDataPacket[i].Speed_CarSumTime;
			CalculationDataPacket[i].Speed_CarSumTime = 0;
			speedcarnum = CalculationDataPacket[i].Speed_CarNum;
			CalculationDataPacket[i].Speed_CarNum = 0;
		}
	}
	
	if (speedcarnum != 0) {
		avgspeedval = 1.0 * speedcarsumtime / speedcarnum;
		avgspeedval /= 1000;
		avgspeedval = SPEEDDISTANCEINTERVAL / avgspeedval;
		avgspeedval *= 3.6;
	}
	
	return avgspeedval;
}

/********************************************** END OF FLEE **********************************************/
