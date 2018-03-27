/**
  *********************************************************************************************************
  * @file    gatfunc.c
  * @author  MoveBroad -- KangYJ
  * @version V1.0
  * @date    
  * @brief   
  *********************************************************************************************************
  * @attention
  *			功能 : 
  *			1.  从接收到的数据中提取数据帧
  *			2.  将数据表转换为数据帧
  *			3.  将数据帧转换为数据表
  *			4.  从数据表中提取消息内容
  *			5.  检测器链路地址发送
  *			6.  检测器接收链路地址读取
  *			7.  检测校验码是否正确-----------错误类型 1
  *			8.  检测协议版本是否兼容---------错误类型 2
  *			9.  检测消息类型是否正确---------错误类型 3
  *			10. 检测消息内容数据是否正确-----错误类型 4
  *			11. 获取操作类型值
  *			12. 获取对象标识值
  *********************************************************************************************************
  */

#include "gatfunc.h"
#include "string.h"


/**********************************************************************************************************
 @Function			u8 GAT_LinkAddress_Send(u8 *linkaddress)
 @Description			检测器链路地址发送
 @Input				存放检测器链路数据的地址
 @Return				0 : ERROR
					1 : 单字节字节数
					2 : 双字节字节数
**********************************************************************************************************/
u8 GAT_LinkAddress_Send(u8 *linkaddress)
{
	u8 ubyte = 0;
	u8 i;
	u8 tmpbuf[2] = {0x00, 0x00};
	u16 linknum = LINKADDRESS;
	
	if (linknum > 8191) {												//超出范围
		return 0;
	}
	else if (linknum <= 63) {											//单字节
		tmpbuf[0] |= 0x01;												//第一位置1
		tmpbuf[0] |= ((linknum << 2) & 0xfc);								//高6位赋值
		ubyte = 1;
	}
	else {															//双字节
		tmpbuf[0] |= ((linknum << 2) & 0xfc);								//低字节高6位赋值
		tmpbuf[1] |= 0x01;												//高字节第一位置1
		tmpbuf[1] |= ((linknum >> 5) & 0xfe);
		ubyte = 2;
	}
	
	for (i = 0; i < ubyte; i++) {
		linkaddress[i] = tmpbuf[i];
	}
	
	return ubyte;
}

/**********************************************************************************************************
 @Function			u16 GAT_LinkAddress_Receive(u8 *data_sheet)
 @Description			检测器接收数据表中链路地址读取
 @Input				*data_sheet   				: 数据表地址
 @Return				链路地址值
**********************************************************************************************************/
u16 GAT_LinkAddress_Receive(u8 *data_sheet)
{
	u16 linknum = 0;
	
	if ((data_sheet[0] & 0x01) == 1) {										//链路地址为单字节
		linknum |= ((data_sheet[0] >> 2) & 0x3f);
	}
	else {															//链路地址为双字节
		linknum |= ((data_sheet[0] >> 2) & 0x3f);
		linknum |= ((data_sheet[1] >> 1) & 0x7f) << 6;
	}
	
	return linknum;
}

/**********************************************************************************************************
 @Function			u16 GAT_ReceiveBufToDataFrame(u8 *receive_buf, u16 receive_length)
 @Description			从接收到的数据中提取数据帧
 @Input				*receive_buf   			: 接收数据缓存区
					data_length 				: 接收数据长度
 @Return				packed_frame_length 		: 数据帧长度
**********************************************************************************************************/
u16 GAT_ReceiveBufToDataFrame(u8 *receive_buf, u16 receive_length)
{
	u8 tmpbuf[GAT_CACHE_SIZE];
	u8 startflag = 0;
	u16 packed_frame_length = 0, i = 0, j = 0;
	
	for (i = 0; i < receive_length; i++) {
		if ((receive_buf[i] == 0x7E) && (startflag == 0)) {					//帧开始
			tmpbuf[j++] = receive_buf[i];
			startflag = 1;
		}
		else if ((receive_buf[i] == 0x7E) && (startflag == 1)) {				//帧结束
			tmpbuf[j++] = receive_buf[i];
			startflag = 0;
			break;
		}
		else if ((receive_buf[i] != 0x7E) && (startflag == 1)) {				//帧中数据
			tmpbuf[j++] = receive_buf[i];
		}
	}
	
	memset(receive_buf, 0x0, receive_length);
	if ((j != 0) && (startflag == 0)) {									//接收到数据
		for (i = 0; i < j; i++) {
			receive_buf[i] = tmpbuf[i];
		}
		packed_frame_length = j;
	}
	
	return packed_frame_length;
}

