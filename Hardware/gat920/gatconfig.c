/**
  *********************************************************************************************************
  * @file    gatconfig.c
  * @author  MoveBroad -- KangYJ
  * @version V1.0
  * @date    
  * @brief   
  *********************************************************************************************************
  * @attention
  *			功能 : 
  *			1.  擦除GAT参数存储Flash区
  *			2.  读取GAT参数存储Flash区地址
  *			3.  写入GAT参数存储Flash区数据
  *			4.  保存GAT检测器参数到Flash
  *			5.  读取GAT检测器参数到Ram
  *			6.  初始化GAT检测器参数
  *			7.  读取output_ID输出端口的参数到GAT脉冲上传通道号对应地磁编号
  *			8.  GAT初始化事件
  *			9.  GAT准备中事件
  *			10. GAT接收数据前期处理事件
  *			11. GAT接收数据分析处理事件
  *			12. GAT发送数据后期处理事件
  *			13. GAT主动上传事件
  *			14. 处理GAT协议各个事件
  *********************************************************************************************************
  */

#include "gatconfig.h"
#include "gatfunc.h"
#include "gatconnect.h"
#include "gatserial.h"
#include "gatupload.h"
#include "string.h"
#include "stm32_config.h"


#define GAT_RO_FLashAddr  	((u32)0x0803E800)								//SN存储地址
#define GAT_RW_StartAddr		((u32)0x0803F800)
#define GAT_RW_FLashAddr_p1	((u32)0x0803F800)								//GAT检测器参数保存区
#define GAT_RW_FLashAddr_p2	((u32)0x0803F900)								//GAT脉冲数据上传配置参数保存区
#define GAT_RW_FLashAddr_p3	((u32)0x0803FA00)								//GAT设备配置参数保存区
#define GAT_RW_FLashAddr_p4	((u32)0x0803FB00)
#define GAT_RW_FLashAddr_p5	((u32)0x0803FC00)
#define GAT_RW_FLashAddr_p6	((u32)0x0803FD00)
#define GAT_RW_FLashAddr_p7	((u32)0x0803FE00)
#define GAT_RW_FLashAddr_p8	((u32)0x0803FF00)
#define GAT_EndAddr			((u32)0x08040000)
#define GAT_PageSize		((u16)0x800)									//RCT6 PageSize = 2048Byte

#define GATPARAMDETECTORLENGTH	(sizeof(gatParamDetector) % 4) != 0 ? (sizeof(gatParamDetector) / 4 + 1) : (sizeof(gatParamDetector) / 4)
#define GATPARAMPULSELENGTH		(sizeof(gatParamPulse) % 4) != 0 ? (sizeof(gatParamPulse) / 4 + 1) : (sizeof(gatParamPulse) / 4)
#define GATPARAMEQUIPMENTLENGTH	(sizeof(gatParamEquipment) % 4) != 0 ? (sizeof(gatParamEquipment) / 4 + 1) : (sizeof(gatParamEquipment) / 4)

Gat_Param_Detector			gatParamDetector;								//GAT检测器参数
Gat_PulseUploadConfig_Data	gatParamPulse;									//GAT脉冲数据上传配置参数
Gat_Param_Equipment_Config	gatParamEquipment;								//GAT设备配置参数

GATEventType				GATEvent = GAT_EVENT_INITIALIZED;					//GAT状态机状态值
GATConnectType				GATConnect = GAT_OFFLINE;						//GAT连接状态
GATUPLOADAckType			GATUPLOADAck = GAT_ReceivedAck;					//GAT上传数据接收应答状态
GATActiveUpLoadModeType		GATActiveUpLoadMode = GAT_ActiveUpLoadMode_NONE;		//GAT主动上传模式

/**********************************************************************************************************
 @Function			void GAT_FlashErase(u32 StartAddr)
 @Description			擦除GAT参数存储Flash区
 @Input				StartAddr : 起始地址
 @Return				void
**********************************************************************************************************/
void GAT_FlashErase(u32 StartAddr)
{
	volatile FLASH_Status FLASHStatus;
	volatile u32 NbrOfPage = 0;
	u32 EraseCounter = 0x0;
	
	FLASHStatus = FLASH_COMPLETE;
	NbrOfPage = (GAT_EndAddr - StartAddr) >> 11;
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	for (EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++) {
		FLASHStatus = FLASH_ErasePage(StartAddr + (GAT_PageSize * EraseCounter));
	}
}

