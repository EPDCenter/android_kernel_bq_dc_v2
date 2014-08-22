
#include "generic_sensor.h"

/*
*      Driver Version Note
*v0.0.1: this driver is compatible with generic_sensor
*v0.1.1:
*        add sensor_focus_af_const_pause_usr_cb;
*/
static int version = KERNEL_VERSION(0,1,1);
module_param(version, int, S_IRUGO);



static int debug;
module_param(debug, int, S_IRUGO|S_IWUSR);


#define dprintk(level, fmt, arg...) do {			\
	if (debug > level) 					\
	printk(KERN_WARNING fmt , ## arg); } while (0)

/* Sensor Driver Configuration Begin */
#define SENSOR_NAME RK29_CAM_SENSOR_HM2057
#define SENSOR_V4L2_IDENT V4L2_IDENT_HM2057
#define SENSOR_ID 0x2056
#define SENSOR_BUS_PARAM                     (SOCAM_MASTER |\
                                             SOCAM_PCLK_SAMPLE_RISING|SOCAM_HSYNC_ACTIVE_HIGH| SOCAM_VSYNC_ACTIVE_LOW|\
                                             SOCAM_DATA_ACTIVE_HIGH | SOCAM_DATAWIDTH_8  |SOCAM_MCLK_24MHZ)
#define SENSOR_PREVIEW_W                     800
#define SENSOR_PREVIEW_H                     600
#define SENSOR_PREVIEW_FPS                   15000     // 15fps 
#define SENSOR_FULLRES_L_FPS                 7500      // 7.5fps
#define SENSOR_FULLRES_H_FPS                 7500      // 7.5fps
#define SENSOR_720P_FPS                      0
#define SENSOR_1080P_FPS                     0

#define SENSOR_REGISTER_LEN                  2         // sensor register address bytes
#define SENSOR_VALUE_LEN                     1         // sensor register value bytes

static unsigned int SensorConfiguration = (CFG_WhiteBalance|CFG_Effect);
static unsigned int SensorChipID[] = {SENSOR_ID,0x2057};

/* Sensor Driver Configuration End */


#define SENSOR_NAME_STRING(a) STR(CONS(SENSOR_NAME, a))
#define SENSOR_NAME_VARFUN(a) CONS(SENSOR_NAME, a)

#define SensorRegVal(a,b) CONS4(SensorReg,SENSOR_REGISTER_LEN,Val,SENSOR_VALUE_LEN)(a,b)
#define sensor_write(client,reg,v) CONS4(sensor_write_reg,SENSOR_REGISTER_LEN,val,SENSOR_VALUE_LEN)(client,(reg),(v))
#define sensor_read(client,reg,v) CONS4(sensor_read_reg,SENSOR_REGISTER_LEN,val,SENSOR_VALUE_LEN)(client,(reg),(v))
#define sensor_write_array generic_sensor_write_array

#define SENSOR_PLL_ENABLE 1
//ͬ����֡�� bypass PLL���ع�ʱ����Enalble PLL��2��
//Enable PLL: 0x0025=0x00 
//bypass PLL: 0x0025=0x80


#define SENSOR_FAST_MODE 1
//����ģʽ��֡��Խ�죬����Խ�������Ȼ�Խ��
//�ٶȺ����������߲��ܼ��

struct specific_sensor{
	struct generic_sensor common_sensor;
	//define user data below
	

};

/*
* Local define
*/


/*
*  The follow setting need been filled.
*  
*  Must Filled:
*  sensor_init_data :               Sensor initial setting;
*  sensor_fullres_lowfps_data :     Sensor full resolution setting with best auality, recommand for video;
*  sensor_preview_data :            Sensor preview resolution setting, recommand it is vga or svga;
*  sensor_softreset_data :          Sensor software reset register;
*  sensor_check_id_data :           Sensir chip id register;
*
*  Optional filled:
*  sensor_fullres_highfps_data:     Sensor full resolution setting with high framerate, recommand for video;
*  sensor_720p:                     Sensor 720p setting, it is for video;
*  sensor_1080p:                    Sensor 1080p setting, it is for video;
*
*  :::::WARNING:::::
*  The SensorEnd which is the setting end flag must be filled int the last of each setting;
*/

