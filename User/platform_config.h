/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : platform_config.h
* Author             : MoveBroad Team
* Version            : V2.0.3
* Date               : 09/22/2008
* Description        : Evaluation board specific configuration file.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PLATFORM_CONFIG_H
#define   __PLATFORM_CONFIG_H

//配置参数
#define REV 0x01

//IO
#define OUTPUT_MAX		32

#define CHECK_PKG_MAX	50

/* GPIO LEG 是否开启点亮一遍 */
#define LED_GPIO		1

//自动写SN, CROSSID, jFlash烧写程序版本需要去掉
//#define WRITE_SN_CROSSID
#define MAC_SN			1804018001
#define CROSSID		18040101
#define INTERVALTIME	60												//统计时长,默认60(单位秒),0上传实时数据
/*
		MAC_SN : 倒数第三位
		0 :  Modbus --> USART2, Gat920 --> USART1, Socket --> DISABLE, SocketTime --> DISABLE, Gat920 --> DISABLE, GPRS/RJ45 --> DISABLE
		1 :  Modbus --> USART1, Gat920 --> USART2, Socket --> DISABLE, SocketTime --> DISABLE, Gat920 --> DISABLE, GPRS/RJ45 --> DISABLE
		2 :  Modbus --> USART2, Gat920 --> USART1, Socket --> ENABLE,  SocketTime --> DISABLE, Gat920 --> DISABLE, RJ45 --> ENABLE
		3 :  Modbus --> USART2, Gat920 --> USART1, Socket --> ENABLE,  SocketTime --> ENABLE,  Gat920 --> DISABLE, RJ45 --> ENABLE
		4 :  Modbus --> USART2, Gat920 --> USART1, Socket --> DISABLE, SocketTime --> DISABLE, Gat920 --> ENABLE,  GPRS/RJ45 --> DISABLE
		5 :  Modbus --> USART1, Gat920 --> USART2, Socket --> DISABLE, SocketTime --> DISABLE, Gat920 --> ENABLE,  GPRS/RJ45 --> DISABLE
		6 :  Modbus --> USART2, Gat920 --> USART1, Socket --> ENABLE,  SocketTime --> DISABLE, Gat920 --> DISABLE, GPRS --> ENABLE
		7 :  Modbus --> USART2, Gat920 --> USART1, Socket --> ENABLE,  SocketTime --> ENABLE,  Gat920 --> DISABLE, GPRS --> ENABLE
*/

/* USR使能, 如果注释该宏定义则不配置USR模块 */
#define	USR_ENABLE
#define	LOCALPORTMODE		1											//本地端口 0:随机 1:固定
#define	LOCALPORT			"4008"										//本地端口值0~65535
#define	DHCP				0											//0:静态IP 1:DHCP

/* ModBus使能, 如注释该宏定义则不使用ModBus协议 */
#define	MODBUS_ENABLE
/* ModBus可使用串口, 具体使用哪个需通过SN选择 */
#define	MB_SERIALPORT_USART1
#define	MB_SERIALPORT_USART2
//#define	MB_SERIALPORT_USART3

/* Socket使能, 如注释该宏定义则不使用Socket协议  */
#define	SOCKET_ENABLE
/* Socket使用串口选择 */
//#define	SOCKET_SERIALPORT_USART1
//#define	SOCKET_SERIALPORT_USART2
#define	SOCKET_SERIALPORT_USART3

/* Gat920使能, 如注释该宏定义则不使用Gat920协议  */
#define	GAT920_ENABLE
/* Gat920可使用串口, 具体使用哪个需通过SN选择 */
#define	GAT920_SERIALPORT_USART1
#define	GAT920_SERIALPORT_USART2
//#define	GAT920_SERIALPORT_USART3

#define	HARDWAREIWDG													//硬件看门狗使能
#define	SOFTWAREIWDG													//软件看门狗使能


/* 超过该时间没收到地磁发送数据则重启接收机0~65535(秒) */
#define	RESETTIME				650


#define	GPIO_RS485_CTL			GPIOA
#define	GPIO_RS485_CTL_PIN		GPIO_Pin_1

#define	USART485				USART1
#define	GPIO485				GPIOA
#define	RS485_RxPin			GPIO_Pin_10
#define	RS485_TxPin			GPIO_Pin_9

