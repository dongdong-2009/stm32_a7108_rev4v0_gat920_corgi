#ifndef __SOCKET_PARK_H
#define   __SOCKET_PARK_H

#include "stm32f10x_lib.h"
#include "platform_config.h"


u16 SOCKET_ParkObtainPacketHead(u8 *addr);									//读取SocketPark包头数据并填入缓存
u16 SOCKET_ParkObtainPacketData(u8 *addr);									//读取SocketPark停车场数据包并填入缓存

void SOCKET_ParkImplement(u8 laneNo, u16 CarNum);								//Socket数据实时上传
void SOCKET_ParkImplementHeartbeat(u8 *buf);									//Socket数据实时上传(心跳包上传数据)

#endif
