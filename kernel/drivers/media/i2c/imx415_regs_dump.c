static __maybe_unused const struct regval imx415_global_10bit_3864x2192_regs[] = {
        {0x3002, 0x00},
        {IMX415_BCWAIT_TIME_L, 0xFF}, //74.25[Mhz]
        {IMX415_CPWAIT_TIME_L, 0xB6},
        {0x3031, 0x00},
        {0x3032, 0x00},
        {0x30C1, 0x00},
        {IMX415_INCKSEL2, 0x28},
        {IMX415_INCKSEL5, 0x28},
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
        {IMX415_TXCLKESC_FREQ_L, IMX415_FETCH_16BIT_L(0x1290)},
        {IMX415_TXCLKESC_FREQ_H, IMX415_FETCH_16BIT_H(0x1290)},
        {REG_NULL, 0x00},
};

// crap they don't have get ctrl in kernel code
/*static void consti10_debug_current_v4l2_values(struct imx415* imx415){
    s32 curr_h_blank=-1,curr_v_blank=-1,curr_pixel_rate=-1;
    dev_dbg(&imx415->client->dev, "Consti10: %s\n",__FUNCTION__);
    curr_h_blank=__v4l2_ctrl_s_ctrl_int64(imx415->vblank);
    curr_v_blank=__v4l2_ctrl_s_ctrl_int64(imx415->hblank);
    curr_pixel_rate=__v4l2_ctrl_s_ctrl_int64(imx415->pixel_rate);

    dev_dbg(&imx415->client->dev, "Consti10: curr_h_blank:%d,curr_v_blank:%d,curr_pixel_rate:%d,",(int)curr_h_blank,(int)curr_v_blank,(int)curr_pixel_rate);
}*/

//0x08CA=2250 | 0x226=550
//0x08BE=2238  | 0x16D = 365
// now works properly with modified iqfile (1080p!) but only up to 60 fps
static __maybe_unused const struct regval imx415_linear_10bit_binning2x2_1782_regs[] = {
        {IMX415_VMAX_L, 0xCA}, //maybe same
        {IMX415_VMAX_M, 0x08}, //maybe same
        {IMX415_HMAX_L,IMX415_FETCH_16BIT_L(0x226)},
        {IMX415_HMAX_H,IMX415_FETCH_16BIT_H(0x226)},
        {0x302C, 0x00}, //cannot find
        {0x302D, 0x00}, //cannot find
        {IMX415_SYS_MODE, 0x04},
        {IMX415_SHR0_L, 0x08},
        {IMX415_SHR0_M, 0x00},
        {0x3054, 0x19}, //cannot find in spec, but is IMX415_SF1_EXPO_REG_L in rockchip
        {0x3058, 0x3E}, //cannot find in spec, but is IMX415_SF2_EXPO_REG_L in rockchip
        {0x3060, 0x25}, //cannot find in spec, but is IMX415_RHS1_REG_L     in rockchip
        {0x3064, 0x4a}, //maybe same          ,but is IMX415_RHS2_REG_L     in rockchip
        {0x30CF, 0x00}, //cannot find
        {IMX415_INCKSEL3_L, 0xC0},
        {0x3260, 0x01}, //cannot find, but is mentioned in the rockchip comments (set to 0x01 in normal mode, something else in hdr)
        {IMX415_INCKSEL6, 0x01}, //changed

        {IMX415_TCLKPOST, 0xB7},   //here applies the 0x00xx workaround
        {IMX415_TCLKPREPARE, 0x67},//here applies the 0x00xx workaround
        {IMX415_TCLKTRAIL, 0x6F},  //here applies the 0x00xx workaround
        {IMX415_TCLKZERO_L, 0xDF}, //why the heck is this the only one of all where the higher bits need to be set to 0 argh
        {IMX415_TCLKZERO_H, 0x01}, // -- " --
        {IMX415_THSPREPARE, 0x6F}, //here applies the 0x00xx workaround
        {IMX415_THSZERO, 0xCF},    //here applies the 0x00xx workaround
        {IMX415_THSTRAIL, 0x6F},   //here applies the 0x00xx workaround
        {IMX415_THSEXIT, 0xB7},    //here applies the 0x00xx workaround
        {IMX415_TLPX, 0x5F},       //here applies the 0x00xx workaround
        {IMX415_INCKSEL7, 0x00}, //changed
        // added for testing Consti10:
        {0x301C,0x00}, //WINMODE //0: All-pixel mode, Horizontal/Vertical 2/2-line binning 4: Window cropping mode
        {0x3020,0x01}, //HADD //0h: All-pixel mode 1h: Horizontal 2 binning
        {0x3021,0x01}, //VADD //0h: All-pixel mode 1h: Vertical 2 binning
        {0x3022,0x01}, //ADDMODE //0h: All-pixel mode 1h: Horizontal/Vertical 2/2-line binning
        //
        // to resolve:
        {0x3031,0x00}, //ADBIT //set by global to 0 , 0=10bit 1=12bit
        {0x3032,0x00}, //MDBIT //set by global to 0
        //
        {0x30D9,0x02}, //DIG_CLP_VSTAET ? 0x02=binning 0x06=All-pixel scan mode , default 0x06
        {0x30DA,0x01}, //DIG_CLP_VNUM ? 0x01=binning 0x02=all-pixel scan mode, default 0x02
        // added for testing Consti10 end

        {REG_NULL, 0x00},
};

