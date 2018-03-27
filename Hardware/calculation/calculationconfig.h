#ifndef __CALCULATION_CONFIG_H
#define   __CALCULATION_CONFIG_H

#include "stm32f10x_lib.h"
#include "platform_config.h"


typedef struct
{
	u16		OutputID;													//地磁ID号
	
	u16		CarNumState;												//地磁保存的车辆次数状态值
	u16		CarNumProcess;												//一段时间内该地磁车流量计算数
	
	u8		Speed_CarinState;											//车检入状态标志
	u8		Speed_CaroutState;											//车检出状态标志
	u16		Speed_CarinUseTime;											//车检入使用时间ms
	u16		Speed_CaroutUseTime;										//车检出使用时间ms
	u32		Speed_CarSumTime;											//一段时间内该车道车辆总使用时间
	u16		Speed_CarNum;												//一段时间内该车道车辆总数
	
	u8		Occupancy_CarState;											//车检状态标志位 1 : 入 0 : 出
	u32		Occupancy_CarUseTime;										//单次占有时间
	u16		Occupancy_CarNumState;										//标记同车(同车车检入,车检出CarNum值相同)
	u32		Occupancy_CarSumTime;										//一段时间内该车道车辆总占有时间
	u16		Occupancy_CarSumNum;										//一段时间内该车道车辆总数
	
	u8		Headtime_CarState;											//车头时距时间累计器标志位 1 : 开 0 : 关
	u32		Headtime_CarUseTime;										//单次间距时间
	u16		Headtime_CarNumState;										//经过车辆数保存值
	u32		Headtime_CarSumTime;										//一段时间内该车道车辆总间距时间
	u16		Headtime_CarSumNum;											//一段时间内该车道车辆总数
}Calculation_Data_Packet;												//计算数据包

/* calculation外部接口数据读取控制管理器 */
struct _m_calculation_dev
{
	void		(*ReadOutputID)(u16 *outputid);								//读取output_ID输出端口的参数到Calculation计算数据包
	
	void		(*GetVolume)(u8 *buf);										//根据地磁发送的数据包计算车流量数
	u16		(*ReadVolume)(u16 outputid);									//获取该车道车流量值
	
	void		(*GetAvgSpeed)(u8 *buf);										//根据地磁发送的数据包计算平均速度
	float	(*ReadAvgSpeed)(u16 outputid);								//获取该车道平均速度值
	
	void		(*GetAvgOccupancy)(u8 *buf);									//根据地磁发送的数据包计算平均占有时间
	u16		(*ReadAvgOccupancy)(u16 outputid);								//获取该车道平均占有时间值
	u32		(*ReadAvgOccupancyExtend)(u16 outputid);						//获取该车道平均占有时间值(扩增支持数据为4字节int)
	
	void		(*GetAvgHeadTime)(u8 *buf);									//根据地磁发送的数据包计算平均车头时距
	u16		(*ReadAvgHeadTime)(u16 outputid);								//获取该车道平均车头时距
	u32		(*ReadAvgHeadTimeExtend)(u16 outputid);							//获取该车道平均车头时距(扩增支持数据为4字节int)
};


extern Calculation_Data_Packet			CalculationDataPacket[OUTPUT_MAX];		//数据计算包
extern struct _m_calculation_dev 			calculation_dev;					//calculation外部接口数据读取例化各函数

void CALCULATION_GetOutputID(u16 *outputid);									//读取output_ID输出端口的参数到Calculation计算数据包


#endif
