/**
  *********************************************************************************************************
  * @file    usrgprs.c
  * @author  MoveBroad -- KangYJ
  * @version V1.0
  * @date    
  * @brief   USR-GPRS-7S3驱动程序
  *********************************************************************************************************
  * @attention
  *		
  *			
  *********************************************************************************************************
  */

#include "usrgprs.h"
#include "usrserial.h"
#include "string.h"
#include "stdio.h"
#include "socketconfig.h"


extern GPIO_TypeDef* OUTPUT_TYPE[16];
extern u16 OUTPUT_PIN[16];
extern void delay_1ms(u32 nCount);
extern mvb_param_net_config		param_net_cfg;									//网络配置参数

u8 usrtmpbuf[USR_CACHE_SIZE];
u8 tmpbuf1[20];
u8 tmpbuf2[10];

/**********************************************************************************************************
 @Function			void USRGPRS_ReadModbusPrarm(void)
 @Description			读取数据Modbus设置
 @Input				void
 @Return				void
**********************************************************************************************************/
void USRGPRS_ReadModbusPrarm(void)
{
	USRCheckChange.socketA_addr_h = param_net_cfg.socketA_addr_h;
	USRCheckChange.socketA_addr_l = param_net_cfg.socketA_addr_l;
	USRCheckChange.socketA_port = param_net_cfg.socketA_port;
	USRCheckChange.socketA_connect_mode = param_net_cfg.socketA_connect_mode;
}

/**********************************************************************************************************
 @Function			void USRGPRS_MonitorChange(void)
 @Description			监听GPRS参数是否改变
 @Input				void
 @Return				void
**********************************************************************************************************/
void USRGPRS_MonitorChange(void)
{
	if ( (USRCheckChange.socketA_addr_h != param_net_cfg.socketA_addr_h) || 
		(USRCheckChange.socketA_addr_l != param_net_cfg.socketA_addr_l) || 
		(USRCheckChange.socketA_port != param_net_cfg.socketA_port) || 
		(USRCheckChange.socketA_connect_mode != param_net_cfg.socketA_connect_mode) )
	{
		GPIO_SetBits(OUTPUT_TYPE[1], OUTPUT_PIN[1]);
		USRGPRS_ReadModbusPrarm();
		if (USR_GRPS_Dev.SocketNetConfig() != USR_ENOERR) {
			GPIO_SetBits(OUTPUT_TYPE[1], OUTPUT_PIN[1]);
		}
		else {
			GPIO_ResetBits(OUTPUT_TYPE[1], OUTPUT_PIN[1]);
		}
	}
}

/**********************************************************************************************************
 @Function			u8* USRGPRS_CheckCmd(u8 *str)
 @Description			发送命令后, 检测接收到的应答
 @Input				str		: 期待的应答结果
 @Return				0		: 没有得到期待的应答结果
					其他		: 期待应答结果的位置(str的位置)
**********************************************************************************************************/
u8* USRGPRS_CheckCmd(u8 *str)
{
	char *strx=0;
	
	if (USRReceiveLength & 0X8000) {										//接收到一次数据了
		USRReceiveBuf[USRReceiveLength & 0X7FFF] = 0;						//添加结束符
		strx = strstr((const char*)USRReceiveBuf, (const char*)str);
	}
	
	return (u8*)strx;
}

