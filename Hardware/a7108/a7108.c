/**
  *********************************************************************************************************
  * @file		a7108.c
  * @author	MoveBroad
  * @version	V1.1
  * @date    
  * @brief	Copyright (C) 2011 AMICCOM Corp
  *********************************************************************************************************
  * @attention
  *			2017-08-02 KangYJ优化代码 增加对100K 50K 10K的宏定义开关
  *********************************************************************************************************
  */

#include "stm32f10x_lib.h"
#include "define.h"
#include "a7108reg.h"
#include "a7108.h"
#include "stm32_config.h"
#include "platform_config.h"
#include "NRF24L01.h"


//#define MOD_470M_10KBPS
#define MOD_470M_50KBPS
//#define MOD_470M_100KBPS

//#define MOD_433M_10KBPS
//#define MOD_433M_50KBPS
//#define MOD_433M_100KBPS

/*********************************************************************
**  I/O Declaration
*********************************************************************/
#define CSN			GPIO_Pin_12
#define SPIDIO			GPIO_Pin_15
#define SPICK			GPIO_Pin_13
#define GPIO2			GPIO_Pin_0
#define GPIO1			GPIO_Pin_1
#define CKO			GPIO_Pin_14

/*********************************************************************
**  Constant Declaration
*********************************************************************/
#define TIMEOUT		155
#define t0hrel			1000
#define CMD_CTRLW		0x00						//000x,xxxx	control register write
#define CMD_CTRLR		0x80						//100x,xxxx	control register read
#define CMD_IDW		0x20						//001x,xxxx	ID write
#define CMD_IDR		0xA0						//101x,xxxx	ID Read
#define CMD_DATAW		0x40						//010x,xxxx	TX FIFO Write
#define CMD_DATAR		0xC0						//110x,xxxx	RX FIFO Read
#define CMD_TFR		0x60						//0110,xxxx	TX FIFO reset
#define CMD_RFR		0xE0						//1110,xxxx	RX FIFO reset
#define CMD_RFRST		0x70						//x111,xxxx	RF reset

/*********************************************************************
**  Global Variable Declaration
*********************************************************************/
u8		timer;
u16		TimeoutFlag;
u16		RxCnt;
u32		Err_ByteCnt;
u32		Err_BitCnt;
u16		TimerCnt0;
u8		*Uartptr;
u8		UartSendCnt;
u8		CmdBuf[11];
u8		tmpbuf[64];
u16		PLL1;
u16		PLL2;
u8		Flag_MASTER;
u8		FreqBank[2];
u8		Seq;

u8		fifo_length, rf_mode;
u8		rf_buf[32];

const u8 PN9_Tab[] =
{   0xFF,0x83,0xDF,0x17,0x32,0x09,0x4E,0xD1,
    0xE7,0xCD,0x8A,0x91,0xC6,0xD5,0xC4,0xC4,
    0x40,0x21,0x18,0x4E,0x55,0x86,0xF4,0xDC,
    0x8A,0x15,0xA7,0xEC,0x92,0xDF,0x93,0x53,
    0x30,0x18,0xCA,0x34,0xBF,0xA2,0xC7,0x59,
    0x67,0x8F,0xBA,0x0D,0x6D,0xD8,0x2D,0x7D,
    0x54,0x0A,0x57,0x97,0x70,0x39,0xD2,0x7A,
    0xEA,0x24,0x33,0x85,0xED,0x9A,0x1D,0xE0
};// This table are 64bytes PN9 pseudo random code.