/* Sensor initial setting */
static struct rk_sensor_reg sensor_init_data[] = {
{0x0022,0x00},
{0x0004,0x10},
{0x0006,0x00},
{0x000D,0x11},
{0x000E,0x11},
{0x000F,0x10},//00
{0x0011,0x02},
{0x0012,0x1C},
{0x0013,0x01},
{0x0015,0x02},
{0x0016,0x80},
{0x0018,0x00},
{0x001D,0x40},
{0x0020,0x40},

#if SENSOR_PLL_ENABLE
    {0x0025,0x00},
#else
    {0x0025,0x80},
#endif

{0x0026,0x87},
{0x0027,0x10},//0x30},
{0x0040,0x20},
{0x0053,0x0A},
{0x0044,0x06},
{0x0046,0xD8},
{0x004A,0x0A},
{0x004B,0x72},
{0x0075,0x01},
{0x002A,0x1F},
{0x0070,0x5F},
{0x0071,0xFF},
{0x0072,0x55},
{0x0073,0x50},
{0x0080,0xC8},
{0x0082,0xA2},
{0x0083,0xF0},
{0x0085,0x11},//0x12 <lpz 2012.10.24>HMAX ԭ������ ADC POWER ��ǰ��75%������Ҫ�趨��100%
{0x0086,0x02},
{0x0087,0x80},
{0x0088,0x6C},
{0x0089,0x2E},
{0x008A,0x7D},
{0x008D,0x20},
{0x0090,0x00},
{0x0091,0x10},
{0x0092,0x11},
{0x0093,0x12},
{0x0094,0x16},
{0x0095,0x08},
{0x0096,0x00},
{0x0097,0x10},
{0x0098,0x11},
{0x0099,0x12},
{0x009A,0x06},
{0x009B,0x34},
{0x00A0,0x00},
{0x00A1,0x04},
{0x011F,0xF7},
{0x0120,0x36},
{0x0121,0x83},
{0x0122,0x7B},
{0x0123,0xC2},
{0x0124,0xDE},
{0x0125,0xDF},
{0x0126,0x70},
{0x0128,0x1F},
{0x0132,0x10},
{0x0131,0xBD},
{0x0140,0x14},
{0x0141,0x0A},
{0x0142,0x14},
{0x0143,0x0A},
{0x0144,0x04},
{0x0145,0x00},
{0x0146,0x20},
{0x0147,0x0A},
{0x0148,0x10},
{0x0149,0x0C},
{0x014A,0x80},
{0x014B,0x80},
{0x014C,0x2E},
{0x014D,0x2E},
{0x014E,0x05},
{0x014F,0x05},
{0x0150,0x0D},
{0x0155,0x00},
{0x0156,0x10},
{0x0157,0x0A},
{0x0158,0x0A},
{0x0159,0x0A},
{0x015A,0x05},
{0x015B,0x05},
{0x015C,0x05},
{0x015D,0x05},
{0x015E,0x08},
{0x015F,0xFF},
{0x0160,0x50},
{0x0161,0x20},
{0x0162,0x14},
{0x0163,0x0A},
{0x0164,0x10},
{0x0165,0x0A},
{0x0166,0x0A},
{0x018C,0x24},
{0x018D,0x04},
{0x018E,0x00},
{0x018F,0x11},
{0x0190,0x80},
{0x0191,0x47},
{0x0192,0x48},
{0x0193,0x64},
{0x0194,0x32},
{0x0195,0xc8},
{0x0196,0x96},
{0x0197,0x64},
{0x0198,0x32},
{0x0199,0x14},
{0x019A,0x20},
{0x019B,0x14},
{0x01B0,0x55},
{0x01B1,0x0C},
{0x01B2,0x0A},
{0x01B3,0x10},
{0x01B4,0x0E},
{0x01BA,0x10},
{0x01BB,0x04},
{0x01D8,0x40},
{0x01DE,0x60},
{0x01E4,0x10},
{0x01E5,0x10},
{0x01F2,0x0C},
{0x01F3,0x14},
{0x01F8,0x04},
{0x01F9,0x0C},
{0x01FE,0x02},
{0x01FF,0x04},
{0x0220,0x00},
{0x0221,0xB0},
{0x0222,0x00},
{0x0223,0x80},
{0x0224,0x8E},
{0x0225,0x00},
{0x0226,0x88},
{0x022A,0x88},
{0x022B,0x00},
{0x022C,0x8C},
{0x022D,0x13},
{0x022E,0x0B},
{0x022F,0x13},
{0x0230,0x0B},
{0x0233,0x13},
{0x0234,0x0B},
{0x0235,0x28},
{0x0236,0x03},
{0x0237,0x28},
{0x0238,0x03},
{0x023B,0x28},
{0x023C,0x03},
{0x023D,0x5C},
{0x023E,0x02},
{0x023F,0x5C},
{0x0240,0x02},
{0x0243,0x5C},
{0x0244,0x02},
{0x0251,0x0E},
{0x0252,0x00},
{0x0280,0x0A},
{0x0282,0x14},
{0x0284,0x2A},
{0x0286,0x50},
{0x0288,0x60},
{0x028A,0x6D},
{0x028C,0x79},
{0x028E,0x82},
{0x0290,0x8A},
{0x0292,0x91},
{0x0294,0x9C},
{0x0296,0xA7},
{0x0298,0xBA},
{0x029A,0xCD},
{0x029C,0xE0},
{0x029E,0x2D},
{0x02A0,0x06},
{0x02E0,0x04},
{0x02C0,0x8F},
{0x02C1,0x01},
{0x02C2,0x8F},
{0x02C3,0x07},
{0x02C4,0xE3},
{0x02C5,0x07},
{0x02C6,0xC1},
{0x02C7,0x07},
{0x02C8,0x70},
{0x02C9,0x01},
{0x02CA,0xD0},
{0x02CB,0x07},
{0x02CC,0xF7},
{0x02CD,0x07},
{0x02CE,0x5A},
{0x02CF,0x07},
{0x02D0,0xB0},
{0x02D1,0x01},
{0x0302,0x00},
{0x0303,0x00},
{0x0304,0x00},
{0x02F0,0x80},
{0x02F1,0x07},
{0x02F2,0x8E},
{0x02F3,0x00},
{0x02F4,0xF2},
{0x02F5,0x07},
{0x02F6,0xCC},
{0x02F7,0x07},
{0x02F8,0x16},
{0x02F9,0x00},
{0x02FA,0x1E},
{0x02FB,0x00},
{0x02FC,0x9D},
{0x02FD,0x07},
{0x02FE,0xA6},
{0x02FF,0x07},
{0x0300,0xBD},
{0x0301,0x00},
{0x0305,0x00},
{0x0306,0x00},
{0x0307,0x00},
{0x032D,0x00},
{0x032E,0x01},
{0x032F,0x00},
{0x0330,0x01},
{0x0331,0x00},
{0x0332,0x01},
{0x0333,0x82},
{0x0334,0x00},
{0x0335,0x84},
{0x0336,0x00},
{0x0337,0x01},
{0x0338,0x00},
{0x0339,0x01},
{0x033A,0x00},
{0x033B,0x01},
{0x033E,0x04},
{0x033F,0x86},
{0x0340,0x30},
{0x0341,0x44},
{0x0342,0x4A},
{0x0343,0x42},
{0x0344,0x74},
{0x0345,0x4F},
{0x0346,0x67},
{0x0347,0x5C},
{0x0348,0x59},
{0x0349,0x67},
{0x034A,0x4D},
{0x034B,0x6E},
{0x034C,0x44},
{0x0350,0x80},
{0x0351,0x80},
{0x0352,0x18},
{0x0353,0x18},
{0x0354,0x6E},
{0x0355,0x4A},
{0x0356,0x73},
{0x0357,0xC0},
{0x0358,0x06},
{0x035A,0x06},
{0x035B,0xA0},
{0x035C,0x73},
{0x035D,0x50},
{0x035E,0xC0},
{0x035F,0xA0},
{0x0360,0x02},
{0x0361,0x18},
{0x0362,0x80},
{0x0363,0x6C},
{0x0364,0x00},
{0x0365,0xF0},
{0x0366,0x20},
{0x0367,0x0C},
{0x0369,0x00},
{0x036A,0x10},
{0x036B,0x10},
{0x036E,0x20},
{0x036F,0x00},
{0x0370,0x10},
{0x0371,0x18},
{0x0372,0x0C},
{0x0373,0x38},
{0x0374,0x3A},
{0x0375,0x13},
{0x0376,0x22},
{0x0380,0xFF},
{0x0381,0x4c},
{0x0382,0x3c},
{0x038A,0x40},
{0x038B,0x08},
{0x038C,0xC1},
{0x038E,0x44},

#if SENSOR_PLL_ENABLE
    //10 fps
    {0x038F,0x07},
    {0x0390,0x5c},
#else
    #if SENSOR_FAST_MODE
       //10 fps
       {0x038F,0x03},
       {0x0390,0xae},
    #else 
        //7.5 fps
       {0x038F,0x04}, //09
       {0x0390,0xE8}, //18
   #endif
#endif

{0x0391,0x05},
{0x0393,0x80},
{0x0395,0x21},
{0x0398,0x02},
{0x0399,0x84},
{0x039A,0x03},
{0x039B,0x25},
{0x039C,0x03},
{0x039D,0xC6},
{0x039E,0x05},
{0x039F,0x08},
{0x03A0,0x06},
{0x03A1,0x4A},
{0x03A2,0x07},
{0x03A3,0x8C},
{0x03A4,0x0A},
{0x03A5,0x10},
{0x03A6,0x0C},
{0x03A7,0x0E},
{0x03A8,0x10},
{0x03A9,0x18},
{0x03AA,0x20},
{0x03AB,0x28},
{0x03AC,0x1E},
{0x03AD,0x1A},
{0x03AE,0x13},
{0x03AF,0x0C},
{0x03B0,0x0B},
{0x03B1,0x09},
{0x03B3,0x10},
{0x03B4,0x00},
{0x03B5,0x10},
{0x03B6,0x00},
{0x03B7,0xEA},
{0x03B8,0x00},
{0x03B9,0x3A},
{0x03BA,0x01},
{0x03BB,0x9F},
{0x03BC,0xCF},
{0x03BD,0xE7},
{0x03BE,0xF3},
{0x03BF,0x01},
{0x03D0,0xF8},
{0x03E0,0x04},
{0x03E1,0x01},
{0x03E2,0x04},
{0x03E4,0x10},
{0x03E5,0x12},
{0x03E6,0x00},
{0x03E8,0x21},
{0x03E9,0x23},
{0x03EA,0x01},
{0x03EC,0x21},
{0x03ED,0x23},
{0x03EE,0x01},
{0x03F0,0x20},
{0x03F1,0x22},
{0x03F2,0x00},
{0x0420,0x84},
{0x0421,0x00},
{0x0422,0x00},
{0x0423,0x83},
{0x0430,0x08},
{0x0431,0x28},
{0x0432,0x10},
{0x0433,0x08},
{0x0435,0x0C},
{0x0450,0xFF},
{0x0451,0xE8},
{0x0452,0xC4},
{0x0453,0x88},
{0x0454,0x00},
{0x0458,0x70},
{0x0459,0x03},
{0x045A,0x00},
{0x045B,0x30},
{0x045C,0x00},
{0x045D,0x70},
{0x0466,0x14},
{0x047A,0x00},
{0x047B,0x00},
{0x0480,0x58},
{0x0481,0x06},
{0x0482,0x0C},
{0x04B0,0x50},
{0x04B6,0x30},
{0x04B9,0x10},
{0x04B3,0x10},
{0x04B1,0x8E},
{0x04B4,0x20},
{0x0540,0x00},
{0x0541,0x60},//9D
{0x0542,0x00},
{0x0543,0x73},//BC
{0x0580,0x01},
{0x0581,0x0F},
{0x0582,0x04},
{0x0594,0x00},
{0x0595,0x04},
{0x05A9,0x03},
{0x05AA,0x40},
{0x05AB,0x80},
{0x05AC,0x0A},
{0x05AD,0x10},
{0x05AE,0x0C},
{0x05AF,0x0C},
{0x05B0,0x03},
{0x05B1,0x03},
{0x05B2,0x1C},
{0x05B3,0x02},
{0x05B4,0x00},
{0x05B5,0x0C},
{0x05B8,0x80},
{0x05B9,0x32},
{0x05BA,0x00},
{0x05BB,0x80},
{0x05BC,0x03},
{0x05BD,0x00},
{0x05BF,0x05},
{0x05C0,0x10},
{0x05C3,0x00},
{0x05C4,0x0C},
{0x05C5,0x20},
{0x05C7,0x01},
{0x05C8,0x14},
{0x05C9,0x54},
{0x05CA,0x14},
{0x05CB,0xE0},
{0x05CC,0x20},
{0x05CD,0x00},
{0x05CE,0x08},
{0x05CF,0x60},
{0x05D0,0x10},
{0x05D1,0x05},
{0x05D2,0x03},
{0x05D4,0x00},
{0x05D5,0x05},
{0x05D6,0x05},
{0x05D7,0x05},
{0x05D8,0x08},
{0x05DC,0x0C},
{0x05D9,0x00},
{0x05DB,0x00},
{0x05DD,0x0F},
{0x05DE,0x00},
{0x05DF,0x0A},
{0x05E0,0xA0},
{0x05E1,0x00},
{0x05E2,0xA0},
{0x05E3,0x00},
{0x05E4,0x05},
{0x05E5,0x00},
{0x05E6,0x24},
{0x05E7,0x03},
{0x05E8,0x07},
{0x05E9,0x00},
{0x05EA,0x5E},
{0x05EB,0x02},
{0x0660,0x04},
{0x0661,0x16},
{0x0662,0x04},
{0x0663,0x28},
{0x0664,0x04},
{0x0665,0x18},
{0x0666,0x04},
{0x0667,0x21},
{0x0668,0x04},
{0x0669,0x0C},
{0x066A,0x04},
{0x066B,0x25},
{0x066C,0x00},
{0x066D,0x12},
{0x066E,0x00},
{0x066F,0x80},
{0x0670,0x00},
{0x0671,0x0A},
{0x0672,0x04},
{0x0673,0x1D},
{0x0674,0x04},
{0x0675,0x1D},
{0x0676,0x00},
{0x0677,0x7E},
{0x0678,0x01},
{0x0679,0x47},
{0x067A,0x00},
{0x067B,0x73},
{0x067C,0x04},
{0x067D,0x14},
{0x067E,0x04},
{0x067F,0x28},
{0x0680,0x00},
{0x0681,0x22},
{0x0682,0x00},
{0x0683,0xA5},
{0x0684,0x00},
{0x0685,0x1E},
{0x0686,0x04},
{0x0687,0x1D},
{0x0688,0x04},
{0x0689,0x19},
{0x068A,0x04},
{0x068B,0x21},
{0x068C,0x04},
{0x068D,0x0A},
{0x068E,0x04},
{0x068F,0x25},
{0x0690,0x04},
{0x0691,0x15},
{0x0698,0x20},
{0x0699,0x20},
{0x069A,0x01},
{0x069C,0x22},
{0x069D,0x10},
{0x069E,0x10},
{0x069F,0x08},
{0x0000,0x01},
{0x0100,0x01},
{0x0101,0x01},
{0x0005,0x01},

SensorEnd

};
/* Senor full resolution setting: recommand for capture */
static struct rk_sensor_reg sensor_fullres_lowfps_data[] ={
{0x0380,0xFE},
{0x000D,0x00},
{0x000E,0x00},
{0x011F,0x88},
{0x0125,0xDF},
{0x0126,0x70},
{0x0131,0xAC},
{0x0366,0x20},
{0x0433,0x40},
{0x0435,0x50},
{0x05E4,0x0A},
{0x05E5,0x00},
{0x05E6,0x49},
{0x05E7,0x06},
{0x05E8,0x0A},
{0x05E9,0x00},
{0x05EA,0xB9},
{0x05EB,0x04},
{0x0000,0x01},
{0x0100,0x01},
{0x0101,0x01},
{0x0005,0x01},