//0x08CA | 0x446
// cropping
static __maybe_unused const struct regval imx415_linear_10bit_cropping_1782_regs[] = {
        {IMX415_VMAX_L, 0xCA}, //maybe same
        {IMX415_VMAX_M, 0x08}, //maybe same
        {IMX415_HMAX_L,IMX415_FETCH_16BIT_L(0x226)},
        {IMX415_HMAX_H,IMX415_FETCH_16BIT_H(0x226)},
        {0x302C, 0x00}, //cannot find
        {0x302D, 0x00}, //cannot find
        {IMX415_SYS_MODE, 0x04},
        {IMX415_SHR0_L, 0x08},
        {IMX415_SHR0_M, 0x00},
        {0x3054, 0x19}, //cannot find in spec, but is IMX415_SF1_EXPO_REG_L in rockchip
        {0x3058, 0x3E}, //cannot find in spec, but is IMX415_SF2_EXPO_REG_L in rockchip
        {0x3060, 0x25}, //cannot find in spec, but is IMX415_RHS1_REG_L     in rockchip
        {0x3064, 0x4a}, //maybe same          ,but is IMX415_RHS2_REG_L     in rockchip
        {0x30CF, 0x00}, //cannot find
        {IMX415_INCKSEL3_L, 0xC0},
        {0x3260, 0x01}, //cannot find, but is mentioned in the rockchip comments (set to 0x01 in normal mode, something else in hdr)
        {IMX415_INCKSEL6, 0x01}, //changed

        {IMX415_TCLKPOST, 0xB7},   //here applies the 0x00xx workaround
        {IMX415_TCLKPREPARE, 0x67},//here applies the 0x00xx workaround
        {IMX415_TCLKTRAIL, 0x6F},  //here applies the 0x00xx workaround
        {IMX415_TCLKZERO_L, 0xDF}, //why the heck is this the only one of all where the higher bits need to be set to 0 argh
        {IMX415_TCLKZERO_H, 0x01}, // -- " --
        {IMX415_THSPREPARE, 0x6F}, //here applies the 0x00xx workaround
        {IMX415_THSZERO, 0xCF},    //here applies the 0x00xx workaround
        {IMX415_THSTRAIL, 0x6F},   //here applies the 0x00xx workaround
        {IMX415_THSEXIT, 0xB7},    //here applies the 0x00xx workaround
        {IMX415_TLPX, 0x5F},       //here applies the 0x00xx workaround
        {IMX415_INCKSEL7, 0x00}, //changed
        // added for testing Consti10:
        // 1280 x 720
        // H: 3840-1280=2560 | 2560/2= 1280
        // V: 2160-720 =1440 | 1440/2= 720
        {IMX415_WINMODE,0x04}, //WINMODE //0: All-pixel mode, Horizontal/Vertical 2/2-line binning 4: Window cropping mode
        {IMX415_PIX_HST_L,IMX415_FETCH_16BIT_L(1280)}, //PIX_HST Effective pixel Start position (Horizontal direction) | Default in spec: 0x000
        {IMX415_PIX_HST_H,IMX415_FETCH_16BIT_H(1280)}, //""
        {IMX415_PIX_HWIDTH_L,IMX415_FETCH_16BIT_L(1280)}, //PIX_HWIDTH Effective pixel Cropping width (Horizontal direction) | Default in spec: 0x0F18==3864
        {IMX415_PIX_HWIDTH_H,IMX415_FETCH_16BIT_H(1280)},  //""
        {IMX415_PIX_VST_L,IMX415_FETCH_16BIT_L(720*2)}, //PIX_VST Effective pixel Star position (Vertical direction) Designated in V units ( Line×2 ) | Default in spec: 0x000
        {IMX415_PIX_VST_H,IMX415_FETCH_16BIT_H(720*2)}, //""
        {IMX415_PIX_VWIDTH_L,IMX415_FETCH_16BIT_L(720*2)}, //PIX_VWIDTH Effective pixel Cropping width (Vertical direction) Designated in V units ( Line×2 ) | Default in spec: 0x1120==4384
        {IMX415_PIX_VWIDTH_H,IMX415_FETCH_16BIT_H(720*2)}, //""
        /*{IMX415_WINMODE,0x04}, //WINMODE //0: All-pixel mode, Horizontal/Vertical 2/2-line binning 4: Window cropping mode
        {IMX415_PIX_HST_L,IMX415_FETCH_16BIT_L(972)}, //PIX_HST Effective pixel Start position (Horizontal direction) | Default in spec: 0x000
        {IMX415_PIX_HST_H,IMX415_FETCH_16BIT_H(972)}, //""
        {IMX415_PIX_HWIDTH_L,IMX415_FETCH_16BIT_L(1920)}, //PIX_HWIDTH Effective pixel Cropping width (Horizontal direction) | Default in spec: 0x0F18==3864
        {IMX415_PIX_HWIDTH_H,IMX415_FETCH_16BIT_H(1920)},  //""
        {IMX415_PIX_VST_L,IMX415_FETCH_16BIT_L(556*2)}, //PIX_VST Effective pixel Star position (Vertical direction) Designated in V units ( Line×2 ) | Default in spec: 0x000
        {IMX415_PIX_VST_H,IMX415_FETCH_16BIT_H(556*2)}, //""
        {IMX415_PIX_VWIDTH_L,IMX415_FETCH_16BIT_L(1080*2)}, //PIX_VWIDTH Effective pixel Cropping width (Vertical direction) Designated in V units ( Line×2 ) | Default in spec: 0x1120==4384
        {IMX415_PIX_VWIDTH_H,IMX415_FETCH_16BIT_H(1080*2)}, //"" */
        // added for testing Consti10 end

        {REG_NULL, 0x00},
};

