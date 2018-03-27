/**
  *********************************************************************************************************
  * @file    gatconnect.c
  * @author  MoveBroad -- KangYJ
  * @version V1.0
  * @date    
  * @brief   
  *********************************************************************************************************
  * @attention
  *			功能 : 
  *			1.  判断处理不同命令
  *
  *			2.  写入出错应答消息到发送缓存区
  *			3.  写入连接请求应答表到发送缓存区并设定连接状态
  *			4.  检测连接状态并写入连接查询应答表到发送缓存区
  *			5.  检测连接状态并提取时间设置表时间并写入时间设置应答表到发送缓存区
  *			6.  检测连接状态并获取时间写入时间查询应答表到发送缓存区
  *			7.  检测连接状态并获取波特率设置表波特率设置波特率并写入设置设置应答表到发送缓存区
  *			8.  检测连接状态并提取配置参数设置表数据并写入配置参数设置应答表到发送缓存区
  *			9.  检测连接状态并将检测器参数写入检测器参数查询应答表到发送缓存区
  *			10. 检测连接状态并提取脉冲数据配置参数设置表数据并写入脉冲数据上传配置参数设置应答表到发送缓存区
  *			11. 检测连接状态并将脉冲数据上传配置参数写入脉冲数据上传配置参数查询应答表到发送缓存区
  *
  *			12. 检测连接状态并将脉冲数据写入脉冲数据主动上传表到发送缓存区
  *			13. 检测连接状态并根据脉冲数据主动上传应答表做处理
  *
  *			14. 检测连接状态并将统计数据写入统计数据主动上传表到发送缓存区
  *			15. 检测连接状态并根据统计数据主动上传应答表做处理
  *********************************************************************************************************
  */

#include "gatconnect.h"
#include "gatfunc.h"
#include "gatserial.h"
#include "gatupload.h"
#include "string.h"