	SensorEnd
};
/* Senor full resolution setting: recommand for video */
static struct rk_sensor_reg sensor_fullres_highfps_data[] ={
	SensorEnd
};
/* Preview resolution setting*/
static struct rk_sensor_reg sensor_preview_data[] =
{
/*¼������ڷ��ص�Ԥ����Ԥ��ʱ��ˮ���Ƶ�bug�޸�<2012.11.22>*/
 {0x0380,0xFF},
{0x0006,0x00},
{0x000D,0x11},
{0x000E,0x11},
//{0x0012,0x1C},
//{0x0013,0x01},
//{{0x0027,0x18},
{0x002A,0x1F},
//{0x0071,0xFF},
{0x0082,0xA2},
{0x011F,0x80},
{0x0125,0xDF},
{0x0126,0x70},
{0x0131,0xAD},
{0x0144,0x04},
{0x0190,0x80},
{0x0192,0x48},
//{0x0541,0x9D},// 0541��0543�Ǳ�ʾflicker step 
//{0x0543,0xBC},
{0x05E0,0xA0},
{0x05E1,0x00},
{0x05E2,0xA0},
{0x05E3,0x00},
{0x05E4,0x05},
{0x05E5,0x00},
{0x05E6,0x24},
{0x05E7,0x03},
{0x05E8,0x08},
{0x05E9,0x00},
{0x05EA,0x5F},
{0x05EB,0x02},
{0x0000,0x01},
{0x0100,0x01},
{0x0101,0x01},

