/**
  *********************************************************************************************************
  * @file    usrconfig.c
  * @author  MoveBroad -- KangYJ
  * @version V1.0
  * @date    
  * @brief   USR串口转网口顶层外部调用接口文件
  *********************************************************************************************************
  * @attention
  *		
  *			
  *********************************************************************************************************
  */

#include "usrconfig.h"
#include "usrgprs.h"
#include "usrk2.h"
#include "usrserial.h"


USRInitializedType USRInitialized = USR_INITCONFIGOVER;						//USR配置标记(配置中需暂停Socket通信), 默认没配置USR
mvb_param_net_config USRCheckChange;										//检查是否需要配置GPRS/RJ45


/* gat920外部接口例化各函数 */
struct _m_usrgprs_dev USR_GRPS_Dev = 
{
	USRGPRS_EnterATCmd,													//进入AT指令模式
	USRGPRS_ExitATCmd,													//退出AT指令模式
	USRGPRS_RestartModule,												//重启模块
	USRGPRS_EchoSet,													//设置回显
	USRGPRS_WorkModeSet,												//设置工作模式
	USRGPRS_CalenSet,													//设置是否使能通话功能
	USRGPRS_NatenSet,													//设置是否使能网络AT指令
	USRGPRS_UatenSet,													//设置是否使能透传模式下的串口AT指令
	USRGPRS_CachenSet,													//设置是否使能缓存数据
	USRGPRS_RestartTimeSet,												//设置模块自动重启时间
	USRGPRS_SleepSet,													//设置是进入低功耗模式
	USRGPRS_SleepTimeSet,												//设置模块自动进入低功耗时间
	USRGPRS_SaveModule,													//保存当前设置, 模块重启
	USRGPRS_ResetUserConfig,												//恢复用户默认设置, 模块重启
	USRGPRS_ResetConfig,												//恢复出厂设置, 模块重启
	USRGPRS_SocketASet,													//设置 SocketA 参数
	USRGPRS_SocketBSet,													//设置 SocketB 参数
	USRGPRS_SocketAEnSet,												//设置是否使能SocketA
	USRGPRS_SocketBEnSet,												//设置是否使能SocketB
	USRGPRS_SocketASLSet,												//设置SocketA连接方式
	USRGPRS_SocketBSLSet,												//设置SocketB连接方式
	USRGPRS_REGEnSet,													//设置是否使能注册包
	USRGPRS_HEARTEnSet,													//设置是否使能心跳包
	
	USRGPRS_ModuleInit,													//USR GPRS 初始化模块
	USRGPRS_SocketNetConfig,												//USR GPRS 配置Socket连接参数
	
	USRGPRS_ReadModbusPrarm,												//读取数据Modbus设置
	USRGPRS_MonitorChange,												//监听GPRS参数是否改变
};

/* gat920外部接口例化各函数 */
struct _m_usrk2_dev USR_K2_Dev = 
{
	USRK2_EnterATCmd,													//进入AT指令模式
	USRK2_ExitATCmd,													//退出AT指令模式
	USRK2_RestartModule,												//重启模块
	USRK2_EchoSet,														//设置回显
	USRK2_RestoreSetting,												//恢复出厂设置
	USRK2_WANNSet,														//设置 WANN 参数
	USRK2_SocketSet,													//设置 Socket 参数
	USRK2_SocketPortSet,												//设置本地端口号
	USRK2_RFCENSet,													//使能/禁止类RFC2217
	USRK2_CLIENTRSTSet,													//使能/禁止 TCP Client 多次发送失败 reset 功能
	
	USRK2_ModuleInit,													//USR K2 初始化模块
	USRK2_SocketNetConfig,												//USR K2 配置Socket连接参数
	
	USRK2_ReadModbusPrarm,												//读取数据Modbus设置
	USRK2_MonitorChange,												//监听K2参数是否改变
};

/********************************************** END OF FLEE **********************************************/
