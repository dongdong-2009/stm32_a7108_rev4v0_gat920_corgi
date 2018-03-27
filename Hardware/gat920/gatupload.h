#ifndef __GAT_UPLOAD_H
#define   __GAT_UPLOAD_H

#include "stm32f10x_lib.h"
#include "platform_config.h"
#include "gatconfig.h"


#define		UPLOADQUEUELENGTH		30									//脉冲数据存储队列长度
#define		STATISTICALLENGTH		10									//统计数据存储队列长度

#define		GATOVERTIME			2									//超时时间/秒
#define		GATREPEAT				3									//重发次数/次

typedef struct
{
	u16 id;															//地磁id号
	u8  direction;														//该地磁车辆方向 0 车检出 1 车检入
}Gat_UploadData;

typedef struct
{
	Gat_UploadData upload[UPLOADQUEUELENGTH];								//队中数据
	u8 front;															//队头
	u8 rear;															//队尾
}Gat_UploadQueue;														//脉冲数据存储队列


typedef struct
{
	u32 datetime;														//统计数据生成的本地时间
	u8  detection_channels;												//检测通道数    1 ~ 48
	Gat_DetectionChannel_Data detector_statistical_data[DetectionChannels];		//单路检测通道统计数据
}Gat_StatisticalData;

typedef struct
{
	Gat_StatisticalData statistical[STATISTICALLENGTH];						//队中数据
	u8 front;															//队头
	u8 rear;															//队尾
}Gat_StatisticalQueue;													//统计数据存储队列


extern Gat_UploadQueue			gatUploadQueus;							//脉冲数据存储队列
extern Gat_StatisticalQueue		gatStatisticalQueue;						//统计数据存储队列
extern Gat_StatisticalData		gatStatisticalData;							//队列中单条统计数据
extern volatile u8 gatOverTime;											//超时时间
extern volatile u8 gatRepeat;												//超时次数


void GAT_InitUploadQueue(void);											//初始化Upload队列
u8   GAT_UploadQueueisFull(void);											//检查队列是否已满
u8   GAT_UploadQueueisEmpty(void);											//检查队列是否为空
void GAT_UploadEnqueue(u16 id, u8 direction);								//脉冲数据写入队列
u8   GAT_UploadDequeue(u16 *id, u8 *direction);								//脉冲数据读出队列(队列头偏移1)
u8   GAT_UploadDequeueNomove(u16 *id, u8 *direction);							//脉冲数据读出队列(队列头不动)
u8   GAT_UploadDequeueMove(void);											//脉冲数据队列(队列头偏移1)

void GAT_UploadOvertime(void);											//主动上传接收应答超时处理(2秒内没收到应答再次发送,3次发送无应答断开连接)(运行在RTC秒中断)

void GAT_InitStatisticalQueue(void);										//初始化Statistical队列
u8   GAT_StatisticalQueueisFull(void);										//检查Statistical队列是否已满
u8   GAT_StatisticalQueueisEmpty(void);										//检查Statistical队列是否为空
void GAT_StatisticalEnqueue(Gat_StatisticalData *buf);							//统计数据写入队列
u8   GAT_StatisticalDequeue(Gat_StatisticalData *buf);							//统计数据读出队列(队列头偏移1)
u8   GAT_StatisticalDequeueNomove(Gat_StatisticalData *buf);					//统计数据读出队列(队列头不动)
u8   GAT_StatisticalDequeueMove(void);										//统计数据队列(队列头偏移1)

void GAT_ImplementEnqueue(u16 statisticaltime);								//到达统计时间将统计数据存入队列

#endif
