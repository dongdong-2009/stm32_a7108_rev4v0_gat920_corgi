#ifndef __GAT_CONFIG_H
#define   __GAT_CONFIG_H

#include "stm32f10x_lib.h"
#include "platform_config.h"


/* GAT状态机状态值 */
typedef enum
{
	GAT_EVENT_READY,													//Ready
	GAT_EVENT_FRAME_RECEIVED,											//Received Frame
	GAT_EVENT_FRAME_SENT,												//Sent Frame
	GAT_EVENT_EXECUTE,													//Execute function
	GAT_EVENT_INITIALIZED,												//Initialized
	GAT_EVENT_ACTIVEUPLOAD												//ACTIVEUPLOAD
}GATEventType;
extern GATEventType	GATEvent;

/* GAT连接状态 */
typedef enum
{
	GAT_ONLINE,														//连接中
	GAT_OFFLINE														//未连接
}GATConnectType;
extern GATConnectType GATConnect;

/* GAT上传数据接收应答状态 */
typedef enum
{
	GAT_NotReceivedAck,													//未收到主动上传应答
	GAT_ReceivedAck													//收到主动上传应答
}GATUPLOADAckType;
extern GATUPLOADAckType GATUPLOADAck;

/* GAT主动上传模式 */
typedef enum
{
	GAT_ActiveUpLoadMode_NONE,											//无需主动上传
	GAT_ActiveUpLoadMode_STATISTICS,										//统计数据主动上传
	GAT_ActiveUpLoadMode_PULSE,											//脉冲数据主动上传
	GAT_ActiveUpLoadMode_FAULT											//检测器故障主动上传
}GATActiveUpLoadModeType;
extern GATActiveUpLoadModeType GATActiveUpLoadMode;

#define	GAT920_BAUDRATE						9600						//默认波特率

#define	GAT_TRUE								0
#define	GAT_FALSE								1
#define	GAT_DATAFALSE							2
#define	GAT_RECEIVEEVENT						3

#define	GAT_OUTDETECTIONAREA					0						//车辆离开检测区
#define	GAT_INDETECTIONAREA						1						//车辆进入检测区

#define	GAT_CACHE_SIZE							1024						//协议缓存大小 MAX 32768

#define	EquipmentManufacturers					"MoveBroad"				//设备制造商
#define	EquipmentModel							"wvd"					//设备型号
#define	DetectionChannels						32						//最大检测通道数 MAX 48 (必须 <= OUTPUT_MAX 32)
#define	DetectionMode							0x04						//检测手段 1:线圈 2:视屏 3:雷达 4:其他
#define	SignalOutputDelay						0						//信号输出延时
//#define	LINKADDRESS							1234						//链路地址(范围 0 ~ 8191)
#define	LINKADDRESS							param_recv.addr			//链路地址(范围 0 ~ 8191)
#define	PROTOCOL_VER							0x10						//协议版本号

/* 操作类型 */
#define	OPERATIONTYPE_QUERY_REQUEST				0x80						//查询请求
#define	OPERATIONTYPE_SET_REQUEST				0x81						//设置请求
#define	OPERATIONTYPE_ACTIVE_UPLOAD				0x82						//主动上传
#define	OPERATIONTYPE_QUERY_ACK					0x83						//查询应答
#define	OPERATIONTYPE_SET_ACK					0x84						//设置应答
#define	OPERATIONTYPE_ACTIVE_UPLOAD_ACK			0x85						//主动上传应答
#define	OPERATIONTYPE_ERROR_ACK					0x86						//出错应答

/* 对象标识 */
#define	OBJECTID_ONLINE						0x01						//联机表格
#define	OBJECTID_TIME							0x02						//对时
#define	OBJECTID_BAUD							0x03						//通信波特率
#define	OBJECTID_CONFIG_PARAM					0x04						//配置参数
#define	OBJECTID_STATISTICAL					0x05						//统计数据
#define	OBJECTID_HISTORICAL						0x06						//历史数据
#define	OBJECTID_PULSE_UPLOADMODE				0x07						//脉冲数据上传模式
#define	OBJECTID_PULSE_DATA						0x08						//脉冲数据
#define	OBJECTID_ERROR_MESSAGE					0x09						//故障消息

/* 错误类型 */
#define	CHECKCODEERROR							0x01						//校验码错
#define	CHECKVERSIONERROR						0x02						//协议版本不兼容
#define	CHECKMESSAGETYPEERROR					0x03						//消息类型错
#define	CHECKMESSAGECONTENTERROR					0x04						//消息内容中有数据无效

