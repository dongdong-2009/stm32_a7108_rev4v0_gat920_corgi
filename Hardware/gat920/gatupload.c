/**
  *********************************************************************************************************
  * @file    gatupload.c
  * @author  MoveBroad -- KangYJ
  * @version V1.0
  * @date    
  * @brief   
  *********************************************************************************************************
  * @attention
  *			功能 : 
  *			1.  初始化Upload队列
  *			2.  检查队列是否已满
  *			3.  检查队列是否为空
  *			4.  脉冲数据写入队列
  *			5.  脉冲数据读出队列(队列头偏移1)
  *			6.  脉冲数据读出队列(队列头不动)
  *			7.  脉冲数据队列(队列头偏移1)
  *			8.  主动上传接收应答超时处理
  *			9.  初始化Statistical队列
  *			10. 检查Statistical队列是否已满
  *			11. 检查Statistical队列是否为空
  *			12. 统计数据写入队列
  *			13. 统计数据读出队列(队列头偏移1)
  *			14. 统计数据读出队列(队列头不动)
  *			15. 统计数据队列(队列头偏移1)
  *			16. 到达统计时间将统计数据存入队列
  *********************************************************************************************************
  */

#include "gatupload.h"
#include "calculationconfig.h"
#include "string.h"


Gat_UploadQueue			gatUploadQueus;								//脉冲数据存储队列
Gat_StatisticalQueue		gatStatisticalQueue;							//统计数据存储队列
Gat_StatisticalData			gatStatisticalData;								//队列中单条统计数据
volatile u8 gatOverTime = 0;
volatile u8 gatRepeat = 0;

/**********************************************************************************************************
 @Function			void GAT_InitUploadQueue(void)
 @Description			初始化Upload队列
 @Input				void
 @Return				void
**********************************************************************************************************/
void GAT_InitUploadQueue(void)
{
	gatUploadQueus.front = 0;
	gatUploadQueus.rear = 0;
	memset((u8 *)gatUploadQueus.upload, 0x0, sizeof(gatUploadQueus.upload));
}

