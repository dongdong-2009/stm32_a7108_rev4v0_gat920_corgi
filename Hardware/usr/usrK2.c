/**
  *********************************************************************************************************
  * @file    usrK2.c
  * @author  MoveBroad -- KangYJ
  * @version V1.0
  * @date    
  * @brief   USR-K2 Super Port驱动程序
  *********************************************************************************************************
  * @attention
  *		
  *			
  *********************************************************************************************************
  */

#include "usrK2.h"
#include "usrserial.h"
#include "string.h"
#include "stdio.h"
#include "socketconfig.h"


extern GPIO_TypeDef* OUTPUT_TYPE[16];
extern u16 OUTPUT_PIN[16];
extern void delay_1ms(u32 nCount);
extern mvb_param_net_config		param_net_cfg;									//网络配置参数

u8 usrk2tmpbuf[USR_CACHE_SIZE];
u8 tmpk2buf1[20];
u8 tmpk2buf2[20];
u8 tmpk2buf3[20];


/**********************************************************************************************************
 @Function			void USRK2_ReadModbusPrarm(void)
 @Description			读取数据Modbus设置
 @Input				void
 @Return				void
**********************************************************************************************************/
void USRK2_ReadModbusPrarm(void)
{
	USRCheckChange.device_addr_h = param_net_cfg.device_addr_h;
	USRCheckChange.device_addr_l = param_net_cfg.device_addr_l;
	USRCheckChange.device_mask_h = param_net_cfg.device_mask_h;
	USRCheckChange.device_mask_l = param_net_cfg.device_mask_l;
	USRCheckChange.device_bcast_h = param_net_cfg.device_bcast_h;
	USRCheckChange.device_bcast_l = param_net_cfg.device_bcast_l;
	
	USRCheckChange.socketA_addr_h = param_net_cfg.socketA_addr_h;
	USRCheckChange.socketA_addr_l = param_net_cfg.socketA_addr_l;
	USRCheckChange.socketA_port = param_net_cfg.socketA_port;
	USRCheckChange.socketA_connect_mode = param_net_cfg.socketA_connect_mode;
}

/**********************************************************************************************************
 @Function			void USRGPRS_MonitorChange(void)
 @Description			监听K2参数是否改变
 @Input				void
 @Return				void
**********************************************************************************************************/
void USRK2_MonitorChange(void)
{
	if ( (USRCheckChange.device_addr_h != param_net_cfg.device_addr_h) || 
		(USRCheckChange.device_addr_l != param_net_cfg.device_addr_l) || 
		(USRCheckChange.device_mask_h != param_net_cfg.device_mask_h) || 
		(USRCheckChange.device_mask_l != param_net_cfg.device_mask_l) || 
		(USRCheckChange.device_bcast_h != param_net_cfg.device_bcast_h) || 
		(USRCheckChange.device_bcast_l != param_net_cfg.device_bcast_l) || 
		(USRCheckChange.socketA_addr_h != param_net_cfg.socketA_addr_h) || 
		(USRCheckChange.socketA_addr_l != param_net_cfg.socketA_addr_l) || 
		(USRCheckChange.socketA_port != param_net_cfg.socketA_port) || 
		(USRCheckChange.socketA_connect_mode != param_net_cfg.socketA_connect_mode) )
	{
		GPIO_SetBits(OUTPUT_TYPE[1], OUTPUT_PIN[1]);
		USRK2_ReadModbusPrarm();
		if (USR_K2_Dev.SocketNetConfig() != USR_ENOERR) {
			GPIO_SetBits(OUTPUT_TYPE[1], OUTPUT_PIN[1]);
		}
		else {
			GPIO_ResetBits(OUTPUT_TYPE[1], OUTPUT_PIN[1]);
		}
	}
}

/**********************************************************************************************************
 @Function			u8* USRK2_CheckCmd(u8 *str)
 @Description			发送命令后, 检测接收到的应答
 @Input				str		: 期待的应答结果
 @Return				0		: 没有得到期待的应答结果
					其他		: 期待应答结果的位置(str的位置)
**********************************************************************************************************/
u8* USRK2_CheckCmd(u8 *str)
{
	char *strx=0;
	
	if (USRReceiveLength & 0X8000) {										//接收到一次数据了
		USRReceiveBuf[USRReceiveLength & 0X7FFF] = 0;						//添加结束符
		strx = strstr((const char*)USRReceiveBuf, (const char*)str);
	}
	
	return (u8*)strx;
}

