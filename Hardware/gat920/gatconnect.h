#ifndef __GAT_CONNECT_H
#define   __GAT_CONNECT_H

#include "stm32f10x_lib.h"
#include "platform_config.h"
#include "gatconfig.h"


/* Ack Frame Data */
typedef __packed struct
{
	u8  protocolnum;													//协议版本
	u8  operationtype;													//操作类型
	u8  objectid;														//对象标识
}Gat_AckFrame_Data;

/* 出错应答表 */
typedef __packed struct
{
	Gat_AckFrame_Data AckHead;											//应答表头
	u8  errortype;														//错误类型
}Gat_Error_Data;

/* 时间查询应答表 */
typedef __packed struct
{
	Gat_AckFrame_Data AckHead;											//应答表头
	u32 datetime;														//本地时间值
}Gat_Time_Data;

/* 波特率设置应答表 */
typedef __packed struct
{
	Gat_AckFrame_Data AckHead;											//应答表头
	u8 setbaudresult;													//波特率设置成功标志
}Gat_BaudRate_Data;

/* 检测器参数应答表 */
typedef __packed struct
{
	Gat_AckFrame_Data AckHead;											//应答表头
	Gat_Param_Detector ParamDetector;										//检测器参数
}Gat_ConfigParam_Data;

/* 脉冲数据上传配置参数应答表 */
typedef __packed struct
{
	Gat_AckFrame_Data AckHead;											//应答表头
	Gat_PulseUploadConfig_Data ParamPulseUpload;								//脉冲数据上传配置参数
}Gat_PulseUploadParam_Data;

/* 脉冲数据主动上传表 */
typedef __packed struct
{
	Gat_AckFrame_Data AckHead;											//应答表头
	Gat_PulseUpload_Data ParamPulseMessage;									//脉冲数据信号源
}Gat_ActivePulseUpload_Data;

/* 统计数据主动上传表 */
typedef __packed struct
{
	Gat_AckFrame_Data AckHead;											//应答表头
	Gat_Statistical_Data ParamStatisticalMessage;							//统计数据
}Gat_ActiveStatisticalUpload_Data;


u8   GAT_SelectExecuteCmd(u8 *data_sheet, u16 data_length);						//判断处理不同命令

void GAT_ErrorDataWrite(u8 errormessage);									//写入出错应答消息到发送缓存区
u8   GAT_AckSetOnlineWrite(void);											//写入连接请求应答表到发送缓存区并设定连接状态
u8   GAT_AckQueryOnlineWrite(void);										//检测连接状态并写入连接查询应答表到发送缓存区
u8   GAT_AckSetTimeWrite(u8 *msgdata, u16 msglength);							//检测连接状态并提取时间设置表时间并写入时间设置应答表到发送缓存区
u8   GAT_AckQueryTimeWrite(void);											//检测连接状态并获取时间写入时间查询应答表到发送缓存区
u8   GAT_AckSetBaudRateWrite(u8 *msgdata, u16 msglength);						//检测连接状态并获取波特率设置表波特率设置波特率并写入设置设置应答表到发送缓存区
u8   GAT_AckSetConfigParamWrite(u8 *msgdata, u16 msglength);					//检测连接状态并提取配置参数设置表数据并写入配置参数设置应答表到发送缓存区
u8   GAT_AckQueryConfigParamWrite(void);									//检测连接状态并将检测器参数写入检测器参数查询应答表到发送缓存区
u8   GAT_AckSetPulseUploadConfigWrite(u8 *msgdata, u16 msglength);				//检测连接状态并提取脉冲数据配置参数设置表数据并写入脉冲数据上传配置参数设置应答表到发送缓存区
u8   GAT_AckQueryPulseUploadConfigWrite(void);								//检测连接状态并将脉冲数据上传配置参数写入脉冲数据上传配置参数查询应答表到发送缓存区

u8   GAT_ActiveUpLoadDataWrite(void);										//检测连接状态并将脉冲数据写入脉冲数据主动上传表到发送缓存区
u8   GAT_AckInspectUpLoadData(void);										//检测连接状态并根据脉冲数据主动上传应答表做处理

u8   GAT_ActiveStatisticalDataWrite(void);									//检测连接状态并将统计数据写入统计数据主动上传表到发送缓存区
u8   GAT_AckInspectStatisticalData(void);									//检测连接状态并根据统计数据主动上传应答表做处理

#endif
