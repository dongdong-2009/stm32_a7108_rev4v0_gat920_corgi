/**
  *********************************************************************************************************
  * @file    socketinstantia.c
  * @author  MoveBroad -- KangYJ
  * @version V1.0
  * @date    
  * @brief   socket各个数据例化接口
  *********************************************************************************************************
  * @attention
  *			功能 : 
  *			1.  例化获取CrossID路口代码								(外部调用)
  *			2.  例化获取PacketType流水包类型							(外部调用)
  *			3.  例化获取PacketInfo包信息 (发送时:数据包内的记录数)			(外部调用)
  *			4.  例化获取DeviceType设备类型值,地磁检测器,直接填值B(0x42)		(外部调用)
  *			5.  例化获取Interval统计时长								(外部调用)
  *			6.  例化获取LaneNo车道号									(外部调用)
  *			7.  例化获取DateTime流水发生的时间							(外部调用)
  *			8.  例化获取Volume一分中内通过的车辆						(外部调用)
  *			9.  例化获取Volume1微小车辆数量							(外部调用)
  *			10. 例化获取Volume2小车数量								(外部调用)
  *			11. 例化获取Volume3中车数量								(外部调用)
  *			12. 例化获取Volume4大车数量								(外部调用)
  *			13. 例化获取Volume5超大车数量								(外部调用)
  *			14. 例化获取AvgOccupancy平均占有时间						(外部调用)
  *			15. 例化获取AvgHeadTime平均车头时距						(外部调用)
  *			16. 例化获取AvgLength平均车长								(外部调用)
  *			17. 例化获取AvgSpeed平均速度								(外部调用)
  *			18. 例化获取Saturation饱和度								(外部调用)
  *			19. 例化获取Density密度									(外部调用)
  *			20. 例化获取Pcu当量小汽车								(外部调用)
  *			21. 例化获取AvgQueueLength排队长度							(外部调用)
  *			
  *			22. 将数据填入各个数据包									(外部调用)
  *
  *********************************************************************************************************
  */

#include "socketinstantia.h"
#include "socketfunc.h"
#include "socketinitialization.h"
#include "calculationconfig.h"
#include "calculationavgspeed.h"


u32 Crossid;