/**********************************************************************************************************
 @Function			u8 USRGPRS_SendCmd(u8 *cmd, u8 *ack, u16 waittime)
 @Description			向USR GPRS发送命令
 @Input				cmd		: 发送的命令字符串
					length	: 发送命令长度
					ack		: 期待的应答结果,如果为空,则表示不需要等待应答
					waittime	: 等待时间(单位:10ms)
 @Return				0		: 发送成功(得到了期待的应答结果)
					1		: 发送失败
**********************************************************************************************************/
u8 USRGPRS_SendCmd(u8 *cmd, u8 *ack, u16 waittime)
{
	u8 result = 0;
	
	USR_PortSerialEnable(1, 0);											//开启接收中断
	USRReceiveLength = 0;
	memset((void *)USRReceiveBuf, 0x0, sizeof(USRReceiveBuf));
	
	USR_PortSerialSend(cmd, strlen((const char *)cmd));						//发送命令
	
	if (ack && waittime)												//需要等待应答
	{
		while (--waittime)												//等待倒计时
		{
			delay_1ms(10);
			if (USRReceiveLength & 0X8000) {								//接收到期待的应答结果
				if (USRGPRS_CheckCmd(ack)) {
					break;
				}
				USRReceiveLength = 0;
			}
		}
		
		if (waittime == 0) {
			result = 1;
		}
	}
	USR_PortSerialEnable(0, 0);											//关闭接收中断
	
	return result;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_ModuleInit(void)
 @Description			USR GPRS 初始化模块
 @Input				void
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRGPRS_ModuleInit(void)
{
	USRErrorCode err = USR_ENOERR;
	
	USRInitialized = USR_INITCONFIGING;									//USRGPRS进行配置
	USR_PortSerialInit(USART3, 115200, 8, USR_PAR_NONE);						//初始化USR通信串口
	
	memset(tmpbuf1, 0x0, sizeof(tmpbuf1));
	memset(tmpbuf2, 0x0, sizeof(tmpbuf2));
	sprintf((char *)tmpbuf1, "%d.%d.%d.%d", param_net_cfg.socketA_addr_h/256, param_net_cfg.socketA_addr_h%256, param_net_cfg.socketA_addr_l/256, param_net_cfg.socketA_addr_l%256);
	sprintf((char *)tmpbuf2, "%d", param_net_cfg.socketA_port);
	
	if (USRGPRS_EnterATCmd() != USR_ENOERR) {								//进入AT指令模式
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_EchoSet(0) != USR_ENOERR) {									//关闭回显
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_WorkModeSet(3) != USR_ENOERR) {								//设置为网络透传模式
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_CalenSet(0) != USR_ENOERR) {								//关闭通话功能
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_NatenSet(0) != USR_ENOERR) {								//关闭网络AT指令
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_UatenSet(0) != USR_ENOERR) {								//关闭透传模式下的串口AT指令
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_RestartTimeSet() != USR_ENOERR) {							//关闭模块自动重启
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_SleepTimeSet() != USR_ENOERR) {								//关闭模块自动进入低功耗
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_REGEnSet(0) != USR_ENOERR) {								//关闭注册包
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_HEARTEnSet(0) != USR_ENOERR) {								//关闭心跳包
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_SocketASet(0, tmpbuf1, tmpbuf2) != USR_ENOERR) {				//设置 SocketA 参数
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_SocketASLSet(1) != USR_ENOERR) {								//SocketA长连接
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_SocketAEnSet(1) != USR_ENOERR) {								//使能SocketA
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_SocketBEnSet(0) != USR_ENOERR) {								//关闭SocketB
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_SaveModule() != USR_ENOERR) {								//保存当前设置, 模块重启
		return USR_MODULEINITERR;
	}
	
	delay_1ms(500);													//等待重启
	
	USRInitialized = USR_INITCONFIGOVER;									//USRGPRS配置结束
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_SocketNetConfig(void)
 @Description			USR GPRS 配置Socket连接参数
 @Input				void
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRGPRS_SocketNetConfig(void)
{
	USRErrorCode err = USR_ENOERR;
	
	USR_PortSerialEnable(0, 0);											//关闭接收中断
	USRInitialized = USR_INITCONFIGING;									//USRGPRS进行配置
	USR_PortSerialInit(USART3, 115200, 8, USR_PAR_NONE);						//初始化USR通信串口
	
	memset(tmpbuf1, 0x0, sizeof(tmpbuf1));
	memset(tmpbuf2, 0x0, sizeof(tmpbuf2));
	sprintf((char *)tmpbuf1, "%d.%d.%d.%d", param_net_cfg.socketA_addr_h/256, param_net_cfg.socketA_addr_h%256, param_net_cfg.socketA_addr_l/256, param_net_cfg.socketA_addr_l%256);
	sprintf((char *)tmpbuf2, "%d", param_net_cfg.socketA_port);
	
	if (USRGPRS_EnterATCmd() != USR_ENOERR) {								//进入AT指令模式
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_SocketASet(0, tmpbuf1, tmpbuf2) != USR_ENOERR) {				//设置 SocketA 参数
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_SocketASLSet(1) != USR_ENOERR) {								//SocketA长连接
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_SocketAEnSet(1) != USR_ENOERR) {								//使能SocketA
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_SocketBEnSet(0) != USR_ENOERR) {								//关闭SocketB
		return USR_MODULEINITERR;
	}
	
	if (USRGPRS_SaveModule() != USR_ENOERR) {								//保存当前设置, 模块重启
		return USR_MODULEINITERR;
	}
	
	delay_1ms(300);													//等待重启
	socket_dev.PortSerialInit(USART3, 115200);								//初始化Socket协议串口
	TIM_Cmd(TIM2, DISABLE);												//关闭定时器2
	
	USRInitialized = USR_INITCONFIGOVER;									//USRGPRS配置结束
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_EnterATCmd(void)
 @Description			进入AT指令模式
 @Input				void
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRGPRS_EnterATCmd(void)
{
	USRErrorCode err = USR_ENOERR;
	
	if (USRGPRS_SendCmd((u8 *)"+++", (u8 *)"a", 200) == 0) {
		if (USRGPRS_SendCmd((u8 *)"a", (u8 *)"+ok", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ENTERATERR;
		}
	}
	else {
		err = USR_ENTERATERR;
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_ExitATCmd(void)
 @Description			退出AT指令模式
 @Input				void
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRGPRS_ExitATCmd(void)
{
	USRErrorCode err = USR_ENOERR;
	
	if (USRGPRS_SendCmd((u8 *)"AT+ENTM\r", (u8 *)"OK", 200) == 0) {
		err = USR_ENOERR;
	}
	else {
		err = USR_ATEXECUTEERR;
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_RestartModule(void)
 @Description			重启模块
 @Input				void
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRGPRS_RestartModule(void)
{
	USRErrorCode err = USR_ENOERR;
	
	if (USRGPRS_SendCmd((u8 *)"AT+Z\r", (u8 *)"OK", 200) == 0) {
		err = USR_ENOERR;
	}
	else {
		err = USR_ATEXECUTEERR;
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_EchoSet(u8 xEnable)
 @Description			设置回显
 @Input				xEnable	   : 1 on
								0 off
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRGPRS_EchoSet(u8 xEnable)
{
	USRErrorCode err = USR_ENOERR;
	
	if (xEnable == 1) {													//开启回显
		if (USRGPRS_SendCmd((u8 *)"AT+E=\"on\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {															//关闭回显
		if (USRGPRS_SendCmd((u8 *)"AT+E=\"off\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_WorkModeSet(u8 mode)
 @Description			设置工作模式
 @Input				mode		   : 1 CMD	AT指令模式
								2 SMS	短信透传模式
								3 NET	网络透传模式
								4 HTTPD	HTTPD模式
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRGPRS_WorkModeSet(u8 mode)
{
	USRErrorCode err = USR_ENOERR;
	
	if (mode == 1) {
		if (USRGPRS_SendCmd((u8 *)"AT+WKMOD=\"CMD\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else if (mode == 2) {
		if (USRGPRS_SendCmd((u8 *)"AT+WKMOD=\"SMS\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else if (mode == 3) {
		if (USRGPRS_SendCmd((u8 *)"AT+WKMOD=\"NET\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else if (mode == 4) {
		if (USRGPRS_SendCmd((u8 *)"AT+WKMOD=\"HTTPD\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {
		err = USR_ATEXECUTEERR;
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_CalenSet(u8 xEnable)
 @Description			设置是否使能通话功能
 @Input				xEnable	   : 1 on
								0 off
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRGPRS_CalenSet(u8 xEnable)
{
	USRErrorCode err = USR_ENOERR;
	
	if (xEnable == 1) {													//使能通话
		if (USRGPRS_SendCmd((u8 *)"AT+CALEN=\"on\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {															//禁止通话
		if (USRGPRS_SendCmd((u8 *)"AT+CALEN=\"off\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_NatenSet(u8 xEnable)
 @Description			设置是否使能网络AT指令
 @Input				xEnable	   : 1 on
								0 off
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRGPRS_NatenSet(u8 xEnable)
{
	USRErrorCode err = USR_ENOERR;
	
	if (xEnable == 1) {													//使能网络AT指令
		if (USRGPRS_SendCmd((u8 *)"AT+NATEN=\"on\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {															//禁止网络AT指令
		if (USRGPRS_SendCmd((u8 *)"AT+NATEN=\"off\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_UatenSet(u8 xEnable)
 @Description			设置是否使能透传模式下的串口AT指令
 @Input				xEnable	   : 1 on
								0 off
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRGPRS_UatenSet(u8 xEnable)
{
	USRErrorCode err = USR_ENOERR;
	
	if (xEnable == 1) {													//使能透传模式下的串口AT指令
		if (USRGPRS_SendCmd((u8 *)"AT+UATEN=\"on\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {															//禁止透传模式下的串口AT指令
		if (USRGPRS_SendCmd((u8 *)"AT+UATEN=\"off\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_CachenSet(u8 xEnable)
 @Description			设置是否使能缓存数据
 @Input				xEnable	   : 1 on
								0 off
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRGPRS_CachenSet(u8 xEnable)
{
	USRErrorCode err = USR_ENOERR;
	
	if (xEnable == 1) {													//使能缓存数据
		if (USRGPRS_SendCmd((u8 *)"AT+CACHEN=\"on\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {															//禁止缓存数据
		if (USRGPRS_SendCmd((u8 *)"AT+CACHEN=\"off\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_RestartTimeSet(void)
 @Description			设置模块自动重启时间
 @Input				void
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRGPRS_RestartTimeSet(void)
{
	USRErrorCode err = USR_ENOERR;
	
	if (USRGPRS_SendCmd((u8 *)"AT+RSTIM=0\r", (u8 *)"OK", 200) == 0) {
		err = USR_ENOERR;
	}
	else {
		err = USR_ATEXECUTEERR;
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_SleepSet(u8 xEnable)
 @Description			设置是进入低功耗模式
 @Input				xEnable	   : 1 on
								0 off
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRGPRS_SleepSet(u8 xEnable)
{
	USRErrorCode err = USR_ENOERR;
	
	if (xEnable == 1) {													//使能低功耗模式
		if (USRGPRS_SendCmd((u8 *)"AT+SLEEP=\"on\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {															//禁止低功耗模式
		if (USRGPRS_SendCmd((u8 *)"AT+SLEEP=\"off\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_SleepTimeSet(void)
 @Description			设置模块自动进入低功耗时间
 @Input				void
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRGPRS_SleepTimeSet(void)
{
	USRErrorCode err = USR_ENOERR;
	
	if (USRGPRS_SendCmd((u8 *)"AT+SLEEPTIM=0\r", (u8 *)"OK", 200) == 0) {
		err = USR_ENOERR;
	}
	else {
		err = USR_ATEXECUTEERR;
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_SaveModule(void)
 @Description			保存当前设置, 模块重启
 @Input				void
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRGPRS_SaveModule(void)
{
	USRErrorCode err = USR_ENOERR;
	
	if (USRGPRS_SendCmd((u8 *)"AT+S\r", (u8 *)"OK", 200) == 0) {
		err = USR_ENOERR;
	}
	else {
		err = USR_ATEXECUTEERR;
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_ResetUserConfig(void)
 @Description			恢复用户默认设置, 模块重启
 @Input				void
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRGPRS_ResetUserConfig(void)
{
	USRErrorCode err = USR_ENOERR;
	
	if (USRGPRS_SendCmd((u8 *)"AT+RELD\r", (u8 *)"OK", 200) == 0) {
		err = USR_ENOERR;
	}
	else {
		err = USR_ATEXECUTEERR;
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_ResetConfig(void)
 @Description			恢复出厂设置, 模块重启
 @Input				void
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRGPRS_ResetConfig(void)
{
	USRErrorCode err = USR_ENOERR;
	
	if (USRGPRS_SendCmd((u8 *)"AT+CFGTF\r", (u8 *)"OK", 200) == 0) {
		err = USR_ENOERR;
	}
	else {
		err = USR_ATEXECUTEERR;
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_SocketASet(u8 mode, u8 *ipaddr, u8 *port)
 @Description			设置 SocketA 参数
 @Input				mode		   : 0 TCP
								1 UDP
					ipaddr	   : ip地址
					port		   : 端口
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRGPRS_SocketASet(u8 mode, u8 *ipaddr, u8 *port)
{
	USRErrorCode err = USR_ENOERR;
	
	memset((void *)usrtmpbuf, 0x0, sizeof(usrtmpbuf));
	
	if (mode == 0) {
		sprintf((char *)usrtmpbuf, "AT+SOCKA=\"TCP\",\"%s\",%s\r", ipaddr, port);
	}
	else {
		sprintf((char *)usrtmpbuf, "AT+SOCKA=\"UDP\",\"%s\",%s\r", ipaddr, port);
	}
	
	if (USRGPRS_SendCmd(usrtmpbuf, (u8 *)"OK", 200) == 0) {
		err = USR_ENOERR;
	}
	else {
		err = USR_ATEXECUTEERR;
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_SocketBSet(u8 mode, u8 *ipaddr, u8 *port)
 @Description			设置 SocketB 参数
 @Input				mode		   : 0 TCP
								1 UDP
					ipaddr	   : ip地址
					port		   : 端口
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRGPRS_SocketBSet(u8 mode, u8 *ipaddr, u8 *port)
{
	USRErrorCode err = USR_ENOERR;
	
	memset((void *)usrtmpbuf, 0x0, sizeof(usrtmpbuf));
	
	if (mode == 0) {
		sprintf((char *)usrtmpbuf, "AT+SOCKB=\"TCP\",\"%s\",%s\r", ipaddr, port);
	}
	else {
		sprintf((char *)usrtmpbuf, "AT+SOCKB=\"UDP\",\"%s\",%s\r", ipaddr, port);
	}
	
	if (USRGPRS_SendCmd(usrtmpbuf, (u8 *)"OK", 200) == 0) {
		err = USR_ENOERR;
	}
	else {
		err = USR_ATEXECUTEERR;
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_SocketAEnSet(u8 xEnable)
 @Description			设置是否使能SocketA
 @Input				xEnable	   : 1 on
								0 off
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRGPRS_SocketAEnSet(u8 xEnable)
{
	USRErrorCode err = USR_ENOERR;
	
	if (xEnable == 1) {													//使能SocketA
		if (USRGPRS_SendCmd((u8 *)"AT+SOCKAEN=\"on\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {															//禁止SocketA
		if (USRGPRS_SendCmd((u8 *)"AT+SOCKAEN=\"off\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_SocketBEnSet(u8 xEnable)
 @Description			设置是否使能SocketB
 @Input				xEnable	   : 1 on
								0 off
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRGPRS_SocketBEnSet(u8 xEnable)
{
	USRErrorCode err = USR_ENOERR;
	
	if (xEnable == 1) {													//使能SocketA
		if (USRGPRS_SendCmd((u8 *)"AT+SOCKBEN=\"on\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {															//禁止SocketA
		if (USRGPRS_SendCmd((u8 *)"AT+SOCKBEN=\"off\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_SocketASLSet(u8 type)
 @Description			设置SocketA连接方式
 @Input				type		   : 1 long
								0 short
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRGPRS_SocketASLSet(u8 type)
{
	USRErrorCode err = USR_ENOERR;
	
	if (type == 1) {													//long
		if (USRGPRS_SendCmd((u8 *)"AT+SOCKASL=\"long\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {															//short
		if (USRGPRS_SendCmd((u8 *)"AT+SOCKASL=\"short\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_SocketBSLSet(u8 type)
 @Description			设置SocketB连接方式
 @Input				type		   : 1 long
								0 short
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRGPRS_SocketBSLSet(u8 type)
{
	USRErrorCode err = USR_ENOERR;
	
	if (type == 1) {													//long
		if (USRGPRS_SendCmd((u8 *)"AT+SOCKBSL=\"long\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {															//short
		if (USRGPRS_SendCmd((u8 *)"AT+SOCKBSL=\"short\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_REGEnSet(u8 xEnable)
 @Description			设置是否使能注册包
 @Input				xEnable	   : 1 on
								0 off
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRGPRS_REGEnSet(u8 xEnable)
{
	USRErrorCode err = USR_ENOERR;
	
	if (xEnable == 1) {													//使能注册包
		if (USRGPRS_SendCmd((u8 *)"AT+REGEN=\"on\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {															//禁止注册包
		if (USRGPRS_SendCmd((u8 *)"AT+REGEN=\"off\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_HEARTEnSet(u8 xEnable)
 @Description			设置是否使能心跳包
 @Input				xEnable	   : 1 on
								0 off
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRGPRS_HEARTEnSet(u8 xEnable)
{
	USRErrorCode err = USR_ENOERR;
	
	if (xEnable == 1) {													//使能心跳包
		if (USRGPRS_SendCmd((u8 *)"AT+HEARTEN=\"on\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {															//禁止心跳包
		if (USRGPRS_SendCmd((u8 *)"AT+HEARTEN=\"off\"\r", (u8 *)"OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	
	return err;
}

/********************************************** END OF FLEE **********************************************/