#ifdef MOD_470M_100KBPS
const Uint16 A7108Config[] =												//100kbps, 470MHz (IFBW =100KHz, Fdev = 37.5KHz)
{
	0x0078,   //SYSTEM CLOCK register,
	0x1092,   //PLL1 register,
	0xE014,   //PLL2 register,    470.001MHz
	0x0000,   //PLL3 register,
	0x166C,   //PLL4 register,
	0x0001,   //CRYSTAL register,
	0x35C0,   //TX1 register, Tx modulation on, Fdev = 37.5kHz
	0x037F,   //TX2 register, Tx settling delay = 80us，0337->037f,20dBm  0x035A ->FCC认证配置
	0x1895,   //RX1 register, BW = 100kHz
	0x7009,   //RX2 register, By preamble, PMD=16
	0x0000,   //ADC register 121011 0x4C00->0x4400 160310 0x0,程序重新配置
	0x0006,   //FIFO register, 0x0B FIFO_REG 3f->06,7Byte
	0x151F,   //code register,0x1507（CRC disable）->150f（crc enable）->151F(FEC_EN | CRC_EN | MANCHESTER_CODE_OFF)
	0x0C00,   //PIN register,
	0x5A00,   //CALIBRATION register,4887->0x5A00
	0x00C0,   //MODE register,
};
const Uint16  A7108Config_Page[] =											//100kbps
{
	0x35C0,   //TX1 register,     Tx modulation on, Fdev = 37.5kHz
	0x0000,   //WOR1 register
	0x0000,   //WOR2 register,[LSD:0xF800]
	0x0187,   //RFI register,0x0000->0x0187 [LSD:0x0787]
	0x0970,   //PM register,0x0970->0x0950[LSD:0x8B50]
	0x0704,   //RTH register
	0x002F,   //AGC1 register,0x000F->0x002F
	0x0000,   //AGC2 register
	0x0040,   //GIO register, GIO2=WTR,[LSD:0x0041,WTR on GIO2]
	0xD981,   //CKO register,D849->D981 [LSD:D9810x //D949, 0xD849(200us)->0xD8C9 ->0xD901 Crystal Settling Delay 400us.]
	0x0000,   //VCB register
	0x0096,   //CHG1 register, 480.001
	0x049C,   //CHG2 register, 500.001
};
#endif

#ifdef MOD_470M_50KBPS
const Uint16 A7108Config[] = 												//470MHz, 50kbps (IFBW = 50KHz, Fdev = 37.5KHz)
{
	0x0278,   //SYSTEM CLOCK register,
	0x1092,   //PLL1 register,
	0xE014,   //PLL2 register,    470.001MHz
	0x0000,   //PLL3 register,
	0x166C,   //PLL4 register,
	0x0001,   //CRYSTAL register,
	0x35C0,   //TX1 register, Tx modulation on, Fdev = 37.5kHz
	0x037F,   //TX2 register, Tx settling delay = 80us，0337->037f,20dBm  0x035A ->FCC认证配置
	0x1895,   //RX1 register, BW = 100kHz
	0x7009,   //RX2 register, By preamble, PMD=16
	0x0000,   //ADC register 121011 0x4C00->0x4400 160310 0x0,程序重新配置
	0x0006,   //FIFO register, 0x0B FIFO_REG 3f->06,7Byte
	0x151F,   //code register,0x1507（CRC disable）->150f（crc enable）->151F(FEC_EN | CRC_EN | MANCHESTER_CODE_OFF)
	0x0C00,   //PIN register,
	0x5A00,   //CALIBRATION register,4887->0x5A00
	0x00C0,   //MODE register,
};
const Uint16  A7108Config_Page[] =											//50kbps
{
	0x35C0,   //TX1 register,     Tx modulation on, Fdev = 37.5kHz
	0x0000,   //WOR1 register
	0x0000,   //WOR2 register,[LSD:0xF800]
	0x0187,   //RFI register,0x0000->0x0187 [LSD:0x0787]
	0x0970,   //PM register,0x0970->0x0950[LSD:0x8B50]
	0x0704,   //RTH register
	0x002F,   //AGC1 register,0x000F->0x002F
	0x0000,   //AGC2 register
	0x0040,   //GIO register, GIO2=WTR,[LSD:0x0041,WTR on GIO2]
	0xD981,   //CKO register,D849->D981 [LSD:D9810x //D949, 0xD849(200us)->0xD8C9 ->0xD901 Crystal Settling Delay 400us.]
	0x0000,   //VCB register
	0x0096,   //CHG1 register, 480.001
	0x049C,   //CHG2 register, 500.001
};
#endif

