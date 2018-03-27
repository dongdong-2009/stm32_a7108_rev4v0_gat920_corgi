#define NRF_GLOBALS

#include "NRF24L01.h"

//#include "globals.h"


void SPI2_NRF24L01_Init(void);
void RX_Mode(void);
void TX_Mode(void);
extern u8 ack_geted;//120519


uchar TX_ADDRESS[TX_ADR_WIDTH] = {0xb0,0xb2,0xb3,0xb4,0x5a};//0x34,0x43,0x10,0x10,0x01};  // 路由节点地址
uchar RX_ADDRESS0[TX_ADR_WIDTH] = {0xb0,0xb2,0xb3,0xb4,0x5a};
uchar RX_ADDRESS1[TX_ADR_WIDTH] = {0xb2,0xb2,0xb3,0xb4,0x5a};//写地址寄存器是先写低字节，因此后面几个字节相同
uchar RX_ADDRESS2[1] = {0xb1};
uchar RX_ADDRESS3[1] = {0xb5};
uchar RX_ADDRESS4[1] = {0xb3};
uchar RX_ADDRESS5[1] = {0xb4};
uchar RX_ADDRESSES[RECV_MAX][14];//0~4 5~10,11,12,13,14
u8 spi_reg,spi_data,*spi_data_p,spi_data_num,func_num,spi_status;




/****************************************************************************
* 名    称：delay_ms(unsigned int x)
* 功    能：延时基数为1毫秒程序
* 入口参数：x   延时的毫秒数
* 出口参数：无
* 说    明：无
* 调用方法：delay_ms(1);
****************************************************************************/  
void delay_ms(unsigned int x)
{
    unsigned int i,j;
    i=0;
    for(i=0;i<x;i++)
    {
       j=108;
       while(j--);
    }
}

/****************************************************************************
* 名    称：delay_ms(unsigned int x)
* 功    能：延时基数为1毫秒程序
* 入口参数：x   延时的毫秒数
* 出口参数：无
* 说    明：无
* 调用方法：delay_ms(1);
****************************************************************************/  
void delay_10us(unsigned int x)
{
    unsigned int i,j;
    i=0;
    for(i=0;i<x;i++)
    {
       j=1;
       while(j--);
    }
}



//在中断外面处理接收函数,避免尽量避免spi等操作被中断.
void nrf24L01_IST()
{

}
