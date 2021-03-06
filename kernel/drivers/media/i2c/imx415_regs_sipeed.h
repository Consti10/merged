//
// Created by consti10 on 25.05.21.
//

#ifndef MEDIASEVER_IMX415_CONSTI_H
#define MEDIASEVER_IMX415_CONSTI_H


// copy from https://github.com/sipeed/r329-linux-4.9/blob/3eb6e2b325c080e77038acf5c5151cdbb806bb05/drivers/media/platform/sunxi-vin/modules/sensor/imx415_mipi.c

// merge findings: this is how they declare
//struct regval_list {
//    addr_type addr;
//    data_type data;
//};
// those two are defined here: https://github.com/sipeed/r329-linux-4.9/blob/3eb6e2b325c080e77038acf5c5151cdbb806bb05/drivers/media/platform/sunxi-vfe/csi_cci/cci_helper.h
//typedef unsigned short addr_type;
//typedef unsigned short data_type;
// unsigned short==16 bits.
// for comparison, rockchip uses
//struct regval {
//    u16 addr;
//    u8 val;
//};

// they use:
//ret = sensor_write_array(sd, sensor_default_regs,
//				 ARRAY_SIZE(sensor_default_regs));
// argh crap, for now: their sensor write array equivalent looks really different

static struct regval_list sensor_12bit_30fps_regs[] = {
        /*All pixel - 891Mbps - 27MHZ*/
        {0x3000, 0x01},/*stanby                  */
        {0x3001, 0x00},
        {0x3002, 0x01},/*XMSTA start             */
        {0x3003, 0x00},/*XMASTER SELECT MASTER   */
        {0x3008, 0x5D},
        {0x3009, 0x00},
        {0x300A, 0x42},
        {0x300B, 0xA0},
        {0x301C, 0x00},
        {0x301D, 0x08},
        {0x3020, 0x00},
        {0x3021, 0x00},
        {0x3022, 0x00},
        {0x3024, 0xCA},/*VMAX */
        {0x3025, 0x08},
        {0x3026, 0x00},
        {0x3028, 0x4C},/*HMAX*/
        {0x3029, 0x04},/*    */
        {0x302C, 0x00},/*DOL mode 0:normal 1:dol                                                   */
        {0x302D, 0x00},/*wdsel 0:normal exposure 1:dol_2  2:dol_3 3:multiple exposure 4frame       */
        {0x3030, 0x00},/*                                                                          */
        {0x3031, 0x01},/*adbit  0:10bit ,1 :12bit                                                  */
        {0x3032, 0x01},/*mdbit 0:10bit 1:12bit                                                     */
        {0x3033, 0x05},
        {0x3040, 0x00},/*window cropping                                                           */
        {0x3041, 0x00},
        {0x3042, 0x18},
        {0x3043, 0x0F},
        {0x3044, 0x18},
        {0x3045, 0x0F},
        {0x3046, 0x20},
        {0x3047, 0x11},
        {0x3050, 0x08},/*shr0        */
        {0x3051, 0x00},
        {0x3052, 0x00},
        {0x3054, 0x19},/*shr1        */
        {0x3055, 0x00},/*            */
        {0x3056, 0x00},/*            */
        {0x3058, 0x3E},/*shr2        */
        {0x3059, 0x00},/*            */
        {0x305E, 0x00},/*            */
        {0x3060, 0x25},/*rhs1        */
        {0x3061, 0x00},/*            */
        {0x3062, 0x00},/*            */
        {0x3064, 0x4A},/*rhs2        */
        {0x3065, 0x00},/*            */
        {0x3066, 0x00},/*            */
        {0x3090, 0x00},/*gain_pcg_0  */
        {0x3091, 0x00},/*            */
        {0x3092, 0x00},/*gain_pcg_1  */
        {0x3093, 0x00},/*            */
        {0x3094, 0x00},/*gain_pcg_2  */
        {0x3095, 0x00},
        {0x30C0, 0x2A},
        {0x30C1, 0x00},
        {0x30CC, 0x00},
        {0x30CD, 0x00},
        {0x30CF, 0x00},
        {0x30D9, 0x06},
        {0x30DA, 0x02},
        {0x30E2, 0x32},/*BLV */
        {0x30E3, 0x00},
        {0x3115, 0x00},
        {0x3116, 0x23},
        {0x3118, 0xC6},
        {0x3119, 0x00},
        {0x311A, 0xE7},
        {0x311B, 0x00},
        {0x311E, 0x23},
        {0x3260, 0x01},
        {0x32C8, 0x01},
        {0x32D4, 0x21},
        {0x32EC, 0xA1},
        {0x3452, 0x7F},
        {0x3453, 0x03},
        {0x358A, 0x04},
        {0x35A1, 0x02},
        {0x36BC, 0x0C},
        {0x36CC, 0x53},
        {0x36CD, 0x00},
        {0x36CE, 0x3C},
        {0x36D0, 0x8C},
        {0x36D1, 0x00},
        {0x36D2, 0x71},
        {0x36D4, 0x3C},
        {0x36D6, 0x53},
        {0x36D7, 0x00},
        {0x36D8, 0x71},
        {0x36DA, 0x8C},
        {0x36DB, 0x00},
        {0x3701, 0x03},/*ADBIT 00h:10bit  03h:12bit*/
        {0x3724, 0x02},
        {0x3726, 0x02},
        {0x3732, 0x02},
        {0x3734, 0x03},
        {0x3736, 0x03},
        {0x3742, 0x03},
        {0x3862, 0xE0},
        {0x38CC, 0x30},
        {0x38CD, 0x2F},
        {0x395C, 0x0C},
        {0x3A42, 0xD1},
        {0x3A4C, 0x77},
        {0x3AE0, 0x02},
        {0x3AEC, 0x0C},
        {0x3B00, 0x2E},
        {0x3B06, 0x29},
        {0x3B98, 0x25},
        {0x3B99, 0x21},
        {0x3B9B, 0x13},
        {0x3B9C, 0x13},
        {0x3B9D, 0x13},
        {0x3B9E, 0x13},
        {0x3BA1, 0x00},
        {0x3BA2, 0x06},
        {0x3BA3, 0x0B},
        {0x3BA4, 0x10},
        {0x3BA5, 0x14},
        {0x3BA6, 0x18},
        {0x3BA7, 0x1A},
        {0x3BA8, 0x1A},
        {0x3BA9, 0x1A},
        {0x3BAC, 0xED},
        {0x3BAD, 0x01},
        {0x3BAE, 0xF6},
        {0x3BAF, 0x02},
        {0x3BB0, 0xA2},
        {0x3BB1, 0x03},
        {0x3BB2, 0xE0},
        {0x3BB3, 0x03},
        {0x3BB4, 0xE0},
        {0x3BB5, 0x03},
        {0x3BB6, 0xE0},
        {0x3BB7, 0x03},
        {0x3BB8, 0xE0},
        {0x3BBA, 0xE0},
        {0x3BBC, 0xDA},
        {0x3BBE, 0x88},
        {0x3BC0, 0x44},
        {0x3BC2, 0x7B},
        {0x3BC4, 0xA2},
        {0x3BC8, 0xBD},
        {0x3BCA, 0xBD},
        {0x4000, 0x10},
        {0x4001, 0x03},
        {0x4004, 0xC0},
        {0x4005, 0x06},
        {0x400C, 0x00},
        {0x4018, 0x7F},
        {0x4019, 0x00},
        {0x401A, 0x37},
        {0x401B, 0x00},
        {0x401C, 0x37},
        {0x401D, 0x00},
        {0x401E, 0xF7},
        {0x401F, 0x00},
        {0x4020, 0x3F},
        {0x4021, 0x00},
        {0x4022, 0x6F},
        {0x4023, 0x00},
        {0x4024, 0x3F},
        {0x4025, 0x00},
        {0x4026, 0x5F},
        {0x4027, 0x00},
        {0x4028, 0x2F},
        {0x4029, 0x00},
        {0x4074, 0x01},
        {0x3000, 0x00},/*operation */
        {0x3002, 0x00},
};