// maybe this just works - yeah
/*{
        .bus_fmt = MEDIA_BUS_FMT_SGBRG10_1X10,
        .width = 1920,
        .height = 1080,
        .max_fps = {
                .numerator = 10000,
                .denominator = 300000,
        },
        .exp_def = 0x08ca - 0x08, //2250-8=2248
        .hts_def = 0x044c * IMX415_4LANES * 2, //1100*4*2=8800
        .vts_def = 0x08ca,                     // 2250
        //.vts_def = 58 + 1080,
        .global_reg_list = imx415_global_10bit_3864x2192_regs,
        .reg_list = imx415_linear_10bit_3864x2192_891M_regs_binning,
        .hdr_mode = NO_HDR,
        .mipi_freq_idx = 0,
        .bpp = 10,
},*/
/*{
        .bus_fmt = MEDIA_BUS_FMT_SGBRG10_1X10,
        .width = 3864,
        .height = 2192,
        //.width = 1920,
        //.height = 1080,
        .max_fps = {
                .numerator = 10000,
                // per spec sheet, we should actually be able to do 38.5 fps
                .denominator = 300000,
                //.denominator = 385000, looks as if i do so, setting fps to 38 from application results in 30fps now ?!
        },
        .exp_def = 0x08ca - 0x08, //2250-8=2248
        .hts_def = 0x044c * IMX415_4LANES * 2, //1100*4*2=8800 | seems to be just HMAX from spec sheet
        .vts_def = 0x08ca ,                     // 2250        | seems to be VMAX from spec sheet
        .global_reg_list = imx415_global_10bit_3864x2192_regs,
        .reg_list = imx415_linear_10bit_3864x2192_891M_regs,
        .hdr_mode = NO_HDR,
        .mipi_freq_idx = 0,
        .bpp = 10,
},*/
/**Works{
            .bus_fmt = MEDIA_BUS_FMT_SGBRG10_1X10,
            .width = 3864,
            .height = 2192,
            .max_fps = {
                    .numerator = 10000,
                    .denominator = 600000,
            },
            .exp_def = 0x08ca - 0x08, //2250-8=2248
            //.hts_def = 0x044c * IMX415_4LANES * 2, //1100*4*2=8800 | seems to be just HMAX from spec sheet
            .hts_def = 0x226 * IMX415_4LANES * 2,
            .vts_def = 0x08ca ,                     // 2250        | seems to be VMAX from spec sheet
            .global_reg_list = imx415_global_10bit_3864x2192_regs,
            .reg_list = imx415_linear_10bit_3864x2192_1782_regs,
            .hdr_mode = NO_HDR,
            .mipi_freq_idx = 2,
            .bpp = 10,
    },*/