/**********************************************************************************************************
 @Function			u8 GAT_UploadQueueisFull(void)
 @Description			检查Upload队列是否已满
 @Input				void
 @Return				0  : 没满
					1  : 已满
**********************************************************************************************************/
u8 GAT_UploadQueueisFull(void)
{
	if ((gatUploadQueus.rear + 1) % UPLOADQUEUELENGTH == gatUploadQueus.front) {
		return 1;
	}
	else {
		return 0;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_UploadQueueisEmpty(void)
 @Description			检查Upload队列是否为空
 @Input				void
 @Return				0  : 队列有数据
					1  : 队列空数据
**********************************************************************************************************/
u8 GAT_UploadQueueisEmpty(void)
{
	if (gatUploadQueus.front == gatUploadQueus.rear) {
		return 1;
	}
	else {
		return 0;
	}
}

/**********************************************************************************************************
 @Function			void GAT_UploadEnqueue(u16 id, u8 direction)
 @Description			脉冲数据写入队列
 @Input				id 		 : 地磁id号
					direction  : 该地磁车辆方向 0 车检出 1 车检入
 @Return				void
**********************************************************************************************************/
void GAT_UploadEnqueue(u16 id, u8 direction)
{
	if (GAT_UploadQueueisFull()) {										//队列已满
		gatUploadQueus.rear = (gatUploadQueus.rear + 1) % UPLOADQUEUELENGTH;		//队尾偏移1
		gatUploadQueus.upload[gatUploadQueus.rear].id = id;					//写入队列数据
		gatUploadQueus.upload[gatUploadQueus.rear].direction = direction;
		gatUploadQueus.front = (gatUploadQueus.front + 1) % UPLOADQUEUELENGTH;	//队头偏移1
	}
	else {															//队列未满
		gatUploadQueus.rear = (gatUploadQueus.rear + 1) % UPLOADQUEUELENGTH;		//队尾偏移1
		gatUploadQueus.upload[gatUploadQueus.rear].id = id;					//写入队列数据
		gatUploadQueus.upload[gatUploadQueus.rear].direction = direction;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_UploadDequeue(u16 *id, u8 *direction)
 @Description			脉冲数据读出队列(队列头偏移1)
 @Input				&id		 : 存放地磁id地址
					&direction : 存放该地磁车辆方向地址
 @Return				0 	 	 : 读取成功
					1  		 : 队列已空
**********************************************************************************************************/
u8 GAT_UploadDequeue(u16 *id, u8 *direction)
{
	if (GAT_UploadQueueisEmpty()) {										//队列已空
		return 1;
	}
	else {															//队列未空
		gatUploadQueus.front = (gatUploadQueus.front + 1) % UPLOADQUEUELENGTH;	//队头偏移1
		*id = gatUploadQueus.upload[gatUploadQueus.front].id;					//读取队头数据
		*direction = gatUploadQueus.upload[gatUploadQueus.front].direction;
		return 0;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_UploadDequeueNomove(u16 *id, u8 *direction)
 @Description			脉冲数据读出队列(队列头不动)
 @Input				&id		 : 存放地磁id地址
					&direction : 存放该地磁车辆方向地址
 @Return				0 	 	 : 读取成功
					1  		 : 队列已空
**********************************************************************************************************/
u8 GAT_UploadDequeueNomove(u16 *id, u8 *direction)
{
	u8 frontaddr = 0;
	
	if (GAT_UploadQueueisEmpty()) {										//队列已空
		return 1;
	}
	else {															//队列未空
		frontaddr = (gatUploadQueus.front + 1) % UPLOADQUEUELENGTH;
		*id = gatUploadQueus.upload[frontaddr].id;							//读取队头数据
		*direction = gatUploadQueus.upload[frontaddr].direction;
		return 0;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_UploadDequeueMove(void)
 @Description			脉冲数据队列(队列头偏移1)
 @Input				void
 @Return				0 	 	 : 偏移成功
					1  		 : 队列已空
**********************************************************************************************************/
u8 GAT_UploadDequeueMove(void)
{
	if (GAT_UploadQueueisEmpty()) {										//队列已空
		return 1;
	}
	else {															//队列未空
		gatUploadQueus.front = (gatUploadQueus.front + 1) % UPLOADQUEUELENGTH;	//队头偏移1
		return 0;
	}
}

/**********************************************************************************************************
 @Function			void GAT_UploadOvertime(void)
 @Description			主动上传接收应答超时处理(2秒内没收到应答再次发送,3次发送无应答断开连接)(运行在RTC秒中断)
 @Input				void
 @Return				void
**********************************************************************************************************/
void GAT_UploadOvertime(void)
{
	if (GATConnect == GAT_ONLINE) {
		if (GATUPLOADAck == GAT_NotReceivedAck) {							//等待接收应答中
			gatOverTime += 1;
			if (gatOverTime >= GATOVERTIME) {								//到达超时时间
				gatOverTime = 0;
				GATUPLOADAck = GAT_ReceivedAck;							//重发
				gatRepeat += 1;
				if (gatRepeat >= GATREPEAT) {
					gatRepeat = 0;
					GATConnect = GAT_OFFLINE;							//断连
				}
			}
		}
	}
	else {
		gatOverTime = 0;
		gatRepeat = 0;
	}
}

/**********************************************************************************************************
 @Function			void GAT_InitStatisticalQueue(void)
 @Description			初始化Statistical队列
 @Input				void
 @Return				void
**********************************************************************************************************/
void GAT_InitStatisticalQueue(void)
{
	gatStatisticalQueue.front = 0;
	gatStatisticalQueue.rear = 0;
	memset((u8 *)gatStatisticalQueue.statistical, 0x0, sizeof(gatStatisticalQueue.statistical));
}

/**********************************************************************************************************
 @Function			u8 GAT_StatisticalQueueisFull(void)
 @Description			检查Statistical队列是否已满
 @Input				void
 @Return				0  : 没满
					1  : 已满
**********************************************************************************************************/
u8 GAT_StatisticalQueueisFull(void)
{
	if ((gatStatisticalQueue.rear + 1) % STATISTICALLENGTH == gatStatisticalQueue.front) {
		return 1;
	}
	else {
		return 0;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_StatisticalQueueisEmpty(void)
 @Description			检查Statistical队列是否为空
 @Input				void
 @Return				0  : 队列有数据
					1  : 队列空数据
**********************************************************************************************************/
u8 GAT_StatisticalQueueisEmpty(void)
{
	if (gatStatisticalQueue.front == gatStatisticalQueue.rear) {
		return 1;
	}
	else {
		return 0;
	}
}

/**********************************************************************************************************
 @Function			void GAT_StatisticalEnqueue(Gat_StatisticalData *buf)
 @Description			统计数据写入队列
 @Input				Gat_StatisticalData *buf : 单条队列数据
 @Return				void
**********************************************************************************************************/
void GAT_StatisticalEnqueue(Gat_StatisticalData *buf)
{
	u8 i = 0;
	
	if (GAT_StatisticalQueueisFull()) {																//队列已满
		gatStatisticalQueue.rear = (gatStatisticalQueue.rear + 1) % STATISTICALLENGTH;							//队尾偏移1
		gatStatisticalQueue.statistical[gatStatisticalQueue.rear].datetime = buf->datetime;						//写入队列数据
		gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detection_channels = buf->detection_channels;
		for (i = 0; i < DetectionChannels; i++) {
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].detectionChannelNo = buf->detector_statistical_data[i].detectionChannelNo;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].AcarVolume = buf->detector_statistical_data[i].AcarVolume;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].BcarVolume = buf->detector_statistical_data[i].BcarVolume;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].CcarVolume = buf->detector_statistical_data[i].CcarVolume;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].avgShare = buf->detector_statistical_data[i].avgShare;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].avgSpeed = buf->detector_statistical_data[i].avgSpeed;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].avgLength = buf->detector_statistical_data[i].avgLength;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].avgHeadTime = buf->detector_statistical_data[i].avgHeadTime;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].queuingLength = buf->detector_statistical_data[i].queuingLength;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].reserved = buf->detector_statistical_data[i].reserved;
		}
		gatStatisticalQueue.front = (gatStatisticalQueue.front + 1) % STATISTICALLENGTH;						//队头偏移1
	}
	else {																						//队列未满
		gatStatisticalQueue.rear = (gatStatisticalQueue.rear + 1) % STATISTICALLENGTH;							//队尾偏移1
		gatStatisticalQueue.statistical[gatStatisticalQueue.rear].datetime = buf->datetime;						//写入队列数据
		gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detection_channels = buf->detection_channels;
		for (i = 0; i < DetectionChannels; i++) {
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].detectionChannelNo = buf->detector_statistical_data[i].detectionChannelNo;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].AcarVolume = buf->detector_statistical_data[i].AcarVolume;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].BcarVolume = buf->detector_statistical_data[i].BcarVolume;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].CcarVolume = buf->detector_statistical_data[i].CcarVolume;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].avgShare = buf->detector_statistical_data[i].avgShare;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].avgSpeed = buf->detector_statistical_data[i].avgSpeed;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].avgLength = buf->detector_statistical_data[i].avgLength;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].avgHeadTime = buf->detector_statistical_data[i].avgHeadTime;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].queuingLength = buf->detector_statistical_data[i].queuingLength;
			gatStatisticalQueue.statistical[gatStatisticalQueue.rear].detector_statistical_data[i].reserved = buf->detector_statistical_data[i].reserved;
		}
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_StatisticalDequeue(Gat_StatisticalData *buf)
 @Description			统计数据读出队列(队列头偏移1)
 @Input				Gat_StatisticalData *buf : 单条队列数据
 @Return				0 	 	 : 读取成功
					1  		 : 队列已空