/**********************************************************************************************************
 @Function			u16 GAT_DataSheetToDataFrame(u8 *data_sheet, u16 data_length)
 @Description			将数据表转换为数据帧
 @Input				*data_sheet   				: 数据表地址(此地址指向数据由数据表转为数据帧)
					data_length 				: 数据表长度
 @Return				packed_frame_length 		: 数据帧长度
**********************************************************************************************************/
u16 GAT_DataSheetToDataFrame(u8 *data_sheet, u16 data_length)
{
	u8 tmpbuf[GAT_CACHE_SIZE];
	u8 check_num;
	u16 packed_frame_length = 0, i = 0, j =0;
	
	//获取数据表校验码
	check_num = data_sheet[0];
	for (i = 1; i < data_length; i++) {
		check_num = check_num ^ data_sheet[i];
	}
	
	//将数据表中0x7e,0x7d分别替换成0x7d,0x5e和0x7d,0x5d
	for (i = 0; i < data_length; i++) {
		if (data_sheet[i] == 0x7e) {
			tmpbuf[j++] = 0x7d;
			tmpbuf[j++] = 0x5e;
		}
		else if (data_sheet[i] == 0x7d) {
			tmpbuf[j++] = 0x7d;
			tmpbuf[j++] = 0x5d;
		}
		else {
			tmpbuf[j++] = data_sheet[i];
		}
	}
	
	//如果校验码值为0x7e,0x7d分别替换成0x7d,0x5e和0x7d,0x5d
	if (check_num == 0x7e) {
		tmpbuf[j++] = 0x7d;
		tmpbuf[j++] = 0x5e;
	}
	else if (check_num == 0x7d) {
		tmpbuf[j++] = 0x7d;
		tmpbuf[j++] = 0x5d;
	}
	else {
		tmpbuf[j++] = check_num;
	}
	
	//数据帧加头尾0x7e
	data_sheet[0] = 0x7e;
	for (i = 0; i < j; i++) {
		data_sheet[i + 1] = tmpbuf[i];
	}
	data_sheet[j + 1] = 0x7e;
	j = j + 2;
	
	packed_frame_length = j;
	
	return packed_frame_length;
}

/**********************************************************************************************************
 @Function			u16 GAT_DataFrameToDataSheet(u8 *data_frame, u16 data_length)
 @Description			将数据帧转换为数据表
 @Input				*data_frame  	 		: 数据帧地址(此地址指向数据由数据帧转为数据表)
					data_length			: 数据帧长度
 @Return				sheet_length	    other : 数据表长度
									   0 : 错误
**********************************************************************************************************/
u16 GAT_DataFrameToDataSheet(u8 *data_frame, u16 data_length)
{
	u8 tmpbuf[GAT_CACHE_SIZE];
	u16 sheet_length = data_length;
	u16 i, j = 0;
	
	//检测数据帧头帧尾是否为0x7e
	if ((data_frame[0] != 0x7e) || (data_frame[data_length - 1] != 0x7e)) {
		return 0;
	}
	else {
		sheet_length -= 2;
	}
	
	//检测校验码是否为1字节或2字节
	if ((data_frame[data_length - 2] == 0x5e) || (data_frame[data_length - 2] == 0x5d))
	{
		if (data_frame[data_length - 3] == 0x7d) {
			sheet_length -= 2;
		}
		else {
			sheet_length -= 1;
		}
	}
	else {
		sheet_length -= 1;
	}
	
	//检测数据表中0x7d,0x5e和0x7d,0x5d替换为0x7e和0x7d
	for (i = 0; i < sheet_length; i++) {
		if (data_frame[i + 1] == 0x7d) {
			if (data_frame[i + 2] == 0x5e) {
				tmpbuf[j++] = 0x7e;
				i++;
			}
			else if (data_frame[i + 2] == 0x5d) {
				tmpbuf[j++] = 0x7d;
				i++;
			}
			else {
				tmpbuf[j++] = data_frame[i + 1];
			}
		}
		else {
			tmpbuf[j++] = data_frame[i + 1];
		}
	}
	
	//数据重写入缓存
	for (i = 0; i < j; i++) {
		data_frame[i] = tmpbuf[i];
	}
	
	sheet_length = j;
	
	return sheet_length;
}

