#ifndef __IOOUTPUT_CONFIG_H
#define   __IOOUTPUT_CONFIG_H

#include "stm32f10x_lib.h"
#include "platform_config.h"

#define	IOOutputMAX	16												//最大IO输出口
#define	IOOutputNUM	100												//IO输出最大补发数

typedef struct
{
	u16		OutputID;													//输出端口ID号
	u16		CarNumState;												//地磁保存的车辆次数状态值
	u32		IOHighUseTime;												//IO高电平时间
	u8		IOLevel;													//IO电平值
	u8		CarInOrOut;												//车检入/车检出
	
	u8		CarEvenOrOdd;												//车辆数奇偶
	u8		SupplyingCarInOrOut;										//补发中车检入/车检出
	u8		SupplyingFlag;												//补发中标志位 0.补发结束 1.正在补发
	u8		SupplyedIOLevel;											//补发结束电平值
	u32		SupplyCarNum;												//补发车辆数
	u32		SupplyingCarNum;											//补发中收到车辆数
	u32		SupplyUseTime;												//补发电平时间
}IOOutputTypedef;														//IO输出控制结构体


/* IOOutput外部接口控制管理器 */
struct _m_iooutput_dev
{
	void		(*Mode0IOCheck)(u16, u16, u8);								//IO输出校验数据	(输出方式0 : 跟随车辆输出)
	void		(*Mode0Supplying)(void);										//IO输出定时器补发	(输出方式0 : 跟随车辆输出)
	void		(*Mode1IOCheck)(u16, u16, u8);								//IO输出校验数据	(输出方式1 : 车辆进入输出固定时长(记数))
	void		(*Mode1Supplying)(void);										//IO输出定时器补发	(输出方式1 : 车辆进入输出固定时长(记数))
	void		(*Mode2IOCheck)(u16, u16, u8);								//IO输出校验数据	(输出方式2 : 车辆离开输出固定时长(记数))
	void		(*Mode2Supplying)(void);										//IO输出定时器补发	(输出方式2 : 车辆离开输出固定时长(记数))
	void		(*Mode3IOCheck)(u16, u16, u8);								//IO输出校验数据	(输出方式3 : 车辆进入,离开时都输出固定时长(记数))
	void		(*Mode3Supplying)(void);										//IO输出定时器补发	(输出方式3 : 车辆进入,离开时都输出固定时长(记数))
	void		(*Mode4IOCheck)(u16, u16, u8);								//IO输出校验数据	(输出方式4 : 车辆进入输出固定时长(不记数))
	void		(*Mode4Supplying)(void);										//IO输出定时器补发	(输出方式4 : 车辆进入输出固定时长(不记数))
	void		(*Mode5IOCheck)(u16, u16, u8);								//IO输出校验数据	(输出方式5 : 车辆离开输出固定时长(不记数))
	void		(*Mode5Supplying)(void);										//IO输出定时器补发	(输出方式5 : 车辆离开输出固定时长(不记数))
	void		(*Mode6IOCheck)(u16, u16, u8);								//IO输出校验数据	(输出方式6 : 车辆进入,离开时都输出固定时长(不记数))
	void		(*Mode6Supplying)(void);										//IO输出定时器补发	(输出方式6 : 车辆进入,离开时都输出固定时长(不记数))
	
	void		(*ReadOutputID)(u16 *outputid);								//读取output_ID输出端口号到IOOutput控制数据包
};

extern struct _m_iooutput_dev iooutput_dev;

#endif
