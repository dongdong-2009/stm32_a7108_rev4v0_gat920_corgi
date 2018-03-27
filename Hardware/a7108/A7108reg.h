/********************************************************************
*   A7108DREG.h
*   RF Chip-A7108 Hardware Definitions
*
*   This file provides the constants associated with the
*   AMICCOM A7108 device.
*
********************************************************************/
#ifndef _A7108REG_h_
#define _A7108REG_h_
#define SYSTEMCLOCK_REG		0x00
#define PLL1_REG 			0x01
#define PLL2_REG 			0x02
#define PLL3_REG 			0x03
#define PLL4_REG			0x04
#define CRYSTAL_REG			0x05
#define TX1_REG  			0x06
#define TX2_REG  			0x07
#define RX1_REG  			0x08
#define RX2_REG  			0x09
#define ADC_REG  			0x0A
#define FIFO_REG   			0x0B
#define CODE_REG 			0x0C
#define PIN_REG 			0x0D
#define CALIBRATION_REG  	0x0E
#define MODE_REG  			0x0F

#define TX1_PAGE			0x00
#define WOR1_PAGE			0x01
#define WOR2_PAGE			0x02
#define RFI_PAGE			0x03
#define PM_PAGE			0x04
#define RTH_PAGE			0x05
#define AGC1_PAGE			0x06
#define AGC2_PAGE			0x07
#define GIO_PAGE			0x08
#define CKO_PAGE			0x09
#define VCB_PAGE			0x0A
#define CHG1_PAGE			0x0B
#define CHG2_PAGE			0x0C

#define CMD_SLEEP			0x10
#define CMD_IDLE			0x12
#define CMD_STANDBY			0x14
#define CMD_PLL			0x16
#define CMD_RX				0x18
#define CMD_TX				0x1A
#define CMD_DEEP_SLEEP		0x1C

/*
#define CMD_FIFO_WR_PNT_RESET  0xE0
#define CMD_FIFO_RD_PNT_RESET  0xF0
*/
#endif