/**********************************************************************************************************
 @Function			u16 GAT_DataSheetToMessage(u8 *data_sheet, u16 data_length)
 @Description			从数据表中提取消息内容
 @Input				*data_sheet  	 		: 数据表地址(此地址指向数据由数据表转为消息内容)
					data_length			: 数据表长度
 @Return				message_length			: 消息内容长度
**********************************************************************************************************/
u16 GAT_DataSheetToMessage(u8 *data_sheet, u16 data_length)
{
	u8 tmpbuf[GAT_CACHE_SIZE];
	u16 message_length = 0;
	u16 message_index = 0;
	u16 i = 0;
	
	message_length = data_length;
	
	if ((data_sheet[0] & 0x01) == 1) {										//链路地址为单字节
		message_index += 1;
		message_length -= 1;
	}
	else {															//链路地址为双字节
		message_index += 2;
		message_length -= 2;
	}
	message_index += 3;
	message_length -= 3;
	
	for (i = 0; i < message_length; i++) {
		tmpbuf[i] = data_sheet[message_index + i];
	}
	for (i = 0; i < message_length; i++) {
		data_sheet[i] = tmpbuf[i];
	}
	
	return message_length;
}

/**********************************************************************************************************
 @Function			u8 GAT_CheckLinkAddress(u8 *data_sheet, u16 linknum)
 @Description			检测链路地址是否为本机链路地址-----错误类型 0
 @Input				*data_sheet  	 		: 数据表地址
					linknum				: 设备链路地址值
 @Return								1	: 错误
									0	: 正常
**********************************************************************************************************/
u8 GAT_CheckLinkAddress(u8 *data_sheet, u16 linknum)
{
	u8 error = GAT_TRUE;
	u16 RecvLinkAdd = 0;
	
	RecvLinkAdd = GAT_LinkAddress_Receive(data_sheet);
	//接收到的链路地址值与设备链路地址值比较
	if (RecvLinkAdd != linknum) {
		error = GAT_FALSE;
	}
	else {
		error = GAT_TRUE;
	}
	
	return error;
}

/**********************************************************************************************************
 @Function			u8 GAT_CheckCode(u8 *data_frame, u16 data_length)
 @Description			检测校验码是否正确-----错误类型 1
 @Input				*data_frame  	 		: 数据帧地址
					data_length			: 数据帧长度
 @Return								1	: 错误
									0	: 正常
**********************************************************************************************************/
u8 GAT_CheckCode(u8 *data_frame, u16 data_length)
{
	u8 error = GAT_TRUE;
	u8 tmpbuf[GAT_CACHE_SIZE];
	u8 checkcode = 0;
	u8 checknum = 0;
	u16 sheet_length = data_length;
	u16 i, j = 0;
	
	//检测校验码是否为1字节或2字节,并提取校验码
	if ((data_frame[data_length - 2] == 0x5e) || (data_frame[data_length - 2] == 0x5d))
	{
		if (data_frame[data_length - 3] == 0x7d) {
			if (data_frame[data_length - 2] == 0x5e) {
				checkcode = 0x7e;
			}
			else if (data_frame[data_length - 2] == 0x5d) {
				checkcode = 0x7d;
			}
		}
		else {
			checkcode = data_frame[data_length - 2];
		}
	}
	else {
		checkcode = data_frame[data_length - 2];
	}
	
	//检测数据帧头帧尾是否为0x7e
	if ((data_frame[0] != 0x7e) || (data_frame[data_length - 1] != 0x7e)) {
		return GAT_FALSE;
	}
	else {
		sheet_length -= 2;
	}
	
	//检测校验码是否为1字节或2字节
	if ((data_frame[data_length - 2] == 0x5e) || (data_frame[data_length - 2] == 0x5d))
	{
		if (data_frame[data_length - 3] == 0x7d) {
			sheet_length -= 2;
		}
		else {
			sheet_length -= 1;
		}
	}
	else {
		sheet_length -= 1;
	}
	
	//检测数据表中0x7d,0x5e和0x7d,0x5d替换为0x7e和0x7d
	for (i = 0; i < sheet_length; i++) {
		if (data_frame[i + 1] == 0x7d) {
			if (data_frame[i + 2] == 0x5e) {
				tmpbuf[j++] = 0x7e;
				i++;
			}
			else if (data_frame[i + 2] == 0x5d) {
				tmpbuf[j++] = 0x7d;
				i++;
			}
			else {
				tmpbuf[j++] = data_frame[i + 1];
			}
		}
		else {
			tmpbuf[j++] = data_frame[i + 1];
		}
	}
	
	//计算数据表的校验码
	checknum = tmpbuf[0];
	for (i = 1; i < j; i++) {
		checknum = checknum ^ tmpbuf[i];
	}
	
	if (checkcode != checknum) {
		error = GAT_FALSE;
	}
	else {
		error = GAT_TRUE;
	}
	
	return error;
}

