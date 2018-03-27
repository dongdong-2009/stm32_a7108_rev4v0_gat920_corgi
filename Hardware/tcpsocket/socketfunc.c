/**
  *********************************************************************************************************
  * @file    socketfunc.c
  * @author  MoveBroad -- KangYJ
  * @version V1.0
  * @date    
  * @brief   socket内部协议
  *********************************************************************************************************
  * @attention
  *			功能 : 
  *			1.  读取Socket包头数据并填入缓存					(外部调用)
  *			2.  读取Socket流量数据包并填入缓存					(外部调用)
  *			3.  计算校验码并填入生产商编码和校验码				(外部调用)
  *			4.  读取output_ID输出端口的参数到Socket流量数据包		(外部调用)
  *			5.  将crossid转换为字符格式并填入缓存				(内部调用)
  *			6.  将packetinfo转换并填入缓存					(内部调用)
  *			7.  将interval转换并填入缓存						(内部调用)
  *			8.  将lanno转换为字符格式并填入缓存				(内部调用)
  *			9.  将系统时间转换为字符格式并填入缓存				(内部调用)
  *			10. 将2Byte数据转换并填入缓存						(内部调用)
  *			11. 将float数据转换并填入缓存						(内部调用)
  *
  *********************************************************************************************************
  */

#include "socketfunc.h"
#include "rtc.h"


Socket_Packet_Head			SocketHeadPacket;								//Socket数据包头
Socket_Packet_Data			SocketDataPacket[OUTPUT_MAX];						//Socket流量数据包集
volatile u8 SocketSendBuf[SOCKET_CACHE_SIZE];								//Socket发送数据缓存
volatile u8 SocketReceiveBuf[SOCKET_RECVIVE_SIZE];							//Socket接收数据缓存


/**********************************************************************************************************
 @Function			u16 SOCKET_ObtainPacketHead(u8 *addr)
 @Description			读取Socket包头数据并填入缓存
 @Input				*addr		: Socket缓存地址
 @Return				填入缓存数据长度
					SOCKET_FALSE   : 出错
**********************************************************************************************************/
u16 SOCKET_ObtainPacketHead(u8 *addr)
{
	u16 headlength = 0;																				//填入缓存数据长度
	
	if ((SOCKET_ConvertCrossID((u8 *)&addr[headlength], SocketHeadPacket.CrossID)) == SOCKET_FALSE) {				//填入CrossID
		return SOCKET_FALSE;
	}
	else {
		headlength += 8;																			//地址偏移8byte
	}
	
	addr[headlength] = SocketHeadPacket.PacketType;														//填入流水包类型
	headlength += 1;																				//地址偏移1byte
	
	if ((SOCKET_ConvertPacketInfo((u8 *)&addr[headlength], SocketHeadPacket.PacketInfo)) == SOCKET_FALSE) {			//填入输出流量数据包数
		return SOCKET_FALSE;
	}
	else {
		headlength += 4;																			//地址偏移4byte
	}
	
	return headlength;
}