	SensorEnd
};
/* 1280x720 */
static struct rk_sensor_reg sensor_720p[]={
	SensorEnd
};

/* 1920x1080 */
static struct rk_sensor_reg sensor_1080p[]={
	SensorEnd
};


static struct rk_sensor_reg sensor_softreset_data[]={
	SensorEnd
};

static struct rk_sensor_reg sensor_check_id_data[]={
    SensorRegVal(0x0001,0),
    SensorRegVal(0x0002,0),
	SensorEnd
};
/*
*  The following setting must been filled, if the function is turn on by CONFIG_SENSOR_xxxx
*/
static struct rk_sensor_reg sensor_WhiteB_Auto[]=
{    
    {0x0380, 0xFF},  //AWB auto
    {0x0000, 0x01},
    {0x0100, 0x01},
    {0x0101, 0x01},
	SensorEnd
};
/* Cloudy Colour Temperature : 6500K - 8000K  */
static	struct rk_sensor_reg sensor_WhiteB_Cloudy[]=
{
    {0x0380, 0xFD},
    {0x032D, 0x70},
    {0x032E, 0x01},
    {0x032F, 0x00},
    {0x0330, 0x01},
    {0x0331, 0x08},
    {0x0332, 0x01},
    {0x0101, 0xFF},
	SensorEnd
};
/* ClearDay Colour Temperature : 5000K - 6500K	*/
static	struct rk_sensor_reg sensor_WhiteB_ClearDay[]=
{
    {0x0380, 0xFD},
    {0x032D, 0x60},
    {0x032E, 0x01},
    {0x032F, 0x00},
    {0x0330, 0x01},
    {0x0331, 0x20},
    {0x0332, 0x01},
    {0x0101, 0xFF},
	SensorEnd
};
/* Office Colour Temperature : 3500K - 5000K  */
static	struct rk_sensor_reg sensor_WhiteB_TungstenLamp1[]=
{
    {0x0380, 0xFD},
    {0x032D, 0x50},
    {0x032E, 0x01},
    {0x032F, 0x00},
    {0x0330, 0x01},
    {0x0331, 0x30},
    {0x0332, 0x01},
    {0x0101, 0xFF},
	SensorEnd

};
/* Home Colour Temperature : 2500K - 3500K	*/
static	struct rk_sensor_reg sensor_WhiteB_TungstenLamp2[]=
{
    {0x0380, 0xFD},
    {0x032D, 0x10},
    {0x032E, 0x01},
    {0x032F, 0x00},
    {0x0330, 0x01},
    {0x0331, 0xA0},
    {0x0332, 0x01},
    {0x0101, 0xFF},
	SensorEnd
};
static struct rk_sensor_reg *sensor_WhiteBalanceSeqe[] = {sensor_WhiteB_Auto, sensor_WhiteB_TungstenLamp1,sensor_WhiteB_TungstenLamp2,
	sensor_WhiteB_ClearDay, sensor_WhiteB_Cloudy,NULL,
};