#ifdef MOD_470M_10KBPS
const Uint16 A7108Config[] =												//470MHz, 10kbps (IFBW = 10KHz, Fdev = 37.5kHz)
{
	0x1278,   //SYSTEM CLOCK register,
	0x1092,   //PLL1 register,
	0xE014,   //PLL2 register,    470.001MHz
	0x0000,   //PLL3 register,
	0x166C,   //PLL4 register,
	0x0001,   //CRYSTAL register,
	0x35C0,   //TX1 register, Tx modulation on, Fdev = 37.5kHz
	0x037F,   //TX2 register, Tx settling delay = 80us，0337->037f,20dBm  0x035A ->FCC认证配置
	0x1895,   //RX1 register, BW = 100kHz
	0x7009,   //RX2 register, By preamble, PMD=16
	0x0000,   //ADC register 121011 0x4C00->0x4400 160310 0x0,程序重新配置
	0x0006,   //FIFO register, 0x0B FIFO_REG 3f->06,7Byte
	0x151F,   //code register,0x1507（CRC disable）->150f（crc enable）->151F(FEC_EN | CRC_EN | MANCHESTER_CODE_OFF)
	0x0C00,   //PIN register,
	0x5A00,   //CALIBRATION register,4887->0x5A00
	0x00C0,   //MODE register,
};
const Uint16  A7108Config_Page[] =											//10kbps
{
	0x35C0,   //TX1 register,     Tx modulation on, Fdev = 37.5kHz
	0x0000,   //WOR1 register
	0x0000,   //WOR2 register,[LSD:0xF800]
	0x0187,   //RFI register,0x0000->0x0187 [LSD:0x0787]
	0x0970,   //PM register,0x0970->0x0950[LSD:0x8B50]
	0x0704,   //RTH register
	0x002F,   //AGC1 register,0x000F->0x002F
	0x0000,   //AGC2 register
	0x0040,   //GIO register, GIO2=WTR,[LSD:0x0041,WTR on GIO2]
	0xD981,   //CKO register,D849->D981 [LSD:D9810x //D949, 0xD849(200us)->0xD8C9 ->0xD901 Crystal Settling Delay 400us.]
	0x0000,   //VCB register
	0x0096,   //CHG1 register, 480.001
	0x049C,   //CHG2 register, 500.001
};
#endif

#ifdef MOD_433M_100KBPS
const Uint16 A7108Config[] =												//433MHz, 100kbps (IFBW = 100KHz, Fdev = 37.5KHz)
{
	0x0078,   //SYSTEM CLOCK register,
	0x1087,   //PLL1 register,
	0x6814,   //PLL2 register,    433.301MHz
	0x0000,   //PLL3 register,
	0x166C,   //PLL4 register,
	0x0011,   //CRYSTAL register, 05  120811 0x0001  -> 0x0011
	0x35C0,   //TX1 register, Tx modulation on, Fdev = 37.5kHz  0x06
	0x037F,   //TX2 register, Tx settling delay = 80us，0337->037f,20dBm  0x035A ->FCC认证配置
	0x1895,   //RX1 register,     BW = 100kHz
	0x7009,   //RX2 register, By preamble, PMD=16
	0x4C00,   //ADC register
	0x0003,   //FIFO register, 0x0B FIFO_REG 3f->03
	0x151F,   //code register,1507->150f,使能crc 120804
	0x0C00,   //PIN register,
	0x4887,   //CALIBRATION register,
	0x40C0,   //MODE register,
};
const Uint16 A7108Config_Page[] =											//100kbps
{
	0x35C0,   //TX1 register,     Tx modulation on, Fdev = 37.5kHz
	0x0000,   //WOR1 register
	0x0000,   //WOR2 register
	0x0000,   //RFI register
	0x0970,   //PM register   04
	0x0704,   //RTH register
	0x000F,   //AGC1 register
	0x0000,   //AGC2 register
	0x0040,   //GIO register, GIO2=WTR
	0xD849,   //CKO register  09
	0x0000,   //VCB register
	0x0686,   //CHG1 register, 430.001
	0x0F87,   //CHG2 register, 435.001
};
#endif

#ifdef MOD_433M_50KBPS

#endif

#ifdef MOD_433M_10KBPS

#endif


const u8 HopTab[] = {0,1,2}; //hopping channel

#if defined (MOD_470M_100KBPS) || defined(MOD_470M_50KBPS) || defined(MOD_470M_10KBPS)
const u16 MasterFreq_Tab[]=
{
    0x1092, 0xE014,			//CH0, freq 470.001MHz
    0x1099, 0x2014,			//CH1, freq 490.001MHz
    0x109F, 0x6014			//CH2, freq 510.001MHz
};