/**********************************************************************************************************
 @Function			u8 GAT_SelectExecuteCmd(u8 *data_sheet, u16 data_length)
 @Description			判断处理不同命令
 @Input				*data_sheet  	 		: 数据表地址
					data_length 			: 数据表长度
 @Return				error			   0 : 正常
									   1 : 错误
**********************************************************************************************************/
u8 GAT_SelectExecuteCmd(u8 *data_sheet, u16 data_length)
{
	u8 error = GAT_TRUE;
	u8 operationtype = 0;												//操作类型
	u8 objectid = 0;													//对象标识
	u8 *message_data;													//消息内容
	u16 message_length = 0;												//消息长度
	
	//获取操作类型和对象标识
	if ((data_sheet[0] & 0x01) == 1) {										//链路地址为单字节
		operationtype = data_sheet[2];
		objectid = data_sheet[3];
	}
	else {															//链路地址为双字节
		operationtype = data_sheet[3];
		objectid = data_sheet[4];
	}
	
	//提取消息内容和消息长度
	message_data = data_sheet;
	message_length = GAT_DataSheetToMessage(message_data, data_length);
	
	/* 信号机发送连接请求数据表, 消息内容 0 字节 */
	if ((operationtype == OPERATIONTYPE_SET_REQUEST) && (objectid == OBJECTID_ONLINE)) {
		error = GAT_AckSetOnlineWrite();									//接收连接请求表处理
		if (error != GAT_FALSE) {
			return GAT_TRUE;
		}
		else {
			return GAT_FALSE;
		}
	}
	
	/* 信号机发送连接查询数据表, 消息内容 0 字节 */
	if ((operationtype == OPERATIONTYPE_QUERY_REQUEST) && (objectid == OBJECTID_ONLINE)) {
		error = GAT_AckQueryOnlineWrite();									//接收连接查询表处理
		if (error != GAT_FALSE) {
			return GAT_TRUE;
		}
		else {
			return GAT_FALSE;
		}
	}
	
	/* 信号机发送时间设置数据表, 消息内容 4 字节 */
	if ((operationtype == OPERATIONTYPE_SET_REQUEST) && (objectid == OBJECTID_TIME)) {
		error = GAT_AckSetTimeWrite(message_data, message_length);				//接收时间设置表处理
		if (error != GAT_FALSE) {
			return GAT_TRUE;
		}
		else {
			return GAT_FALSE;
		}
	}
	
	/* 信号机发送时间查询数据表, 消息内容 0 字节 */
	if ((operationtype == OPERATIONTYPE_QUERY_REQUEST) && (objectid == OBJECTID_TIME)) {
		error = GAT_AckQueryTimeWrite();									//接收时间查询表处理
		if (error != GAT_FALSE) {
			return GAT_TRUE;
		}
		else {
			return GAT_FALSE;
		}
	}
	
	/* 信号机发送波特率设置数据表, 消息内容 4 字节 */
	if ((operationtype == OPERATIONTYPE_SET_REQUEST) && (objectid == OBJECTID_BAUD)) {
		error = GAT_AckSetBaudRateWrite(message_data, message_length);			//接收波特率设置表处理
		if (error != GAT_FALSE) {
			return GAT_TRUE;
		}
		else {
			return GAT_FALSE;
		}
	}
	
	/* 信号机发送配置参数设置数据表, 消息内容 9 字节 */
	if ((operationtype == OPERATIONTYPE_SET_REQUEST) && (objectid == OBJECTID_CONFIG_PARAM)) {
		error = GAT_AckSetConfigParamWrite(message_data, message_length);		//接收配置参数设置表处理
		if (error == GAT_TRUE) {
			return GAT_TRUE;
		}
		else if (error == GAT_DATAFALSE) {
			return GAT_DATAFALSE;
		}
		else {
			return GAT_FALSE;
		}
	}
	
	/* 信号机发送配置参数查询数据表, 消息内容 0 字节 */
	if ((operationtype == OPERATIONTYPE_QUERY_REQUEST) && (objectid == OBJECTID_CONFIG_PARAM)) {
		error = GAT_AckQueryConfigParamWrite();								//接收配置参数查询表处理
		if (error != GAT_FALSE) {
			return GAT_TRUE;
		}
		else {
			return GAT_FALSE;
		}
	}
	
	/* 信号机发送统计数据主动上传应答数据表, 消息内容 0 字节 */
	if ((operationtype == OPERATIONTYPE_ACTIVE_UPLOAD_ACK) && (objectid == OBJECTID_STATISTICAL)) {
		error = GAT_AckInspectStatisticalData();							//接收统计数据上传应答数据表处理
		if (error == GAT_RECEIVEEVENT) {
			return GAT_RECEIVEEVENT;
		}
		else {
			return GAT_FALSE;
		}
	}
	
	/* 信号机发送历史数据查询数据表, 消息内容 8 字节 */
	if ((operationtype == OPERATIONTYPE_QUERY_REQUEST) && (objectid == OBJECTID_HISTORICAL)) {
		
	}
	
	/* 信号机发送脉冲数据上传模式设置数据表, 消息内容 ((N + 7) / 8) + 1 字节 */
	if ((operationtype == OPERATIONTYPE_SET_REQUEST) && (objectid == OBJECTID_PULSE_UPLOADMODE)) {
		error = GAT_AckSetPulseUploadConfigWrite(message_data, message_length);	//接收脉冲数据上传模式设置表处理
		if (error == GAT_TRUE) {
			return GAT_TRUE;
		}
		else if (error == GAT_DATAFALSE) {
			return GAT_DATAFALSE;
		}
		else {
			return GAT_FALSE;
		}
	}
	
	/* 信号机发送脉冲数据上传模式查询数据表, 消息内容 0 字节 */
	if ((operationtype == OPERATIONTYPE_QUERY_REQUEST) && (objectid == OBJECTID_PULSE_UPLOADMODE)) {
		error = GAT_AckQueryPulseUploadConfigWrite();						//接收脉冲数据上传模式查询表处理
		if (error != GAT_FALSE) {
			return GAT_TRUE;
		}
		else {
			return GAT_FALSE;
		}
	}
	
	/* 信号机发送脉冲数据主动上传应答数据表, 消息内容 0 字节 */
	if ((operationtype == OPERATIONTYPE_ACTIVE_UPLOAD_ACK) && (objectid == OBJECTID_PULSE_DATA)) {
		error = GAT_AckInspectUpLoadData();								//接收脉冲数据上传应答数据表处理
		if (error == GAT_RECEIVEEVENT) {
			return GAT_RECEIVEEVENT;
		}
		else {
			return GAT_FALSE;
		}
	}
	
	/* 信号机发送检测器故障主动上传应答数据表, 消息内容 0 字节 */
	if ((operationtype == OPERATIONTYPE_ACTIVE_UPLOAD_ACK) && (objectid == OBJECTID_ERROR_MESSAGE)) {
		
	}
	
	return error;
}

/**********************************************************************************************************
 @Function			void GAT_ErrorDataWrite(u8 errormessage)
 @Description			写入出错应答消息到发送缓存区
 @Input				errormessage	: 出错消息
 @Return				void
**********************************************************************************************************/
void GAT_ErrorDataWrite(u8 errormessage)
{
	u8 linkaddresslength = 0;																	//链路地址长度
	Gat_Error_Data tmpbuf;
	
	tmpbuf.AckHead.protocolnum = PROTOCOL_VER;														//协议版本
	tmpbuf.AckHead.operationtype = OPERATIONTYPE_ERROR_ACK;											//操作类型->出错应答
	tmpbuf.AckHead.objectid = OBJECTID_ERROR_MESSAGE;													//对象标识->故障消息
	tmpbuf.errortype = errormessage;																//错误类型
	
	linkaddresslength = GAT_LinkAddress_Send((u8 *)GATSendBuf);											//写入链路地址
	memcpy((void *)&GATSendBuf[linkaddresslength], (void *)&tmpbuf, sizeof(tmpbuf));						//写入出错应答表
	GATSendLength = linkaddresslength + sizeof(tmpbuf);												//写入发送数据长度
}