**********************************************************************************************************/
u8 GAT_StatisticalDequeue(Gat_StatisticalData *buf)
{
	u8 i = 0;
	
	if (GAT_StatisticalQueueisEmpty()) {																//队列已空
		return 1;
	}
	else {																						//队列未空
		gatStatisticalQueue.front = (gatStatisticalQueue.front + 1) % STATISTICALLENGTH;						//队头偏移1
		buf->datetime = gatStatisticalQueue.statistical[gatStatisticalQueue.front].datetime;						//读取队头数据
		buf->detection_channels = gatStatisticalQueue.statistical[gatStatisticalQueue.front].detection_channels;
		for (i = 0; i < DetectionChannels; i++) {
			buf->detector_statistical_data[i].detectionChannelNo = gatStatisticalQueue.statistical[gatStatisticalQueue.front].detector_statistical_data[i].detectionChannelNo;
			buf->detector_statistical_data[i].AcarVolume = gatStatisticalQueue.statistical[gatStatisticalQueue.front].detector_statistical_data[i].AcarVolume;
			buf->detector_statistical_data[i].BcarVolume = gatStatisticalQueue.statistical[gatStatisticalQueue.front].detector_statistical_data[i].BcarVolume;
			buf->detector_statistical_data[i].CcarVolume = gatStatisticalQueue.statistical[gatStatisticalQueue.front].detector_statistical_data[i].CcarVolume;
			buf->detector_statistical_data[i].avgShare = gatStatisticalQueue.statistical[gatStatisticalQueue.front].detector_statistical_data[i].avgShare;
			buf->detector_statistical_data[i].avgSpeed = gatStatisticalQueue.statistical[gatStatisticalQueue.front].detector_statistical_data[i].avgSpeed;
			buf->detector_statistical_data[i].avgLength = gatStatisticalQueue.statistical[gatStatisticalQueue.front].detector_statistical_data[i].avgLength;
			buf->detector_statistical_data[i].avgHeadTime = gatStatisticalQueue.statistical[gatStatisticalQueue.front].detector_statistical_data[i].avgHeadTime;
			buf->detector_statistical_data[i].queuingLength = gatStatisticalQueue.statistical[gatStatisticalQueue.front].detector_statistical_data[i].queuingLength;
			buf->detector_statistical_data[i].reserved = gatStatisticalQueue.statistical[gatStatisticalQueue.front].detector_statistical_data[i].reserved;
		}
		return 0;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_StatisticalDequeueNomove(Gat_StatisticalData *buf)
 @Description			统计数据读出队列(队列头不动)
 @Input				Gat_StatisticalData *buf : 单条队列数据
 @Return				0 	 	 : 读取成功
					1  		 : 队列已空
**********************************************************************************************************/
u8 GAT_StatisticalDequeueNomove(Gat_StatisticalData *buf)
{
	u8 i = 0;
	u8 frontaddr = 0;
	
	if (GAT_StatisticalQueueisEmpty()) {																//队列已空
		return 1;
	}
	else {																						//队列未空
		frontaddr = (gatStatisticalQueue.front + 1) % STATISTICALLENGTH;
		buf->datetime = gatStatisticalQueue.statistical[frontaddr].datetime;									//读取队头数据
		buf->detection_channels = gatStatisticalQueue.statistical[frontaddr].detection_channels;
		for (i = 0; i < DetectionChannels; i++) {
			buf->detector_statistical_data[i].detectionChannelNo = gatStatisticalQueue.statistical[frontaddr].detector_statistical_data[i].detectionChannelNo;
			buf->detector_statistical_data[i].AcarVolume = gatStatisticalQueue.statistical[frontaddr].detector_statistical_data[i].AcarVolume;
			buf->detector_statistical_data[i].BcarVolume = gatStatisticalQueue.statistical[frontaddr].detector_statistical_data[i].BcarVolume;
			buf->detector_statistical_data[i].CcarVolume = gatStatisticalQueue.statistical[frontaddr].detector_statistical_data[i].CcarVolume;
			buf->detector_statistical_data[i].avgShare = gatStatisticalQueue.statistical[frontaddr].detector_statistical_data[i].avgShare;
			buf->detector_statistical_data[i].avgSpeed = gatStatisticalQueue.statistical[frontaddr].detector_statistical_data[i].avgSpeed;
			buf->detector_statistical_data[i].avgLength = gatStatisticalQueue.statistical[frontaddr].detector_statistical_data[i].avgLength;
			buf->detector_statistical_data[i].avgHeadTime = gatStatisticalQueue.statistical[frontaddr].detector_statistical_data[i].avgHeadTime;
			buf->detector_statistical_data[i].queuingLength = gatStatisticalQueue.statistical[frontaddr].detector_statistical_data[i].queuingLength;
			buf->detector_statistical_data[i].reserved = gatStatisticalQueue.statistical[frontaddr].detector_statistical_data[i].reserved;
		}
		return 0;
	}
}

/**********************************************************************************************************
 @Function			u8 GAT_StatisticalDequeueMove(void)
 @Description			统计数据队列(队列头偏移1)
 @Input				void
 @Return				0 	 	 : 偏移成功
					1  		 : 队列已空
**********************************************************************************************************/
u8 GAT_StatisticalDequeueMove(void)
{
	if (GAT_StatisticalQueueisEmpty()) {																//队列已空
		return 1;
	}
	else {																						//队列未空
		gatStatisticalQueue.front = (gatStatisticalQueue.front + 1) % STATISTICALLENGTH;						//队头偏移1
		return 0;
	}
}

/**********************************************************************************************************
 @Function			void GAT_ImplementEnqueue(u16 statisticaltime)
 @Description			到达统计时间将统计数据存入队列
 @Input				statisticaltime : 统计时长
 @Return				void
**********************************************************************************************************/
void GAT_ImplementEnqueue(u16 statisticaltime)
{
	static u16 gatstatisticaltime = 0;																	//gat统计时间计时器
	u8 channelsnum = 0;																				//gat检测通道数
	u16 volume = 0;																				//车流量
	u16 avgoccupancyval = 0;																			//平均占有时间
	u16 avgheadtimeval = 0;																			//平均车头时距
	u8 i = 0;
	
	if (statisticaltime != 0) {																		//统计时间需不为0
		gatstatisticaltime += 1;
		if (gatstatisticaltime >= statisticaltime) {														//到达统计时间
			gatstatisticaltime = 0;
			
			memset((u8 *)&gatStatisticalData, 0x0, sizeof(gatStatisticalData));								//清空缓存区
			gatStatisticalData.datetime = RTC_GetCounter();												//获取统计数据生成的本地时间
			for (i = 0; i < DetectionChannels; i++) {													//获取配置通道id的通道数
				if (gatParamEquipment.output_ID[i] != 0x0) {
					channelsnum += 1;
				}
			}
			gatStatisticalData.detection_channels = channelsnum;											//获取检测通道数
			for (i = 0; i < DetectionChannels; i++) {													//获取各个通道统计数据
				if (gatParamEquipment.output_ID[i] != 0x0) {
					
					volume = calculation_dev.ReadVolume(gatParamEquipment.output_ID[i]);						//获取车流量值
					if (volume > 255) {
						volume = 255;
					}
					
					avgoccupancyval = calculation_dev.ReadAvgOccupancy(gatParamEquipment.output_ID[i]);			//获取平均占有时间
					avgoccupancyval = avgoccupancyval / statisticaltime / 500;								//转换为平均占有率
					
					avgheadtimeval = calculation_dev.ReadAvgHeadTime(gatParamEquipment.output_ID[i]);			//获取平均车头时距
					avgheadtimeval = avgheadtimeval / 1000;												//转换单位为秒
					if (avgheadtimeval > 255) {
						avgheadtimeval = 255;
					}
					
					gatStatisticalData.detector_statistical_data[i].detectionChannelNo = i + 1;				//检测通道编号
					gatStatisticalData.detector_statistical_data[i].AcarVolume = 0;							//不区分车型C类车为总车流量
					gatStatisticalData.detector_statistical_data[i].BcarVolume = 0;
					gatStatisticalData.detector_statistical_data[i].CcarVolume = volume;
					gatStatisticalData.detector_statistical_data[i].avgShare = avgoccupancyval;
					gatStatisticalData.detector_statistical_data[i].avgSpeed = 0;
					gatStatisticalData.detector_statistical_data[i].avgLength = 0;
					gatStatisticalData.detector_statistical_data[i].avgHeadTime = avgheadtimeval;
					gatStatisticalData.detector_statistical_data[i].queuingLength = 0;
				}
			}
			/* 将队列中单条统计数据存入统计数据存储队列 */
			GAT_StatisticalEnqueue((Gat_StatisticalData *)&gatStatisticalData);
		}
	}
}

/********************************************** END OF FLEE **********************************************/