/**********************************************************************************************************
 @Function			u8 GAT_CheckVersion(u8 *data_sheet)
 @Description			检测协议版本是否兼容-----错误类型 2
 @Input				*data_sheet  	 		: 数据表地址
 @Return								1	: 不兼容
									0	: 兼容
**********************************************************************************************************/
u8 GAT_CheckVersion(u8 *data_sheet)
{
	u8 error = GAT_TRUE;
	u8 protocolnum = 0;
	
	//提取数据表中协议版本号
	if ((data_sheet[0] & 0x01) == 1) {										//链路地址为单字节
		protocolnum = data_sheet[1];
	}
	else {															//链路地址为双字节
		protocolnum = data_sheet[2];
	}
	
	//比较版本号
	if (protocolnum != PROTOCOL_VER) {
		error = GAT_FALSE;
	}
	else {
		error = GAT_TRUE;
	}
	
	return error;
}

/**********************************************************************************************************
 @Function			u8 GAT_CheckVersion(u8 *data_sheet)
 @Description			检测消息类型是否正确-----错误类型 3
 @Input				*data_sheet  	 		: 数据表地址
 @Return								1	: 错误
									0	: 正常
**********************************************************************************************************/
u8 GAT_CheckMessageType(u8 *data_sheet)
{
	u8 error = GAT_TRUE;
	u8 operationtype = 0;												//操作类型
	u8 objectid = 0;													//对象标识
	
	//提取操作类型和对象标识
	if ((data_sheet[0] & 0x01) == 1) {										//链路地址为单字节
		operationtype = data_sheet[2];
		objectid = data_sheet[3];
	}
	else {															//链路地址为双字节
		operationtype = data_sheet[3];
		objectid = data_sheet[4];
	}
	
	//比较操作类型是否正确
	if ( (operationtype == OPERATIONTYPE_QUERY_REQUEST) || (operationtype == OPERATIONTYPE_SET_REQUEST) ||
		(operationtype == OPERATIONTYPE_ACTIVE_UPLOAD) || (operationtype == OPERATIONTYPE_QUERY_ACK) ||
		(operationtype == OPERATIONTYPE_SET_ACK) || (operationtype == OPERATIONTYPE_ACTIVE_UPLOAD_ACK) || 
		(operationtype == OPERATIONTYPE_ERROR_ACK) )
	{
		error = GAT_TRUE;
	}
	else {
		return GAT_FALSE;
	}
	
	//比较对象标识是否正确
	if ( (objectid == OBJECTID_ONLINE) || (objectid == OBJECTID_TIME) || (objectid == OBJECTID_BAUD) ||
		(objectid == OBJECTID_CONFIG_PARAM) || (objectid == OBJECTID_STATISTICAL) || (objectid == OBJECTID_HISTORICAL) ||
		(objectid == OBJECTID_PULSE_UPLOADMODE) || (objectid == OBJECTID_PULSE_DATA) || (objectid == OBJECTID_ERROR_MESSAGE) )
	{
		error = GAT_TRUE;
	}
	else {
		return GAT_FALSE;
	}
	
	return error;
}