static	struct rk_sensor_reg sensor_Brightness0[]=
{
	// Brightness -2
	SensorEnd
};

static	struct rk_sensor_reg sensor_Brightness1[]=
{
	// Brightness -1

	SensorEnd
};

static	struct rk_sensor_reg sensor_Brightness2[]=
{
	//	Brightness 0

	SensorEnd
};

static	struct rk_sensor_reg sensor_Brightness3[]=
{
	// Brightness +1

	SensorEnd
};

static	struct rk_sensor_reg sensor_Brightness4[]=
{
	//	Brightness +2

	SensorEnd
};

static	struct rk_sensor_reg sensor_Brightness5[]=
{
	//	Brightness +3

	SensorEnd
};
static struct rk_sensor_reg *sensor_BrightnessSeqe[] = {sensor_Brightness0, sensor_Brightness1, sensor_Brightness2, sensor_Brightness3,
	sensor_Brightness4, sensor_Brightness5,NULL,
};

static	struct rk_sensor_reg sensor_Effect_Normal[] =
{
    {0x0488, 0x10},
    {0x0486, 0x00},
    {0x0487, 0xFF},
    {0x0101, 0xFF},
	SensorEnd
};

static	struct rk_sensor_reg sensor_Effect_WandB[] =
{
    {0x0488, 0x12},
    {0x0486, 0x00},
    {0x0487, 0xFF},
    {0x0101, 0xFF},
	SensorEnd
};