#define	RECV_MAX				24 										// 接收的最大数据包数
#define	ACK_LENGTH			7

#define	MVB_SYSCLOCK			8000000
//#define	MVB_SYSCLOCK			72000000


//以下宏定义用来区分不同的数据包
#define	DATATYPE_CARIN_WITHOUT_MAGVALUE				0x00
#define	DATATYPE_CARIN_WITH_MAGVALUE					0x01
#define	DATATYPE_CONFIG							0x02
#define	DATATYPE_CONFIG_L							0x02
#define	DATATYPE_CONFIG_H							0x03
#define	DATATYPE_HEARTBEAT_WITHOUT_MAGENV				0x04
#define	DATATYPE_HEARTBEAT_WITH_MAGENV				0x05
#define	DATATYPE_CAROUT_WITHOUT_MAGVALUE				0x06
#define	DATATYPE_CAROUT_WITH_MAGVALUE					0x07

#define	MVB_PARAM_RECV_BYTE							44
#define	MVB_PARAM_WVD_CFG_BYTE						16
#define	MVB_PARAM_NET_CFG_BYTE						24


//地磁发过来数据的头部定义3Byte
typedef struct
{
	u16 addr_dev;				// 2字节的车检ID 
	u8  sequence_num:5;
	u8  type:3;				// 车检包, 心跳包, 配置包. 
}RF_DataHeader_TypeDef;


//详细配置数据包20Byte
typedef struct
{
	u16 addr_dev;				// 2字节的序列号
	//u8 sequence_num:5;
	//u8 type:3;  
	u8  sequence_type;
	u8  rssi;

	//u8 word_mode;
	//u8 config_time;
	//u16 heart_time;
	u32 p1;

	//u8 carin_threshhold;
	//u8 carout_threshhold;
	//u8 measure_value;
	//u8 recalibration_value;
	u32 p2;

	u8  recalibration_time;
	u8  reserved1;
	u16 reserved2;

	u16 reserved3;
	u16 crc;
}mvb_pkg_wvd_cfg;

typedef struct
{
	u32 head;					// SN
	u16 mac_H;				// 出厂序号的高位
	u16 mac_L;				// 出厂序号的低位
	u32 crossid;				// CROSSID
	u16 crossid_H;				// 出厂路口编码的高位
	u16 crossid_L;				// 出厂路口编码的低位
	u16 produce_year;			// 生产年份
	u16 produce_day;			// 生产月日
	u8  addr;					// 0~255地址
	u8  baud;					// 波特率寄存器值	0		1		2		3		4		  5		6		7		8		9
										   //1200		2400		4800		9600		14400	  19200	38400	56000	57600	115200
	u8  rt_channel;			// 实时上传选择
	u8  init_flag;				// 初始化接收器模块标志
	u8  output_mode;			// 输出信号方式
	u16 vi_output_high_time;		// 车辆进入包输出的高电平时间
	u16 vo_output_high_time;		// 车辆离开包输出的高电平时间
	u16 output_low_min;			// 最小低电平时间
	u16 output_high_max;		// 最大高电平时间
	u8  handle_lost;			// 是否对丢包进行处理
	u8  check_repeat_time;		// 检查数据包是否重复的时间
	u8  simple_mode;			// RT数据模式是否是简单模式
	u16 software_version;		// 软件版本号
	u16 hardware_version;		// 硬件版本号
}mvb_param_recv;				// 接收主机参数共40个字节(占44字节)

typedef struct
{
	u16 addr_dev;				// 0检测器序列号
	u16 config_item;			// 2配置位

	//u8  car_bytes:2;			// 4 标记车辆进过次数的字节数
	//u8  detect_with_magvalue:1;	// 检测包携带地磁信息
	//u8  heart_with_magvalue:1;	// 心跳包携带地磁信息
	//u8  pkg_sent_cnt:2;		// 每帧数据发送次数
	//u8  work_mode:2;			// 00：关闭模式；01：流量统计模式；10：实时上报模式，车辆离开时不发送；11：实时上报模式，车辆离开时也发送
	u8  work_mode;

	u8  config_interval;		// 5 配置包时间间隔单分钟		
	u16 heartbeat_interval;		// 6 心跳包时间间隔单位秒

	u8  vi_threshhold;			// 8 车辆进入阈值
	u8  vo_threshhold;			// 9 车辆离开阈值

	//u8  measure_mode:2;		// 10 测量模式
	//u8  measure_freq:3;		// 测量频率
	//u8  remeasure_num:2;		// 每次输出结果的计算次数
	//u8 :1;					// 保留
	u8  measure_mode;			// 10 测量模式

	u8  recalibration_value:4;	// 11 有效的基准值计算变化范围
	u8  recalibration_time:4;	// 重新计算基准值时间

	u8  reserved1;				// 12 保留
	u8  reserved2;				// 13 保留
}mvb_param_wvd_config;			// 车检器配置参数共14个字节(占16字节)