/**********************************************************************************************************
 @Function			u16 SOCKET_ObtainPacketData(u8 *addr)
 @Description			读取Socket流量数据包并填入缓存
 @Input				*addr		: Socket缓存地址
 @Return				填入缓存数据长度
					SOCKET_FALSE   : 出错
**********************************************************************************************************/
u16 SOCKET_ObtainPacketData(u8 *addr)
{
	u8 i = 0;
	u16 datalength = 0;																				//填入缓存数据长度
	
	datalength += 13;																				//地址偏移出数据包头
	
	for (i = 0; i < OUTPUT_MAX; i++) {																	//遍历找寻配置过的车道号
		if (SocketDataPacket[i].OutputID != 0) {
			
			addr[datalength] = SocketDataPacket[i].DeviceType;													//填入设备编号
			datalength += 1;																		//地址偏移1byte
			
			if ((SOCKET_ConvertInterval((u8 *)&addr[datalength], SocketDataPacket[i].Interval)) == SOCKET_FALSE) {			//填入统计时长
				return SOCKET_FALSE;
			}
			else {
				datalength += 2;																	//地址偏移2byte
			}
			
			if ((SOCKET_ConvertLaneNo((u8 *)&addr[datalength], SocketDataPacket[i].LaneNo)) == SOCKET_FALSE) {				//填入车道号
				return SOCKET_FALSE;
			}
			else {
				datalength += 3;																	//地址偏移3byte
			}
			
			if ((SOCKET_ConvertDateTime((u8 *)&addr[datalength], SocketDataPacket[i].DateTime)) == SOCKET_FALSE) {			//填入流水发生时间
				return SOCKET_FALSE;
			}
			else {
				datalength += 20;																	//地址偏移20byte
			}
			
			if ((SOCKET_Convert2Byte((u8 *)&addr[datalength], SocketDataPacket[i].Volume)) == SOCKET_FALSE) {				//填入一分通过车辆
				return SOCKET_FALSE;
			}
			else {
				datalength += 2;																	//地址偏移2byte
			}
			
			if ((SOCKET_Convert2Byte((u8 *)&addr[datalength], SocketDataPacket[i].Volume1)) == SOCKET_FALSE) {				//填入微小车辆数量
				return SOCKET_FALSE;
			}
			else {
				datalength += 2;																	//地址偏移2byte
			}
			
			if ((SOCKET_Convert2Byte((u8 *)&addr[datalength], SocketDataPacket[i].Volume2)) == SOCKET_FALSE) {				//填入小车数量
				return SOCKET_FALSE;
			}
			else {
				datalength += 2;																	//地址偏移2byte
			}
			
			if ((SOCKET_Convert2Byte((u8 *)&addr[datalength], SocketDataPacket[i].Volume3)) == SOCKET_FALSE) {				//填入中车数量
				return SOCKET_FALSE;
			}
			else {
				datalength += 2;																	//地址偏移2byte
			}
			
			if ((SOCKET_Convert2Byte((u8 *)&addr[datalength], SocketDataPacket[i].Volume4)) == SOCKET_FALSE) {				//填入大车数量
				return SOCKET_FALSE;
			}
			else {
				datalength += 2;																	//地址偏移2byte
			}
			
			if ((SOCKET_Convert2Byte((u8 *)&addr[datalength], SocketDataPacket[i].Volume5)) == SOCKET_FALSE) {				//填入超大车数量
				return SOCKET_FALSE;
			}
			else {
				datalength += 2;																	//地址偏移2byte
			}
			
			if ((SOCKET_Convert4ByteInt((u8 *)&addr[datalength], SocketDataPacket[i].AvgOccupancy)) == SOCKET_FALSE) {		//填入平均占有时间
				return SOCKET_FALSE;
			}
			else {
				datalength += 4;																	//地址偏移4byte
			}
			
			if ((SOCKET_Convert4ByteInt((u8 *)&addr[datalength], SocketDataPacket[i].AvgHeadTime)) == SOCKET_FALSE) {		//填入平均车头时距
				return SOCKET_FALSE;
			}
			else {
				datalength += 4;																	//地址偏移4byte
			}
			
			if ((SOCKET_Convert4Byte((u8 *)&addr[datalength], SocketDataPacket[i].AvgLength)) == SOCKET_FALSE) {			//填入平均车长
				return SOCKET_FALSE;
			}
			else {
				datalength += 4;																	//地址偏移4byte
			}
			
			if ((SOCKET_Convert4Byte((u8 *)&addr[datalength], SocketDataPacket[i].AvgSpeed)) == SOCKET_FALSE) {				//填入平均速度
				return SOCKET_FALSE;
			}
			else {
				datalength += 4;																	//地址偏移4byte
			}
			
			addr[datalength] = SocketDataPacket[i].Saturation;													//填入饱和度
			datalength += 1;																		//地址偏移1byte
			
			if ((SOCKET_Convert2Byte((u8 *)&addr[datalength], SocketDataPacket[i].Density)) == SOCKET_FALSE) {				//填入密度
				return SOCKET_FALSE;
			}
			else {
				datalength += 2;																	//地址偏移2byte
			}
			
			if ((SOCKET_Convert2Byte((u8 *)&addr[datalength], SocketDataPacket[i].Pcu)) == SOCKET_FALSE) {					//填入当量小汽车
				return SOCKET_FALSE;
			}
			else {
				datalength += 2;																	//地址偏移2byte
			}
			
			if ((SOCKET_Convert4Byte((u8 *)&addr[datalength], SocketDataPacket[i].AvgQueueLength)) == SOCKET_FALSE) {		//填入排队长度
				return SOCKET_FALSE;
			}
			else {
				datalength += 4;																	//地址偏移4byte
			}
		}
	}
	
	datalength -= 13;																				//减去数据包头13byte
	
	return datalength;
}