static	struct rk_sensor_reg sensor_Effect_Sepia[] =
{
    {0x0488, 0x11},
    {0x0486, 0x40},
    {0x0487, 0x90},
    {0x0101, 0xFF},
	SensorEnd
};

static	struct rk_sensor_reg sensor_Effect_Negative[] =
{
    {0x0488, 0x12},
    {0x0486, 0x00},
    {0x0487, 0xFF},
    {0x0101, 0xFF},
	SensorEnd
};
static	struct rk_sensor_reg sensor_Effect_Bluish[] =
{
    {0x0488, 0x11},
    {0x0486, 0xB0},
    {0x0487, 0x80},
    {0x0101, 0xFF},
	SensorEnd
};

static	struct rk_sensor_reg sensor_Effect_Green[] =
{
    {0x0488, 0x11},
    {0x0486, 0x60},
    {0x0487, 0x60},
    {0x0101, 0xFF},
	SensorEnd
};
static struct rk_sensor_reg *sensor_EffectSeqe[] = {sensor_Effect_Normal, sensor_Effect_WandB, sensor_Effect_Negative,sensor_Effect_Sepia,
	sensor_Effect_Bluish, sensor_Effect_Green,NULL,
};

static	struct rk_sensor_reg sensor_Exposure0[]=
{
	SensorEnd
};