/**********************************************************************************************************
 @Function			u32 GAT_FlashRead(u32 addr)
 @Description			读取GAT参数存储Flash区地址
 @Input				addr : 读取地址
 @Return				该地址值
**********************************************************************************************************/
u32 GAT_FlashRead(u32 addr)
{
	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
	
	return (*(volatile u32*) addr);
}

/**********************************************************************************************************
 @Function			u32 GAT_FlashWrite(u32 addr, u32 *data, u16 num)
 @Description			写入GAT参数存储Flash区数据
 @Input				addr  : 写入地址
					*data : 数据
					num   : 长度/4
 @Return				FLASHStatus
**********************************************************************************************************/
u32 GAT_FlashWrite(u32 addr, u32 *data, u16 num)
{
	u16 i = 0;
	volatile FLASH_Status FLASHStatus;
	
	while ((FLASHStatus == FLASH_COMPLETE) && (i < num)) {
		FLASHStatus = FLASH_ProgramWord((addr + i * 4), data[i]);
		i++;
	}
	
	return FLASHStatus;
}

/**********************************************************************************************************
 @Function			void GAT_ParamSaveToFlash(void)
 @Description			保存GAT检测器参数到Flash
 @Input				void
 @Return				void
**********************************************************************************************************/
void GAT_ParamSaveToFlash(void)
{
	FLASH_Unlock();
	GAT_FlashErase(GAT_RW_StartAddr);												//擦除对应存储FlashPage
	GAT_FlashWrite(GAT_RW_FLashAddr_p1, (u32*) &gatParamDetector, GATPARAMDETECTORLENGTH);	//写入GAT检测器参数
	GAT_FlashWrite(GAT_RW_FLashAddr_p2, (u32*) &gatParamPulse, GATPARAMPULSELENGTH);		//写入GAT脉冲数据上传配置参数
	GAT_FlashWrite(GAT_RW_FLashAddr_p3, (u32*) &gatParamEquipment, GATPARAMEQUIPMENTLENGTH);	//写入GAT设备配置参数
	FLASH_Lock();
}

/**********************************************************************************************************
 @Function			void GAT_ParamReadToFlash(void)
 @Description			读取GAT检测器参数到Ram
 @Input				void
 @Return				void
**********************************************************************************************************/
void GAT_ParamReadToFlash(void)
{
	u32 *params;
	u16 i = 0, length = 0;
	
	length = GATPARAMDETECTORLENGTH;												//读取GAT检测器参数
	params = (u32 *)&gatParamDetector;
	for (i = 0; i < length; i++) {
		delay_5us(240);
		params[i] = GAT_FlashRead(GAT_RW_FLashAddr_p1 + i * 4);
	}
	
	length = GATPARAMPULSELENGTH;													//读取GAT脉冲数据上传配置参数
	params = (u32 *)&gatParamPulse;
	for (i = 0; i < length; i++) {
		delay_5us(240);
		params[i] = GAT_FlashRead(GAT_RW_FLashAddr_p2 + i * 4);
	}
	
	length = GATPARAMEQUIPMENTLENGTH;												//读取GAT设备配置参数
	params = (u32 *)&gatParamEquipment;
	for (i = 0; i < length; i++) {
		delay_5us(240);
		params[i] = GAT_FlashRead(GAT_RW_FLashAddr_p3 + i * 4);
	}
}

