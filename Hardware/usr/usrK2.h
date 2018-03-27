#ifndef __USR_K2_H
#define   __USR_K2_H

#include "stm32f10x_lib.h"
#include "platform_config.h"
#include "usrconfig.h"


u8* USRK2_CheckCmd(u8 *str);												//发送命令后, 检测接收到的应答
u8 USRK2_SendCmd(u8 *cmd, u8 *ack, u16 waittime);								//向USR K2发送命令


void USRK2_ReadModbusPrarm(void);											//读取数据Modbus设置
void USRK2_MonitorChange(void);											//监听K2参数是否改变

USRErrorCode USRK2_ModuleInit(void);										//USR K2 初始化模块
USRErrorCode USRK2_SocketNetConfig(void);									//USR K2 配置Socket连接参数


USRErrorCode USRK2_EnterATCmd(void);										//进入AT指令模式
USRErrorCode USRK2_ExitATCmd(void);										//退出AT指令模式
USRErrorCode USRK2_RestartModule(void);										//重启模块
USRErrorCode USRK2_EchoSet(u8 xEnable);										//设置回显
USRErrorCode USRK2_RestoreSetting(void);									//恢复出厂设置
USRErrorCode USRK2_WANNSet(u8 mode, u8 *address, u8 *mask, u8 *gateway);			//设置 WANN 参数
USRErrorCode USRK2_SocketSet(u8 mode, u8 *ipaddr, u8 *port);					//设置 Socket 参数
USRErrorCode USRK2_SocketPortSet(u8 mode, u8 *port);							//设置本地端口号
USRErrorCode USRK2_RFCENSet(u8 xEnable);									//使能/禁止类RFC2217
USRErrorCode USRK2_CLIENTRSTSet(u8 xEnable);									//使能/禁止 TCP Client 多次发送失败 reset 功能

#endif