const u16 SlaveFreq_Tab[]=
{
    0x1092, 0xF014,			//CH0, freq 470.201MHz
    0x1099, 0x3014,			//CH1, freq 490.201MHz
    0x109F, 0x7014			//CH2, freq 510.201MHz
};
#endif

#if defined (MOD_433M_100KBPS) || defined(MOD_433M_50KBPS) || defined(MOD_433M_10KBPS)
const u16 MasterFreq_Tab[]=
{
	0x1087, 0x6814,		//CH0, freq 433.301MHz
	0x1087, 0x8014,		//CH1, freq 433.601MHz
	0x1087, 0xA014			//CH2, freq 434.001MHz
};

const u16 SlaveFreq_Tab[]=
{
	0x1087, 0x7814,		//CH0, freq 433.501MHz
	0x1087, 0x9014,		//CH1, freq 433.801MHz
	0x1087, 0xB014			//CH2, freq 434.201MHz
};
#endif

/*********************************************************************
**  function Declaration
*********************************************************************/
void InitTimer0(void);
void initUart1(void);
void Timer0ISR (void);
void A7108_Reset(void);
void A7108_ResetTXFIFO(void);
void A7108_ResetRXFIFO(void);
void A7108_WriteID(u8* ID_Tab);
void A7108_WriteFIFO(void);
void A7108_WriteReg(u8 address, u16 dataWord);
u16  A7108_ReadReg(u8 address);
void A7108_WritePage(u8 address, u16 dataWord);
u16  A7108_ReadPage(u8 address);
void ByteSend(u8 src);
u8   ByteRead(void);
void StrobeCmd(u8 cmd);
void RxPacket(void);
void A7108_Cal(void);
void TX_En(void);
void RX_En(void);
void TX_En1(void);
void RX_En1(void);
void initUart0(void);
void Err_State(void);
void CheckTemp(void);
void FreqSet(u8);
void A7108_Config(void);