/**********************************************************************************************************
 @Function			u8 USRK2_SendCmd(u8 *cmd, u8 *ack, u16 waittime)
 @Description			向USR K2发送命令
 @Input				cmd		: 发送的命令字符串
					length	: 发送命令长度
					ack		: 期待的应答结果,如果为空,则表示不需要等待应答
					waittime	: 等待时间(单位:10ms)
 @Return				0		: 发送成功(得到了期待的应答结果)
					1		: 发送失败
**********************************************************************************************************/
u8 USRK2_SendCmd(u8 *cmd, u8 *ack, u16 waittime)
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
				if (USRK2_CheckCmd(ack)) {
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
 @Function			USRErrorCode USRK2_ModuleInit(void)
 @Description			USR K2 初始化模块
 @Input				void
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRK2_ModuleInit(void)
{
	USRErrorCode err = USR_ENOERR;
	
	USRInitialized = USR_INITCONFIGING;									//USRK2进行配置
	USR_PortSerialInit(USART3, 115200, 8, USR_PAR_NONE);						//初始化USR通信串口
	
	memset(tmpk2buf1, 0x0, sizeof(tmpk2buf1));
	memset(tmpk2buf2, 0x0, sizeof(tmpk2buf2));
	memset(tmpk2buf3, 0x0, sizeof(tmpk2buf3));
	sprintf((char *)tmpk2buf1, "%d.%d.%d.%d", param_net_cfg.device_addr_h/256, param_net_cfg.device_addr_h%256, param_net_cfg.device_addr_l/256, param_net_cfg.device_addr_l%256);
	sprintf((char *)tmpk2buf2, "%d.%d.%d.%d", param_net_cfg.device_mask_h/256, param_net_cfg.device_mask_h%256, param_net_cfg.device_mask_l/256, param_net_cfg.device_mask_l%256);
	sprintf((char *)tmpk2buf3, "%d.%d.%d.%d", param_net_cfg.device_bcast_h/256, param_net_cfg.device_bcast_h%256, param_net_cfg.device_bcast_l/256, param_net_cfg.device_bcast_l%256);
	
	if (USRK2_EnterATCmd() != USR_ENOERR) {									//进入AT指令模式
		return USR_MODULEINITERR;
	}
	
	if (USRK2_EchoSet(0) != USR_ENOERR) {									//关闭回显
		return USR_MODULEINITERR;
	}
	
	if (USRK2_RFCENSet(1) != USR_ENOERR) {									//使能类RFC2217
		return USR_MODULEINITERR;
	}
	
	if (USRK2_CLIENTRSTSet(1) != USR_ENOERR) {								//使能 TCP Client 多次发送失败 reset 功能
		return USR_MODULEINITERR;
	}
	
	if (USRK2_WANNSet(DHCP, tmpk2buf1, tmpk2buf2, tmpk2buf3) != USR_ENOERR) {		//设置 WANN 参数
		return USR_MODULEINITERR;
	}
	
	if (USRK2_SocketPortSet(LOCALPORTMODE, (u8 *)LOCALPORT) != USR_ENOERR) {		//设置本地端口号
		return USR_MODULEINITERR;
	}
	
	memset(tmpk2buf1, 0x0, sizeof(tmpk2buf1));
	memset(tmpk2buf2, 0x0, sizeof(tmpk2buf2));
	sprintf((char *)tmpk2buf1, "%d.%d.%d.%d", param_net_cfg.socketA_addr_h/256, param_net_cfg.socketA_addr_h%256, param_net_cfg.socketA_addr_l/256, param_net_cfg.socketA_addr_l%256);
	sprintf((char *)tmpk2buf2, "%d", param_net_cfg.socketA_port);
	
	if (USRK2_SocketSet(1, tmpk2buf1, tmpk2buf2) != USR_ENOERR) {				//设置 Socket 参数
		return USR_MODULEINITERR;
	}
	
	if (USRK2_RestartModule() != USR_ENOERR) {								//重启退出AT指令模式
		return USR_MODULEINITERR;
	}
	
	delay_1ms(500);													//等待重启
	
	USRInitialized = USR_INITCONFIGOVER;									//USRGPRS配置结束
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRGPRS_SocketNetConfig(void)
 @Description			USR K2 配置Socket连接参数
 @Input				void
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRK2_SocketNetConfig(void)
{
	USRErrorCode err = USR_ENOERR;
	
	USR_PortSerialEnable(0, 0);											//关闭接收中断
	USRInitialized = USR_INITCONFIGING;									//USRGPRS进行配置
	USR_PortSerialInit(USART3, 115200, 8, USR_PAR_NONE);						//初始化USR通信串口
	
	memset(tmpk2buf1, 0x0, sizeof(tmpk2buf1));
	memset(tmpk2buf2, 0x0, sizeof(tmpk2buf2));
	memset(tmpk2buf3, 0x0, sizeof(tmpk2buf3));
	sprintf((char *)tmpk2buf1, "%d.%d.%d.%d", param_net_cfg.device_addr_h/256, param_net_cfg.device_addr_h%256, param_net_cfg.device_addr_l/256, param_net_cfg.device_addr_l%256);
	sprintf((char *)tmpk2buf2, "%d.%d.%d.%d", param_net_cfg.device_mask_h/256, param_net_cfg.device_mask_h%256, param_net_cfg.device_mask_l/256, param_net_cfg.device_mask_l%256);
	sprintf((char *)tmpk2buf3, "%d.%d.%d.%d", param_net_cfg.device_bcast_h/256, param_net_cfg.device_bcast_h%256, param_net_cfg.device_bcast_l/256, param_net_cfg.device_bcast_l%256);

	if (USRK2_EnterATCmd() != USR_ENOERR) {									//进入AT指令模式
		return USR_MODULEINITERR;
	}
	
	if (USRK2_WANNSet(DHCP, tmpk2buf1, tmpk2buf2, tmpk2buf3) != USR_ENOERR) {		//设置 WANN 参数
		return USR_MODULEINITERR;
	}
	
	memset(tmpk2buf1, 0x0, sizeof(tmpk2buf1));
	memset(tmpk2buf2, 0x0, sizeof(tmpk2buf2));
	sprintf((char *)tmpk2buf1, "%d.%d.%d.%d", param_net_cfg.socketA_addr_h/256, param_net_cfg.socketA_addr_h%256, param_net_cfg.socketA_addr_l/256, param_net_cfg.socketA_addr_l%256);
	sprintf((char *)tmpk2buf2, "%d", param_net_cfg.socketA_port);
	
	if (USRK2_SocketSet(1, tmpk2buf1, tmpk2buf2) != USR_ENOERR) {				//设置 Socket 参数
		return USR_MODULEINITERR;
	}
	
	if (USRK2_RestartModule() != USR_ENOERR) {								//重启退出AT指令模式
		return USR_MODULEINITERR;
	}
	
	delay_1ms(500);													//等待重启
	socket_dev.PortSerialInit(USART3, 115200);								//初始化Socket协议串口
	TIM_Cmd(TIM2, DISABLE);												//关闭定时器2
	
	USRInitialized = USR_INITCONFIGOVER;									//USRGPRS配置结束
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRK2_EnterATCmd(void)
 @Description			进入AT指令模式
 @Input				void
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRK2_EnterATCmd(void)
{
	USRErrorCode err = USR_ENOERR;
	
	if (USRK2_SendCmd((u8 *)"+++", (u8 *)"a", 200) == 0) {
		if (USRK2_SendCmd((u8 *)"a", (u8 *)"+ok", 200) == 0) {
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
 @Function			USRErrorCode USRK2_ExitATCmd(void)
 @Description			退出AT指令模式
 @Input				void
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRK2_ExitATCmd(void)
{
	USRErrorCode err = USR_ENOERR;
	
	if (USRK2_SendCmd((u8 *)"AT+ENTM\r", (u8 *)"+OK", 200) == 0) {
		err = USR_ENOERR;
	}
	else {
		err = USR_ATEXECUTEERR;
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRK2_RestartModule(void)
 @Description			重启模块
 @Input				void
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRK2_RestartModule(void)
{
	USRErrorCode err = USR_ENOERR;
	
	if (USRK2_SendCmd((u8 *)"AT+Z\r", (u8 *)"+OK", 200) == 0) {
		err = USR_ENOERR;
	}
	else {
		err = USR_ATEXECUTEERR;
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRK2_EchoSet(u8 xEnable)
 @Description			设置回显
 @Input				xEnable	   : 1 on
								0 off
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRK2_EchoSet(u8 xEnable)
{
	USRErrorCode err = USR_ENOERR;
	
	if (xEnable == 1) {													//开启回显
		if (USRK2_SendCmd((u8 *)"AT+E=ON\r", (u8 *)"+OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {															//关闭回显
		if (USRK2_SendCmd((u8 *)"AT+E=OFF\r", (u8 *)"+OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRK2_RestoreSetting(void)
 @Description			恢复出厂设置
 @Input				void
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRK2_RestoreSetting(void)
{
	USRErrorCode err = USR_ENOERR;
	
	if (USRK2_SendCmd((u8 *)"AT+RELD\r", (u8 *)"+OK", 200) == 0) {
		err = USR_ENOERR;
	}
	else {
		err = USR_ATEXECUTEERR;
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRK2_WANNSet(u8 mode, u8 *address, u8 *mask, u8 *gateway)
 @Description			设置 WANN 参数
 @Input				mode		   : 0 static
								1 DHCP
					address	   : IP地址
					mask		   : 子网掩码
					gateway	   : 网关地址
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRK2_WANNSet(u8 mode, u8 *address, u8 *mask, u8 *gateway)
{
	USRErrorCode err = USR_ENOERR;
	
	memset((void *)usrk2tmpbuf, 0x0, sizeof(usrk2tmpbuf));
	
	if (mode == 0) {
		sprintf((char *)usrk2tmpbuf, "AT+WANN=static,%s,%s,%s\r", address, mask, gateway);
		
		if (USRK2_SendCmd(usrk2tmpbuf, (u8 *)"+OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {
		if (USRK2_SendCmd((u8 *)"AT+WANN=DHCP\r", (u8 *)"+OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRK2_SocketSet(u8 mode, u8 *ipaddr, u8 *port)
 @Description			设置 Socket 参数
 @Input				mode		   : 0 TCP Server
								1 TCP Client 
								2 UDP Server
								3 UDP Client
								4 Httpd Client
					ipaddr	   : ip地址
					port		   : 端口
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRK2_SocketSet(u8 mode, u8 *ipaddr, u8 *port)
{
	USRErrorCode err = USR_ENOERR;
	
	memset((void *)usrk2tmpbuf, 0x0, sizeof(usrk2tmpbuf));
	
	if (mode == 0) {
		sprintf((char *)usrk2tmpbuf, "AT+SOCK=TCPS,%s,%s\r", ipaddr, port);
	}
	else if (mode == 1) {
		sprintf((char *)usrk2tmpbuf, "AT+SOCK=TCPC,%s,%s\r", ipaddr, port);
	}
	else if (mode == 2) {
		sprintf((char *)usrk2tmpbuf, "AT+SOCK=UDPS,%s,%s\r", ipaddr, port);
	}
	else if (mode == 3) {
		sprintf((char *)usrk2tmpbuf, "AT+SOCK=UDPC,%s,%s\r", ipaddr, port);
	}
	else if (mode == 4) {
		sprintf((char *)usrk2tmpbuf, "AT+SOCK=HTPC,%s,%s\r", ipaddr, port);
	}
	else {
		sprintf((char *)usrk2tmpbuf, "AT+SOCK=TCPC,%s,%s\r", ipaddr, port);
	}
	
	if (USRK2_SendCmd(usrk2tmpbuf, (u8 *)"+OK", 200) == 0) {
		err = USR_ENOERR;
	}
	else {
		err = USR_ATEXECUTEERR;
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRK2_SocketPortSet(u8 mode, u8 *port)
 @Description			设置本地 Socket 端口号
 @Input				mode		   : 0 随机端口
								1 设置端口
					port		   : 端口
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRK2_SocketPortSet(u8 mode, u8 *port)
{
	USRErrorCode err = USR_ENOERR;
	
	memset((void *)usrk2tmpbuf, 0x0, sizeof(usrk2tmpbuf));
	
	if (mode == 0) {
		if (USRK2_SendCmd((u8 *)"AT+SOCKPORT=0\r", (u8 *)"+OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {
		sprintf((char *)usrk2tmpbuf, "AT+SOCKPORT=%s\r", port);
		
		if (USRK2_SendCmd(usrk2tmpbuf, (u8 *)"+OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRK2_RFCENSet(u8 xEnable)
 @Description			使能/禁止类RFC2217
 @Input				xEnable	   : 1 on
								0 off
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRK2_RFCENSet(u8 xEnable)
{
	USRErrorCode err = USR_ENOERR;
	
	if (xEnable == 1) {													//开启RFC2217
		if (USRK2_SendCmd((u8 *)"AT+RFCEN=ON\r", (u8 *)"+OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {															//关闭RFC2217
		if (USRK2_SendCmd((u8 *)"AT+RFCEN=OFF\r", (u8 *)"+OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	
	return err;
}

/**********************************************************************************************************
 @Function			USRErrorCode USRK2_CLIENTRSTSet(u8 xEnable)
 @Description			使能/禁止 TCP Client 多次发送失败 reset 功能
 @Input				xEnable	   : 1 on
								0 off
 @Return				USRErrorCode : 错误信息
**********************************************************************************************************/
USRErrorCode USRK2_CLIENTRSTSet(u8 xEnable)
{
	USRErrorCode err = USR_ENOERR;
	
	if (xEnable == 1) {													//开启CLIENTRST
		if (USRK2_SendCmd((u8 *)"AT+CLIENTRST=ON\r", (u8 *)"+OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	else {															//关闭CLIENTRST
		if (USRK2_SendCmd((u8 *)"AT+CLIENTRST=OFF\r", (u8 *)"+OK", 200) == 0) {
			err = USR_ENOERR;
		}
		else {
			err = USR_ATEXECUTEERR;
		}
	}
	
	return err;
}

/********************************************** END OF FLEE **********************************************/
