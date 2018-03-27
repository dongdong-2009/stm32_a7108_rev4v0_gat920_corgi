#ifndef __SOCKET_FUNC_H
#define   __SOCKET_FUNC_H

#include "stm32f10x_lib.h"
#include "socketconfig.h"

extern Socket_Packet_Head			SocketHeadPacket;						//Socket数据包头
extern Socket_Packet_Data			SocketDataPacket[OUTPUT_MAX];				//Socket流量数据包集
extern volatile u8 SocketSendBuf[SOCKET_CACHE_SIZE];							//Socket发送数据缓存
extern volatile u8 SocketReceiveBuf[SOCKET_RECVIVE_SIZE];						//Socket接收数据缓存


u16  SOCKET_ObtainPacketHead(u8 *addr);										//读取Socket包头数据并填入缓存
u16  SOCKET_ObtainPacketData(u8 *addr);										//读取Socket流量数据包并填入缓存
u16  SOCKET_ObtainPacketManuCheck(u8 *addr, u16 buflength);						//计算校验码并填入生产商编码和校验码


u8   SOCKET_ConvertCrossID(u8 *addr, u32 crossid);							//将crossid转换为字符格式并填入缓存
u8   SOCKET_ConvertPacketInfo(u8 *addr, u32 packetinfo);						//将packetinfo转换并填入缓存
u8   SOCKET_ConvertInterval(u8 *addr, u16 interval);							//将interval转换并填入缓存
u8   SOCKET_ConvertLaneNo(u8 *addr, u16 lanno);								//将lanno转换为字符格式并填入缓存
u8   SOCKET_ConvertDateTime(u8 *addr, u32 rtccounter);							//将系统时间转换为字符格式并填入缓存
u8   SOCKET_Convert2Byte(u8 *addr, u16 data);								//将2Byte数据转换并填入缓存
u8   SOCKET_Convert4Byte(u8 *addr, float data);								//将float数据转换并填入缓存
u8	SOCKET_Convert4ByteInt(u8 *addr, u32 data);								//将Int数据转换并填入缓存

void SOCKET_GetOutputID(u16 *outputid);										//读取output_ID输出端口的参数到Socket流量数据包
u16  SOCKET_PackagedStorage(void);											//Socket将数据打包存入缓存区

#endif