static	struct rk_sensor_reg sensor_Exposure1[]=
{
	SensorEnd
};

static	struct rk_sensor_reg sensor_Exposure2[]=
{
	SensorEnd
};

static	struct rk_sensor_reg sensor_Exposure3[]=
{
	SensorEnd
};

static	struct rk_sensor_reg sensor_Exposure4[]=
{
	SensorEnd
};

static	struct rk_sensor_reg sensor_Exposure5[]=
{
	SensorEnd
};

static	struct rk_sensor_reg sensor_Exposure6[]=
{
	SensorEnd
};

static struct rk_sensor_reg *sensor_ExposureSeqe[] = {sensor_Exposure0, sensor_Exposure1, sensor_Exposure2, sensor_Exposure3,
	sensor_Exposure4, sensor_Exposure5,sensor_Exposure6,NULL,
};

static	struct rk_sensor_reg sensor_Saturation0[]=
{
	SensorEnd
};

static	struct rk_sensor_reg sensor_Saturation1[]=
{
	SensorEnd
};

static	struct rk_sensor_reg sensor_Saturation2[]=
{
	SensorEnd
};
static struct rk_sensor_reg *sensor_SaturationSeqe[] = {sensor_Saturation0, sensor_Saturation1, sensor_Saturation2, NULL,};

static	struct rk_sensor_reg sensor_Contrast0[]=
{
	SensorEnd
};

static	struct rk_sensor_reg sensor_Contrast1[]=
{
	SensorEnd
};

static	struct rk_sensor_reg sensor_Contrast2[]=
{
	SensorEnd
};

static	struct rk_sensor_reg sensor_Contrast3[]=
{
	SensorEnd
};

static	struct rk_sensor_reg sensor_Contrast4[]=
{
	SensorEnd
};


static	struct rk_sensor_reg sensor_Contrast5[]=
{
	SensorEnd
};

static	struct rk_sensor_reg sensor_Contrast6[]=
{
	SensorEnd
};
static struct rk_sensor_reg *sensor_ContrastSeqe[] = {sensor_Contrast0, sensor_Contrast1, sensor_Contrast2, sensor_Contrast3,
	sensor_Contrast4, sensor_Contrast5, sensor_Contrast6, NULL,
};
static	struct rk_sensor_reg sensor_SceneAuto[] =
{
	SensorEnd
};

static	struct rk_sensor_reg sensor_SceneNight[] =
{
	SensorEnd
};
static struct rk_sensor_reg *sensor_SceneSeqe[] = {sensor_SceneAuto, sensor_SceneNight,NULL,};

static struct rk_sensor_reg sensor_Zoom0[] =
{
	SensorEnd
};

static struct rk_sensor_reg sensor_Zoom1[] =
{
	SensorEnd
};

static struct rk_sensor_reg sensor_Zoom2[] =
{
	SensorEnd
};


static struct rk_sensor_reg sensor_Zoom3[] =
{
	SensorEnd
};
static struct rk_sensor_reg *sensor_ZoomSeqe[] = {sensor_Zoom0, sensor_Zoom1, sensor_Zoom2, sensor_Zoom3, NULL,};

/*
* User could be add v4l2_querymenu in sensor_controls by new_usr_v4l2menu
*/
static struct v4l2_querymenu sensor_menus[] =
{
};
/*
* User could be add v4l2_queryctrl in sensor_controls by new_user_v4l2ctrl
*/
static struct sensor_v4l2ctrl_usr_s sensor_controls[] =
{
};

//MUST define the current used format as the first item   
static struct rk_sensor_datafmt sensor_colour_fmts[] = {
	{V4L2_MBUS_FMT_UYVY8_2X8, V4L2_COLORSPACE_JPEG} 
};
static struct soc_camera_ops sensor_ops;


/*
**********************************************************
* Following is local code:
* 
* Please codeing your program here 
**********************************************************
*/

/*
**********************************************************
* Following is callback
* If necessary, you could coding these callback
**********************************************************
*/
/*
* the function is called in open sensor  
*/
static int sensor_activate_cb(struct i2c_client *client)
{

    SENSOR_DG("%s",__FUNCTION__);	

	
	return 0;
}
/*
* the function is called in close sensor
*/
static int sensor_deactivate_cb(struct i2c_client *client)
{
	//struct generic_sensor *sensor = to_generic_sensor(client);

    SENSOR_DG("%s",__FUNCTION__);
    
	return 0;
}
/*
* the function is called before sensor register setting in VIDIOC_S_FMT  
*/
static int sensor_s_fmt_cb_th(struct i2c_client *client,struct v4l2_mbus_framefmt *mf, bool capture)
{
    return 0;
}
/*
* the function is called after sensor register setting finished in VIDIOC_S_FMT  
*/
static int sensor_s_fmt_cb_bh (struct i2c_client *client,struct v4l2_mbus_framefmt *mf, bool capture)
{
    return 0;
}
static int sensor_softrest_usr_cb(struct i2c_client *client,struct rk_sensor_reg *series)
{
	
	return 0;
}
static int sensor_check_id_usr_cb(struct i2c_client *client,struct rk_sensor_reg *series)
{
	return 0;
}
static int sensor_try_fmt_cb_th(struct i2c_client *client,struct v4l2_mbus_framefmt *mf)
{
	return 0;
}