/**********************************************************************************************************
 @Function			u16 SOCKET_ObtainPacketManuCheck(u8 *addr, u16 buflength)
 @Description			计算校验码并填入生产商编码和校验码
 @Input				*addr		: Socket缓存地址
					buflength		: 数据长度
 @Return				生产商编码和校验码长度
**********************************************************************************************************/
u16 SOCKET_ObtainPacketManuCheck(u8 *addr, u16 buflength)
{
	u8 checkcode = 0;
	u16 i = 0;
	u16 dateindex = 0;
	
	dateindex += 13;													//地址偏移出数据包头
	checkcode = addr[dateindex];
	dateindex += 1;
	for (i = dateindex; i < buflength; i++) {								//计算校验码
		checkcode = checkcode ^ addr[i];
	}
	
	addr[buflength] = MANUFACTURERCODE;									//填入生产商编码
	addr[buflength + 1] = checkcode;										//填入校验码
	
	return 2;															//生产商编码和校验码字节数
}

/**********************************************************************************************************
 @Function			u8 SOCKET_ConvertCrossID(u8 *addr, u32 crossid)
 @Description			将crossid转换为字符格式并填入缓存
 @Input				*addr		: 存放转换后数据的地址
					crossid		: CrossId值
 @Return				1			: 错误
					0			: 正常
**********************************************************************************************************/
u8 SOCKET_ConvertCrossID(u8 *addr, u32 crossid)
{
	u8 i = 0;
	u8 tempbuf[8];
	
	if (crossid > 99999999) {											//检测CrossId值是否错误
		return SOCKET_FALSE;
	}
	
	tempbuf[0] = (crossid / 10000000) + '0';
	tempbuf[1] = ((crossid % 10000000) / 1000000) + '0';
	tempbuf[2] = ((crossid % 1000000) / 100000) + '0';
	tempbuf[3] = ((crossid % 100000) / 10000) + '0';
	tempbuf[4] = ((crossid % 10000) / 1000) + '0';
	tempbuf[5] = ((crossid % 1000) / 100) + '0';
	tempbuf[6] = ((crossid % 100) / 10) + '0';
	tempbuf[7] = (crossid % 10) + '0';
	
	for (i = 0; i < 8; i++) {
		addr[i] = tempbuf[i];
	}
	
	return SOCKET_TRUE;
}

/**********************************************************************************************************
 @Function			u8 SOCKET_ConvertPacketInfo(u8 *addr, u32 packetinfo)
 @Description			将packetinfo转换并填入缓存
 @Input				*addr		: 存放转换后数据的地址
					packetinfo	: PacketInfo值
 @Return				1			: 错误
					0			: 正常
**********************************************************************************************************/
u8 SOCKET_ConvertPacketInfo(u8 *addr, u32 packetinfo)
{
	u8 i = 0;
	u8 tempbuf[4];
	
	tempbuf[0] = packetinfo & 0xff;
	tempbuf[1] = (packetinfo >> 8) & 0xff;
	tempbuf[2] = (packetinfo >> 16) & 0xff;
	tempbuf[3] = packetinfo >> 24;
	
	for (i = 0; i < 4; i++) {
		addr[i] = tempbuf[i];
	}
	
	return SOCKET_TRUE;
}