/**********************************************************************************************************
 @Function			void GAT_InitParamDetectorDefault(u32 sn)
 @Description			初始化GAT检测器参数
 @Input				sn
 @Return				void
**********************************************************************************************************/
void GAT_InitParamDetectorDefault(u32 sn)
{
	gatParamDetector.manufacturers_length 				= sizeof(EquipmentManufacturers);	//检测器制造商名称字节数
	strcpy((char *)gatParamDetector.manufacturers_name, EquipmentManufacturers);			//检测器制造商名称
	gatParamDetector.model_length						= sizeof(EquipmentModel);		//检测器型号的字节数
	strcpy((char *)gatParamDetector.model_name, EquipmentModel);						//检测器的型号
	gatParamDetector.detection_channels				= DetectionChannels;			//最大检测通道数
	gatParamDetector.statistical_mode					= STATISTICALMODE_NONEMODE;		//统计方式
	gatParamDetector.avgshare						= AVGSHARE_ENABLE;				//车辆平均占有率
	gatParamDetector.avgspeed						= AVGSPEED_DISABLE;				//车辆平均行驶速度
	gatParamDetector.avglength						= AVGLENGTH_DISABLE;			//车辆平均车长
	gatParamDetector.avgheadtime						= AVGHEADTIME_ENABLE;			//车辆平均车头时距
	gatParamDetector.queuing_length					= QUEUINGLENGTH_DISABLE;			//车辆排队长度
	gatParamDetector.reserved						= 0;							//保留字段
	gatParamDetector.detection_mode					= DetectionMode;				//检测手段
	gatParamDetector.signal_output_delay				= SignalOutputDelay;			//信号输出延时
	gatParamDetector.detector_config.statistical_period	= 60;						//统计数据的计算周期
	gatParamDetector.detector_config.Acar_langth			= 200;						//A类车车长
	gatParamDetector.detector_config.Bcar_length			= 100;						//B类车车长
	gatParamDetector.detector_config.Ccar_length			= 50;						//C类车车长
	
	gatParamPulse.pulseUploadChannel					= 1;							//脉冲上传检测通道数
	memset(gatParamPulse.pulseUploadBit, 0x0, sizeof(gatParamPulse.pulseUploadBit));		//脉冲数据上传通道使能位
	
	gatParamEquipment.SN							= sn;						//SN
	gatParamEquipment.baudRate						= GAT920_BAUDRATE;				//默认通信波特率
	memset((u8 *)gatParamEquipment.output_ID, 0x0, sizeof(gatParamEquipment.output_ID));		//清脉冲上传通道号对应地磁编号
}

/**********************************************************************************************************
 @Function			void GAT_GetOutputID(u16 *outputid)
 @Description			读取output_ID输出端口的参数到GAT脉冲上传通道号对应地磁编号并保存到FLASH
 @Input				Output_ID存放地址
 @Return				void
**********************************************************************************************************/
void GAT_GetOutputID(u16 *outputid)
{
	u8 i = 0;
	
	for (i = 0; i < DetectionChannels; i++) {
		gatParamEquipment.output_ID[i] = outputid[i];
	}
	
	GAT_ParamSaveToFlash();
}

/**********************************************************************************************************
 @Function			u8 GAT_EventInitialized(void)
 @Description			GAT初始化事件
 @Input				void
 @Return				GAT_TRUE  : 正确
					GAT_FALSE : 错误
**********************************************************************************************************/
u8 GAT_EventInitialized(void)
{
	u8 Initialized = GAT_TRUE;
	u32 romSN = 0;
	USART_TypeDef* GAT920_USART = USART1;
	
	if (PlatformGat920Usart == Gat920_USART1) {											//根据SN选择Gat920连接串口1
		GAT920_USART = USART1;
	}
	else {																		//根据SN选择Gat920连接串口2
		GAT920_USART = USART2;
	}
	
	romSN = GAT_FlashRead(GAT_RO_FLashAddr);											//读取设备SN
	GAT_ParamReadToFlash();															//读取Flash中GAT配置信息
	if (romSN == 0xFFFFFFFF) {														//SN为空
		GAT_InitParamDetectorDefault(0x12345678);
		GAT_ParamSaveToFlash();
	}
	else if (gatParamEquipment.SN != romSN) {											//SN不为GAT配置信息中sn
		GAT_InitParamDetectorDefault(romSN);
		GAT_ParamSaveToFlash();
	}

	GAT_EnterCriticalSection();
	
	Initialized = GAT_PortSerialInit(GAT920_USART, GAT920_BAUDRATE, 8, GAT_PAR_NONE);			//串口硬件初始化默认波特率
	
	memset((u8 *)&GATReceiveBuf, 0x0, sizeof(GATReceiveBuf));								//清空接收数据缓存区
	memset((u8 *)&GATSendBuf, 0x0, sizeof(GATSendBuf));									//清空发送数据缓存区
	GATReceiveLength = 0;															//接收数据长度清0
	GATSendLength = 0;																//发送数据长度清0
	
	GAT_ExitCriticalSection();
	
	if (Initialized != GAT_FALSE) {
		GATEvent = GAT_EVENT_READY;													//进入READY事件
		GAT_PortSerialEnable(1, 0);													//开启接收中断
	}
	else {
		GATEvent = GAT_EVENT_INITIALIZED;												//进入INITIALIZED事件
		GAT_PortSerialEnable(0, 0);													//关闭串口中断
	}
	
	return Initialized;
}

