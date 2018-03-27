#ifndef _MB_CONFIG_H
#define _MB_CONFIG_H

#ifdef __cplusplus
PR_BEGIN_EXTERN_C
#endif
/* ----------------------- Defines ------------------------------------------*/
/*! \defgroup modbus_cfg Modbus Configuration
 *
 * Most modules in the protocol stack are completly optional and can be
 * excluded. This is specially important if target resources are very small
 * and program memory space should be saved.<br>
 *
 * All of these settings are available in the file <code>mbconfig.h</code>
 */
/*! \addtogroup modbus_cfg
 *  @{
 */
/*! \brief If Modbus ASCII support is enabled. */
#define MB_ASCII_ENABLED                        (  0 )

/*! \brief If Modbus RTU support is enabled. */
#define MB_RTU_ENABLED                          (  1 )

/*! \brief If Modbus TCP support is enabled. */
#define MB_TCP_ENABLED                          (  0 )

/*! \brief The character timeout value for Modbus ASCII.
 *
 * The character timeout value is not fixed for Modbus ASCII and is therefore
 * a configuration option. It should be set to the maximum expected delay
 * time of the network.
 */
#define MB_ASCII_TIMEOUT_SEC                    (  1 )

/*! \brief Timeout to wait in ASCII prior to enabling transmitter.
 *
 * If defined the function calls vMBPortSerialDelay with the argument
 * MB_ASCII_TIMEOUT_WAIT_BEFORE_SEND_MS to allow for a delay before
 * the serial transmitter is enabled. This is required because some
 * targets are so fast that there is no time between receiving and
 * transmitting the frame. If the master is to slow with enabling its 
 * receiver then he will not receive the response correctly.
 */
#ifndef MB_ASCII_TIMEOUT_WAIT_BEFORE_SEND_MS
#define MB_ASCII_TIMEOUT_WAIT_BEFORE_SEND_MS    ( 0 )
#endif

/*! \brief Maximum number of Modbus functions codes the protocol stack
 *    should support.
 *
 * The maximum number of supported Modbus functions must be greater than
 * the sum of all enabled functions in this file and custom function
 * handlers. If set to small adding more functions will fail.
 */
#define MB_FUNC_HANDLERS_MAX                    ( 16 )

/*! \brief Number of bytes which should be allocated for the <em>Report Slave ID
 *    </em>command.
 *
 * This number limits the maximum size of the additional segment in the
 * report slave id function. See eMBSetSlaveID(  ) for more information on
 * how to set this value. It is only used if MB_FUNC_OTHER_REP_SLAVEID_ENABLED
 * is set to <code>1</code>.
 */
#define MB_FUNC_OTHER_REP_SLAVEID_BUF           ( 32 )

/*! \brief If the <em>Report Slave ID</em> function should be enabled. */
#define MB_FUNC_OTHER_REP_SLAVEID_ENABLED       (  1 )

/*! \brief If the <em>Read Input Registers</em> function should be enabled. */
#define MB_FUNC_READ_INPUT_ENABLED              (  1 )

/*! \brief If the <em>Read Holding Registers</em> function should be enabled. */
#define MB_FUNC_READ_HOLDING_ENABLED            (  1 )

/*! \brief If the <em>Write Single Register</em> function should be enabled. */
#define MB_FUNC_WRITE_HOLDING_ENABLED           (  1 )

/*! \brief If the <em>Write Multiple registers</em> function should be enabled. */
#define MB_FUNC_WRITE_MULTIPLE_HOLDING_ENABLED  (  1 )

/*! \brief If the <em>Read Coils</em> function should be enabled. */
#define MB_FUNC_READ_COILS_ENABLED              (  1 )

/*! \brief If the <em>Write Coils</em> function should be enabled. */
#define MB_FUNC_WRITE_COIL_ENABLED              (  1 )

/*! \brief If the <em>Write Multiple Coils</em> function should be enabled. */
#define MB_FUNC_WRITE_MULTIPLE_COILS_ENABLED    (  1 )

/*! \brief If the <em>Read Discrete Inputs</em> function should be enabled. */
#define MB_FUNC_READ_DISCRETE_INPUTS_ENABLED    (  1 )

/*! \brief If the <em>Read/Write Multiple Registers</em> function should be enabled. */
#define MB_FUNC_READWRITE_HOLDING_ENABLED       (  1 )

/* ----------------------- Defines  Register---------------------------------*/
#define REG_RECV_START 0
#define REG_RECV_NREGS 32

#define REG_NET_START 0x0100
#define REG_NET_NREGS 16

#define REG_WVD_START 0x1000
#define REG_WVD_NREGS 16

#define REG_OUTPUT_START 0x2000


