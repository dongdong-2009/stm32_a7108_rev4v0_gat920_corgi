#ifndef __USR_GPRS_H
#define   __USR_GPRS_H

#include "stm32f10x_lib.h"
#include "platform_config.h"
#include "usrconfig.h"


u8* USRGPRS_CheckCmd(u8 *str);											//发送命令后, 检测接收到的应答
u8 USRGPRS_SendCmd(u8 *cmd, u8 *ack, u16 waittime);							//向USR GPRS发送命令


USRErrorCode USRGPRS_ModuleInit(void);										//USR GPRS 初始化模块
USRErrorCode USRGPRS_SocketNetConfig(void);									//USR GPRS 配置Socket连接参数

void USRGPRS_ReadModbusPrarm(void);										//读取数据Modbus设置
void USRGPRS_MonitorChange(void);											//监听GPRS参数是否改变


USRErrorCode USRGPRS_EnterATCmd(void);										//进入AT指令模式
USRErrorCode USRGPRS_ExitATCmd(void);										//退出AT指令模式
USRErrorCode USRGPRS_RestartModule(void);									//重启模块
USRErrorCode USRGPRS_EchoSet(u8 xEnable);									//设置回显
USRErrorCode USRGPRS_WorkModeSet(u8 mode);									//设置工作模式
USRErrorCode USRGPRS_CalenSet(u8 xEnable);									//设置是否使能通话功能
USRErrorCode USRGPRS_NatenSet(u8 xEnable);									//设置是否使能网络AT指令
USRErrorCode USRGPRS_UatenSet(u8 xEnable);									//设置是否使能透传模式下的串口AT指令
USRErrorCode USRGPRS_CachenSet(u8 xEnable);									//设置是否使能缓存数据
USRErrorCode USRGPRS_RestartTimeSet(void);									//设置模块自动重启时间
USRErrorCode USRGPRS_SleepSet(u8 xEnable);									//设置是进入低功耗模式
USRErrorCode USRGPRS_SleepTimeSet(void);									//设置模块自动进入低功耗时间
USRErrorCode USRGPRS_SaveModule(void);										//保存当前设置, 模块重启
USRErrorCode USRGPRS_ResetUserConfig(void);									//恢复用户默认设置, 模块重启
USRErrorCode USRGPRS_ResetConfig(void);										//恢复出厂设置, 模块重启
USRErrorCode USRGPRS_SocketASet(u8 mode, u8 *ipaddr, u8 *port);					//设置 SocketA 参数
USRErrorCode USRGPRS_SocketBSet(u8 mode, u8 *ipaddr, u8 *port);					//设置 SocketB 参数
USRErrorCode USRGPRS_SocketAEnSet(u8 xEnable);								//设置是否使能SocketA
USRErrorCode USRGPRS_SocketBEnSet(u8 xEnable);								//设置是否使能SocketB
USRErrorCode USRGPRS_SocketASLSet(u8 type);									//设置SocketA连接方式
USRErrorCode USRGPRS_SocketBSLSet(u8 type);									//设置SocketB连接方式
USRErrorCode USRGPRS_REGEnSet(u8 xEnable);									//设置是否使能注册包
USRErrorCode USRGPRS_HEARTEnSet(u8 xEnable);									//设置是否使能心跳包

#endif