/**********************************************************************************************************
 @Function			u8 GAT_EventReady(void)
 @Description			GAT准备中事件
 @Input				void
 @Return				GAT_TRUE  : 正确
					GAT_FALSE : 错误
**********************************************************************************************************/
u8 GAT_EventReady(void)
{
	u8 error = GAT_TRUE;
	
	/* 主线查询/设置事件 */
	if (GATReceiveLength & 0X8000) {													//接收到一帧串口数据
		GAT_PortSerialEnable(0, 0);													//关闭串口中断
		GATEvent = GAT_EVENT_FRAME_RECEIVED;											//进入FRAME_RECEIVED事件
	}
	/* 支线脉冲数据主动上传 */
	else if ((GAT_UploadQueueisEmpty() == 0) && (GATUPLOADAck != GAT_NotReceivedAck)) {			//脉冲数据上传队列有待处理数据且无需等待主动上传应答
		GAT_PortSerialEnable(0, 0);													//关闭串口中断
		GATActiveUpLoadMode = GAT_ActiveUpLoadMode_PULSE;									//脉冲数据主动上传
		GATEvent = GAT_EVENT_ACTIVEUPLOAD;												//进入ACTIVEUPLOAD事件
	}
	/* 支线统计数据主动上传 */
	else if ((GAT_StatisticalQueueisEmpty() == 0) && (GATUPLOADAck != GAT_NotReceivedAck)) {		//统计数据上传队列有待处理数据且无需等待主动上传应答
		GAT_PortSerialEnable(0, 0);													//关闭串口中断
		GATActiveUpLoadMode = GAT_ActiveUpLoadMode_STATISTICS;								//统计数据主动上传
		GATEvent = GAT_EVENT_ACTIVEUPLOAD;												//进入ACTIVEUPLOAD事件
	}
	/* 空闲事件 */
	else {
		GATEvent = GAT_EVENT_READY;													//进入READY事件
	}
	
	return error;
}

/**********************************************************************************************************
 @Function			u8 GAT_EventFrameReceived(void)
 @Description			GAT接收数据前期处理事件
 @Input				void
 @Return				GAT_TRUE  : 正确
					GAT_FALSE : 错误
**********************************************************************************************************/
u8 GAT_EventFrameReceived(void)
{
	u8 error = GAT_TRUE;
	u16 frameLength = 0;															//接收数据长度
	u16 changeLength = 0;															//转换后数据长度
	
	frameLength = GATReceiveLength & (~(1<<15));											//获取接收数据长度
	if (frameLength == 0) {															//没有接受到数据而进入接收数据处理事件
		memset((u8 *)&GATReceiveBuf, 0x0, sizeof(GATReceiveBuf));							//清空接收数据缓存区
		GATReceiveLength = 0;														//接收数据长度清0
		GATEvent = GAT_EVENT_READY;													//进入READY事件
		GAT_PortSerialEnable(1, 0);													//开启接收中断
		return GAT_FALSE;
	}
	
	changeLength = GAT_ReceiveBufToDataFrame((u8 *)GATReceiveBuf, frameLength);				//提取GAT数据帧
	if (changeLength == 0) {															//无有效GAT数据帧
		memset((u8 *)&GATReceiveBuf, 0x0, sizeof(GATReceiveBuf));							//清空接收数据缓存区
		GATReceiveLength = 0;														//接收数据长度清0
		GATEvent = GAT_EVENT_READY;													//进入READY事件
		GAT_PortSerialEnable(1, 0);													//开启接收中断
		return GAT_FALSE;
	}
	else {																		//有效GAT数据帧
		GATReceiveLength = changeLength;												//帧长传递
		GATEvent = GAT_EVENT_EXECUTE;													//进入EXECUTE事件
	}
	
	return error;
}