/* 检测器检测项目 */
#define	STATISTICALMODE_NONEMODE					0x00						//不区分车型进行流量统计
#define	STATISTICALMODE_A_C						0x01						//分A、C两类车型进行流量统计
#define	STATISTICALMODE_A_B_C					0x02						//分A、B、C三类车型进行流量统计
#define	STATISTICALMODE_NONE					0x03						//不提供车流量统计数据
#define	AVGSHARE_ENABLE						0x00						//提供车辆平均占有率
#define	AVGSHARE_DISABLE						0x01						//不提供车辆平均占有率
#define	AVGSPEED_ENABLE						0x00						//提供车辆平均行驶速度
#define	AVGSPEED_DISABLE						0x01						//不提供车辆平均行驶速度
#define	AVGLENGTH_ENABLE						0x00						//提供平均车长
#define	AVGLENGTH_DISABLE						0x01						//不提供平均车长
#define	AVGHEADTIME_ENABLE						0x00						//提供平均车头时距
#define	AVGHEADTIME_DISABLE						0x01						//不提供平均车头时距
#define	QUEUINGLENGTH_ENABLE					0x00						//提供车辆排队长度
#define	QUEUINGLENGTH_DISABLE					0x01						//不提供车辆排队长度

/* 检测器配置参数 */
typedef __packed struct
{
	u16 statistical_period;												//0 统计数据的计算周期,单位s	0 ~ 1000
	u8  Acar_langth;													//2 A类车车长				0 ~ 255
	u8  Bcar_length;													//3 B类车车长				0 ~ 150
	u8  Ccar_length;													//4 C类车车长				0 ~ 50
	u32 reserved;														//5 保留字段
}Gat_Param_Detector_Config;												//Gat检测器配置参数 共9个字节

/* 检测器参数 */
typedef __packed struct
{
	u8  manufacturers_length;											//检测器制造商名称字节数		0 ~ 100
	u8  manufacturers_name[sizeof(EquipmentManufacturers)];					//检测器制造商名称
	u8  model_length;													//检测器型号的字节数		0 ~ 100
	u8  model_name[sizeof(EquipmentModel)];									//检测器的型号
	u8  detection_channels;												//最大检测通道数			1 ~ 128
	u16 statistical_mode:2;												//统计方式
	u16 avgshare:1;													//车辆平均占有率			0.提供 1.不提供
	u16 avgspeed:1;													//车辆平均行驶速度			0.提供 1.不提供
	u16 avglength:1;													//车辆平均车长				0.提供 1.不提供
	u16 avgheadtime:1;													//车辆平均车头时距			0.提供 1.不提供
	u16 queuing_length:1;												//车辆排队长度				0.提供 1.不提供
	u16 reserved:9;													//保留字段
	u8  detection_mode;													//检测手段
	u8  signal_output_delay;												//信号输出延时单位10ms		0 ~ 255
	Gat_Param_Detector_Config detector_config;								//检测器配置参数
}Gat_Param_Detector;													//检测器参数 最大216字节

/* 单路检测通道统计数据 */
typedef __packed struct
{
	u8  detectionChannelNo;												//检测通道编号				1 ~ 255
	u8  AcarVolume;													//A类车流量				0 ~ 255
	u8  BcarVolume;													//B类车流量				0 ~ 255
	u8  CcarVolume;													//C类车流量				0 ~ 255
	u8  avgShare;														//平均占有率				0 ~ 200 单位0.5%
	u8  avgSpeed;														//平均车速				0 ~ 255 单位km/h
	u8  avgLength;														//平均车长				0 ~ 255 单位0.1m
	u8  avgHeadTime;													//平均车头时距				0 ~ 255 单位s
	u8  queuingLength;													//车辆排队长度				0 ~ 255 单位m
	u32 reserved;														//保留字段
}Gat_DetectionChannel_Data;												//单路检测通道统计数据 共13字节

/* 统计数据 */
typedef __packed struct
{
	u32 DateTime;														//统计数据生成的本地时间
	Gat_Param_Detector_Config detector_config;								//检测器配置参数
	u8  detection_channels;												//检测通道数				1 ~ 48
	Gat_DetectionChannel_Data detector_statistical_data[DetectionChannels];		//单路检测通道统计数据
}Gat_Statistical_Data;													//统计数据 最大638字节

/* 历史统计数据 */
typedef __packed struct
{
	u8  historyDataNo;													//历史数据流水号			0 ~ 255
	Gat_Statistical_Data statistical_data;									//该流水号统计数据
}Gat_History_Statistical_Data;											//历史统计数据

/* 脉冲数据信号源 */
typedef __packed struct
{
	u8  pulseUploadChannelNo;											//脉冲上传检测通道序号
	u8  pulseUploadCarInOut;												//脉冲上传车辆方向 0:车辆离开 1:车辆进入
}Gat_PulseUpload_Data;													//脉冲数据信号源 共2字节

/* 脉冲数据上传配置参数 */
typedef __packed struct
{
	u8  pulseUploadChannel;												//脉冲上传检测通道数		1 ~ 128
	u8  pulseUploadBit[16];												//脉冲数据上传通道使能位
}Gat_PulseUploadConfig_Data;

/* 其他配置参数 */
typedef __packed struct
{
	u32 SN;															//设备SN码
	u32 baudRate;														//波特率配置值
	u16 output_ID[DetectionChannels];										//脉冲上传通道号对应地磁编号
}Gat_Param_Equipment_Config;