/**********************************************************************************************************
 @Function			void SOCKET_FillData(void)
 @Description			将数据填入各个数据包
 @Input				void
 @Return				void
**********************************************************************************************************/
void SOCKET_FillData(void)
{
	u8 indexA = 0;
	u8 indexB = 0;
	
	SocketHeadPacket.CrossID = socket_dev.GetCrossID();													//写入CrossID
	SocketHeadPacket.PacketType = socket_dev.GetPacketType();												//写入PacketType
	SocketHeadPacket.PacketInfo = socket_dev.GetPacketInfo(SocketDataPacket);									//写入PacketInfo
	
	//以下获取数据顺序不可改变
	for (indexA = 0; indexA < OUTPUT_MAX; indexA++) {														//遍历找寻配置过的车道号
		if (SocketDataPacket[indexA].OutputID != 0) {
			if (indexA < SPEEDLANNUMMAX) {															//主地磁需获取数据
				for (indexB = indexA; indexB > 0; indexB--) {											//寻找本包与上一包同ID数据
					if (SocketDataPacket[indexA].OutputID == SocketDataPacket[indexB - 1].OutputID) {
						SocketDataPacket[indexA].DeviceType = SocketDataPacket[indexB - 1].DeviceType;						//写入DeviceType
						SocketDataPacket[indexA].Interval = SocketDataPacket[indexB - 1].Interval;							//写入Interval
						SocketDataPacket[indexA].LaneNo = indexA + 1;												//写入LaneNo
						SocketDataPacket[indexA].DateTime = SocketDataPacket[indexB - 1].DateTime;							//写入DateTime
						SocketDataPacket[indexA].Volume = SocketDataPacket[indexB - 1].Volume;							//写入Volume
						SocketDataPacket[indexA].Volume1 = SocketDataPacket[indexB - 1].Volume1;							//写入Volume1
						SocketDataPacket[indexA].Volume2 = SocketDataPacket[indexB - 1].Volume2;							//写入Volume2
						SocketDataPacket[indexA].Volume3 = SocketDataPacket[indexB - 1].Volume3;							//写入Volume3
						SocketDataPacket[indexA].Volume4 = SocketDataPacket[indexB - 1].Volume4;							//写入Volume4
						SocketDataPacket[indexA].Volume5 = SocketDataPacket[indexB - 1].Volume5;							//写入Volume5
						SocketDataPacket[indexA].AvgOccupancy = SocketDataPacket[indexB - 1].AvgOccupancy;					//写入AvgOccupancy
						SocketDataPacket[indexA].AvgHeadTime = SocketDataPacket[indexB - 1].AvgHeadTime;					//写入AvgHeadTime
						SocketDataPacket[indexA].AvgSpeed = SocketDataPacket[indexB - 1].AvgSpeed;							//写入AvgSpeed
						SocketDataPacket[indexA].AvgLength = SocketDataPacket[indexB - 1].AvgLength;						//写入AvgLength
						SocketDataPacket[indexA].Saturation = SocketDataPacket[indexB - 1].Saturation;						//写入Saturation
						SocketDataPacket[indexA].Density = SocketDataPacket[indexB - 1].Density;							//写入Density
						SocketDataPacket[indexA].Pcu = SocketDataPacket[indexB - 1].Pcu;									//写入Pcu
						SocketDataPacket[indexA].AvgQueueLength = SocketDataPacket[indexB - 1].AvgQueueLength;				//写入AvgQueueLength
						break;
					}
				}
				if (indexB == 0) {																	//本数据包无同ID数据包,获取数据
					SocketDataPacket[indexA].DeviceType = socket_dev.GetDeviceType(SocketDataPacket[indexA].OutputID);			//写入DeviceType
					SocketDataPacket[indexA].Interval = socket_dev.GetInterval(SocketDataPacket[indexA].OutputID);				//写入Interval
					SocketDataPacket[indexA].LaneNo = indexA + 1;													//写入LaneNo
					SocketDataPacket[indexA].DateTime = socket_dev.GetDateTime(SocketDataPacket[indexA].OutputID);				//写入DateTime
					SocketDataPacket[indexA].Volume = socket_dev.GetVolume(SocketDataPacket[indexA].OutputID);				//写入Volume
					SocketDataPacket[indexA].Volume1 = socket_dev.GetVolume1(SocketDataPacket[indexA].OutputID);				//写入Volume1
					SocketDataPacket[indexA].Volume2 = socket_dev.GetVolume2(SocketDataPacket[indexA].OutputID);				//写入Volume2
					SocketDataPacket[indexA].Volume3 = socket_dev.GetVolume3(SocketDataPacket[indexA].OutputID);				//写入Volume3
					SocketDataPacket[indexA].Volume4 = socket_dev.GetVolume4(SocketDataPacket[indexA].OutputID);				//写入Volume4
					SocketDataPacket[indexA].Volume5 = socket_dev.GetVolume5(SocketDataPacket[indexA].OutputID);				//写入Volume5
					SocketDataPacket[indexA].AvgOccupancy = socket_dev.GetAvgOccupancy(SocketDataPacket[indexA].OutputID);		//写入AvgOccupancy
					SocketDataPacket[indexA].AvgHeadTime = socket_dev.GetAvgHeadTime(SocketDataPacket[indexA].OutputID);		//写入AvgHeadTime
					SocketDataPacket[indexA].AvgSpeed = socket_dev.GetAvgSpeed(SocketDataPacket[indexA].OutputID);				//写入AvgSpeed
					SocketDataPacket[indexA].AvgLength = socket_dev.GetAvgLength(SocketDataPacket[indexA].OutputID);			//写入AvgLength
					SocketDataPacket[indexA].Saturation = socket_dev.GetSaturation(SocketDataPacket[indexA].OutputID);			//写入Saturation
					SocketDataPacket[indexA].Density = socket_dev.GetDensity(SocketDataPacket[indexA].OutputID);				//写入Density
					SocketDataPacket[indexA].Pcu = socket_dev.GetPcu(SocketDataPacket[indexA].OutputID);						//写入Pcu
					SocketDataPacket[indexA].AvgQueueLength = socket_dev.GetAvgQueueLength(SocketDataPacket[indexA].OutputID);	//写入AvgQueueLength
				}
			}
			else {																				//辅地磁获取数据
				if (SocketDataPacket[indexA - SPEEDLANNUMMAX].OutputID != 0) {								//辅地磁对应主地磁有配置
					SocketDataPacket[indexA].DeviceType = SocketDataPacket[indexA - SPEEDLANNUMMAX].DeviceType;				//写入DeviceType
					SocketDataPacket[indexA].Interval = SocketDataPacket[indexA - SPEEDLANNUMMAX].Interval;					//写入Interval
					SocketDataPacket[indexA].LaneNo = indexA + 1;													//写入LaneNo
					SocketDataPacket[indexA].DateTime = SocketDataPacket[indexA - SPEEDLANNUMMAX].DateTime;					//写入DateTime
					SocketDataPacket[indexA].Volume = SocketDataPacket[indexA - SPEEDLANNUMMAX].Volume;						//写入Volume
					SocketDataPacket[indexA].Volume1 = SocketDataPacket[indexA - SPEEDLANNUMMAX].Volume1;					//写入Volume1
					SocketDataPacket[indexA].Volume2 = SocketDataPacket[indexA - SPEEDLANNUMMAX].Volume2;					//写入Volume2
					SocketDataPacket[indexA].Volume3 = SocketDataPacket[indexA - SPEEDLANNUMMAX].Volume3;					//写入Volume3
					SocketDataPacket[indexA].Volume4 = SocketDataPacket[indexA - SPEEDLANNUMMAX].Volume4;					//写入Volume4
					SocketDataPacket[indexA].Volume5 = SocketDataPacket[indexA - SPEEDLANNUMMAX].Volume5;					//写入Volume5
					SocketDataPacket[indexA].AvgOccupancy = SocketDataPacket[indexA - SPEEDLANNUMMAX].AvgOccupancy;			//写入AvgOccupancy
					SocketDataPacket[indexA].AvgHeadTime = SocketDataPacket[indexA - SPEEDLANNUMMAX].AvgHeadTime;				//写入AvgHeadTime
					SocketDataPacket[indexA].AvgSpeed = SocketDataPacket[indexA - SPEEDLANNUMMAX].AvgSpeed;					//写入AvgSpeed
					SocketDataPacket[indexA].AvgLength = SocketDataPacket[indexA - SPEEDLANNUMMAX].AvgLength;					//写入AvgLength
					SocketDataPacket[indexA].Saturation = SocketDataPacket[indexA - SPEEDLANNUMMAX].Saturation;				//写入Saturation
					SocketDataPacket[indexA].Density = SocketDataPacket[indexA - SPEEDLANNUMMAX].Density;					//写入Density
					SocketDataPacket[indexA].Pcu = SocketDataPacket[indexA - SPEEDLANNUMMAX].Pcu;							//写入Pcu
					SocketDataPacket[indexA].AvgQueueLength = SocketDataPacket[indexA - SPEEDLANNUMMAX].AvgQueueLength;			//写入AvgQueueLength
				}
				else {																			//辅地磁对应主地磁无配置
					for (indexB = indexA; indexB > SPEEDLANNUMMAX; indexB--) {								//寻找本包与上一包同ID数据
						if (SocketDataPacket[indexA].OutputID == SocketDataPacket[indexB - 1].OutputID) {
							SocketDataPacket[indexA].DeviceType = SocketDataPacket[indexB - 1].DeviceType;						//写入DeviceType
							SocketDataPacket[indexA].Interval = SocketDataPacket[indexB - 1].Interval;							//写入Interval
							SocketDataPacket[indexA].LaneNo = indexA + 1;												//写入LaneNo
							SocketDataPacket[indexA].DateTime = SocketDataPacket[indexB - 1].DateTime;							//写入DateTime
							SocketDataPacket[indexA].Volume = SocketDataPacket[indexB - 1].Volume;							//写入Volume
							SocketDataPacket[indexA].Volume1 = SocketDataPacket[indexB - 1].Volume1;							//写入Volume1
							SocketDataPacket[indexA].Volume2 = SocketDataPacket[indexB - 1].Volume2;							//写入Volume2
							SocketDataPacket[indexA].Volume3 = SocketDataPacket[indexB - 1].Volume3;							//写入Volume3
							SocketDataPacket[indexA].Volume4 = SocketDataPacket[indexB - 1].Volume4;							//写入Volume4
							SocketDataPacket[indexA].Volume5 = SocketDataPacket[indexB - 1].Volume5;							//写入Volume5
							SocketDataPacket[indexA].AvgOccupancy = SocketDataPacket[indexB - 1].AvgOccupancy;					//写入AvgOccupancy
							SocketDataPacket[indexA].AvgHeadTime = SocketDataPacket[indexB - 1].AvgHeadTime;					//写入AvgHeadTime
							SocketDataPacket[indexA].AvgSpeed = SocketDataPacket[indexB - 1].AvgSpeed;							//写入AvgSpeed
							SocketDataPacket[indexA].AvgLength = SocketDataPacket[indexB - 1].AvgLength;						//写入AvgLength
							SocketDataPacket[indexA].Saturation = SocketDataPacket[indexB - 1].Saturation;						//写入Saturation
							SocketDataPacket[indexA].Density = SocketDataPacket[indexB - 1].Density;							//写入Density
							SocketDataPacket[indexA].Pcu = SocketDataPacket[indexB - 1].Pcu;									//写入Pcu
							SocketDataPacket[indexA].AvgQueueLength = SocketDataPacket[indexB - 1].AvgQueueLength;				//写入AvgQueueLength
							break;
						}
					}
					if (indexB == SPEEDLANNUMMAX) {													//本数据包无同ID数据包,获取数据
						SocketDataPacket[indexA].DeviceType = socket_dev.GetDeviceType(SocketDataPacket[indexA].OutputID);			//写入DeviceType
						SocketDataPacket[indexA].Interval = socket_dev.GetInterval(SocketDataPacket[indexA].OutputID);				//写入Interval
						SocketDataPacket[indexA].LaneNo = indexA + 1;													//写入LaneNo
						SocketDataPacket[indexA].DateTime = socket_dev.GetDateTime(SocketDataPacket[indexA].OutputID);				//写入DateTime
						SocketDataPacket[indexA].Volume = socket_dev.GetVolume(SocketDataPacket[indexA].OutputID);				//写入Volume
						SocketDataPacket[indexA].Volume1 = socket_dev.GetVolume1(SocketDataPacket[indexA].OutputID);				//写入Volume1
						SocketDataPacket[indexA].Volume2 = socket_dev.GetVolume2(SocketDataPacket[indexA].OutputID);				//写入Volume2
						SocketDataPacket[indexA].Volume3 = socket_dev.GetVolume3(SocketDataPacket[indexA].OutputID);				//写入Volume3
						SocketDataPacket[indexA].Volume4 = socket_dev.GetVolume4(SocketDataPacket[indexA].OutputID);				//写入Volume4
						SocketDataPacket[indexA].Volume5 = socket_dev.GetVolume5(SocketDataPacket[indexA].OutputID);				//写入Volume5
						SocketDataPacket[indexA].AvgOccupancy = socket_dev.GetAvgOccupancy(SocketDataPacket[indexA].OutputID);		//写入AvgOccupancy
						SocketDataPacket[indexA].AvgHeadTime = socket_dev.GetAvgHeadTime(SocketDataPacket[indexA].OutputID);		//写入AvgHeadTime
						SocketDataPacket[indexA].AvgSpeed = socket_dev.GetAvgSpeed(SocketDataPacket[indexA].OutputID);				//写入AvgSpeed
						SocketDataPacket[indexA].AvgLength = socket_dev.GetAvgLength(SocketDataPacket[indexA].OutputID);			//写入AvgLength
						SocketDataPacket[indexA].Saturation = socket_dev.GetSaturation(SocketDataPacket[indexA].OutputID);			//写入Saturation
						SocketDataPacket[indexA].Density = socket_dev.GetDensity(SocketDataPacket[indexA].OutputID);				//写入Density
						SocketDataPacket[indexA].Pcu = socket_dev.GetPcu(SocketDataPacket[indexA].OutputID);						//写入Pcu
						SocketDataPacket[indexA].AvgQueueLength = socket_dev.GetAvgQueueLength(SocketDataPacket[indexA].OutputID);	//写入AvgQueueLength
					}
				}
			}
		}
	}
}

