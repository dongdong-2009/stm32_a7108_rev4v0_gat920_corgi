/**
  *********************************************************************************************************
  * @file    calculationconfig.c
  * @author  MoveBroad -- KangYJ
  * @version V1.0
  * @date    
  * @brief   内部数据计算配置
  *********************************************************************************************************
  * @attention
  *			
  *
  *********************************************************************************************************
  */

#include "calculationconfig.h"
#include "calculationvolume.h"
#include "calculationavgspeed.h"
#include "calculationavgoccupancy.h"
#include "calculationavgheadtime.h"


Calculation_Data_Packet			CalculationDataPacket[OUTPUT_MAX];				//数据计算包


/* calculation外部接口数据读取例化各函数 */
struct _m_calculation_dev calculation_dev = 
{
	CALCULATION_GetOutputID,
	
	CALCULATION_GetVolume,
	CALCULATION_ReadVolume,
	
	CALCULATION_GetAvgSpeed,
	CALCULATION_ReadAvgSpeed,
	
	CALCULATION_GetAvgOccupancy,
	CALCULATION_ReadAvgOccupancy,
	CALCULATION_ReadAvgOccupancyExtend,
	
	CALCULATION_GetAvgHeadTime,
	CALCULATION_ReadAvgHeadTime,
	CALCULATION_ReadAvgHeadTimeExtend,
};

/**********************************************************************************************************
 @Function			void CALCULATION_GetOutputID(u16 *outputid)
 @Description			读取output_ID输出端口的参数到Calculation计算数据包
 @Input				Output_ID存放地址
 @Return				void
**********************************************************************************************************/
void CALCULATION_GetOutputID(u16 *outputid)
{
	u8 i = 0;
	
	for (i = 0; i < OUTPUT_MAX; i++) {
		CalculationDataPacket[i].OutputID = outputid[i];
	}
}

/********************************************** END OF FLEE **********************************************/