/**********************************************************************************************************
 @Function			u8 SOCKET_ConvertInterval(u8 *addr, u16 interval)
 @Description			将interval转换并填入缓存
 @Input				*addr		: 存放转换后数据的地址
					interval		: Interval值
 @Return				1			: 错误
					0			: 正常
**********************************************************************************************************/
u8 SOCKET_ConvertInterval(u8 *addr, u16 interval)
{
	u8 i = 0;
	u8 tempbuf[2];
	
	tempbuf[0] = interval & 0xff;
	tempbuf[1] = (interval >> 8) & 0xff;

	for (i = 0; i < 2; i++) {
		addr[i] = tempbuf[i];
	}
	
	return SOCKET_TRUE;
}

/**********************************************************************************************************
 @Function			u8 SOCKET_ConvertLaneNo(u8 *addr, u16 lanno)
 @Description			将lanno转换为字符格式并填入缓存
 @Input				*addr		: 存放转换后数据的地址
					lanno		: LaneNo值
 @Return				1			: 错误
					0			: 正常
**********************************************************************************************************/
u8 SOCKET_ConvertLaneNo(u8 *addr, u16 lanno)
{
	u8 i = 0;
	u8 tempbuf[3];
	
	if (lanno > 999) {													//检测LaneNo值是否错误
		return SOCKET_FALSE;
	}
	
	tempbuf[0] = (lanno / 100) + '0';
	tempbuf[1] = ((lanno % 100) / 10) + '0';
	tempbuf[2] = (lanno % 10) + '0';
	
	for (i = 0; i < 3; i++) {
		addr[i] = tempbuf[i];
	}
	
	return SOCKET_TRUE;
}

/**********************************************************************************************************
 @Function			u8 SOCKET_ConvertDateTime(u8 *addr, u32 rtccounter)
 @Description			将系统时间转换为字符格式并填入缓存
 @Input				*addr		: 存放转换后数据的地址
					rtccounter	: RTC计数器值
 @Return				1			: 错误
					0			: 正常
**********************************************************************************************************/
u8 SOCKET_ConvertDateTime(u8 *addr, u32 rtccounter)
{
	u8 i = 0;
	u8 tempbuf[20];
	
	to_tm(rtccounter, &systemtime);										//RTC计数器值转换为时间结构体
	
	tempbuf[0] = (systemtime.tm_year / 1000) + '0';
	tempbuf[1] = ((systemtime.tm_year % 1000) / 100) + '0';
	tempbuf[2] = ((systemtime.tm_year % 100) / 10) + '0';
	tempbuf[3] = (systemtime.tm_year % 10) + '0';
	tempbuf[4] = 0x2d;
	tempbuf[5] = (systemtime.tm_mon / 10) + '0';
	tempbuf[6] = (systemtime.tm_mon % 10) + '0';
	tempbuf[7] = 0x2d;
	tempbuf[8] = (systemtime.tm_mday / 10) + '0';
	tempbuf[9] = (systemtime.tm_mday % 10) + '0';
	tempbuf[10] = 0x20;
	tempbuf[11] = (systemtime.tm_hour / 10) + '0';
	tempbuf[12] = (systemtime.tm_hour % 10) + '0';
	tempbuf[13] = 0x3a;
	tempbuf[14] = (systemtime.tm_min / 10) + '0';
	tempbuf[15] = (systemtime.tm_min % 10) + '0';
	tempbuf[16] = 0x3a;
	tempbuf[17] = (systemtime.tm_sec / 10) + '0';
	tempbuf[18] = (systemtime.tm_sec % 10) + '0';
	tempbuf[19] = 0x00;
	
	for (i = 0; i < 20; i++) {
		addr[i] = tempbuf[i];
	}
	
	return SOCKET_TRUE;
}