/* GAT920外部接口控制管理器 */
struct _m_gat920_dev
{
	void		(*PollExecution)(void);										//处理GAT协议各个事件
	u8		(*EventInitialized)(void);									//GAT初始化事件
	u8		(*EventReady)(void);										//GAT准备中事件
	u8		(*EventFrameReceived)(void);									//GAT接收数据前期处理事件
	u8		(*EventExecute)(void);										//GAT接收数据分析处理事件
	u8		(*EventFrameSent)(void);										//GAT发送数据后期处理事件
	u8		(*EventActiveUpload)(void);									//GAT主动上传事件
	
	void		(*ParamSaveToFlash)(void);									//保存GAT检测器参数到Flash
	void		(*ParamReadToFlash)(void);									//读取GAT检测器参数到Ram
	void		(*InitParamDetectorDefault)(u32 sn);							//初始化GAT检测器参数
	void		(*ReadOutputID)(u16 *outputid);								//读取output_ID输出端口的参数到GAT脉冲上传通道号对应地磁编号
	
	u16		(*ReceiveBufToDataFrame)(u8 *receive_buf, u16 receive_length);		//从接收到的数据中提取数据帧
	u16		(*DataSheetToDataFrame)(u8 *data_sheet, u16 data_length);			//将数据表转换为数据帧
	u16		(*DataFrameToDataSheet)(u8 *data_frame, u16 data_length);			//将数据帧转换为数据表
	u16		(*DataSheetToMessage)(u8 *data_sheet, u16 data_length);			//从数据表中提取消息内容
	
	u8		(*CheckCode)(u8 *data_frame, u16 data_length);					//检测校验码是否正确-----------错误类型 1
	u8		(*CheckVersion)(u8 *data_sheet);								//检测协议版本是否兼容---------错误类型 2
	u8		(*CheckMessageType)(u8 *data_sheet);							//检测消息类型是否正确---------错误类型 3
	u8		(*CheckMessageContent)(u8 *data_sheet, u16 data_length);			//检测消息内容数据是否正确-----错误类型 4		
	
	u8		(*LinkAddress_Send)(u8 *linkaddress);							//检测器链路地址发送
	u16		(*LinkAddress_Receive)(u8 *data_sheet);							//检测器接收链路地址读取
	
	u8		(*GatOperationType)(u8 *data_sheet);							//获取操作类型值
	u8		(*GatObjectId)(u8 *data_sheet);								//获取对象标识值
	
	void		(*UARTx_IRQ)(USART_TypeDef* USARTx);							//GAT协议串口中断处理函数
	void		(*EnterCriticalSection)(void);								//进入关键操作,关闭中断,等待中断结束返回
	void		(*ExitCriticalSection)(void);									//退出关键操作,开启中断
	
	void		(*InitUploadQueue)(void);									//初始化Upload队列
	void		(*UploadEnqueue)(u16 id, u8 direction);							//脉冲数据写入队列
	u8		(*UploadDequeue)(u16 *id, u8 *direction);						//脉冲数据读出队列
	
	void		(*UploadOvertime)(void);										//主动上传接收应答超时处理
	
	void		(*ImplementEnqueue)(u16 statisticaltime);						//到达统计时间将统计数据存入队列
};
extern struct _m_gat920_dev gat920_dev;


extern Gat_Param_Detector			gatParamDetector;						//GAT检测器参数
extern Gat_PulseUploadConfig_Data		gatParamPulse;							//GAT脉冲数据上传配置参数
extern Gat_Param_Equipment_Config		gatParamEquipment;						//GAT设备配置参数


void GAT_FlashErase(u32 StartAddr);										//擦除GAT参数存储Flash区
u32  GAT_FlashRead(u32 addr);												//读取GAT参数存储Flash区地址
u32  GAT_FlashWrite(u32 addr, u32 *data, u16 num);							//写入GAT参数存储Flash区数据
void GAT_ParamSaveToFlash(void);											//保存GAT检测器参数到Flash
void GAT_ParamReadToFlash(void);											//读取GAT检测器参数到Ram
void GAT_InitParamDetectorDefault(u32 sn);									//初始化GAT检测器参数
void GAT_GetOutputID(u16 *outputid);										//读取output_ID输出端口的参数到GAT脉冲上传通道号对应地磁编号并保存到FLASH

u8   GAT_EventInitialized(void);											//GAT初始化事件
u8   GAT_EventReady(void);												//GAT准备中事件
u8   GAT_EventFrameReceived(void);											//GAT接收数据前期处理事件
u8   GAT_EventExecute(void);												//GAT接收数据分析处理事件
u8   GAT_EventFrameSent(void);											//GAT发送数据后期处理事件
u8   GAT_EventActiveUpload(void);											//GAT主动上传事件
void GAT_PollExecution(void);												//处理GAT协议各个事件

#endif
