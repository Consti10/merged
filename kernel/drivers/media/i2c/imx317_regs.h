//
// Created by consti10 on 07.09.21.
//

#ifndef MERGED_IMX317_REGS_H
#define MERGED_IMX317_REGS_H

#define MDSEL1 0x3004
#define MDSEL2 0x3005
#define MDSEL3 0x3006
#define MDSEL4 0x3007
#define SVR_L 0x300E
#define SVR_H 0x300F

#define HMAX_L 0x30F6
#define HMAX_H 0x30F7
#define VMAX_L 0x30F8
#define VMAX_M 0x30F9
#define VMAX_H 0x30FA

// for imx317_1932x1094_regs_4lane
// HMAX: 0x1e,0x01      => 0x011e = 286
// VMAX: 0xD0,0x20,0x00 => 0x20D0 = 8400
// (72*10^6) / (286*8400*(0+1) )=29.97003

// for imx317_3864x2174_regs_4lane
// HMAX:  0x10,0x02      => 0x0210= 528
// VMAX: 0xc6, 0x11,0x00 => 0x11C6 = 4550
// (72*10^6) / (528*4550*(0+1) ) = 29.97003

// imx219 also has MEDIA_BUS_FMT_SRGGB10_1X10, but no iqfile.
// iqfiles for imxXXX : imx307, imx334, imx335, imx347,imx378, imx415
// imx307 also has        MEDIA_BUS_FMT_SRGGB10_1X10
// in contrast,imx415 has MEDIA_BUS_FMT_SGBRG10_1X10

#endif //MERGED_IMX317_REGS_H