/**********************************************************************************************************
 @Function			u8 GAT_CheckMessageContent(u8 *data_sheet, u16 data_length)
 @Description			检测消息内容数据是否正确-----错误类型 4
 @Input				*data_sheet  	 		: 数据表地址
					data_length 			: 数据表长度
 @Return								1	: 错误
									0	: 正常
**********************************************************************************************************/
u8 GAT_CheckMessageContent(u8 *data_sheet, u16 data_length)
{
	u8 operationtype = 0;												//操作类型
	u8 objectid = 0;													//对象标识
	u8 detectionchannel = 0;												//检测通道数
	u16 messagelength = data_length;										//消息内容长度
	
	//提取操作类型和对象标识
	if ((data_sheet[0] & 0x01) == 1) {										//链路地址为单字节
		operationtype = data_sheet[2];
		objectid = data_sheet[3];
		messagelength -= 1;												//去除链路地址长度
	}
	else {															//链路地址为双字节
		operationtype = data_sheet[3];
		objectid = data_sheet[4];
		messagelength -= 2;												//去除链路地址长度
	}
	messagelength -= 3;													//去除协议版本号、操作类型、对象标识 3个字节长度
	
	/* 信号机发送连接请求数据表, 消息内容 0 字节 */
	if ((operationtype == OPERATIONTYPE_SET_REQUEST) && (objectid == OBJECTID_ONLINE)) {
		if (messagelength != 0) {
			return GAT_FALSE;
		}
		else {
			return GAT_TRUE;
		}
	}
	
	/* 信号机发送连接查询数据表, 消息内容 0 字节 */
	if ((operationtype == OPERATIONTYPE_QUERY_REQUEST) && (objectid == OBJECTID_ONLINE)) {
		if (messagelength != 0) {
			return GAT_FALSE;
		}
		else {
			return GAT_TRUE;
		}
	}
	
	/* 信号机发送时间设置数据表, 消息内容 4 字节 */
	if ((operationtype == OPERATIONTYPE_SET_REQUEST) && (objectid == OBJECTID_TIME)) {
		if (messagelength != 4) {
			return GAT_FALSE;
		}
		else {
			return GAT_TRUE;
		}
	}
	
	/* 信号机发送时间查询数据表, 消息内容 0 字节 */
	if ((operationtype == OPERATIONTYPE_QUERY_REQUEST) && (objectid == OBJECTID_TIME)) {
		if (messagelength != 0) {
			return GAT_FALSE;
		}
		else {
			return GAT_TRUE;
		}
	}
	
	/* 信号机发送波特率设置数据表, 消息内容 4 字节 */
	if ((operationtype == OPERATIONTYPE_SET_REQUEST) && (objectid == OBJECTID_BAUD)) {
		if (messagelength != 4) {
			return GAT_FALSE;
		}
		else {
			return GAT_TRUE;
		}
	}
	
	/* 信号机发送配置参数设置数据表, 消息内容 9 字节 */
	if ((operationtype == OPERATIONTYPE_SET_REQUEST) && (objectid == OBJECTID_CONFIG_PARAM)) {
		if (messagelength != 9) {
			return GAT_FALSE;
		}
		else {
			return GAT_TRUE;
		}
	}
	
	/* 信号机发送配置参数查询数据表, 消息内容 0 字节 */
	if ((operationtype == OPERATIONTYPE_QUERY_REQUEST) && (objectid == OBJECTID_CONFIG_PARAM)) {
		if (messagelength != 0) {
			return GAT_FALSE;
		}
		else {
			return GAT_TRUE;
		}
	}
	
	/* 信号机发送统计数据主动上传应答数据表, 消息内容 0 字节 */
	if ((operationtype == OPERATIONTYPE_ACTIVE_UPLOAD_ACK) && (objectid == OBJECTID_STATISTICAL)) {
		if (messagelength != 0) {
			return GAT_FALSE;
		}
		else {
			return GAT_TRUE;
		}
	}
	
	/* 信号机发送历史数据查询数据表, 消息内容 8 字节 */
	if ((operationtype == OPERATIONTYPE_QUERY_REQUEST) && (objectid == OBJECTID_HISTORICAL)) {
		if (messagelength != 8) {
			return GAT_FALSE;
		}
		else {
			return GAT_TRUE;
		}
	}
	
	/* 信号机发送脉冲数据上传模式设置数据表, 消息内容 ((N + 7) / 8) + 1 字节 最少2字节 */
	if ((operationtype == OPERATIONTYPE_SET_REQUEST) && (objectid == OBJECTID_PULSE_UPLOADMODE)) {
		if (messagelength < 2) {
			return GAT_FALSE;
		}
		else {
			if ((data_sheet[0] & 0x01) == 1) {										//链路地址为单字节
				detectionchannel = data_sheet[4];
			}
			else {															//链路地址为双字节
				detectionchannel = data_sheet[5];
			}
			if ((detectionchannel == 0) || (detectionchannel > 128)) {					//范围判断
				return GAT_FALSE;
			}
			detectionchannel = (detectionchannel + 7) / 8;							//转换为检测通道字节数
			detectionchannel += 1;												//加检测通道数的1字节数
			
			if (messagelength != detectionchannel) {
				return GAT_FALSE;
			}
			else {
				return GAT_TRUE;
			}
		}
	}
	
	/* 信号机发送脉冲数据上传模式查询数据表, 消息内容 0 字节 */
	if ((operationtype == OPERATIONTYPE_QUERY_REQUEST) && (objectid == OBJECTID_PULSE_UPLOADMODE)) {
		if (messagelength != 0) {
			return GAT_FALSE;
		}
		else {
			return GAT_TRUE;
		}
	}
	
	/* 信号机发送脉冲数据主动上传应答数据表, 消息内容 0 字节 */
	if ((operationtype == OPERATIONTYPE_ACTIVE_UPLOAD_ACK) && (objectid == OBJECTID_PULSE_DATA)) {
		if (messagelength != 0) {
			return GAT_FALSE;
		}
		else {
			return GAT_TRUE;
		}
	}
	
	/* 信号机发送检测器故障主动上传应答数据表, 消息内容 0 字节 */
	if ((operationtype == OPERATIONTYPE_ACTIVE_UPLOAD_ACK) && (objectid == OBJECTID_ERROR_MESSAGE)) {
		if (messagelength != 0) {
			return GAT_FALSE;
		}
		else {
			return GAT_TRUE;
		}
	}
	
	return GAT_FALSE;
}

