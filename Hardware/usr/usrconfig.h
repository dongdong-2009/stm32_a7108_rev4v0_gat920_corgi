#ifndef __USR_CONFIG_H
#define   __USR_CONFIG_H

#include "stm32f10x_lib.h"
#include "platform_config.h"

extern mvb_param_net_config USRCheckChange;										//检查是否需要配置GPRS/RJ45

typedef enum
{
	USR_ENOERR,						//No Error
	USR_EPORTINITERR,					//Serial Port Initialized Error
	USR_ENTERATERR,					//EnterATCmd Error
	USR_ATEXECUTEERR,					//AT Cmd Execute Error
	USR_MODULEINITERR,					//Module Initialized Error
	USR_NETCONFIGERR					//Net Config Error
}USRErrorCode;

typedef enum
{
	USR_INITCONFIGOVER,					//USR配置结束
	USR_INITCONFIGING					//USR配置中
}USRInitializedType;					//USR配置中需暂停Socket通信
extern USRInitializedType USRInitialized;


/* USRGPRS外部接口控制管理器 */
struct _m_usrgprs_dev
{
	USRErrorCode	(*EnterATCmd)(void);										//进入AT指令模式
	USRErrorCode	(*ExitATCmd)(void);											//退出AT指令模式
	USRErrorCode	(*RestartModule)(void);										//重启模块
	USRErrorCode	(*EchoSet)(u8 xEnable);										//设置回显
	USRErrorCode	(*WorkModeSet)(u8 mode);										//设置工作模式
	USRErrorCode	(*CalenSet)(u8 xEnable);										//设置是否使能通话功能
	USRErrorCode	(*NatenSet)(u8 xEnable);										//设置是否使能网络AT指令
	USRErrorCode	(*UatenSet)(u8 xEnable);										//设置是否使能透传模式下的串口AT指令
	USRErrorCode	(*CachenSet)(u8 xEnable);									//设置是否使能缓存数据
	USRErrorCode	(*RestartTimeSet)(void);										//设置模块自动重启时间
	USRErrorCode	(*SleepSet)(u8 xEnable);										//设置是进入低功耗模式
	USRErrorCode	(*SleepTimeSet)(void);										//设置模块自动进入低功耗时间
	USRErrorCode	(*SaveModule)(void);										//保存当前设置, 模块重启
	USRErrorCode	(*ResetUserConfig)(void);									//恢复用户默认设置, 模块重启
	USRErrorCode	(*ResetConfig)(void);										//恢复出厂设置, 模块重启
	USRErrorCode	(*SocketASet)(u8 mode, u8 *ipaddr, u8 *port);					//设置 SocketA 参数
	USRErrorCode	(*SocketBSet)(u8 mode, u8 *ipaddr, u8 *port);					//设置 SocketB 参数
	USRErrorCode	(*SocketAEnSet)(u8 xEnable);									//设置是否使能SocketA
	USRErrorCode	(*SocketBEnSet)(u8 xEnable);									//设置是否使能SocketB
	USRErrorCode	(*SocketASLSet)(u8 type);									//设置SocketA连接方式
	USRErrorCode	(*SocketBSLSet)(u8 type);									//设置SocketB连接方式
	USRErrorCode	(*REGEnSet)(u8 xEnable);										//设置是否使能注册包
	USRErrorCode	(*HEARTEnSet)(u8 xEnable);									//设置是否使能心跳包
	
	USRErrorCode	(*ModuleInit)(void);										//USR GPRS 初始化模块
	USRErrorCode	(*SocketNetConfig)(void);									//USR GPRS 配置Socket连接参数
	
	void			(*ReadModbusPrarm)(void);									//读取数据Modbus设置
	void			(*MonitorChange)(void);										//监听GPRS参数是否改变
};
extern struct _m_usrgprs_dev USR_GRPS_Dev;

/* USRK2外部接口控制管理器 */
struct _m_usrk2_dev
{
	USRErrorCode	(*EnterATCmd)(void);										//进入AT指令模式
	USRErrorCode	(*ExitATCmd)(void);											//退出AT指令模式
	USRErrorCode	(*RestartModule)(void);										//重启模块
	USRErrorCode	(*EchoSet)(u8 xEnable);										//设置回显
	USRErrorCode	(*RestoreSetting)(void);										//恢复出厂设置
	USRErrorCode	(*WANNSet)(u8 mode, u8 *address, u8 *mask, u8 *gateway);			//设置 WANN 参数
	USRErrorCode	(*SocketSet)(u8 mode, u8 *ipaddr, u8 *port);						//设置 Socket 参数
	USRErrorCode	(*SocketPortSet)(u8 mode, u8 *port);							//设置本地端口号
	USRErrorCode	(*RFCENSet)(u8 xEnable);										//使能/禁止类RFC2217
	USRErrorCode	(*CLIENTRSTSet)(u8 xEnable);									//使能/禁止 TCP Client 多次发送失败 reset 功能
	
	USRErrorCode	(*ModuleInit)(void);										//USR K2 初始化模块
	USRErrorCode	(*SocketNetConfig)(void);									//USR K2 配置Socket连接参数
	
	void			(*ReadModbusPrarm)(void);									//读取数据Modbus设置
	void			(*MonitorChange)(void);										//监听K2参数是否改变
};
extern struct _m_usrk2_dev USR_K2_Dev;

#endif