u8 KEY1, KEY2;
void Select_a7108_NRF() {
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
}
void NotSelect_a7108_NRF() {
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
}
void sdio_out(){
	GPIO_InitTypeDef GPIO_InitStructure;
	/* A7108片选  PB12 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//输出模式最大速度50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			//通用推挽输出模式
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
void sdio_in(){
	GPIO_InitTypeDef GPIO_InitStructure;
	/* A7108片选  PB12 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;			  					 
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//输出模式最大速度50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;			//通用推挽输出模式
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}
/************************************************************************
**  Reset_RF
************************************************************************/
void A7108_Reset(void)
{
	Select_a7108_NRF();
	ByteSend(CMD_RFRST);
	NotSelect_a7108_NRF();
}

/************************************************************************
**  A7108_ResetTXFIFO The address pointer of TX FIFO is reset to 0x00
************************************************************************/
void A7108_ResetTXFIFO(void)
{
	Select_a7108_NRF();
	ByteSend(CMD_TFR);
	NotSelect_a7108_NRF();
}

/************************************************************************
**  A7108_ResetRXFIFO  The address pointer of RX FIFO is reset to 0x00 
************************************************************************/
void A7108_ResetRXFIFO(void)
{
	Select_a7108_NRF();
	ByteSend(CMD_RFR);
	NotSelect_a7108_NRF();
}

/************************************************************************
**  WriteID固定地址 0x55 aa 55 xaa
************************************************************************/
void A7108_WriteID(u8* ID_Tab)
{
	u8 i;
	u8 d1,d2,d3,d4;
	
	Select_a7108_NRF();
	ByteSend(CMD_IDW);
	for (i = 0; i < 4; i++) {
		ByteSend(ID_Tab[i]);
	}
	NotSelect_a7108_NRF();

	Select_a7108_NRF();
	ByteSend(0xA0);
	d1 = ByteRead();
	d2 = ByteRead();
	d3 = ByteRead();
	d4 = ByteRead();
	if (d1 != ID_Tab[0]) {
		while(1);
	}
	else {
		if (d2 != ID_Tab[1]) {
			while(1);
		}
		else {
			if (d3 != ID_Tab[2]) {
				while(1);
			}
			else {
				if (d4 != ID_Tab[3]) {
					while(1);
				}
				else {
					__nop();
				}
			}
		}
	}

	NotSelect_a7108_NRF();
}

/************************************************************************
**  FreqSet
************************************************************************/
void FreqSet(u8 ch)
{
	if (Flag_MASTER){
		A7108_WriteReg(PLL1_REG, MasterFreq_Tab[2*ch]);			//setting PLL1
		A7108_WriteReg(PLL2_REG, MasterFreq_Tab[2*ch+1]);			//setting PLL2
	}else{
		A7108_WriteReg(PLL1_REG, SlaveFreq_Tab[2*ch]);			//setting PLL1
		A7108_WriteReg(PLL2_REG, SlaveFreq_Tab[2*ch+1]);			//setting PLL2
	}
}

/************************************************************************
**  A7108_WriteReg
************************************************************************/
void A7108_WriteReg(u8 address, u16 dataWord)
{
	u8 i;

	sdio_out();
	Select_a7108_NRF();
	address |= CMD_CTRLW;
	for(i = 0; i < 8; i++)
	{
		if(address & 0x80)
			GPIO_SetBits(GPIOB, SPIDIO);
		else
			GPIO_ResetBits(GPIOB, SPIDIO);

		GPIO_SetBits(GPIOB, SPICK);
		__nop();
		GPIO_ResetBits(GPIOB, SPICK);
		address = address << 1;
	}
	__nop();

	for (i = 0; i < 16; i++)
	{
		if (dataWord & 0x8000)
			GPIO_SetBits(GPIOB, SPIDIO);
		else
			GPIO_ResetBits(GPIOB, SPIDIO);

		GPIO_SetBits(GPIOB, SPICK);
		__nop();
		if (i < 15)
			GPIO_ResetBits(GPIOB, SPICK);
		dataWord = dataWord<< 1;
	}
	sdio_in();
	GPIO_ResetBits(GPIOB, SPICK);

	NotSelect_a7108_NRF();
}

/************************************************************************
**  A7108_ReadReg
************************************************************************/
u16 A7108_ReadReg(u8 address)
{
	u8 i;
	u16 tmp;

	sdio_out();
	Select_a7108_NRF();
	address |= CMD_CTRLR;
	for (i = 0; i < 8; i++) {
		if (address & 0x80)
			GPIO_SetBits(GPIOB, SPIDIO);
		else
			GPIO_ResetBits(GPIOB, SPIDIO);
		__nop();
		GPIO_SetBits(GPIOB, SPICK);
		__nop();
		if (i < 7)
			GPIO_ResetBits(GPIOB, SPICK);
        address = address << 1;
	}
	sdio_in();												//因为数据是在下降沿后输出的,所以要在下降沿前切换为输入状态.
	GPIO_ResetBits(GPIOB, SPICK);
	__nop();
	for (i = 0; i < 16; i++) {
		if(GPIO_ReadInputDataBit(GPIOB,SPIDIO)==0)
			tmp = (tmp << 1);
		else
			tmp = (tmp << 1) | 0x01;
		GPIO_SetBits(GPIOB, SPICK);
		__nop();
		GPIO_ResetBits(GPIOB, SPICK);
	}
	NotSelect_a7108_NRF();
	
	return tmp;
}

/************************************************************************
**  A7108_WritePage
************************************************************************/
void A7108_WritePage(u8 address, u16 dataWord)
{
	u16 tmp;

	tmp = address;
	tmp = ((tmp << 12) | A7108Config[CRYSTAL_REG]);
	A7108_WriteReg(CRYSTAL_REG, tmp);
	A7108_WriteReg(TX1_REG, dataWord);
}

/************************************************************************
**  A7108_ReadPage
************************************************************************/
u16 A7108_ReadPage(u8 address)
{
	u16 tmp;

	tmp = address;
	tmp = ((tmp << 12) | A7108Config[CRYSTAL_REG]);
	A7108_WriteReg(CRYSTAL_REG, tmp);
	tmp = A7108_ReadReg(TX1_REG);

	return tmp;
}


/************************************************************************
**  ByteSend
************************************************************************/
void ByteSend(u8 src)
{
	u8 i;

	sdio_out();
	for (i = 0; i < 8; i++) {
		if (src & 0x80)
			GPIO_SetBits(GPIOB, SPIDIO);
		else
			GPIO_ResetBits(GPIOB, SPIDIO);
		__nop();
		GPIO_SetBits(GPIOB, SPICK);
		__nop();
		if (i < 7)
			GPIO_ResetBits(GPIOB, SPICK);
        src = src << 1;
	}
	sdio_in();										//重新设置为输入状态
	GPIO_ResetBits(GPIOB, SPICK);
}

/************************************************************************
**  ByteRead
************************************************************************/
u8 ByteRead(void)
{
	u8 i,tmp;

	sdio_in();
	for(i = 0; i < 8; i++)
	{
		if (GPIO_ReadInputDataBit(GPIOB, SPIDIO) == 0)
			tmp = tmp << 1;
		else
			tmp = (tmp << 1) | 0x01;
		GPIO_SetBits(GPIOB, SPICK);
		__nop();
		GPIO_ResetBits(GPIOB, SPICK);
	}
	
	return tmp;
}

/*********************************************************************
** initRF
*********************************************************************/
void initRF_a7108(void)
{
 	u8 ID[4]={0x55,0xaa,0x55,0xaa};
	a7108_initialized = 0;

	NotSelect_a7108_NRF();
	GPIO_ResetBits(GPIOB, SPICK);
	rf_mode = 0;
	fifo_length = 0;
	
	A7108_Reset();//reset A7108 chip 寄存器估计都恢复到初始值了
	A7108_WriteReg(PIN_REG, A7108Config[PIN_REG]);

	delay_5us(120);

	A7108_WritePage(CKO_PAGE, A7108Config_Page[CKO_PAGE]);
	A7108_Config();//config A7108 chip
	RSSI_measurement_enable();//121012   

	delay_5us(100);
  
	A7108_WriteID(ID);//write ID code
	A7108_Cal();//calibration IF,vco
	if (Flag_MASTER){//100kbps时，此代码可以把发和收的频率错开200Khz。
	}else{
	A7108_WriteReg(RX1_REG, (A7108Config[RX1_REG] | 0x0002));   //bit ULS=1(low side band)
	}
	Seq = 0;//0:470M,1:490M,2:510M
	FreqSet(HopTab[Seq]);
	a7108_initialized = 1;
}
//将a7108设置成master或者slave,在cfg模式时需要设置沉master模式.
void a7108_setmaster(u8 master)
{
	if (Flag_MASTER) {//100kbps时，此代码可以把发和收的频率错开200Khz。
		A7108_WriteReg(RX1_REG, (A7108Config[RX1_REG]));   //bit ULS=1(low side band)
	} else {
		A7108_WriteReg(RX1_REG, (A7108Config[RX1_REG] | 0x0002));   //bit ULS=1(low side band)
	}
}


/*********************************************************************
** A7108_WriteFIFO
*********************************************************************/
void A7108_WriteFIFO(void)
{
	u8 i;

	A7108_ResetTXFIFO();
	Select_a7108_NRF();
	ByteSend(CMD_DATAW);//send write fifo cmmmand
	for (i = 0; i <64; i++) {
		ByteSend(PN9_Tab[i]);
	}
	NotSelect_a7108_NRF();
}
void mvb_A7108_WriteFIFO(u8* pbuf,u8 length)
{
	u8 i;

	A7108_ResetTXFIFO();
	Select_a7108_NRF();
	ByteSend(CMD_DATAW);//send write fifo cmmmand
	for (i = 0; i < length; i++) {
		ByteSend(pbuf[i]);
	}
	NotSelect_a7108_NRF();
}

/*********************************************************************
** Err_State
*********************************************************************/
void Err_State(void)
{
    while(1);
}

/*********************************************************************
** TX_En
*********************************************************************/
void TX_En(void)
{
	A7108_WriteReg(MODE_REG, 0x4078);
}

/*********************************************************************
** Rx_En
*********************************************************************/
void RX_En(void)
{
	A7108_WriteReg(MODE_REG, 0x4068);
}


/*********************************************************************
** RxPacket
*********************************************************************/
void mvb_RxPacket(u8* pbuf,u8 length)
{
	u8 i;
	
	RxCnt++;
	if(fifo_length != length) {
		A7108_WriteReg(FIFO_REG,(length-1));//设置fifo长度
		fifo_length = length;
  	}
	A7108_ResetRXFIFO();
	Select_a7108_NRF();
	ByteSend(CMD_DATAR);//send read fifo command
	for (i=0; i <length; i++) {
		pbuf[i] = ByteRead();
	}

	NotSelect_a7108_NRF();
}
/*********************************************************************
** A7108_Cal
*********************************************************************/
void A7108_Cal(void)
{
    u16 tmp;
    u8 fbcf,vbcf;

    A7108_WriteReg(MODE_REG, (A7108Config[MODE_REG] | 0x1802));     //RSSI, VCC & IF calibration
    do {
		tmp = A7108_ReadReg(MODE_REG);
		tmp &= 0x1802;
    } while (tmp);

    tmp = A7108_ReadReg(CALIBRATION_REG);

    fbcf = (tmp >>4) & 0x01;
    if(fbcf)
      Err_State();
       
    A7108_WriteReg(PLL1_REG, MasterFreq_Tab[2]);
    A7108_WriteReg(PLL2_REG, MasterFreq_Tab[3]);
    A7108_WriteReg(MODE_REG, (A7108Config[MODE_REG] | 0x0004));
    do{
        tmp = A7108_ReadReg(MODE_REG);
        tmp &= 0x0004;
    }while (tmp);
    tmp = A7108_ReadReg(CALIBRATION_REG);
    vbcf = (tmp >>8) & 0x01;
    if (vbcf)
        Err_State();
}


/*********************************************************************
** A7108_Config
*********************************************************************/
void A7108_Config(void)
{
    u8 i;

    for (i=0; i<16; i++)
        A7108_WriteReg(i, A7108Config[i]);
    for (i=0; i<13; i++)
        A7108_WritePage(i, A7108Config_Page[i]);
}


/*********************************************************************
** Strobe Command
*********************************************************************/
void StrobeCmd(u8 cmd)
{
	u8 i;

	sdio_out();

	Select_a7108_NRF();
	for(i = 0; i < 8; i++){
		if(cmd & 0x80)
			GPIO_SetBits(GPIOB, SPIDIO);
		else
			GPIO_ResetBits(GPIOB, SPIDIO);

		__nop();

		GPIO_SetBits(GPIOB, SPICK);

		if (i < 7)
			GPIO_ResetBits(GPIOB, SPICK);
		cmd = cmd << 1;
	}
	sdio_in();//120923	 重新返回到输入状态
	GPIO_ResetBits(GPIOB, SPICK);
	 
	NotSelect_a7108_NRF();
}

/**************************************************
函数：mvb_Check_ACK()

描述：
    检查接收设备有无接收到数据包，设定没有收到应答信
	号是否重发
**************************************************/
u8 mvb_Check_ACK(u8 length)
{
	u32 temp32_this = 0;
	
	mvb_RX_Mode(length);//进入接收模式

	while (!ack_geted) {											//没有获得应当信号,同时重复发送次数没溢出. 120707
		if (temp32_this >= 1000) {									//等待5ms 
			break;
		}
		delay_5us(1);
		temp32_this++;
	}

	if(ack_geted == 1){//收到应答
		ack_geted = 0;
		return(0x00);
	}else{
		ack_geted = 0;
		return(0xff);
	}
}

void mvb_powerdown_mode_int()
{
	func_num = A7108_SLEEP_M;
	StrobeCmd(CMD_SLEEP);
	delay_5us(10);				//这个延时没有的话,会抖动.
	rf_mode = SLEEP_MODE;
}
void mvb_powerdown_mode()
{
	u8 delay_count = 255;
	
	func_num = A7108_SLEEP_M;
	EXTI_GenerateSWInterrupt(EXTI_Line1);
	while (func_num&&delay_count) {	//120718 等待中断被处理
		delay_count--;
	}
}
void mvb_RX_Mode_int(u8 length)//增加一个mvb_md01_data参数.
{
	u16 dt;
	
	StrobeCmd(CMD_STANDBY);

	StrobeCmd(CMD_RX);
#if defined (MOD_470M_10KBPS) || defined (MOD_433M_10KBPS)
	dt = 1000;
#endif
#if defined (MOD_470M_50KBPS) || defined (MOD_433M_50KBPS)
	dt = 200;
#endif
#if defined (MOD_470M_100KBPS) || defined (MOD_433M_100KBPS)
	dt = 100;
#endif
	while((GPIO_ReadInputDataBit(GPIOA,GPIO2)==0)&&(dt--)){//120811 等待拉高
		delay_5us(1);
	}
    	rf_mode = RX_MODE;

	if(fifo_length != length) {
		A7108_WriteReg(FIFO_REG,(length-1));//设置fifo长度
		fifo_length = length;
	}
}

void mvb_RX_Mode(u8 length)//增加一个mvb_md01_data参数.
{
#if defined (MOD_470M_10KBPS) || defined (MOD_433M_10KBPS)
	u16 delay_count = 2550;
#endif
#if defined (MOD_470M_50KBPS) || defined (MOD_433M_50KBPS)
	u16 delay_count = 510;
#endif
#if defined (MOD_470M_100KBPS) || defined (MOD_433M_100KBPS)
	u8 delay_count = 255;
#endif
	spi_data_num = length;
	func_num = RX_MODE_M;
	EXTI_GenerateSWInterrupt(EXTI_Line1);
	while(func_num&&delay_count){//120718 等待中断被处理???
		delay_count--;
	}
}

u8 mvb_TX_Mode_int(u8* pbuf,u8 length)//增加一个mvb_md01_data参数.
{
	u16 dt;

	StrobeCmd(CMD_STANDBY);

	if(fifo_length != length) {
		A7108_WriteReg(FIFO_REG,(length-1));//设置fifo长度
		fifo_length = length;
	}

	mvb_A7108_WriteFIFO(pbuf,length);

	StrobeCmd(CMD_TX);
#if defined (MOD_470M_10KBPS) || defined (MOD_433M_10KBPS)
	dt = 1000;
#endif
#if defined (MOD_470M_50KBPS) || defined (MOD_433M_50KBPS)
	dt = 200;
#endif
#if defined (MOD_470M_100KBPS) || defined (MOD_433M_100KBPS)
	dt = 100;
#endif
	while((GPIO_ReadInputDataBit(GPIOA,GPIO2)==0)&&(dt--)){//120811等待拉高
		delay_5us(1);
	}

	rf_mode = TX_MODE;
	
#if defined (MOD_470M_10KBPS) || defined (MOD_433M_10KBPS)
	dt = 20000;//等待10ms 等待数据发完
#endif
#if defined (MOD_470M_50KBPS) || defined (MOD_433M_50KBPS)
	dt = 4000;//等待10ms 等待数据发完
#endif
#if defined (MOD_470M_100KBPS) || defined (MOD_433M_100KBPS)
	dt = 2000;//等待10ms 等待数据发完
#endif
	
	while ((GPIO_ReadInputDataBit(GPIOA,GPIO2))&&dt)
		dt--;

	__nop();

	if (dt == 0) {			//发送不成功
		return 1;
	} else {				//发送成功
		return 0;
	}
}
unsigned char mvb_TX_Mode(u8 *pBuf, u8 bytes2send, u8 bytes2recv)
{
#if defined (MOD_470M_10KBPS) || defined (MOD_433M_10KBPS)
	u16 delay_count = 2550;
#endif
#if defined (MOD_470M_50KBPS) || defined (MOD_433M_50KBPS)
	u16 delay_count = 510;
#endif
#if defined (MOD_470M_100KBPS) || defined (MOD_433M_100KBPS)
	u8 delay_count = 255;
#endif
	spi_data_p = pBuf;
	spi_data_num = bytes2send;
	func_num = TX_MODE_M;
	EXTI_GenerateSWInterrupt(EXTI_Line1);
	while (func_num&&delay_count) {		//120718 等待中断被处理
		delay_count--;
	}
	mvb_RX_Mode(bytes2recv);				//发完进入RX_MODE
	
	return(spi_status);          		  //返回发送状态,0表示发送成功.
}

/*********************************************************************
** RSSI_measurement_enable
*********************************************************************/
void RSSI_measurement_enable(void)
{
	A7108_WriteReg(ADC_REG, A7108Config[ADC_REG] | 0x8100);	//ARSSI=1 and CDM=1	0x8100	 
	A7108_WriteReg(MODE_REG, A7108Config[MODE_REG] |1);		//ADCM = 1
}