//Modbus寄存器地址
/*
0x0000	地址	1-254	R,W	
0x0001	波特率	0-9	R,W	
0x0002	主动上传的通道		R,W	
0x0003	接收器的出厂序号的高位		R	
0x0004	接收器的出厂序号的低位		R	
0x0005	接收器的生产年份		R	
0x0006	接收器的生产月日		R	
0x0007	初始化接收器模块标志		W	
0x0008	接收器可配对的发送器的端口数		R	
0x0009	接收器检测到对应车检器后的输出信号方式	0-6	R,W	
0x000A	车辆进入包输出的高电平时间（模式0时用于检测到车辆进入包丢失时的输出时间），以100ms为单位，用于模式0/1/3/4/6	1-255	R,W	
0x000B	车辆离开包输出的高电平时间，以100ms为单位，用于模式2/3/5/6。	1-255	R,W	
0x000C	丢包时的低电平输出时间，以100ms为单位，用于模式0/1/2/3/4/5/6	0-255	R,W	
0x000D	高电平的最长输出时间,用于0	0-255	R,W	
0x000E	检测到发送器丢包时，是否对丢包进行处理，为0表示只对接收到的数据包进行处理，为1表示检测到丢包时，会通过逻辑判断对对应的丢包进行处理	0/1	R,W	
0x000F	检查数据包是否重复的时间，以200ms为单位，默认为0。	0-255	R,W	
0x0010	接收器软件版本号			0-65535	R,O
0x0011	接收器硬件版本号			0-65535	R,O
0x0012	接收器的系统时间的高位		0-65535	R,W
0x0013	接收器的系统时间的低位		0-65535	R,W
0x0014	接收器的出厂路口编码的高位	0-65535	R,O
0x0015	接收器的出厂路口编码的低位	0-65535	R,O

0x1000	要配置的车检器出厂序号				R,W
0x1001	要配置的车检器信息标志位				R,W
0x1002	要配置的车检器工作模式及通信参数		R,W
0x1003	要配置的车检器流量统计或心跳时间间隔	R,W
0x1004	要配置的车检器配置时间间隔			R,W
0x1005	要配置的车检器灵敏度阈值				R,W
0x1006	要配置的车检器车辆离开参数			R,W
0x1007	地磁检测参数						R,W
0x1008	要配置的车检器基准值更新阈值			R,W
0x1009	要配置的车检器重新计算基准值的参数		R,W

0x0100	设备网络接口IPv4地址高字节			R,W
0x0101	设备网络接口IPv4地址低字节			R,W
0x0102	设备网络接口IPv4子网掩码高字节		R,W
0x0103	设备网络接口IPv4子网掩码低字节		R,W
0x0104	设备网络接口IPv4网关高字节			R,W
0x0105	设备网络接口IPv4网关低字节			R,W
0x0106	SOCKET接口A的连接方式			0-3	R,W
0x0107	SOCKET接口A的服务器IP高字节			R,W
0x0108	SOCKET接口A的服务器IP低字节			R,W
0x0109	SOCKET接口A的服务器端口				R,W
0x010A	SOCKET接口A的连接状态			0-3	R,O
*/

#define REG_READ_SIMPLE_DATA					0xFC00	//读取简单信息地址
#define REG_READ_COMPLETE_DATA				0xFD00	//读取完整信息地址
#define REG_READ_MAC_ADDRESS					0xFFF0	//读取完整信息地址

#define REG_RW_ADDR							0x0000
#define REG_RW_BAUD							0x0001
#define REG_RW_RT_CHANNEL					0x0002
#define REG_RO_MAC_H						0x0003
#define REG_RO_MAC_L						0x0004
#define REG_RO_PRODUCE_YEAR					0x0005
#define REG_RO_PRODUCE_DAY					0x0006
#define REG_RW_INIT_FLAG						0x0007
#define REG_RO_PORT_NUM						0x0008
#define REG_RW_OUTPUT_MODE					0x0009
#define REG_RW_VI_OUTPUT_HIGH_TIME				0x000A
#define REG_RW_VO_OUTPUT_HIGH_TIME				0x000B
#define REG_RW_OUTPUT_LOW_MIN					0x000C
#define REG_RW_OUTPUT_HIGH_MAX				0x000D
#define REG_RW_HANDLE_LOST					0x000E
#define REG_RW_CHECK_REPEAT_TIME				0x000F
#define REG_RO_SOFTWARE_VERSION				0x0010
#define REG_RO_HARDWARE_VERSION				0x0011
#define REG_RW_RTC_TIME_H					0x0012
#define REG_RW_RTC_TIME_L					0x0013
#define REG_RO_CROSSID_H						0x0014
#define REG_RO_CROSSID_L						0x0015

#define REG_RW_WVD_ID						0x1000
#define REG_RW_WVD_CONFIG_FLAG				0x1001
#define REG_RW_WVD_WORKMODE					0x1002
#define REG_RW_WVD_HEARTBEAT_INTERVAL			0x1003
#define REG_RW_WVD_CONFIG_INTERVAL				0x1004
#define REG_RW_WVD_VI_THRESHHOLD				0x1005
#define REG_RW_WVD_VO_THRESHHOLD				0x1006
#define REG_RW_WVD_MEASURE_MODE				0x1007
#define REG_RW_WVD_REBASE_VALUE				0x1008
#define REG_RW_WVD_REBASE_TIME				0x1009

#define REG_RW_NET_DEVICE_ADDR_H				0x0100
#define REG_RW_NET_DEVICE_ADDR_L				0x0101
#define REG_RW_NET_DEVICE_MASK_H				0x0102
#define REG_RW_NET_DEVICE_MASK_L				0x0103
#define REG_RW_NET_DEVICE_BCAST_H				0x0104
#define REG_RW_NET_DEVICE_BCAST_L				0x0105
#define REG_RW_NET_SOCKETA_CONNECT_MODE			0x0106
#define REG_RW_NET_SOCKETA_ADDR_H				0x0107
#define REG_RW_NET_SOCKETA_ADDR_L				0x0108
#define REG_RW_NET_SOCKETA_PORT				0x0109
#define REG_RO_NET_SOCKETA_CONNECT_STATE		0x010A



/*! @} */
#ifdef __cplusplus
    PR_END_EXTERN_C
#endif
#endif