/**********************************************************************************************************
 @Function			u32 SOCKET_FillCrossID(void)
 @Description			例化获取CrossID路口代码
 @Input				
 @Return				u32 CrossID 值
**********************************************************************************************************/
u32 SOCKET_GetCrossID(void)
{
	if (Crossid > 99999999) {
		Crossid = 99999999;
	}
	
	return Crossid;
}

/**********************************************************************************************************
 @Function			u32 SOCKET_GatPacketType(void)
 @Description			例化获取PacketType流水包类型
 @Input				
 @Return				u8 PacketType 值
**********************************************************************************************************/
u8 SOCKET_GetPacketType(void)
{
	if (SOCKET_RTC_CHECK == PACKETTYPE_RTCCHECKINIT) {						//判断是否需要RTC上电对时校验
		if (PlatformSockettime == SocketTime_DISABLE) {						//根据SN选择是否对时
			return PACKETTYPE_FLOWMESSAGE;								//发送普通交通流量信息
		}
		else {
			return PACKETTYPE_RTCCHECKINIT;								//发送RTC上电对时校验命令
		}
	}
	else if (SOCKET_RTC_CHECK == PACKETTYPE_RTCCHECK) {						//判断是否需要RTC运行对时校验
		if (PlatformSockettime == SocketTime_DISABLE) {						//根据SN选择是否对时
			return PACKETTYPE_FLOWMESSAGE;								//发送普通交通流量信息
		}
		else {
			return PACKETTYPE_RTCCHECK;									//发送RTC运行对时校验命令
		}
	}
	else {
		return PACKETTYPE_FLOWMESSAGE;									//发送普通交通流量信息
	}
}

