#ifndef __GAT_FUNC_H
#define   __GAT_FUNC_H

#include "stm32f10x_lib.h"
#include "platform_config.h"
#include "gatconfig.h"


u16 GAT_ReceiveBufToDataFrame(u8 *receive_buf, u16 receive_length);				//从接收到的数据中提取数据帧
u16 GAT_DataSheetToDataFrame(u8 *data_sheet, u16 data_length);					//将数据表转换为数据帧
u16 GAT_DataFrameToDataSheet(u8 *data_frame, u16 data_length);					//将数据帧转换为数据表
u16 GAT_DataSheetToMessage(u8 *data_sheet, u16 data_length);					//从数据表中提取消息内容

u8  GAT_LinkAddress_Send(u8 *linkaddress);									//检测器链路地址发送
u16 GAT_LinkAddress_Receive(u8 *data_sheet);									//检测器接收链路地址读取

u8  GAT_CheckLinkAddress(u8 *data_sheet, u16 linknum);							//检测链路地址是否为本机链路地址-----错误类型 0
u8  GAT_CheckCode(u8 *data_frame, u16 data_length);							//检测校验码是否正确-----------------错误类型 1
u8  GAT_CheckVersion(u8 *data_sheet);										//检测协议版本是否兼容---------------错误类型 2
u8  GAT_CheckMessageType(u8 *data_sheet);									//检测消息类型是否正确---------------错误类型 3
u8  GAT_CheckMessageContent(u8 *data_sheet, u16 data_length);					//检测消息内容数据是否正确-----------错误类型 4

u8  GAT_GatOperationType(u8 *data_sheet);									//获取操作类型值
u8  GAT_GatObjectId(u8 *data_sheet);										//获取对象标识值

#endif