/**********************************************************************************************************
 @Function			u8 SOCKET_Convert2Byte(u8 *addr, u16 data)
 @Description			将2Byte数据转换并填入缓存
 @Input				*addr		: 存放转换后数据的地址
					data			: 数值
 @Return				1			: 错误
					0			: 正常
**********************************************************************************************************/
u8 SOCKET_Convert2Byte(u8 *addr, u16 data)
{
	u8 i = 0;
	u8 tempbuf[2];
	
	tempbuf[0] = data & 0xff;
	tempbuf[1] = (data >> 8) & 0xff;

	for (i = 0; i < 2; i++) {
		addr[i] = tempbuf[i];
	}
	
	return SOCKET_TRUE;
}

/**********************************************************************************************************
 @Function			u8 SOCKET_Convert4Byte(u8 *addr, float data)
 @Description			将float数据转换并填入缓存
 @Input				*addr		: 存放转换后数据的地址
					data			: 数值
 @Return				1			: 错误
					0			: 正常
**********************************************************************************************************/
u8 SOCKET_Convert4Byte(u8 *addr, float data)
{
	u8 i = 0;
	u8 tempbuf[4];
	float temp, *ptemp;
	u32 *ptemp1;
	
	temp = data;
	ptemp = &temp;
	ptemp1 = (u32 *)ptemp;
	
	tempbuf[0] = *ptemp1 & 0xff;
	tempbuf[1] = (*ptemp1 >> 8) & 0xff;
	tempbuf[2] = (*ptemp1 >> 16) & 0xff;
	tempbuf[3] = *ptemp1 >> 24;
	
	for (i = 0; i < 4; i++) {
		addr[i] = tempbuf[i];
	}
	
	return SOCKET_TRUE;
}

/**********************************************************************************************************
 @Function			u8 SOCKET_Convert4ByteInt(u8 *addr, u32 data)
 @Description			将Int数据转换并填入缓存
 @Input				*addr		: 存放转换后数据的地址
					data			: 数值
 @Return				1			: 错误
					0			: 正常
**********************************************************************************************************/
u8 SOCKET_Convert4ByteInt(u8 *addr, u32 data)
{
	u8 i = 0;
	u8 tempbuf[4];
	
	tempbuf[0] = data & 0xff;
	tempbuf[1] = (data >> 8) & 0xff;
	tempbuf[2] = (data >> 16) & 0xff;
	tempbuf[3] = data >> 24;
	
	for (i = 0; i < 4; i++) {
		addr[i] = tempbuf[i];
	}
	
	return SOCKET_TRUE;
}

/**********************************************************************************************************
 @Function			void SOCKET_GetOutputID(u16 *outputid)
 @Description			读取output_ID输出端口的参数到Socket流量数据包
 @Input				Output_ID存放地址
 @Return				void
**********************************************************************************************************/
void SOCKET_GetOutputID(u16 *outputid)
{
	u8 i = 0;
	
	for (i = 0; i < OUTPUT_MAX; i++) {
		SocketDataPacket[i].OutputID = outputid[i];
	}
}

/**********************************************************************************************************
 @Function			u16 SOCKET_PackagedStorage(void)
 @Description			Socket将数据打包存入缓存区
 @Input				void
 @Return				u16 SendLength		: 发送数据长度
**********************************************************************************************************/
u16 SOCKET_PackagedStorage(void)
{
	u16 headlength = 0;
	u16 datalength = 0;
	
	socket_dev.FillData();												//将数据填入各个数据包
	headlength = socket_dev.ObtainPacketHead((u8 *)SocketSendBuf);				//读取Socket包头数据并填入缓存
	datalength = socket_dev.ObtainPacketData((u8 *)SocketSendBuf);				//读取Socket流量数据包并填入缓存
	headlength = headlength + datalength;
	datalength = socket_dev.ObtainPacketManuCheck((u8 *)SocketSendBuf, headlength);
	headlength = headlength + datalength;

	return headlength;
}

/********************************************** END OF FLEE **********************************************/