/**********************************************************************************************************
 @Function			u32 SOCKET_GatPacketInfo(Socket_Packet_Data *addr)
 @Description			例化获取PacketInfo包信息 (发送时:数据包内的记录数)
 @Input				Socket_Packet_Data *addr 	: Socket流量数据包集地址
 @Return				u32 PacketInfo 值
**********************************************************************************************************/
u32 SOCKET_GetPacketInfo(Socket_Packet_Data *addr)
{
	u8 i = 0;
	u32 outputnum = 0;
	
	for (i = 0; i < OUTPUT_MAX; i++) {
		if (addr[i].OutputID != 0) {
			outputnum += 1;
		}
	}
	
	return outputnum;
}

/**********************************************************************************************************
 @Function			u8 SOCKET_GatDeviceType(u16 outputid)
 @Description			例化获取DeviceType设备类型值,地磁检测器,直接填值B(0x42)
 @Input				u16 outputid   : 		车道ID号
 @Return				u8  DeviceType : 		该车道DeviceType值
**********************************************************************************************************/
u8 SOCKET_GetDeviceType(u16 outputid)
{
	return DEVICETYPE_DICI;
}

/**********************************************************************************************************
 @Function			u16 SOCKET_GatInterval(u16 outputid)
 @Description			例化获取Interval统计时长
 @Input				u16 outputid   : 		车道ID号
 @Return				u16 Interval	: 		该车道Interval值
**********************************************************************************************************/
u16 SOCKET_GetInterval(u16 outputid)
{
	return INTERVALTIME;
}