/**{
            .bus_fmt = MEDIA_BUS_FMT_SGBRG10_1X10,
            .width = 1920+(6+6+12),
            .height = 1080+(1+6+4+4+1+1),
            .max_fps = {
                    .numerator = 10000,
                    .denominator = 720000,
            },
            .exp_def = 0x08ca - 0x08, //2250-8=2248
            .hts_def = 0x226 * IMX415_4LANES * 2,
            .vts_def = 0x08ca ,                     // 2250        | seems to be VMAX from spec sheet
            .global_reg_list = imx415_global_10bit_3864x2192_regs,
            .reg_list = imx415_linear_10bit_binning2x2_1782_regs,
            .hdr_mode = NO_HDR,
            .mipi_freq_idx = 2,
            .bpp = 10,
    },*/
/*arghh{
        .bus_fmt = MEDIA_BUS_FMT_SGBRG10_1X10,
        .width = 1920,
        .height = 1080,
        .max_fps = {
                .numerator = 10000,
                .denominator = 900000,
        },
        .exp_def = 0x446 - 0x08, //2250-8=2248 | = (1080+46)=1126=0x446
        .hts_def = 0x226 * IMX415_4LANES * 2,
        .vts_def = 0x446 ,                     // 2250        | seems to be VMAX from spec sheet
        .global_reg_list = imx415_global_10bit_3864x2192_regs,
        .reg_list = imx415_linear_10bit_cropping_1782_regs,
        .hdr_mode = NO_HDR,
        .mipi_freq_idx = 2,
        .bpp = 10,
},*/
/*{
    .bus_fmt = MEDIA_BUS_FMT_SGBRG10_1X10,
    .width = 3864,
    .height = 2192,
    .max_fps = {
        .numerator = 10000,
        // per spec sheet, we should actually be able to do 38.5 fps
        .denominator = 300000,
        //.denominator = 385000, looks as if i do so, setting fps to 38 from application results in 30fps now ?!
    },
    .exp_def = 0x08ca - 0x08, //2250-8=2248
    .hts_def = 0x044c * IMX415_4LANES * 2, //1100*4*2=8800
    .vts_def = 0x08ca,                     // 2250
    .global_reg_list = imx415_global_10bit_3864x2192_regs,
    .reg_list = imx415_linear_10bit_3864x2192_891M_regs,
    .hdr_mode = NO_HDR,
    .mipi_freq_idx = 0,
    .bpp = 10,
},
{
    .bus_fmt = MEDIA_BUS_FMT_SGBRG10_1X10,
    .width = 3864,
    .height = 2192,
    .max_fps = {
        .numerator = 10000,
        .denominator = 300000,
    },
    .exp_def = 0x08fc * 2 - 0x0da8,
    .hts_def = 0x0226 * IMX415_4LANES * 2,
     //
     // IMX415 HDR mode T-line is half of Linear mode,
     // make vts double to workaround.
     //
    .vts_def = 0x08fc * 2,
    .global_reg_list = imx415_global_10bit_3864x2192_regs,
    .reg_list = imx415_hdr2_10bit_3864x2192_1485M_regs,
    .hdr_mode = HDR_X2,
    .mipi_freq_idx = 1,
    .bpp = 10,
    .vc[PAD0] = V4L2_MBUS_CSI2_CHANNEL_1,
    .vc[PAD1] = V4L2_MBUS_CSI2_CHANNEL_0,//L->csi wr0
    .vc[PAD2] = V4L2_MBUS_CSI2_CHANNEL_1,
    .vc[PAD3] = V4L2_MBUS_CSI2_CHANNEL_1,//M->csi wr2
},
{
    .bus_fmt = MEDIA_BUS_FMT_SGBRG10_1X10,
    .width = 3864,
    .height = 2192,
    .max_fps = {
        .numerator = 10000,
        .denominator = 200000,
    },
    .exp_def = 0x13e,
    .hts_def = 0x021A * IMX415_4LANES * 2,
    //
    //IMX415 HDR mode T-line is half of Linear mode,
    // make vts double to workaround.
    //
    .vts_def = 0x06BD * 4,
    .global_reg_list = imx415_global_10bit_3864x2192_regs,
    .reg_list = imx415_hdr3_10bit_3864x2192_1485M_regs,
    .hdr_mode = HDR_X3,
    .mipi_freq_idx = 1,
    .bpp = 10,
    .vc[PAD0] = V4L2_MBUS_CSI2_CHANNEL_2,
    .vc[PAD1] = V4L2_MBUS_CSI2_CHANNEL_1,//M->csi wr0
    .vc[PAD2] = V4L2_MBUS_CSI2_CHANNEL_0,//L->csi wr0
    .vc[PAD3] = V4L2_MBUS_CSI2_CHANNEL_2,//S->csi wr2
},
{
    .bus_fmt = MEDIA_BUS_FMT_SGBRG10_1X10,
    .width = 3864,
    .height = 2192,
    .max_fps = {
        .numerator = 10000,
        .denominator = 200000,
    },
    .exp_def = 0x13e,
    .hts_def = 0x01ca * IMX415_4LANES * 2,
    //
    // IMX415 HDR mode T-line is half of Linear mode,
    // make vts double to workaround.
    //
    .vts_def = 0x07ea * 4,
    .global_reg_list = imx415_global_10bit_3864x2192_regs,
    .reg_list = imx415_hdr3_10bit_3864x2192_1782M_regs,
    .hdr_mode = HDR_X3,
    .mipi_freq_idx = 2,
    .bpp = 10,
    .vc[PAD0] = V4L2_MBUS_CSI2_CHANNEL_2,
    .vc[PAD1] = V4L2_MBUS_CSI2_CHANNEL_1,//M->csi wr0
    .vc[PAD2] = V4L2_MBUS_CSI2_CHANNEL_0,//L->csi wr0
    .vc[PAD3] = V4L2_MBUS_CSI2_CHANNEL_2,//S->csi wr2
},
{
    // 1H period = (1100 clock) = (1100 * 1 / 74.25MHz)
    .bus_fmt = MEDIA_BUS_FMT_SGBRG12_1X12,
    .width = 3864,
    .height = 2192,
    .max_fps = {
        .numerator = 10000,
        .denominator = 300000,
    },
    .exp_def = 0x08ca - 0x08,
    .hts_def = 0x044c * IMX415_4LANES * 2,
    .vts_def = 0x08ca,
    .global_reg_list = imx415_global_12bit_3864x2192_regs,
    .reg_list = imx415_linear_12bit_3864x2192_891M_regs,
    .hdr_mode = NO_HDR,
    .mipi_freq_idx = 0,
    .bpp = 12,
},
{
    .bus_fmt = MEDIA_BUS_FMT_SGBRG12_1X12,
    .width = 3864,
    .height = 2192,
    .max_fps = {
        .numerator = 10000,
        .denominator = 300000,
    },
    .exp_def = 0x08CA * 2 - 0x0d90,
    .hts_def = 0x0226 * IMX415_4LANES * 2,
    //
    // IMX415 HDR mode T-line is half of Linear mode,
    // make vts double(that is FSC) to workaround.
    //
    .vts_def = 0x08CA * 2,
    .global_reg_list = imx415_global_12bit_3864x2192_regs,
    .reg_list = imx415_hdr2_12bit_3864x2192_1782M_regs,
    .hdr_mode = HDR_X2,
    .mipi_freq_idx = 2,
    .bpp = 12,
    .vc[PAD0] = V4L2_MBUS_CSI2_CHANNEL_1,
    .vc[PAD1] = V4L2_MBUS_CSI2_CHANNEL_0,//L->csi wr0
    .vc[PAD2] = V4L2_MBUS_CSI2_CHANNEL_1,
    .vc[PAD3] = V4L2_MBUS_CSI2_CHANNEL_1,//M->csi wr2
},
{
    .bus_fmt = MEDIA_BUS_FMT_SGBRG12_1X12,
    .width = 3864,
    .height = 2192,
    .max_fps = {
        .numerator = 10000,
        .denominator = 200000,
    },
    .exp_def = 0x114,
    .hts_def = 0x0226 * IMX415_4LANES * 2,
    //
    //IMX415 HDR mode T-line is half of Linear mode,
    // make vts double(that is FSC) to workaround.
    //
    .vts_def = 0x0696 * 4,
    .global_reg_list = imx415_global_12bit_3864x2192_regs,
    .reg_list = imx415_hdr3_12bit_3864x2192_1782M_regs,
    .hdr_mode = HDR_X3,
    .mipi_freq_idx = 2,
    .bpp = 12,
    .vc[PAD0] = V4L2_MBUS_CSI2_CHANNEL_2,
    .vc[PAD1] = V4L2_MBUS_CSI2_CHANNEL_1,//M->csi wr0
    .vc[PAD2] = V4L2_MBUS_CSI2_CHANNEL_0,//L->csi wr0
    .vc[PAD3] = V4L2_MBUS_CSI2_CHANNEL_2,//S->csi wr2
},*/