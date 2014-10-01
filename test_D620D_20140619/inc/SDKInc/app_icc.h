#ifndef _APP_ICC_H_
#define _APP_ICC_H_

//card slot.(icc_style)
#define	CARD_USER		0x00
#define	CARD_SAM1		0x01
#define	CARD_SAM2		0x02
#define	CARD_SAM3		0x03

//card type.
#define ST_EMV 			0x01
#define ST_SIM 			0x02
//20130314 add below
//ST_SAM_TYPE2:fidi=186
//ST_SAM_TYPE3:fidi=93
#define ST_SAM_TYPE2    0X03    //not support now.
#define ST_SAM_TYPE3    0X04    //supported.

//card voltage.(wk_vol)
#define	VOLTAGE_1_8V		0
#define	VOLTAGE_3V			1
#define	VOLTAGE_5V			2

//auto response or manual response.
#define	AUTO_RESP			0
#define	MANUAL_RESP			1

//AT24CXX SlaveAddr.
#define AT24C01         0XA0
#define AT24C02         0XA0
#define AT24C04_PAGE0   0XA0
#define AT24C04_PAGE1   0XA1
#define AT24C08_PAGE0   0XA0
#define AT24C08_PAGE1   0XA1
#define AT24C08_PAGE3   0XA2
#define AT24C08_PAGE4   0XA3
#define AT24C16_PAGE0   0XA0
#define AT24C16_PAGE1   0XA1
#define AT24C16_PAGE2   0XA2
#define AT24C16_PAGE3   0XA3
#define AT24C16_PAGE4   0XA4
#define AT24C16_PAGE5   0XA5
#define AT24C16_PAGE6   0XA6
#define AT24C16_PAGE7   0XA7

typedef struct
{
    unsigned char icc_style;    //卡类型:ST_EMV or ST_SIM
    unsigned char wk_vol;       //工作电压:VOLTAGE_1_8V/VOLTAGE_3V/VOLTAGE_5V
    unsigned char show_info;    //是否显示卡信息
    unsigned char reserved;     //2=AUTO PPS.1=MANUAL PPS.default set to 2(auto pps).
    unsigned long wait_time;    //timeout.0=default.
}WORK_MODE;



typedef struct 
{
	unsigned char Command[4];   //4bytes command.
	unsigned int Lc;	        //
	unsigned char *DataIn;
	int Le;	
}APDU_SEND;

typedef struct 
{
	unsigned int LenOut;
	unsigned char *DataOut;
	unsigned char SWA;
	unsigned char SWB;
}APDU_RESP;


//ERROR CODE
#define     ERR_SUCCESS 	    	0		//函数成功调用
#define 	ERR_NOSUPPORT  	    	1001	//不支持的操作, 比如对CPU 进行寻址操作lseek.
#define 	ERR_OVERFLOW   	    	1002	//地址超过范围, 比如lseek超过mem卡容量
#define 	ERR_ICCSTYLE   	    	1003	//卡类型错误
#define 	ERR_NOREADY 			1005	//数据未准备好
#define 	ERR_PPSFIRSTBYTE 		1006	//PPS设置 第一个字节非FF
#define 	ERR_PPSPCK		    	1007	//PPS校验错误
#define 	ERR_PPSLENGTH	  		1008 	//PPS长度错误
#define 	ERR_APDU	  	    	1009  	//APDU错误, 不符合4种类型
#define	    ERR_PROTOCOL			1036	//协议错误
#define	    ERR_SETEXTIRQ			1037
#define	    ERR_REQEXTIRQ			1038
#define	    ERR_HIS_TIMEOUT	    	1039
#define     ERR_HIS_PARITY_EDC		1040
#define     ERR_OPEN_ONCE			1044	//设备只能打开一次, 不支持连续open
#define 	ERR_PARITY_EDC 			2001	//EDC校验错误
#define 	ERR_TIMEOUT				2002	//超时
#define 	ERR_NOATR				2003	//复位没有信息返回
#define		ERR_POWER_FAILURE		2004	//power failure
#define 	ERR_INVALID_TS 			2005	//复位时 无效的TS值
#define  	ERR_INVALID_TA1			2006	//atr rejected
#define		ERR_INVALID_TB1			2007
#define		ERR_INVALID_TD1			2008
#define		ERR_INVALID_TA2			2009
#define		ERR_INVALID_TB2			2010
#define		ERR_INVALID_TC2			2011
#define		ERR_INVALID_TD2			2012
#define		ERR_INVALID_TA3			2013
#define		ERR_INVALID_TB3			2014
#define		ERR_INVALID_TC3			2015
#define		ERR_INVALID_TCK			2016
#define		ERR_INVALID_SW			2017
#define		ERR_CARD_ERRORS			2018	//other errors
#define		ERR_CARD_ABORTED		2019	//card aborted request (only in T=1)
#define		ERR_INVALID_SLOT		2020	//invalid card slot
#define		ERR_OPEN_SLOT_ERROR		2021	//open card slot error
#define		ERR_OTHER_ERROR			2022	//other errors
#define		ERR_INVALID_PARAM		2023	//parameter error
#define  	ERR_ATR_TIMEOUT			2024
#define  	ERR_ATR_TIMEOUT_ALL  	2025
#define  	ERR_ATR_LEN				2026
#define  	ERR_T1_IFSD				2030
#define  	ERR_T1_EDC				2031
#define  	ERR_T1_SYNTAX			2032
#define  	ERR_T1_BLOCK_TIMEOUT	2033

//universal
int IccOpen(int slot);
int IccDetect(int slot);
int IccClose(int slot);

//for CPU card.
//int IccInit(int slot, unsigned char *ATR, unsigned int *atr_len );
int IccInitEx(int slot, unsigned char *ATR, unsigned int *atr_len, WORK_MODE workMode );
int IccAutoResp(int slot, int autoresp);
int IccVoltage(int slot, int voltage);
int IccIsoCommand(int slot, APDU_SEND *ApduSend,  APDU_RESP *ApduRecv);

//for SLE4442
int IccMemCardInit(int slot);
int IccMemCardRW(int slot, int read_flag, int RST, int CLK, int IO, unsigned char *Read_byte);
int sle4442_reset(unsigned char *atr);
int sle_read_mm(unsigned char addr,unsigned char *buf,int length);
int sle_read_pm(unsigned char addr,unsigned char *buf,int length);
int sle_read_sm(unsigned char addr,unsigned char *buf,int length);
int sle_update_mm(unsigned char addr,unsigned char *buf,int length);
int sle_update_pm(unsigned char addr,unsigned char *buf,int length);
int sle_update_sm(unsigned char addr,unsigned char *buf,int length);
int sle_verify_EC(unsigned char *pw,unsigned char debug);

//FOR AT24CXX
int at24c_init();
int at24c_write_sub(unsigned char SlaveAddr,unsigned char SubAddr,unsigned char size,unsigned char *dat);
int at24c_read_sub(unsigned char SlaveAddr,unsigned char SubAddr,unsigned char size,unsigned char *dat);

//
/**********************************************************
*函数： IccUserPower
*说明:  IC卡上/下电.仅适用于USER大卡.其中上电功能仅作为调试用.
*输入： 0:off;1:on.
*返回：	0:success;others:fail.
*NOTE:  20130107 add this func.
***********************************************************/
int IccUserPower(int status);



#endif