/**********************************************************************************************************
 @Function			u16 SOCKET_GatInterval(u16 outputid)
 @Description			例化获取LaneNo车道号
 @Input				u16 outputid   : 		车道ID号
 @Return				u16 LaneNo	: 		该车道LaneNo值
**********************************************************************************************************/
u16 SOCKET_GetLaneNo(u16 outputid)
{
	u8 i = 0;
	u16 laneno = 1;
	
	for (i = 0; i < OUTPUT_MAX; i++) {
		if (SocketDataPacket[i].OutputID == outputid) {
			laneno = i + 1;
		}
	}
	
	return laneno;
}

/**********************************************************************************************************
 @Function			u32 SOCKET_GatDateTime(u16 outputid)
 @Description			例化获取DateTime流水发生的时间
 @Input				u16 outputid   : 		车道ID号
 @Return				u32 DateTime	: 		该车道DateTime值
**********************************************************************************************************/
u32 SOCKET_GetDateTime(u16 outputid)
{
	u32 datetime = 0;
	
	datetime = RTC_GetCounter();
	
	return datetime;
}

/**********************************************************************************************************
 @Function			u16 SOCKET_GatVolume(u16 outputid)
 @Description			例化获取Volume一分中内通过的车辆
 @Input				u16 outputid   : 		车道ID号
 @Return				u16 Volume	: 		该车道Volume值
**********************************************************************************************************/
u16 SOCKET_GetVolume(u16 outputid)
{
	u16 volumeval = 0;
	
	volumeval = calculation_dev.ReadVolume(outputid);
	
	return volumeval;
}