/**********************************************************************************************************
 @Function			u8 GAT_EventExecute(void)
 @Description			GAT接收数据分析处理事件
 @Input				void
 @Return				GAT_TRUE  : 正确
					GAT_FALSE : 错误
**********************************************************************************************************/
u8 GAT_EventExecute(void)
{
	u8 error = GAT_TRUE;
	u16 sheetlength = 0;															//数据表长度
	
	error = GAT_CheckCode((u8 *)GATReceiveBuf, GATReceiveLength);							//校验码检测
	if (error != GAT_TRUE) {															//校验码错误
		GAT_ErrorDataWrite(CHECKCODEERROR);											//出错应答写入缓存区
		GATEvent = GAT_EVENT_FRAME_SENT;												//进入SENT事件
		return GAT_TRUE;
	}
	
	sheetlength = GAT_DataFrameToDataSheet((u8 *)GATReceiveBuf, GATReceiveLength);				//将数据帧转换为数据表
	if (sheetlength != 0) {															//转换成功
		GATReceiveLength = sheetlength;												//传递数据表长度
		
		error = GAT_CheckLinkAddress((u8 *)GATReceiveBuf, LINKADDRESS);						//链路地址检测
		if (error != GAT_TRUE) {														//链路地址不为本机设备链路地址
			error = GAT_FALSE;
			memset((u8 *)&GATReceiveBuf, 0x0, sizeof(GATReceiveBuf));						//清空接收数据缓存区
			memset((u8 *)&GATSendBuf, 0x0, sizeof(GATSendBuf));							//清空发送数据缓存区
			GATReceiveLength = 0;													//接收数据长度清0
			GATSendLength = 0;														//发送数据长度清0
			GATEvent = GAT_EVENT_READY;												//进入READY事件
			GAT_PortSerialEnable(1, 0);												//开启接收中断
			return GAT_FALSE;
		}
		
		error = GAT_CheckVersion((u8 *)GATReceiveBuf);									//协议版本检测
		if (error != GAT_TRUE) {														//协议版本不兼容
			GAT_ErrorDataWrite(CHECKVERSIONERROR);										//出错应答写入缓存区
			GATEvent = GAT_EVENT_FRAME_SENT;											//进入SENT事件
			return GAT_TRUE;
		}
		
		error = GAT_CheckMessageType((u8 *)GATReceiveBuf);								//消息类型检测
		if (error != GAT_TRUE) {														//消息类型错误
			GAT_ErrorDataWrite(CHECKMESSAGETYPEERROR);									//出错应答写入缓存区
			GATEvent = GAT_EVENT_FRAME_SENT;											//进入SENT事件
			return GAT_TRUE;
		}
		
		error = GAT_CheckMessageContent((u8 *)GATReceiveBuf, GATReceiveLength);				//消息内容检测
		if (error != GAT_TRUE) {														//消息内容错误
			GAT_ErrorDataWrite(CHECKMESSAGECONTENTERROR);								//出错应答写入缓存区
			GATEvent = GAT_EVENT_FRAME_SENT;											//进入SENT事件
			return GAT_TRUE;
		}
		
		/* 数据帧转换成功并校验无误 */
		error = GAT_SelectExecuteCmd((u8 *)GATReceiveBuf, GATReceiveLength);					//判断处理不同数据表
		if (error == GAT_TRUE) {														//执行正确
			GATEvent = GAT_EVENT_FRAME_SENT;											//进入SENT事件
			return GAT_TRUE;
		}
		else if (error == GAT_DATAFALSE) {												//消息内容错误
			memset((u8 *)&GATReceiveBuf, 0x0, sizeof(GATReceiveBuf));						//清空接收数据缓存区
			memset((u8 *)&GATSendBuf, 0x0, sizeof(GATSendBuf));							//清空发送数据缓存区
			GATReceiveLength = 0;													//接收数据长度清0
			GATSendLength = 0;														//发送数据长度清0
			GAT_ErrorDataWrite(CHECKMESSAGECONTENTERROR);								//出错应答写入缓存区
			GATEvent = GAT_EVENT_FRAME_SENT;											//进入SENT事件
			return GAT_TRUE;
		}
		else if (error == GAT_RECEIVEEVENT) {											//接收到主动上传应答表
			memset((u8 *)&GATReceiveBuf, 0x0, sizeof(GATReceiveBuf));						//清空接收数据缓存区
			memset((u8 *)&GATSendBuf, 0x0, sizeof(GATSendBuf));							//清空发送数据缓存区
			GATReceiveLength = 0;													//接收数据长度清0
			GATSendLength = 0;														//发送数据长度清0
			GATEvent = GAT_EVENT_READY;												//进入READY事件
			GAT_PortSerialEnable(1, 0);												//开启接收中断
			return GAT_TRUE;
		}
		else {																	//执行错误
			error = GAT_FALSE;
			memset((u8 *)&GATReceiveBuf, 0x0, sizeof(GATReceiveBuf));						//清空接收数据缓存区
			memset((u8 *)&GATSendBuf, 0x0, sizeof(GATSendBuf));							//清空发送数据缓存区
			GATReceiveLength = 0;													//接收数据长度清0
			GATSendLength = 0;														//发送数据长度清0
			GATEvent = GAT_EVENT_READY;												//进入READY事件
			GAT_PortSerialEnable(1, 0);												//开启接收中断
			return GAT_FALSE;
		}
	}
	else {																		//转换失败
		error = GAT_FALSE;
		memset((u8 *)&GATReceiveBuf, 0x0, sizeof(GATReceiveBuf));							//清空接收数据缓存区
		GATReceiveLength = 0;														//接收数据长度清0
		GATEvent = GAT_EVENT_READY;													//进入READY事件
		GAT_PortSerialEnable(1, 0);													//开启接收中断
		return GAT_FALSE;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_EventFrameSent(void)
 @Description			GAT发送数据后期处理事件
 @Input				void
 @Return				GAT_TRUE  : 正确
					GAT_FALSE : 错误
**********************************************************************************************************/
u8 GAT_EventFrameSent(void)
{
	u8 error = GAT_TRUE;
	u16 framelength = 0;															//数据帧长度
	static u8 sending = 0;															//发送数据状态 1.发送中 0.无发送 
	
	if (sending == 0) {																//无数据发送
		if (GATSendLength != 0) {													//有数据需要发送
			framelength = GAT_DataSheetToDataFrame((u8 *)GATSendBuf, GATSendLength);			//将数据表转换为数据帧
			GATSendLength = framelength;												//传递数据帧长度
			sending = 1;															//标记发送数据中
			GATEvent = GAT_EVENT_FRAME_SENT;											//进入SENT事件
			GAT_PortSerialEnable(0, 1);												//开启发送中断
			return GAT_TRUE;
		}
		else {																	//无数据需要发送
			error = GAT_FALSE;
			memset((u8 *)&GATReceiveBuf, 0x0, sizeof(GATReceiveBuf));						//清空接收数据缓存区
			memset((u8 *)&GATSendBuf, 0x0, sizeof(GATSendBuf));							//清空发送数据缓存区
			GATReceiveLength = 0;													//接收数据长度清0
			GATSendLength = 0;														//发送数据长度清0
			GATEvent = GAT_EVENT_READY;												//进入READY事件
			GAT_PortSerialEnable(1, 0);												//开启接收中断
			return GAT_FALSE;
		}
	}
	else {																		//发送数据中
		if (GATSendLength & 0X8000) {													//发送完一帧数据
			GAT_PortSerialEnable(0, 0);												//关闭串口中断
			sending = 0;															//标记无数据发送中
			memset((u8 *)&GATReceiveBuf, 0x0, sizeof(GATReceiveBuf));						//清空接收数据缓存区
			memset((u8 *)&GATSendBuf, 0x0, sizeof(GATSendBuf));							//清空发送数据缓存区
			GATReceiveLength = 0;													//接收数据长度清0
			GATSendLength = 0;														//发送数据长度清0
			GATEvent = GAT_EVENT_READY;												//进入READY事件
			GAT_PortSerialEnable(1, 0);												//开启接收中断
			return GAT_TRUE;
		}
		else {																	//正在发送中
			GATEvent = GAT_EVENT_FRAME_SENT;											//进入SENT事件
		}
	}
	
	return error;
}

/**********************************************************************************************************
 @Function			u8 GAT_EventActiveUpload(void)
 @Description			GAT主动上传事件
 @Input				void
 @Return				GAT_TRUE  : 正确
					GAT_FALSE : 错误
**********************************************************************************************************/
u8 GAT_EventActiveUpload(void)
{
	u8 error = GAT_TRUE;
	
	/* 统计数据主动上传 */
	if (GATActiveUpLoadMode == GAT_ActiveUpLoadMode_STATISTICS) {
		error = GAT_ActiveStatisticalDataWrite();										//将统计数据写入脉冲数据主动上传表到发送缓存区
		if (error == GAT_TRUE) {														//执行正确
			GATEvent = GAT_EVENT_FRAME_SENT;											//进入SENT事件
			GATUPLOADAck = GAT_NotReceivedAck;											//等待接收应答表
			GATActiveUpLoadMode = GAT_ActiveUpLoadMode_NONE;
			return GAT_TRUE;
		}
		else {																	//执行错误
			error = GAT_FALSE;
			GATActiveUpLoadMode = GAT_ActiveUpLoadMode_NONE;
			GAT_StatisticalDequeueMove();
			memset((u8 *)&GATReceiveBuf, 0x0, sizeof(GATReceiveBuf));						//清空接收数据缓存区
			memset((u8 *)&GATSendBuf, 0x0, sizeof(GATSendBuf));							//清空发送数据缓存区
			GATReceiveLength = 0;													//接收数据长度清0
			GATSendLength = 0;														//发送数据长度清0
			GATEvent = GAT_EVENT_READY;												//进入READY事件
			GAT_PortSerialEnable(1, 0);												//开启接收中断
			return GAT_FALSE;
		}
	}
	/* 脉冲数据主动上传 */
	else if (GATActiveUpLoadMode == GAT_ActiveUpLoadMode_PULSE) {
		error = GAT_ActiveUpLoadDataWrite();											//将脉冲数据写入脉冲数据主动上传表到发送缓存区
		if (error == GAT_TRUE) {														//执行正确
			GATEvent = GAT_EVENT_FRAME_SENT;											//进入SENT事件
			GATUPLOADAck = GAT_NotReceivedAck;											//等待接收应答表
			GATActiveUpLoadMode = GAT_ActiveUpLoadMode_NONE;
			return GAT_TRUE;
		}
		else {																	//执行错误
			error = GAT_FALSE;
			GATActiveUpLoadMode = GAT_ActiveUpLoadMode_NONE;
			GAT_UploadDequeueMove();
			memset((u8 *)&GATReceiveBuf, 0x0, sizeof(GATReceiveBuf));						//清空接收数据缓存区
			memset((u8 *)&GATSendBuf, 0x0, sizeof(GATSendBuf));							//清空发送数据缓存区
			GATReceiveLength = 0;													//接收数据长度清0
			GATSendLength = 0;														//发送数据长度清0
			GATEvent = GAT_EVENT_READY;												//进入READY事件
			GAT_PortSerialEnable(1, 0);												//开启接收中断
			return GAT_FALSE;
		}
	}
	/* 检测器故障主动上传 */
	else if (GATActiveUpLoadMode == GAT_ActiveUpLoadMode_FAULT) {
		
	}
	else {
		return GAT_FALSE;
	}
	
	return error;
}

/**********************************************************************************************************
 @Function			void GAT_PollExecution(void)
 @Description			处理GAT协议各个事件
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
							    <-----  |             <--error--        |
执行错误:			SENT ERR MESSAGE   <-----  |  <--error--  |                |
									  |              |                |
主线状态:				INITIALIZED  ---->  READY  ---->  RECEIVED  ---->  EXECUTE  ---->  SENT
									  |                                              |
执行成功:								  |  <----------------successd-----------------  |
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
执行错误:			                           |    <--error--   |
									  |                 |
支线状态:				INITIALIZED  ---->  READY  ---->  ACTIVEUPLOAD  ---->  SENT
									  |                                  |
执行成功:								  |  <----------successd-----------  |
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 @Input				void
 @Return				void
**********************************************************************************************************/
void GAT_PollExecution(void)
{
	switch (GATEvent)															//GAT协议处理状态转换机
	{
	case GAT_EVENT_READY:														//READY
		if (GAT_EventReady() != GAT_TRUE) {
			GATEvent = GAT_EVENT_READY;
		}
		break;
	
	case GAT_EVENT_FRAME_RECEIVED:												//RECEIVED
		if (GAT_EventFrameReceived() != GAT_TRUE) {
			GATEvent = GAT_EVENT_READY;
		}
		break;
	
	case GAT_EVENT_FRAME_SENT:													//SENT
		if (GAT_EventFrameSent() != GAT_TRUE) {
			GATEvent = GAT_EVENT_READY;
		}
		break;
	
	case GAT_EVENT_EXECUTE:														//EXECUTE
		if (GAT_EventExecute() != GAT_TRUE) {
			GATEvent = GAT_EVENT_READY;
		}
		break;
	
	case GAT_EVENT_INITIALIZED:													//INITIALIZED
		if (GAT_EventInitialized() != GAT_TRUE) {
			GATEvent = GAT_EVENT_INITIALIZED;
		}
		break;
		
	case GAT_EVENT_ACTIVEUPLOAD:													//ACTIVEUPLOAD
		if (GAT_EventActiveUpload() != GAT_TRUE) {
			GATEvent = GAT_EVENT_READY;
		}
		break;
	}
}

/* gat920外部接口例化各函数 */
struct _m_gat920_dev gat920_dev = 
{
	GAT_PollExecution,															//处理GAT协议各个事件
	GAT_EventInitialized,														//GAT初始化事件
	GAT_EventReady,															//GAT准备中事件
	GAT_EventFrameReceived,														//GAT接收数据前期处理事件
	GAT_EventExecute,															//GAT接收数据分析处理事件
	GAT_EventFrameSent,															//GAT发送数据后期处理事件
	GAT_EventActiveUpload,														//GAT主动上传事件
	
	GAT_ParamSaveToFlash,														//保存GAT检测器参数到Flash
	GAT_ParamReadToFlash,														//读取GAT检测器参数到Ram
	GAT_InitParamDetectorDefault,													//初始化GAT检测器参数
	GAT_GetOutputID,															//读取output_ID输出端口的参数到GAT脉冲上传通道号对应地磁编号
	
	GAT_ReceiveBufToDataFrame,													//从接收到的数据中提取数据帧
	GAT_DataSheetToDataFrame,													//将数据表转换为数据帧
	GAT_DataFrameToDataSheet,													//将数据帧转换为数据表
	GAT_DataSheetToMessage,														//从数据表中提取消息内容
	
	GAT_CheckCode,																//检测校验码是否正确-----------错误类型 1
	GAT_CheckVersion,															//检测协议版本是否兼容---------错误类型 2
	GAT_CheckMessageType,														//检测消息类型是否正确---------错误类型 3
	GAT_CheckMessageContent,														//检测消息内容数据是否正确-----错误类型 4
	
	GAT_LinkAddress_Send,														//检测器链路地址发送
	GAT_LinkAddress_Receive,														//检测器接收链路地址读取
	
	GAT_GatOperationType,														//获取操作类型值
	GAT_GatObjectId,															//获取对象标识值
	
	GAT_UARTx_IRQ,																//GAT协议串口中断处理函数
	GAT_EnterCriticalSection,													//进入关键操作,关闭中断,等待中断结束返回
	GAT_ExitCriticalSection,														//退出关键操作,开启中断
	
	GAT_InitUploadQueue,														//初始化Upload队列
	GAT_UploadEnqueue,															//脉冲数据写入队列
	GAT_UploadDequeue,															//脉冲数据读出队列
	
	GAT_UploadOvertime,															//主动上传接收应答超时处理
	
	GAT_ImplementEnqueue,														//到达统计时间将统计数据存入队列
};

/********************************************** END OF FLEE **********************************************/
