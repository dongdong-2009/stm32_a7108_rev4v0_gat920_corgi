#ifndef __SOCKET_TINSTANTIA_H
#define   __SOCKET_TINSTANTIA_H

#include "stm32f10x_lib.h"
#include "socketconfig.h"

#define	AVGLENGTHMIN		3											//平均车长最小值
#define	AVGLENGTHSINGLE	5											//单地磁测速默认车长


u32   SOCKET_GetCrossID(void);											//例化获取CrossID路口代码
u8    SOCKET_GetPacketType(void);											//例化获取PacketType流水包类型
u32   SOCKET_GetPacketInfo(Socket_Packet_Data *addr);							//例化获取PacketInfo包信息 (发送时:数据包内的记录数)
u8    SOCKET_GetDeviceType(u16 outputid);									//例化获取DeviceType设备类型值,地磁检测器,直接填值B(0x42)
u16   SOCKET_GetInterval(u16 outputid);										//例化获取Interval统计时长
u16   SOCKET_GetLaneNo(u16 outputid);										//例化获取LaneNo车道号
u32   SOCKET_GetDateTime(u16 outputid);										//例化获取DateTime流水发生的时间
u16   SOCKET_GetVolume(u16 outputid);										//例化获取Volume一分中内通过的车辆
u16   SOCKET_GetVolume1(u16 outputid);										//例化获取Volume1微小车辆数量
u16   SOCKET_GetVolume2(u16 outputid);										//例化获取Volume2小车数量
u16   SOCKET_GetVolume3(u16 outputid);										//例化获取Volume3中车数量
u16   SOCKET_GetVolume4(u16 outputid);										//例化获取Volume4大车数量
u16   SOCKET_GetVolume5(u16 outputid);										//例化获取Volume5超大车数量
u32 	 SOCKET_GetAvgOccupancy(u16 outputid);									//例化获取AvgOccupancy平均占有时间
u32   SOCKET_GetAvgHeadTime(u16 outputid);									//例化获取AvgHeadTime平均车头时距
float SOCKET_GetAvgLength(u16 outputid);									//例化获取AvgLength平均车长
float SOCKET_GetAvgSpeed(u16 outputid);										//例化获取AvgSpeed平均速度
u8    SOCKET_GetSaturation(u16 outputid);									//例化获取Saturation饱和度
u16   SOCKET_GetDensity(u16 outputid);										//例化获取Density密度
u16   SOCKET_GetPcu(u16 outputid);											//例化获取Pcu当量小汽车
float SOCKET_GetAvgQueueLength(u16 outputid);								//例化获取AvgQueueLength排队长度

void  SOCKET_FillData(void);												//将数据填入各个数据包


#endif