/**********************************************************************************************************
 @Function			u16 SOCKET_GatVolume1(u16 outputid)
 @Description			例化获取Volume1微小车辆数量
 @Input				u16 outputid   : 		车道ID号
 @Return				u16 Volume1	: 		该车道Volume1值
**********************************************************************************************************/
u16 SOCKET_GetVolume1(u16 outputid)
{
	return 0;
}

/**********************************************************************************************************
 @Function			u16 SOCKET_GatVolume2(u16 outputid)
 @Description			例化获取Volume2小车数量
 @Input				u16 outputid   : 		车道ID号
 @Return				u16 Volume2	: 		该车道Volume2值
**********************************************************************************************************/
u16 SOCKET_GetVolume2(u16 outputid)
{
	return 0;
}

/**********************************************************************************************************
 @Function			u16 SOCKET_GatVolume3(u16 outputid)
 @Description			例化获取Volume3中车数量
 @Input				u16 outputid   : 		车道ID号
 @Return				u16 Volume3	: 		该车道Volume3值
**********************************************************************************************************/
u16 SOCKET_GetVolume3(u16 outputid)
{
	return 0;
}

/**********************************************************************************************************
 @Function			u16 SOCKET_GatVolume4(u16 outputid)
 @Description			例化获取Volume4大车数量
 @Input				u16 outputid   : 		车道ID号
 @Return				u16 Volume4	: 		该车道Volume4值
**********************************************************************************************************/
u16 SOCKET_GetVolume4(u16 outputid)
{
	return 0;
}

/**********************************************************************************************************
 @Function			u16 SOCKET_GatVolume5(u16 outputid)
 @Description			例化获取Volume5超大车数量
 @Input				u16 outputid   : 		车道ID号
 @Return				u16 Volume5	: 		该车道Volume5值
**********************************************************************************************************/
u16 SOCKET_GetVolume5(u16 outputid)
{
	return 0;
}

/**********************************************************************************************************
 @Function			u32 SOCKET_GetAvgOccupancy(u16 outputid)
 @Description			例化获取AvgOccupancy平均占有时间
 @Input				u16 outputid   	: 		车道ID号
 @Return				u32 AvgOccupancy	: 		该车道AvgOccupancy值
**********************************************************************************************************/
u32 SOCKET_GetAvgOccupancy(u16 outputid)
{
	u32 avgoccupancyval = 0;
	
	avgoccupancyval = calculation_dev.ReadAvgOccupancyExtend(outputid);
	
	return avgoccupancyval;
}

/**********************************************************************************************************
 @Function			u32 SOCKET_GetAvgHeadTime(u16 outputid)
 @Description			例化获取AvgHeadTime平均车头时距
 @Input				u16 outputid   	: 		车道ID号
 @Return				u32 AvgHeadTime	: 		该车道AvgHeadTime值
**********************************************************************************************************/
u32 SOCKET_GetAvgHeadTime(u16 outputid)
{
	u32 avgheadtimeval = 0;
	
	avgheadtimeval = calculation_dev.ReadAvgHeadTimeExtend(outputid);
	
	return avgheadtimeval;
}

