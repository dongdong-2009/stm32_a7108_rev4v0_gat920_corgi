#ifndef _A7108_H_
#define _A7108_H_

#include "platform_config.h"

#define RX_MODE 1
#define TX_MODE 2
#define SLEEP_MODE 3

extern u8 ack_geted;
extern u8 Flag_MASTER;
extern u8 rf_buf[32];
extern u8 fifo_length, rf_mode;

extern u8 mvb_TX_Mode(u8 *pBuf, u8 bytes2send, u8 bytes2recv);
extern u8 mvb_TX_Mode_int(u8* pbuf, u8 length);
extern u8 mvb_Check_ACK(u8 length);
extern void mvb_RX_Mode(u8 length);
extern void mvb_RX_Mode_int(u8 length);
extern void A7108_WriteID(u8* ID_Tab);
extern void initRF_a7108(void);
extern void mvb_powerdown_mode(void);
extern void mvb_powerdown_mode_int(void);
extern void mvb_RxPacket(u8* pbuf, u8 length);
extern u16 A7108_ReadReg(u8 address);
extern void StrobeCmd(u8 cmd);
extern void a7108_setmaster(u8 master);
extern void RSSI_measurement_enable(void);

#endif
