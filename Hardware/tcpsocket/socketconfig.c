/**
  *********************************************************************************************************
  * @file    socketconfig.c
  * @author  MoveBroad -- KangYJ
  * @version V1.0
  * @date    
  * @brief   例化各个外部函数接口
  *********************************************************************************************************
  * @attention
  *		
  *			
  *********************************************************************************************************
  */

#include "socketconfig.h"
#include "socketfunc.h"
#include "socketinstantia.h"
#include "socketinitialization.h"

/* socket外部接口数据读取例化各函数 */
struct _m_socket_dev socket_dev = 
{
	SOCKET_GetCrossID,													//获取CrossID路口代码
	SOCKET_GetPacketType,												//获取PacketType流水包类型
	SOCKET_GetPacketInfo,												//获取PacketInfo包信息 (发送时:数据包内的记录数)
	
	SOCKET_GetDeviceType,												//获取DeviceType设备类型值,地磁检测器,直接填值B(0x42)
	SOCKET_GetInterval,													//获取Interval统计时长
	SOCKET_GetLaneNo,													//获取LaneNo车道号
	SOCKET_GetDateTime,													//获取DateTime流水发生的时间
	SOCKET_GetVolume,													//获取Volume一分中内通过的车辆
	SOCKET_GetVolume1,													//获取Volume1微小车辆数量
	SOCKET_GetVolume2,													//获取Volume2小车数量
	SOCKET_GetVolume3,													//获取Volume3中车数量
	SOCKET_GetVolume4,													//获取Volume4大车数量
	SOCKET_GetVolume5,													//获取Volume5超大车数量
	SOCKET_GetAvgOccupancy,												//获取AvgOccupancy平均占有时间
	SOCKET_GetAvgHeadTime,												//获取AvgHeadTime平均车头时距
	SOCKET_GetAvgLength,												//获取AvgLength平均车长
	SOCKET_GetAvgSpeed,													//获取AvgSpeed平均速度
	SOCKET_GetSaturation,												//获取Saturation饱和度
	SOCKET_GetDensity,													//获取Density密度
	SOCKET_GetPcu,														//获取Pcu当量小汽车
	SOCKET_GetAvgQueueLength,											//获取AvgQueueLength排队长度
	
	SOCKET_FillData,													//将数据填入各个数据包
	SOCKET_ObtainPacketHead,												//读取Socket包头数据并填入缓存
	SOCKET_ObtainPacketData,												//读取Socket流量数据包并填入缓存
	SOCKET_ObtainPacketManuCheck,											//计算校验码并填入生产商编码和校验码
	
	SOCKET_GetOutputID,													//读取output_ID输出端口的参数到Socket流量数据包
	SOCKET_PackagedStorage,												//Socket将数据打包存入缓存区
	
	SOCKET_PortSerialInit,												//初始化Socket串口
	SOCKET_UARTx_IRQ,													//Socket协议串口中断处理函数
	SOCKET_Init,														//初始化Socket协议
	SOCKET_Implement,													//Socket协议处理
};

/********************************************** END OF FLEE **********************************************/