typedef struct
{
	u16 device_addr_h;			// 设备网络接口IPv4地址高字节		例: 192.168.0.1 	192 * 256 + 168 = 49320
	u16 device_addr_l;			// 设备网络接口IPv4地址低字节						0   * 256 + 1	 = 1
	
	u16 device_mask_h;			// 设备网络接口IPv4子网掩码高字节 	例: 255.254.10.1	255 * 256 + 254 = 65534
	u16 device_mask_l;			// 设备网络接口IPv4子网掩码低字节					10  * 256 + 1	 = 2561
	
	u16 device_bcast_h;			// 设备网络接口IPv4网关高字节		例: 192.168.0.254	192 * 256 + 168 = 49320
	u16 device_bcast_l;			// 设备网络接口IPv4网关低字节						0   * 256 + 254 = 254
	
	u8  socketA_connect_mode;	// SOCKET接口A的连接方式			00: 该端口没有启用; 01: UDP通信; 02: TCP客户端; 03: TCP服务器端
	
	u16 socketA_addr_h;			// SOCKET接口A的服务器IP高字节	例: 192.168.0.1 	192 * 256 + 168 = 49320
	u16 socketA_addr_l;			// SOCKET接口A的服务器IP低字节					0   * 256 + 1	 = 1
	
	u16 socketA_port;			// SOCKET接口A的服务器端口
	
	u8  socketA_connect_state;	// SOCKET接口A的连接状态			00: 接口断开; 01: 接口已连接; 02: 接口正在监听(服务端); 03: 接口正在连接服务器(客户端)
}mvb_param_net_config;			// 网络配置参数共20个字节(占24字节)


/* 项目功能选择 */
typedef enum
{
	SocketTime_DISABLE,			// Socket不对时
	SocketTime_ENABLE			// Socket对时
}PlatFormSocketTime;			// Socket对时项
extern PlatFormSocketTime PlatformSockettime;

typedef enum
{
	Socket_DISABLE,			// Socket不使能
	Socket_ENABLE				// Socket使能
}PlatFormSocket;				// Socket使能项
extern PlatFormSocket PlatformSocket;

typedef enum
{
	Gat920_DISABLE,			// GAT920不使能
	Gat920_ENABLE				// GAT920使能
}PlatFormGat920;				// GAT920使能项
extern PlatFormGat920 PlatformGat920;

typedef enum
{
	Modbus_USART1,				// Modbus <--> USART1
	Modbus_USART2,				// Modbus <--> USART2
}PlatFormModBusUSART;			// Modbus连接串口项
extern PlatFormModBusUSART PlatformModbusUsart;

typedef enum
{
	Gat920_USART1,				// Gat920 <--> USART1
	Gat920_USART2				// Gat920 <--> USART2
}PlatFormGat920USART;			// Gat920连接串口项
extern PlatFormGat920USART PlatformGat920Usart;

typedef enum
{
	GPRS_DISABLE,				// GPRS不使能
	GPRS_ENABLE				// GPRS使能
}PlatFormGPRS;					// GPRS使能项
extern PlatFormGPRS PlatformGPRS;

typedef enum
{
	RJ45_DISABLE,				// RJ45不使能
	RJ45_ENABLE				// RJ45使能
}PlatFormRJ45;					// RJ45使能项
extern PlatFormRJ45 PlatformRJ45;

void PlatForm_SnToFunction(u32 SN);										// 根据SN码选择开启对应功能项

extern mvb_param_recv param_recv;
extern u8 a7108_initialized;
extern mvb_param_wvd_config 	param_wvd_cfg;									// 地磁配置数据


#endif /* __PLATFORM_CONFIG_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *******************/