/**********************************************************************************************************
 @Function			u8 GAT_GatOperationType(u8 *data_frame)
 @Description			获取操作类型值
 @Input				*data_sheet  	 		: 数据表地址
 @Return				operationtype			: 操作类型(0x80 ~ 0x86)
**********************************************************************************************************/
u8 GAT_GatOperationType(u8 *data_sheet)
{
	u8 operationtype = 0;
	
	//提取操作类型值
	if ((data_sheet[0] & 0x01) == 1) {										//链路地址为单字节
		operationtype = data_sheet[2];
	}
	else {															//链路地址为双字节
		operationtype = data_sheet[3];
	}
	
	return operationtype;
}

/**********************************************************************************************************
 @Function			u8 GAT_GatObjectId(u8 *data_sheet)
 @Description			获取对象标识值
 @Input				*data_sheet  	 		: 数据表地址
 @Return				objectid				: 对象标识(0x01 ~ 0x09)
**********************************************************************************************************/
u8 GAT_GatObjectId(u8 *data_sheet)
{
	u8 objectid = 0;
	
	//提取对象标识值
	if ((data_sheet[0] & 0x01) == 1) {										//链路地址为单字节
		objectid = data_sheet[3];
	}
	else {															//链路地址为双字节
		objectid = data_sheet[4];
	}
	
	return objectid;
}

/********************************************** END OF FLEE **********************************************/