/**********************************************************************************************************
 @Function			float SOCKET_GatAvgLength(u16 outputid)
 @Description			例化获取AvgLength平均车长
					如为单地磁测速模式 车长由宏定义配置,不可改变
					如为双地磁测速模式 车长由速度与占有时间决定
 @Input				u16 outputid   	: 		车道ID号
 @Return				u16 AvgLength		: 		该车道AvgLength值
**********************************************************************************************************/
float SOCKET_GetAvgLength(u16 outputid)
{
	u8 i = 0;
	u8 outputnum = 0;
	float avglengthval = 0;
	float avgspeedval = 0;
	u16 avgoccupancyval = 0;
	
	for (i = 0; i < OUTPUT_MAX; i++) {										//获取已配置ID号最大值
		if (SocketDataPacket[i].OutputID != 0) {
			outputnum = i;
		}
	}
	
	if (outputnum >= SPEEDLANNUMMAX) 										//判断单地磁测速还是双地磁测速
	{
		for (i = 0; i < OUTPUT_MAX; i++) {
			if (SocketDataPacket[i].OutputID == outputid) {
				if (i < SPEEDLANNUMMAX) {
					avgspeedval = SocketDataPacket[i].AvgSpeed;
					avgoccupancyval = SocketDataPacket[i].AvgOccupancy;
				}
				else {
					avgspeedval = SocketDataPacket[i - SPEEDLANNUMMAX].AvgSpeed;
					avgoccupancyval = SocketDataPacket[i - SPEEDLANNUMMAX].AvgOccupancy;
				}
			}
		}
		
		avgspeedval /= 3.6;
		avglengthval = avgspeedval * avgoccupancyval;
		avglengthval /= 1000;
		
		if ((avglengthval < AVGLENGTHMIN) && (avglengthval > 0.1)) {
			avglengthval = AVGLENGTHMIN;
		}
	}
	else {
		avglengthval = AVGLENGTHSINGLE;
	}
	
	return avglengthval;
}

/**********************************************************************************************************
 @Function			float SOCKET_GatAvgSpeed(u16 outputid)
 @Description			例化获取AvgSpeed平均速度
					如为单地磁测速,测速值由默认车长与占有时间决定
					如为双地磁测速,测速值由双地磁数据获取
 @Input				u16 outputid   	: 		车道ID号
 @Return				u16 AvgSpeed		: 		该车道AvgSpeed值
**********************************************************************************************************/
float SOCKET_GetAvgSpeed(u16 outputid)
{
	u8 i = 0;
	u8 outputnum = 0;
	u32 avgoccupancyval = 0;
	float avgspeedval = 0;
	
	for (i = 0; i < OUTPUT_MAX; i++) {										//获取已配置ID号最大值
		if (SocketDataPacket[i].OutputID != 0) {
			outputnum = i;
		}
	}
	
	if (outputnum >= SPEEDLANNUMMAX) 										//判断单地磁测速还是双地磁测速
	{
		avgspeedval = calculation_dev.ReadAvgSpeed(outputid);
	}
	else {
		for (i = 0; i < OUTPUT_MAX; i++) {
			if (SocketDataPacket[i].OutputID == outputid) {
				avgoccupancyval = SocketDataPacket[i].AvgOccupancy;
			}
		}
		if (avgoccupancyval != 0) {
			avgspeedval = 3600.0 * AVGLENGTHSINGLE / avgoccupancyval;
		}
	}
	
	return avgspeedval;
}

/**********************************************************************************************************
 @Function			u8 SOCKET_GatSaturation(u16 outputid)
 @Description			例化获取Saturation饱和度
 @Input				u16 outputid   	: 		车道ID号
 @Return				u16 Saturation		: 		该车道Saturation值
**********************************************************************************************************/
u8 SOCKET_GetSaturation(u16 outputid)
{
	return 0;
}

/**********************************************************************************************************
 @Function			u16 SOCKET_GatDensity(u16 outputid)
 @Description			例化获取Density密度
 @Input				u16 outputid   	: 		车道ID号
 @Return				u16 Density		: 		该车道Density值
**********************************************************************************************************/
u16 SOCKET_GetDensity(u16 outputid)
{
	return 0;
}

/**********************************************************************************************************
 @Function			u16 SOCKET_GatPcu(u16 outputid)
 @Description			例化获取Pcu当量小汽车
 @Input				u16 outputid   	: 		车道ID号
 @Return				u16 Pcu			: 		该车道Pcu值
**********************************************************************************************************/
u16 SOCKET_GetPcu(u16 outputid)
{
	return 0;
}

/**********************************************************************************************************
 @Function			float SOCKET_GatAvgQueueLength(u16 outputid)
 @Description			例化获取AvgQueueLength排队长度
 @Input				u16 outputid   			: 		车道ID号
 @Return				u16 AvgQueueLength			: 		该车道AvgQueueLength值
**********************************************************************************************************/
float SOCKET_GetAvgQueueLength(u16 outputid)
{
	return 0;
}

/********************************************** END OF FLEE **********************************************/
