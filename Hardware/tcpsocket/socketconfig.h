#ifndef __SOCKET_CONFIG_H
#define   __SOCKET_CONFIG_H

#include "stm32f10x_lib.h"
#include "platform_config.h"


#define	SOCKET_CACHE_SIZE				2048						//2K协议缓存, 32路车道需1903Byte
#define	SOCKET_RECVIVE_SIZE				20						//接收缓存,数据包头13Byte

#define	SOCKET_AGAINSENDTIME			5						//3秒后没收到回包重发数据

#define	SOCKET_TRUE					0
#define	SOCKET_FALSE					1


//流水包类型
#define	PACKETTYPE_FLOWMESSAGE			0x06						//交通流量信息
#define	PACKETTYPE_ERRORMESSAGE			0x02						//故障信息
#define	PACKETTYPE_BUTTONEVENT			0x03						//行人按钮事件
#define	PACKETTYPE_BUSPRIORITYEVENT		0x04						//公交优先事件
#define	PACKETTYPE_PARKINGLOTDATA		0x05						//停车场数据
#define	PACKETTYPE_RTCCHECKINIT			0x80						//RTC上电对时
#define	PACKETTYPE_RTCCHECK				0x81						//RTC运行对时

#define	DEVICETYPE_DICI				0x42						//地磁设备

#define	MANUFACTURERCODE				0x01						//生产商编码


/**********************************************************************************************************
 @attention : 
 * Socket 包头   ( CrossID, PacketType, PacketInfo ) 														共 13 Byte
 * Socket 流量包 ( DeviceType, Interval, LaneNo, DateTime, Volume, Volume1, Volume2, Volume3, Volume4, Volume5, 
 *			    AvgOccupancy, AvgHeadTime, AvgLength, AvgSpeed, Saturation, Density, Pcu, AvgQueueLength ) 		共 63 Byte
 * Socket 校验   ( ManufacturerCode, CheckCode ) 															共 2 Byte
 *
**********************************************************************************************************/
typedef struct
{
	u32		CrossID;											//路口代码
	u8		PacketType;										//流水包类型
	u32		PacketInfo;										//包信息(发送时:数据包内的记录数,回馈时:02/00/00/00成功收取)
}Socket_Packet_Head;											//数据包头

typedef struct
{
	u16		OutputID;											//输出端口的参数
	u8		DeviceType;										//地磁检测器,直接填值B(0x42)
	u16		Interval;											//统计时长
	u16		LaneNo;											//车道号
	u32		DateTime;											//流水发生的时间
	u16		Volume;											//一分中内通过的车辆
	u16		Volume1;											//微小车辆数量
	u16		Volume2;											//小车数量
	u16		Volume3;											//中车数量
	u16		Volume4;											//大车数量
	u16		Volume5;											//超大车数量
	u32		AvgOccupancy;										//平均占有时间
	u32		AvgHeadTime;										//平均车头时距
	float	AvgLength;										//平均车长
	float	AvgSpeed;											//平均速度
	u8		Saturation;										//饱和度
	u16		Density;											//密度
	u16		Pcu;												//当量小汽车
	float	AvgQueueLength;									//排队长度
}Socket_Packet_Data;											//流量数据包


/* socket外部接口数据读取控制管理器 */
struct _m_socket_dev
{
	u32		(*GetCrossID)(void);								//获取CrossID路口代码
	u8		(*GetPacketType)(void);								//获取PacketType流水包类型
	u32		(*GetPacketInfo)(Socket_Packet_Data *addr);				//获取PacketInfo包信息 (发送时:数据包内的记录数)
	
	u8		(*GetDeviceType)(u16);								//获取DeviceType设备类型值,地磁检测器,直接填值B(0x42)
	u16		(*GetInterval)(u16);								//获取Interval统计时长
	u16		(*GetLaneNo)(u16);									//获取LaneNo车道号
	u32		(*GetDateTime)(u16);								//获取DateTime流水发生的时间
	u16		(*GetVolume)(u16);									//获取Volume一分中内通过的车辆
	u16		(*GetVolume1)(u16);									//获取Volume1微小车辆数量
	u16		(*GetVolume2)(u16);									//获取Volume2小车数量
	u16		(*GetVolume3)(u16);									//获取Volume3中车数量
	u16		(*GetVolume4)(u16);									//获取Volume4大车数量
	u16		(*GetVolume5)(u16);									//获取Volume5超大车数量
	u32		(*GetAvgOccupancy)(u16);								//获取AvgOccupancy平均占有时间
	u32		(*GetAvgHeadTime)(u16);								//获取AvgHeadTime平均车头时距
	float	(*GetAvgLength)(u16);								//获取AvgLength平均车长
	float	(*GetAvgSpeed)(u16);								//获取AvgSpeed平均速度
	u8		(*GetSaturation)(u16);								//获取Saturation饱和度
	u16		(*GetDensity)(u16);									//获取Density密度
	u16		(*GetPcu)(u16);									//获取Pcu当量小汽车
	float	(*GetAvgQueueLength)(u16);							//获取AvgQueueLength排队长度
	
	void		(*FillData)(void);									//将数据填入各个数据包
	u16		(*ObtainPacketHead)(u8 *addr);						//读取Socket包头数据并填入缓存
	u16		(*ObtainPacketData)(u8 *addr);						//读取Socket流量数据包并填入缓存
	u16		(*ObtainPacketManuCheck)(u8 *addr, u16);				//计算校验码并填入生产商编码和校验码
	
	void		(*ReadOutputID)(u16 *outputid);						//读取output_ID输出端口的参数到Socket流量数据包
	u16		(*PackagedStorage)(void);							//Socket将数据打包存入缓存区
	
	void		(*PortSerialInit)(USART_TypeDef* USARTx, u32 BaudRate);	//初始化Socket串口
	void		(*UARTx_IRQ)(USART_TypeDef* USARTx);					//Socket协议串口中断处理函数
	void		(*Init)(void);										//初始化Socket协议
	void		(*Implement)(u16 sendtime);							//Socket协议处理
};

extern struct _m_socket_dev socket_dev;
extern u32 Crossid;

#endif