/**********************************************************************************************************
 @Function			u8 GAT_AckSetOnlineWrite(void)
 @Description			写入连接请求应答表到发送缓存区并设定连接状态
 @Input				void
 @Return				GAT_TRUE  : 正确
					GAT_FALSE : 错误
**********************************************************************************************************/
u8 GAT_AckSetOnlineWrite(void)
{
	u8 linkaddresslength = 0;																	//链路地址长度
	Gat_AckFrame_Data tmpbuf;
	
	tmpbuf.protocolnum = PROTOCOL_VER;																//协议版本
	tmpbuf.operationtype = OPERATIONTYPE_SET_ACK;													//操作类型->设置应答
	tmpbuf.objectid = OBJECTID_ONLINE;																//对象标识->联机表格
	
	linkaddresslength = GAT_LinkAddress_Send((u8 *)GATSendBuf);											//写入链路地址
	memcpy((void *)&GATSendBuf[linkaddresslength], (void *)&tmpbuf, sizeof(tmpbuf));						//写入连接请求应答表
	GATSendLength = linkaddresslength + sizeof(tmpbuf);												//写入发送数据长度
	
	GATConnect = GAT_ONLINE;																		//连接状态设为已连接
	
	return GAT_TRUE;
}

/**********************************************************************************************************
 @Function			u8 GAT_AckQueryOnlineWrite(void)
 @Description			检测连接状态并写入连接查询应答表到发送缓存区(连接写入, 未连接不写入)
 @Input				void
 @Return				GAT_TRUE  : 正确
					GAT_FALSE : 错误
**********************************************************************************************************/
u8 GAT_AckQueryOnlineWrite(void)
{
	u8 linkaddresslength = 0;																	//链路地址长度
	Gat_AckFrame_Data tmpbuf;
	
	if (GATConnect == GAT_ONLINE) {																//已连接
		tmpbuf.protocolnum = PROTOCOL_VER;															//协议版本
		tmpbuf.operationtype = OPERATIONTYPE_QUERY_ACK;												//操作类型->查询应答
		tmpbuf.objectid = OBJECTID_ONLINE;															//对象标识->联机表格
		
		linkaddresslength = GAT_LinkAddress_Send((u8 *)GATSendBuf);										//写入链路地址
		memcpy((void *)&GATSendBuf[linkaddresslength], (void *)&tmpbuf, sizeof(tmpbuf));					//写入连接查询应答表
		GATSendLength = linkaddresslength + sizeof(tmpbuf);											//写入发送数据长度
		
		return GAT_TRUE;
	}
	else {																					//未连接
		return GAT_FALSE;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_AckSetTimeWrite(u8 *msgdata, u16 msglength)
 @Description			检测连接状态并提取时间设置表时间并写入时间设置应答表到发送缓存区(连接写入, 未连接不写入)
 @Input				msgdata	: 消息内容
					msglength	: 消息长度
 @Return				GAT_TRUE  : 正确
					GAT_FALSE : 错误
**********************************************************************************************************/
u8 GAT_AckSetTimeWrite(u8 *msgdata, u16 msglength)
{
	u8 linkaddresslength = 0;																	//链路地址长度
	Gat_AckFrame_Data tmpbuf;
	u32 datetime = 0;
	
	if (GATConnect == GAT_ONLINE) {																//已连接
		if (msglength == 4) {																	//字节数检测
			datetime |= msgdata[0];																//获取时间值
			datetime |= msgdata[1] << 8;
			datetime |= msgdata[2] << 16;
			datetime |= msgdata[3] << 24;
			RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
			PWR_BackupAccessCmd(ENABLE);
			RTC_WaitForLastTask();
			RTC_SetCounter(datetime);															//写入RTC寄存器值
			RTC_WaitForLastTask();
			
			tmpbuf.protocolnum = PROTOCOL_VER;														//协议版本
			tmpbuf.operationtype = OPERATIONTYPE_SET_ACK;											//操作类型->设置应答
			tmpbuf.objectid = OBJECTID_TIME;														//对象标识->对时
			
			linkaddresslength = GAT_LinkAddress_Send((u8 *)GATSendBuf);									//写入链路地址
			memcpy((void *)&GATSendBuf[linkaddresslength], (void *)&tmpbuf, sizeof(tmpbuf));				//写入时间设置应答表
			GATSendLength = linkaddresslength + sizeof(tmpbuf);										//写入发送数据长度
			
			return GAT_TRUE;
		}
		else {
			return GAT_FALSE;
		}
	}
	else {																					//未连接
		return GAT_FALSE;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_AckQueryTimeWrite(void)
 @Description			检测连接状态并获取时间写入时间查询应答表到发送缓存区(连接写入, 未连接不写入)
 @Input				void
 @Return				GAT_TRUE  : 正确
					GAT_FALSE : 错误
**********************************************************************************************************/
u8 GAT_AckQueryTimeWrite(void)
{
	u8 linkaddresslength = 0;																	//链路地址长度
	Gat_Time_Data tmpbuf;
	
	if (GATConnect == GAT_ONLINE) {																//已连接
		tmpbuf.AckHead.protocolnum = PROTOCOL_VER;													//协议版本
		tmpbuf.AckHead.operationtype = OPERATIONTYPE_QUERY_ACK;										//操作类型->查询应答
		tmpbuf.AckHead.objectid = OBJECTID_TIME;													//对象标识->对时
		tmpbuf.datetime = RTC_GetCounter();														//获取本地时间值
		
		linkaddresslength = GAT_LinkAddress_Send((u8 *)GATSendBuf);										//写入链路地址
		memcpy((void *)&GATSendBuf[linkaddresslength], (void *)&tmpbuf, sizeof(tmpbuf));					//写入时间查询应答表
		GATSendLength = linkaddresslength + sizeof(tmpbuf);											//写入发送数据长度
		
		return GAT_TRUE;
	}
	else {																					//未连接
		return GAT_FALSE;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_AckSetTimeWrite(u8 *msgdata, u16 msglength)
 @Description			检测连接状态并获取波特率设置表波特率设置波特率并写入波特率设置应答表到发送缓存区(连接写入, 未连接不写入)
 @Input				msgdata	: 消息内容
					msglength	: 消息长度
 @Return				GAT_TRUE  : 正确
					GAT_FALSE : 错误
**********************************************************************************************************/
u8 GAT_AckSetBaudRateWrite(u8 *msgdata, u16 msglength)
{
	u8 error = GAT_TRUE;
	u8 linkaddresslength = 0;																		//链路地址长度
	Gat_BaudRate_Data tmpbuf;
	u32 baudrate = 0;
	USART_TypeDef* GAT920_USART = USART1;
	
	if (PlatformGat920Usart == Gat920_USART1) {															//根据SN选择Gat920连接串口1
		GAT920_USART = USART1;
	}
	else {																						//根据SN选择Gat920连接串口2
		GAT920_USART = USART2;
	}
	
	if (GATConnect == GAT_ONLINE) {																	//已连接
		if (msglength == 4) {																		//字节数检测
			baudrate |= msgdata[0];																	//获取波特率值
			baudrate |= msgdata[1] << 8;
			baudrate |= msgdata[2] << 16;
			baudrate |= msgdata[3] << 24;
			gatParamEquipment.baudRate = baudrate;														//波特率设置值
			error = GAT_PortSerialInit(GAT920_USART, gatParamEquipment.baudRate, 8, GAT_PAR_NONE);
			if (error != GAT_FALSE) {
				tmpbuf.setbaudresult = 1;															//设置成功
			}
			else {
				tmpbuf.setbaudresult = 0;															//设置失败
			}
			
			tmpbuf.AckHead.protocolnum = PROTOCOL_VER;													//协议版本
			tmpbuf.AckHead.operationtype = OPERATIONTYPE_SET_ACK;											//操作类型->设置应答
			tmpbuf.AckHead.objectid = OBJECTID_BAUD;													//对象标识->通信波特率
			
			linkaddresslength = GAT_LinkAddress_Send((u8 *)GATSendBuf);										//写入链路地址
			memcpy((void *)&GATSendBuf[linkaddresslength], (void *)&tmpbuf, sizeof(tmpbuf));					//写入波特率设置应答表
			GATSendLength = linkaddresslength + sizeof(tmpbuf);											//写入发送数据长度
			
			return GAT_TRUE;
		}
		else {
			return GAT_FALSE;
		}
	}
	else {																						//未连接
		return GAT_FALSE;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_AckSetConfigParamWrite(u8 *msgdata, u16 msglength)
 @Description			检测连接状态并提取配置参数设置表数据并写入配置参数设置应答表到发送缓存区(连接写入, 未连接不写入)
 @Input				msgdata		: 消息内容
					msglength		: 消息长度
 @Return				GAT_TRUE  	: 正确
					GAT_FALSE 	: 错误
					GAT_DATAFALSE	: 消息内容错误
**********************************************************************************************************/
u8 GAT_AckSetConfigParamWrite(u8 *msgdata, u16 msglength)
{
	u8 linkaddresslength = 0;																		//链路地址长度
	Gat_AckFrame_Data tmpbuf;
	Gat_Param_Detector_Config configparam;																//配置参数
	
	memset((u8 *)&configparam, 0x0, sizeof(configparam));													//清空缓存栈
	
	if (GATConnect == GAT_ONLINE) {																	//已连接
		if (msglength == 9) {																		//字节数检测
			configparam.statistical_period |= msgdata[0];												//提取参数值
			configparam.statistical_period |= msgdata[1] << 8;
			configparam.Acar_langth |= msgdata[2];
			configparam.Bcar_length |= msgdata[3];
			configparam.Ccar_length |= msgdata[4];
			configparam.reserved |= msgdata[5];
			configparam.reserved |= msgdata[6];
			configparam.reserved |= msgdata[7];
			configparam.reserved |= msgdata[8];
			
			/* 检测消息内容数据是否正确 */
			if ( (configparam.statistical_period > 1000) || (configparam.Acar_langth > 255) ||
				(configparam.Bcar_length > 150) || (configparam.Ccar_length > 50) ) {							//消息内容错误
				return GAT_DATAFALSE;
			}
			else {																				//消息内容正确
				gatParamDetector.detector_config.statistical_period = configparam.statistical_period;
				gatParamDetector.detector_config.Acar_langth = configparam.Acar_langth;
				gatParamDetector.detector_config.Bcar_length = configparam.Bcar_length;
				gatParamDetector.detector_config.Ccar_length = configparam.Ccar_length;
				gatParamDetector.detector_config.reserved = configparam.reserved;
				GAT_ParamSaveToFlash();																//配置参数存入Flash
				tmpbuf.protocolnum = PROTOCOL_VER;														//协议版本
				tmpbuf.operationtype = OPERATIONTYPE_SET_ACK;											//操作类型->设置应答
				tmpbuf.objectid = OBJECTID_CONFIG_PARAM;												//对象标识->配置参数
				
				linkaddresslength = GAT_LinkAddress_Send((u8 *)GATSendBuf);									//写入链路地址
				memcpy((void *)&GATSendBuf[linkaddresslength], (void *)&tmpbuf, sizeof(tmpbuf));				//写入配置参数设置应答表
				GATSendLength = linkaddresslength + sizeof(tmpbuf);										//写入发送数据长度
				
				return GAT_TRUE;
			}
		}
		else {
			return GAT_FALSE;
		}
	}
	else {																						//未连接
		return GAT_FALSE;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_AckQueryConfigParamWrite(void)
 @Description			检测连接状态并将检测器参数写入检测器参数查询应答表到发送缓存区(连接写入, 未连接不写入)
 @Input				void
 @Return				GAT_TRUE  : 正确
					GAT_FALSE : 错误
**********************************************************************************************************/
u8 GAT_AckQueryConfigParamWrite(void)
{
	u8 linkaddresslength = 0;																		//链路地址长度
	Gat_ConfigParam_Data tmpbuf;
	
	if (GATConnect == GAT_ONLINE) {																	//已连接
		tmpbuf.AckHead.protocolnum = PROTOCOL_VER;														//协议版本
		tmpbuf.AckHead.operationtype = OPERATIONTYPE_QUERY_ACK;											//操作类型->查询应答
		tmpbuf.AckHead.objectid = OBJECTID_CONFIG_PARAM;													//对象标识->配置参数
		
		tmpbuf.ParamDetector.manufacturers_length = gatParamDetector.manufacturers_length;						//检测器制造商名称字节数
		strcpy((char *)tmpbuf.ParamDetector.manufacturers_name, (char *)gatParamDetector.manufacturers_name);		//检测器制造商名称
		tmpbuf.ParamDetector.model_length = gatParamDetector.model_length;									//检测器型号的字节数
		strcpy((char *)tmpbuf.ParamDetector.model_name, (char *)gatParamDetector.model_name);					//检测器的型号
		tmpbuf.ParamDetector.detection_channels = gatParamDetector.detection_channels;							//最大检测通道数
		tmpbuf.ParamDetector.statistical_mode = gatParamDetector.statistical_mode;								//统计方式
		tmpbuf.ParamDetector.avgshare = gatParamDetector.avgshare;											//车辆平均占有率
		tmpbuf.ParamDetector.avgspeed = gatParamDetector.avgspeed;											//车辆平均行驶速度
		tmpbuf.ParamDetector.avglength = gatParamDetector.avglength;										//车辆平均车长
		tmpbuf.ParamDetector.avgheadtime = gatParamDetector.avgheadtime;										//车辆平均车头时距
		tmpbuf.ParamDetector.queuing_length = gatParamDetector.queuing_length;								//车辆排队长度
		tmpbuf.ParamDetector.reserved = gatParamDetector.reserved;											//保留字段
		tmpbuf.ParamDetector.detection_mode = gatParamDetector.detection_mode;								//检测手段
		tmpbuf.ParamDetector.signal_output_delay = gatParamDetector.signal_output_delay;						//信号输出延时
		tmpbuf.ParamDetector.detector_config.statistical_period = gatParamDetector.detector_config.statistical_period;//统计数据的计算周期
		tmpbuf.ParamDetector.detector_config.Acar_langth = gatParamDetector.detector_config.Acar_langth;			//A类车车长
		tmpbuf.ParamDetector.detector_config.Bcar_length = gatParamDetector.detector_config.Bcar_length;			//B类车车长
		tmpbuf.ParamDetector.detector_config.Ccar_length = gatParamDetector.detector_config.Ccar_length;			//C类车车长
		tmpbuf.ParamDetector.detector_config.reserved = gatParamDetector.detector_config.reserved;
		
		linkaddresslength = GAT_LinkAddress_Send((u8 *)GATSendBuf);											//写入链路地址
		memcpy((void *)&GATSendBuf[linkaddresslength], (void *)&tmpbuf, sizeof(tmpbuf));						//写入配置参数查询应答表
		GATSendLength = linkaddresslength + sizeof(tmpbuf);												//写入发送数据长度
		
		return GAT_TRUE;
	}
	else {																						//未连接
		return GAT_FALSE;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_AckSetPulseUploadConfigWrite(u8 *msgdata, u16 msglength)
 @Description			检测连接状态并提取脉冲数据配置参数设置表数据并写入脉冲数据上传配置参数设置应答表到发送缓存区(连接写入, 未连接不写入)
 @Input				msgdata		: 消息内容
					msglength		: 消息长度
 @Return				GAT_TRUE  	: 正确
					GAT_FALSE 	: 错误
					GAT_DATAFALSE	: 消息内容错误
**********************************************************************************************************/
u8 GAT_AckSetPulseUploadConfigWrite(u8 *msgdata, u16 msglength)
{
	u8 linkaddresslength = 0;																		//链路地址长度
	u16 channelbyte = 0, i = 0;
	Gat_AckFrame_Data tmpbuf;
	Gat_PulseUploadConfig_Data configparam;																//脉冲数据上传配置参数
	
	if (msglength < 2) {
		return GAT_DATAFALSE;
	}
	memset((u8 *)&configparam, 0x0, sizeof(configparam));													//清空缓存栈
	channelbyte = msglength - 1;
	
	if (GATConnect == GAT_ONLINE) {																	//已连接
		configparam.pulseUploadChannel |= msgdata[0];													//提取参数值
		for (i = 0; i < channelbyte; i++) {
			configparam.pulseUploadBit[i] |= msgdata[i + 1];
		}
		
		gatParamPulse.pulseUploadChannel = configparam.pulseUploadChannel;									//脉冲上传检测通道数
		for (i = 0; i < 16; i++) {
			gatParamPulse.pulseUploadBit[i] = configparam.pulseUploadBit[i];									//脉冲数据上传通道使能位
		}
		GAT_ParamSaveToFlash();																		//配置参数存入Flash
		
		tmpbuf.protocolnum = PROTOCOL_VER;																//协议版本
		tmpbuf.operationtype = OPERATIONTYPE_SET_ACK;													//操作类型->设置应答
		tmpbuf.objectid = OBJECTID_PULSE_UPLOADMODE;														//对象标识->脉冲数据上传模式
		
		linkaddresslength = GAT_LinkAddress_Send((u8 *)GATSendBuf);											//写入链路地址
		memcpy((void *)&GATSendBuf[linkaddresslength], (void *)&tmpbuf, sizeof(tmpbuf));						//写入配置参数设置应答表
		GATSendLength = linkaddresslength + sizeof(tmpbuf);												//写入发送数据长度
		
		return GAT_TRUE;
	}
	else {																						//未连接
		return GAT_FALSE;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_AckQueryPulseUploadConfigWrite(void)
 @Description			检测连接状态并将脉冲数据上传配置参数写入脉冲数据上传配置参数查询应答表到发送缓存区(连接写入, 未连接不写入)
 @Input				void
 @Return				GAT_TRUE  : 正确
					GAT_FALSE : 错误
**********************************************************************************************************/
u8 GAT_AckQueryPulseUploadConfigWrite(void)
{
	u8 i = 0;
	u8 linkaddresslength = 0;																		//链路地址长度
	Gat_PulseUploadParam_Data tmpbuf;
	
	memset((u8 *)&tmpbuf, 0x0, sizeof(tmpbuf));															//清空缓存栈
	
	if (GATConnect == GAT_ONLINE) {																	//已连接
		tmpbuf.AckHead.protocolnum = PROTOCOL_VER;														//协议版本
		tmpbuf.AckHead.operationtype = OPERATIONTYPE_QUERY_ACK;											//操作类型->查询应答
		tmpbuf.AckHead.objectid = OBJECTID_PULSE_UPLOADMODE;												//对象标识->脉冲数据上传模式
		
		tmpbuf.ParamPulseUpload.pulseUploadChannel = gatParamPulse.pulseUploadChannel;							//脉冲上传检测通道数
		for (i = 0; i < 16; i++) {
			tmpbuf.ParamPulseUpload.pulseUploadBit[i] = gatParamPulse.pulseUploadBit[i];						//脉冲数据上传通道使能位
		}
		
		linkaddresslength = GAT_LinkAddress_Send((u8 *)GATSendBuf);											//写入链路地址
		memcpy((void *)&GATSendBuf[linkaddresslength], (void *)&tmpbuf, sizeof(tmpbuf));						//写入配置参数查询应答表
		
		i = (gatParamPulse.pulseUploadChannel + 7) / 8;													//转换为检测通道字节数
		i += 1;																					//加检测通道数的1字节数
		
		GATSendLength = linkaddresslength + 3 + i;														//写入发送数据长度
		
		return GAT_TRUE;
	}
	else {																						//未连接
		return GAT_FALSE;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_ActiveUpLoadDataWrite(void)
 @Description			检测连接状态并将脉冲数据写入脉冲数据主动上传表到发送缓存区(连接写入, 未连接不写入)
 @Input				void
 @Return				GAT_TRUE  : 正确
					GAT_FALSE : 错误
**********************************************************************************************************/
u8 GAT_ActiveUpLoadDataWrite(void)
{
	u16 id = 0;
	u8 direction = 0;
	u8 channelnum = 0;
	u8 linkaddresslength = 0;																		//链路地址长度
	Gat_ActivePulseUpload_Data tmpbuf;
	
	memset((u8 *)&tmpbuf, 0x0, sizeof(tmpbuf));															//清空缓存栈
	
	if (GATConnect == GAT_ONLINE) {																	//已连接
		tmpbuf.AckHead.protocolnum = PROTOCOL_VER;														//协议版本
		tmpbuf.AckHead.operationtype = OPERATIONTYPE_ACTIVE_UPLOAD;											//操作类型->主动上传
		tmpbuf.AckHead.objectid = OBJECTID_PULSE_DATA;													//对象标识->脉冲数据
		
		if (GAT_UploadDequeueNomove(&id, &direction) == 1) {												//检查队列数据并提取队头数据
			return GAT_FALSE;
		}
		for (channelnum = 0; channelnum < DetectionChannels; channelnum++) {
			if (gatParamEquipment.output_ID[channelnum] == id) {
				break;
			}
		}
		channelnum = channelnum + 1;
		
		tmpbuf.ParamPulseMessage.pulseUploadChannelNo = channelnum;											//检测通道序号
		tmpbuf.ParamPulseMessage.pulseUploadCarInOut = direction;											//车辆方向
		
		linkaddresslength = GAT_LinkAddress_Send((u8 *)GATSendBuf);											//写入链路地址
		memcpy((void *)&GATSendBuf[linkaddresslength], (void *)&tmpbuf, sizeof(tmpbuf));						//写入脉冲数据主动上传表
		GATSendLength = linkaddresslength + sizeof(tmpbuf);												//写入发送数据长度
		
		return GAT_TRUE;
	}
	else {																						//未连接
		return GAT_FALSE;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_AckInspectUpLoadData(void)
 @Description			检测连接状态并根据脉冲数据主动上传应答表做处理
 @Input				void
 @Return				GAT_RECEIVEEVENT    : 正确
					GAT_FALSE 		: 错误
**********************************************************************************************************/
u8 GAT_AckInspectUpLoadData(void)
{
	u8 error = GAT_TRUE;
	
	if (GATConnect == GAT_ONLINE) {																	//已连接
		if (GATUPLOADAck == GAT_ReceivedAck) {															//并未主动上传而收到应答
			return GAT_FALSE;
		}
		else {
			error = GAT_UploadDequeueMove();															//脉冲数据队列(队列头偏移1)
			if (error != GAT_TRUE) {
				return GAT_FALSE;
			}
			GATUPLOADAck = GAT_ReceivedAck;															//收到主动上传应答
			gatOverTime = 0;
			gatRepeat = 0;
			
			return GAT_RECEIVEEVENT;
		}
	}
	else {																						//未连接
		return GAT_FALSE;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_ActiveStatisticalDataWrite(void)
 @Description			检测连接状态并将统计数据写入统计数据主动上传表到发送缓存区(连接写入, 未连接不写入)
 @Input				void
 @Return				GAT_TRUE  : 正确
					GAT_FALSE : 错误
**********************************************************************************************************/
u8 GAT_ActiveStatisticalDataWrite(void)
{
	u8 i = 0;
	u8 linkaddresslength = 0;																		//链路地址长度
	Gat_ActiveStatisticalUpload_Data tmpbuf;
	
	memset((u8 *)&tmpbuf, 0x0, sizeof(tmpbuf));															//清空缓存栈
	
	if (GATConnect == GAT_ONLINE) {																	//已连接
		tmpbuf.AckHead.protocolnum = PROTOCOL_VER;														//协议版本
		tmpbuf.AckHead.operationtype = OPERATIONTYPE_ACTIVE_UPLOAD;											//操作类型->主动上传
		tmpbuf.AckHead.objectid = OBJECTID_STATISTICAL;													//对象标识->统计数据
		/* 提取统计数据存储队列数据 */
		memset((u8 *)&gatStatisticalData, 0x0, sizeof(gatStatisticalData));									//清空缓存区
		if (GAT_StatisticalDequeueNomove((Gat_StatisticalData *)&gatStatisticalData) == 1) {						//检查队列数据并提取队头数据
			return GAT_FALSE;
		}
		/* 写入统计数据主动上传表 */
		tmpbuf.ParamStatisticalMessage.DateTime = gatStatisticalData.datetime;											//统计数据生成的本地时间
		tmpbuf.ParamStatisticalMessage.detector_config.statistical_period = gatParamDetector.detector_config.statistical_period;	//统计数据的计算周期
		tmpbuf.ParamStatisticalMessage.detector_config.Acar_langth = gatParamDetector.detector_config.Acar_langth;				//A类车车长
		tmpbuf.ParamStatisticalMessage.detector_config.Bcar_length = gatParamDetector.detector_config.Bcar_length;				//B类车车长
		tmpbuf.ParamStatisticalMessage.detector_config.Ccar_length = gatParamDetector.detector_config.Ccar_length;				//C类车车长
		tmpbuf.ParamStatisticalMessage.detector_config.reserved = gatParamDetector.detector_config.reserved;					//保留字段
		tmpbuf.ParamStatisticalMessage.detection_channels = gatStatisticalData.detection_channels;							//获取检测通道数
		
		for (i = 0; i < gatStatisticalData.detection_channels; i++) {
			tmpbuf.ParamStatisticalMessage.detector_statistical_data[i].detectionChannelNo = gatStatisticalData.detector_statistical_data[i].detectionChannelNo;
			tmpbuf.ParamStatisticalMessage.detector_statistical_data[i].AcarVolume = gatStatisticalData.detector_statistical_data[i].AcarVolume;
			tmpbuf.ParamStatisticalMessage.detector_statistical_data[i].BcarVolume = gatStatisticalData.detector_statistical_data[i].BcarVolume;
			tmpbuf.ParamStatisticalMessage.detector_statistical_data[i].CcarVolume = gatStatisticalData.detector_statistical_data[i].CcarVolume;
			tmpbuf.ParamStatisticalMessage.detector_statistical_data[i].avgShare = gatStatisticalData.detector_statistical_data[i].avgShare;
			tmpbuf.ParamStatisticalMessage.detector_statistical_data[i].avgSpeed = gatStatisticalData.detector_statistical_data[i].avgSpeed;
			tmpbuf.ParamStatisticalMessage.detector_statistical_data[i].avgLength = gatStatisticalData.detector_statistical_data[i].avgLength;
			tmpbuf.ParamStatisticalMessage.detector_statistical_data[i].avgHeadTime = gatStatisticalData.detector_statistical_data[i].avgHeadTime;
			tmpbuf.ParamStatisticalMessage.detector_statistical_data[i].queuingLength = gatStatisticalData.detector_statistical_data[i].queuingLength;
			tmpbuf.ParamStatisticalMessage.detector_statistical_data[i].reserved = gatStatisticalData.detector_statistical_data[i].reserved;
		}
		
		/* 统计数据主动上传表写入发送缓存区 */
		linkaddresslength = GAT_LinkAddress_Send((u8 *)GATSendBuf);											//写入链路地址
		memcpy((void *)&GATSendBuf[linkaddresslength], (void *)&tmpbuf, sizeof(tmpbuf));						//写入统计数据主动上传表
		/* 写入发送数据长度 */
		GATSendLength = linkaddresslength + sizeof(tmpbuf) - ((DetectionChannels - tmpbuf.ParamStatisticalMessage.detection_channels) * sizeof(Gat_DetectionChannel_Data));
		
		return GAT_TRUE;
	}
	else {																						//未连接
		return GAT_FALSE;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_AckInspectStatisticalData(void)
 @Description			检测连接状态并根据统计数据主动上传应答表做处理
 @Input				void
 @Return				GAT_RECEIVEEVENT    : 正确
					GAT_FALSE 		: 错误
**********************************************************************************************************/
u8 GAT_AckInspectStatisticalData(void)
{
	u8 error = GAT_TRUE;
	
	if (GATConnect == GAT_ONLINE) {																	//已连接
		if (GATUPLOADAck == GAT_ReceivedAck) {															//并未主动上传而收到应答
			return GAT_FALSE;
		}
		else {
			error = GAT_StatisticalDequeueMove();														//脉冲数据队列(队列头偏移1)
			if (error != GAT_TRUE) {
				return GAT_FALSE;
			}
			GATUPLOADAck = GAT_ReceivedAck;															//收到主动上传应答
			gatOverTime = 0;
			gatRepeat = 0;
			
			return GAT_RECEIVEEVENT;
		}
	}
	else {																						//未连接
		return GAT_FALSE;
	}
}

/********************************************** END OF FLEE **********************************************/