static int sensor_suspend(struct soc_camera_device *icd, pm_message_t pm_msg)
{
	//struct i2c_client *client = to_i2c_client(to_soc_camera_control(icd));
		
	if (pm_msg.event == PM_EVENT_SUSPEND) {
		SENSOR_DG("Suspend");
		
	} else {
		SENSOR_TR("pm_msg.event(0x%x) != PM_EVENT_SUSPEND\n",pm_msg.event);
		return -EINVAL;
	}
	return 0;
}

static int sensor_resume(struct soc_camera_device *icd)
{

	SENSOR_DG("Resume");

	return 0;

}
static int sensor_mirror_cb (struct i2c_client *client, int mirror)
{
	int err = 0;
    
    SENSOR_DG("mirror: %d",mirror);
	

	return err;    
}
/*
* the function is v4l2 control V4L2_CID_HFLIP callback  
*/
static int sensor_v4l2ctrl_mirror_cb(struct soc_camera_device *icd, struct sensor_v4l2ctrl_info_s *ctrl_info, 
                                                     struct v4l2_ext_control *ext_ctrl)
{
	struct i2c_client *client = to_i2c_client(to_soc_camera_control(icd));

    if (sensor_mirror_cb(client,ext_ctrl->value) != 0)
		SENSOR_TR("sensor_mirror failed, value:0x%x",ext_ctrl->value);
	
	SENSOR_DG("sensor_mirror success, value:0x%x",ext_ctrl->value);
	return 0;
}

static int sensor_flip_cb(struct i2c_client *client, int flip)
{
	int err = 0;	

    SENSOR_DG("flip: %d",flip);
	
	return err;    
}
/*
* the function is v4l2 control V4L2_CID_VFLIP callback  
*/
static int sensor_v4l2ctrl_flip_cb(struct soc_camera_device *icd, struct sensor_v4l2ctrl_info_s *ctrl_info, 
                                                     struct v4l2_ext_control *ext_ctrl)
{
	struct i2c_client *client = to_i2c_client(to_soc_camera_control(icd));

    if (sensor_flip_cb(client,ext_ctrl->value) != 0)
		SENSOR_TR("sensor_flip failed, value:0x%x",ext_ctrl->value);
	
	SENSOR_DG("sensor_flip success, value:0x%x",ext_ctrl->value);
	return 0;
}
/*
* the functions are focus callbacks
*/
static int sensor_focus_init_usr_cb(struct i2c_client *client){
	return 0;
}

static int sensor_focus_af_single_usr_cb(struct i2c_client *client){
	return 0;
}

static int sensor_focus_af_near_usr_cb(struct i2c_client *client){
	return 0;
}

static int sensor_focus_af_far_usr_cb(struct i2c_client *client){
	return 0;
}

static int sensor_focus_af_specialpos_usr_cb(struct i2c_client *client,int pos) {
	return 0;
}

static int sensor_focus_af_const_usr_cb(struct i2c_client *client){
	return 0;
}
static int sensor_focus_af_const_pause_usr_cb(struct i2c_client *client)
{
    return 0;
}
static int sensor_focus_af_close_usr_cb(struct i2c_client *client){
	return 0;
}

static int sensor_focus_af_zoneupdate_usr_cb(struct i2c_client *client, int *zone_tm_pos)
{
	return 0;
}

/*
face defect call back
*/
static int 	sensor_face_detect_usr_cb(struct i2c_client *client,int on){
	return 0;
}

/*
*   The function can been run in sensor_init_parametres which run in sensor_probe, so user can do some
* initialization in the function. 
*/
static void sensor_init_parameters_user(struct specific_sensor* spsensor,struct soc_camera_device *icd)
{
    return;
}

/*
* :::::WARNING:::::
* It is not allowed to modify the following code
*/

sensor_init_parameters_default_code();

sensor_v4l2_struct_initialization();

sensor_probe_default_code();

sensor_remove_default_code();

sensor_driver_default_module_code();