static struct regval_list sensor_10bit_30fps_regs[] = {
/*ALL PIXEL  - 720Mbps - 24MHZ*/
        {0x3000, 0x01},/*stanby*/
        {0x3001, 0x00},
        {0x3008, 0x54},
        {0x300A, 0x3B},
        {0x3024, 0xCB},/*VMAX*/
        {0x3025, 0x08},
        {0x3026, 0x00},
        {0x3028, 0x2A},/*HMAX*/
        {0x3029, 0x04},
        {0x3031, 0x00},
        {0x3032, 0x00},
        {0x3033, 0x09},
        {0x3050, 0x08},
        {0x30C1, 0x00},
        {0x3116, 0x23},
        {0x3118, 0xB4},
        {0x311A, 0xFC},
        {0x311E, 0x23},
        {0x32D4, 0x21},
        {0x32EC, 0xA1},
        {0x3452, 0x7F},
        {0x3453, 0x03},
        {0x358A, 0x04},
        {0x35A1, 0x02},
        {0x36BC, 0x0C},
        {0x36CC, 0x53},
        {0x36CD, 0x00},
        {0x36CE, 0x3C},
        {0x36D0, 0x8C},
        {0x36D1, 0x00},
        {0x36D2, 0x71},
        {0x36D4, 0x3C},
        {0x36D6, 0x53},
        {0x36D7, 0x00},
        {0x36D8, 0x71},
        {0x36DA, 0x8C},
        {0x36DB, 0x00},
        {0x3701, 0x00},
        {0x3724, 0x02},
        {0x3726, 0x02},
        {0x3732, 0x02},
        {0x3734, 0x03},
        {0x3736, 0x03},
        {0x3742, 0x03},
        {0x3862, 0xE0},
        {0x38CC, 0x30},
        {0x38CD, 0x2F},
        {0x395C, 0x0C},
        {0x3A42, 0xD1},
        {0x3A4C, 0x77},
        {0x3AE0, 0x02},
        {0x3AEC, 0x0C},
        {0x3B00, 0x2E},
        {0x3B06, 0x29},
        {0x3B98, 0x25},
        {0x3B99, 0x21},
        {0x3B9B, 0x13},
        {0x3B9C, 0x13},
        {0x3B9D, 0x13},
        {0x3B9E, 0x13},
        {0x3BA1, 0x00},
        {0x3BA2, 0x06},
        {0x3BA3, 0x0B},
        {0x3BA4, 0x10},
        {0x3BA5, 0x14},
        {0x3BA6, 0x18},
        {0x3BA7, 0x1A},
        {0x3BA8, 0x1A},
        {0x3BA9, 0x1A},
        {0x3BAC, 0xED},
        {0x3BAD, 0x01},
        {0x3BAE, 0xF6},
        {0x3BAF, 0x02},
        {0x3BB0, 0xA2},
        {0x3BB1, 0x03},
        {0x3BB2, 0xE0},
        {0x3BB3, 0x03},
        {0x3BB4, 0xE0},
        {0x3BB5, 0x03},
        {0x3BB6, 0xE0},
        {0x3BB7, 0x03},
        {0x3BB8, 0xE0},
        {0x3BBA, 0xE0},
        {0x3BBC, 0xDA},
        {0x3BBE, 0x88},
        {0x3BC0, 0x44},
        {0x3BC2, 0x7B},
        {0x3BC4, 0xA2},
        {0x3BC8, 0xBD},
        {0x3BCA, 0xBD},
        {0x4004, 0x00},
        {0x4005, 0x06},
        {0x400C, 0x00},
        {0x4018, 0x6F},
        {0x401A, 0x2F},
        {0x401C, 0x2F},
        {0x401E, 0xBF},
        {0x401F, 0x00},
        {0x4020, 0x2F},
        {0x4022, 0x57},
        {0x4024, 0x2F},
        {0x4026, 0x4F},
        {0x4028, 0x27},
        {0x4074, 0x01},
        {0x3000, 0x00}, /*operation*/
        {0x3002, 0x00},
};
static struct regval_list sensor_10bit_60fps_regs[] = {

/*All Pixel - 1440Mbps - 24MHZ*/
        {0x3000, 0x01},/*
	{0x3001, 0x00},/*
	{0x3008, 0x54},/* BCWAIT_TIME[9:0]*/
        {0x300A, 0x3B},/* CPWAIT_TIME[9:0]*/
        {0x3024, 0xCB},/* VMAX[19:0]      */
        {0x3025, 0x08},
        {0x3028, 0x15},/* HMAX[15:0]      */
        {0x3029, 0x02},/*                 */
        {0x3031, 0x00},/* ADBIT[1:0]      */
        {0x3032, 0x00},/* MDBIT           */
        {0x3033, 0x08},/* SYS_MODE[3:0]   */
        {0x3050, 0x08},/* SHR0[19:0]      */
        {0x30C1, 0x00},/* XVS_DRV[1:0]    */
        {0x3116, 0x23},/* INCKSEL2[7:0]   */
        {0x3118, 0xB4},/* INCKSEL3[10:0]  */
        {0x311A, 0xFC},/* INCKSEL4[10:0]  */
        {0x311E, 0x23},/* INCKSEL5[7:0]   */
        {0x32D4, 0x21},
        {0x32EC, 0xA1},
        {0x3452, 0x7F},
        {0x3453, 0x03},
        {0x358A, 0x04},
        {0x35A1, 0x02},
        {0x36BC, 0x0C},
        {0x36CC, 0x53},
        {0x36CD, 0x00},
        {0x36CE, 0x3C},
        {0x36D0, 0x8C},
        {0x36D1, 0x00},
        {0x36D2, 0x71},
        {0x36D4, 0x3C},
        {0x36D6, 0x53},
        {0x36D7, 0x00},
        {0x36D8, 0x71},
        {0x36DA, 0x8C},
        {0x36DB, 0x00},
        {0x3701, 0x00},/* ADBIT1[7:0]*/
        {0x3724, 0x02},
        {0x3726, 0x02},
        {0x3732, 0x02},
        {0x3734, 0x03},
        {0x3736, 0x03},
        {0x3742, 0x03},
        {0x3862, 0xE0},
        {0x38CC, 0x30},
        {0x38CD, 0x2F},
        {0x395C, 0x0C},
        {0x3A42, 0xD1},
        {0x3A4C, 0x77},
        {0x3AE0, 0x02},
        {0x3AEC, 0x0C},
        {0x3B00, 0x2E},
        {0x3B06, 0x29},
        {0x3B98, 0x25},
        {0x3B99, 0x21},
        {0x3B9B, 0x13},
        {0x3B9C, 0x13},
        {0x3B9D, 0x13},
        {0x3B9E, 0x13},
        {0x3BA1, 0x00},
        {0x3BA2, 0x06},
        {0x3BA3, 0x0B},
        {0x3BA4, 0x10},
        {0x3BA5, 0x14},
        {0x3BA6, 0x18},
        {0x3BA7, 0x1A},
        {0x3BA8, 0x1A},
        {0x3BA9, 0x1A},
        {0x3BAC, 0xED},
        {0x3BAD, 0x01},
        {0x3BAE, 0xF6},
        {0x3BAF, 0x02},
        {0x3BB0, 0xA2},
        {0x3BB1, 0x03},
        {0x3BB2, 0xE0},
        {0x3BB3, 0x03},
        {0x3BB4, 0xE0},
        {0x3BB5, 0x03},
        {0x3BB6, 0xE0},
        {0x3BB7, 0x03},
        {0x3BB8, 0xE0},
        {0x3BBA, 0xE0},
        {0x3BBC, 0xDA},
        {0x3BBE, 0x88},
        {0x3BC0, 0x44},
        {0x3BC2, 0x7B},
        {0x3BC4, 0xA2},
        {0x3BC8, 0xBD},
        {0x3BCA, 0xBD},
        {0x4004, 0x00},/* TXCLKESC_FREQ[15:0]  */
        {0x4005, 0x06},/*                      */
        {0x4018, 0x9F},/* TCLKPOST[15:0]       */
        {0x401A, 0x57},/* TCLKPREPARE[15:0]    */
        {0x401C, 0x57},/* TCLKTRAIL[15:0]      */
        {0x401E, 0x87},/* TCLKZERO[15:0]       */
        {0x4020, 0x5F},/* THSPREPARE[15:0]     */
        {0x4022, 0xA7},/* THSZERO[15:0]        */
        {0x4024, 0x5F},/* THSTRAIL[15:0]       */
        {0x4026, 0x97},/* THSEXIT[15:0]        */
        {0x4028, 0x4F},/* TLPX[15:0]           */
        {0x3000, 0x00},/*operation             */
        {0x3002, 0x00},
};
static struct regval_list sensor_12bit_60fps_regs[] = {
        /*All Pixel - 1782Mbps - 27MHz*/
        {0x3000, 0x01},
        {0x3001, 0x00},
        {0x3008, 0x5D}, /* BCWAIT_TIME[9:0]*/
        {0x300A, 0x42}, /* CPWAIT_TIME[9:0]*/
        {0x3050, 0x08}, /* SHR0[19:0]*/
        {0x3024, 0xCA}, /*/ VMAX[19:0]*/
        {0x3025, 0x08},
        {0x3026, 0x00},
        {0x3028, 0x26}, /*/ HMAX[15:0]*/
        {0x3029, 0x02},
        {0x30C1, 0x00}, /* XVS_DRV[1:0]  */
        {0x3116, 0x23}, /* INCKSEL2[7:0] */
        {0x3118, 0xC6}, /* INCKSEL3[10:0]*/
        {0x311A, 0xE7}, /* INCKSEL4[10:0]*/
        {0x311E, 0x23}, /* INCKSEL5[7:0] */
        {0x32D4, 0x21},
        {0x32EC, 0xA1},
        {0x3452, 0x7F},
        {0x3453, 0x03},
        {0x358A, 0x04},
        {0x35A1, 0x02},
        {0x36BC, 0x0C},
        {0x36CC, 0x53},
        {0x36CD, 0x00},
        {0x36CE, 0x3C},
        {0x36D0, 0x8C},
        {0x36D1, 0x00},
        {0x36D2, 0x71},
        {0x36D4, 0x3C},
        {0x36D6, 0x53},
        {0x36D7, 0x00},
        {0x36D8, 0x71},
        {0x36DA, 0x8C},
        {0x36DB, 0x00},
        {0x3724, 0x02},
        {0x3726, 0x02},
        {0x3732, 0x02},
        {0x3734, 0x03},
        {0x3736, 0x03},
        {0x3742, 0x03},
        {0x3862, 0xE0},
        {0x38CC, 0x30},
        {0x38CD, 0x2F},
        {0x395C, 0x0C},
        {0x3A42, 0xD1},
        {0x3A4C, 0x77},
        {0x3AE0, 0x02},
        {0x3AEC, 0x0C},
        {0x3B00, 0x2E},
        {0x3B06, 0x29},
        {0x3B98, 0x25},
        {0x3B99, 0x21},
        {0x3B9B, 0x13},
        {0x3B9C, 0x13},
        {0x3B9D, 0x13},
        {0x3B9E, 0x13},
        {0x3BA1, 0x00},
        {0x3BA2, 0x06},
        {0x3BA3, 0x0B},
        {0x3BA4, 0x10},
        {0x3BA5, 0x14},
        {0x3BA6, 0x18},
        {0x3BA7, 0x1A},
        {0x3BA8, 0x1A},
        {0x3BA9, 0x1A},
        {0x3BAC, 0xED},
        {0x3BAD, 0x01},
        {0x3BAE, 0xF6},
        {0x3BAF, 0x02},
        {0x3BB0, 0xA2},
        {0x3BB1, 0x03},
        {0x3BB2, 0xE0},
        {0x3BB3, 0x03},
        {0x3BB4, 0xE0},
        {0x3BB5, 0x03},
        {0x3BB6, 0xE0},
        {0x3BB7, 0x03},
        {0x3BB8, 0xE0},
        {0x3BBA, 0xE0},
        {0x3BBC, 0xDA},
        {0x3BBE, 0x88},
        {0x3BC0, 0x44},
        {0x3BC2, 0x7B},
        {0x3BC4, 0xA2},
        {0x3BC8, 0xBD},
        {0x3BCA, 0xBD},
        {0x4004, 0xC0}, /* TXCLKESC_FREQ[15:0]*/
        {0x4005, 0x06},
        {0x3000, 0x00}, /*operation*/
        {0x3002, 0x00},
};
static struct regval_list sensor_10bit_90fps_regs[] = {
        /*All Pixel - 23/*Mbps - 27MHz*/
        {0x3000, 0x00},
        {0x3001, 0x00},
        {0x3008, 0x5D}, /* BCWAIT_TIME[9:0]*/
        {0x300A, 0x42}, /* CPWAIT_TIME[9:0]*/
        {0x3024, 0xCE}, /*VMAX             */
        {0x3025, 0x08},
        {0x3026, 0x00},
        {0x3028, 0x6E}, /* HMAX            */
        {0x3029, 0x01},
        {0x3031, 0x00}, /* ADBIT[1:0]      */
        {0x3032, 0x00}, /* MDBIT           */
        {0x3033, 0x00}, /* SYS_MODE[3:0]   */
        {0x3050, 0x08}, /* SHR0[19:0]      */
        {0x30C1, 0x00}, /* XVS_DRV[1:0]    */
        {0x3116, 0x23}, /* INCKSEL2[7:0]   */
        {0x3118, 0x08}, /* INCKSEL3[10:0]  */
        {0x3119, 0x01},
        {0x311A, 0xE7}, /* INCKSEL4[10:0]  */
        {0x311E, 0x23}, /* INCKSEL5[7:0]   */
        {0x32D4, 0x21},
        {0x32EC, 0xA1},
        {0x3452, 0x7F},
        {0x3453, 0x03},
        {0x358A, 0x04},
        {0x35A1, 0x02},
        {0x36BC, 0x0C},
        {0x36CC, 0x53},
        {0x36CD, 0x00},
        {0x36CE, 0x3C},
        {0x36D0, 0x8C},
        {0x36D1, 0x00},
        {0x36D2, 0x71},
        {0x36D4, 0x3C},
        {0x36D6, 0x53},
        {0x36D7, 0x00},
        {0x36D8, 0x71},
        {0x36DA, 0x8C},
        {0x36DB, 0x00},
        {0x3701, 0x00}, /* ADBIT1[7:0]*/
        {0x3724, 0x02},
        {0x3726, 0x02},
        {0x3732, 0x02},
        {0x3734, 0x03},
        {0x3736, 0x03},
        {0x3742, 0x03},
        {0x3862, 0xE0},
        {0x38CC, 0x30},
        {0x38CD, 0x2F},
        {0x395C, 0x0C},
        {0x3A42, 0xD1},
        {0x3A4C, 0x77},
        {0x3AE0, 0x02},
        {0x3AEC, 0x0C},
        {0x3B00, 0x2E},
        {0x3B06, 0x29},
        {0x3B98, 0x25},
        {0x3B99, 0x21},
        {0x3B9B, 0x13},
        {0x3B9C, 0x13},
        {0x3B9D, 0x13},
        {0x3B9E, 0x13},
        {0x3BA1, 0x00},
        {0x3BA2, 0x06},
        {0x3BA3, 0x0B},
        {0x3BA4, 0x10},
        {0x3BA5, 0x14},
        {0x3BA6, 0x18},
        {0x3BA7, 0x1A},
        {0x3BA8, 0x1A},
        {0x3BA9, 0x1A},
        {0x3BAC, 0xED},
        {0x3BAD, 0x01},
        {0x3BAE, 0xF6},
        {0x3BAF, 0x02},
        {0x3BB0, 0xA2},
        {0x3BB1, 0x03},
        {0x3BB2, 0xE0},
        {0x3BB3, 0x03},
        {0x3BB4, 0xE0},
        {0x3BB5, 0x03},
        {0x3BB6, 0xE0},
        {0x3BB7, 0x03},
        {0x3BB8, 0xE0},
        {0x3BBA, 0xE0},
        {0x3BBC, 0xDA},
        {0x3BBE, 0x88},
        {0x3BC0, 0x44},
        {0x3BC2, 0x7B},
        {0x3BC4, 0xA2},
        {0x3BC8, 0xBD},
        {0x3BCA, 0xBD},
        {0x4004, 0xC0}, /* TXCLKESC_FREQ[15:0]*/
        {0x4005, 0x06},
        {0x4018, 0xE7}, /* TCLKPOST[15:0]     */
        {0x401A, 0x8F}, /* TCLKPREPARE[15:0]  */
        {0x401C, 0x8F}, /* TCLKTRAIL[15:0]    */
        {0x401E, 0x7F}, /* TCLKZERO[15:0]     */
        {0x401F, 0x02},
        {0x4020, 0x97}, /* THSPREPARE[15:0]   */
        {0x4022, 0x0F}, /* THSZERO[15:0]      */
        {0x4023, 0x01},
        {0x4024, 0x97}, /* THSTRAIL[15:0]     */
        {0x4026, 0xF7}, /* THSEXIT[15:0]      */
        {0x4028, 0x7F}, /* TLPX[15:0]         */
        {0x3000, 0x00}, /*operation           */
        {0x3002, 0x00},
};

#endif //MEDIASEVER_IMX415_CONSTI_H
