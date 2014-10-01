//////////////////////////////////////////////////////////////////////////////

#ifndef __WIFI_H__
#define __WIFI_H__

//////////////////////////////////////////////////////////////////////////////
//-------------不再使用---------------start---------------------------------------

#define WIFIBUFLEN		1024
/////通讯状态
#define WIFI_WAIT_SYN     	0X00
#define WIFI_WAIT_CTL     	0x01
#define WIFI_LENGTH1      	0x02
#define WIFI_LENGTH2	   	0x03
#define WIFI_CHCK	   	0x04
#define WIFI_RECEDATA	   	0x05
#define WIFI_WAITPADDING  	0x06
#define WIFI_SYN          	0XAA 

//////////////Wifi Parameter/////////////////
//网络参数配置
#define WIFI_BSSID        	0X01   	// Ap Mac Address
#define WIFI_CHANEL       	0X02   	// The Chanel of ap
#define WIFI_SSID         	0x03	// Wifi Bss network ssid
#define WIFI_ENCRYPTTYPE  	0X04	// encrypt switch 0--disbale 1--enable
#define WIFI_ENKEY        	0X05	//  WIFI Key in encrypt mode
#define WIFI_DATATYPE     	0x06	// LLC Data Format         0－RAW，1－UDP，2 － TCP
#define WIFI_TCPPORT      	0x07	// TCP Sercer Port
#define WIFI_TCPIP        	0X08	// WIFI Module IP address
#define WIFI_TCPSERVERIP  	0x09	// Server IP address
#define WIFI_TCPSERVERMAC 	0x0a	// Server MAC Address(only in RAW mode)
#define WIFI_TCPMASET     	0X0b	// NetMask  1--255.255.255.0; 2--255.255.0.0; 3--255.0.0.0
#define WIFI_TCPGATEWAY   	0X0C	//  GateWay IP address
///系统参数配置
#define WIFI_BAUDBATE     	0XA2	// 波特率0－19200,1－38400?－57600?－115200,4-9600,5-4800,6-2400
#define WIFI_NETMODE      	0XA7	// jion Mode               0-Auto 1-manual
#define WIFI_TRANSPARENT  	0XA9	// 透明传输,模式选择 0－关闭，1－使能透明传输（仅在自动联网模式及UDP数据格式下有效）
#define WIFI_TCPMONITOR   	0XAA	// TCP Server Switch       0--disbale 1--enable
#define WIFI_APBSSIDENABLE    	0XAB    // only for 1.70 firmware  0--Eable 1--disable
// 数据类型
#define CTL_DATA     		0X00	////Control Data	
#define NOA_DATA  		0X10	////Normal Data
#define ACK_DATA   	    	0X20	////Ack 
// 数据描述
#define COMAND_DATA 		0x00   //// Host to wifi
#define MESSAGE_DATA 		0X40   //// Wifi to Host
// Wifi控制命令
#define SCANNETWORK_DATA   	0x00  	// 开始扫描网络命令
#define LINKNETWORK_DATA   	0X01	// 开始连接网络命令
#define CLOSENETWORK_DATA  	0x02	// 开始断开网络命令
#define SETNETPARA_DATA    	0X03	// 设置参数命令
#define QUERYNETPARA_DATA  	0X04	// 查询参数命令
#define RESTWIFI_DATA      	0X05	// 复位命令
#define TCPLINK_DATA       	0x06	// TCP连接命令
#define TCPSENDFAILED_DATA 	0x07	// TCP发送失败
// 消息类型，模块到主机 属于消息
#define SCANNET_END		0X40	// 扫描网络结果消息
#define LINKNET_END		0X41	// 网络已连接消息 
#define CLOSENET_END		0x42	// 网络已断开消息
#define SETNETPARA_END     	0X43	// 设置参数结果消息
#define QUERYNETPARA_END   	0X44	// 查询参数结果消息
#define RESTWIFI_END       	0X45	// 初始化完成消息
#define TCPLINK_END        	0x46	// TCP连接状态消息
////数据帧位置////////////////////////
#define SYN        		0	    //
#define CTL        		1	    //
#define LENL        		2	    //
#define LENH        		3	    //
#define CHK        		4	    //
#define CMD_DATA       		5	    //
////////套接字类型///////////////////////
#define RAW			0
#define UDP			1
#define TCP			2
//////////////////////////////////////////////////////////////////////////////
#define SERIAL_DELAY		10
//////////////////////////////////////////////////////////////////////////////
extern unsigned char __crc8_tbl[256];

typedef struct NetWorkBuffer
{
	  unsigned char gcIdx;
	  unsigned char gcChanle;
	  unsigned char gcBssId[6];
	  unsigned char gcLeg;
	  unsigned char gcSsid[20];
} NetWorkBuffer;

typedef struct NetParaBuffer
{
   unsigned char M_id;
   unsigned char cLength;
   unsigned char cInfo[64];
}NetParaBuffer;

typedef struct Buffer2
{
	unsigned char  	gcATReceData[WIFIBUFLEN];  // wifi 模块一次传输1440字节，这里有51芯片只有1280个xdata
	unsigned int 		gcATReceCount;
} Buffer2;

//-------------不再使用-------end-----------------------------------------------
typedef struct KEY_WEP
{
	unsigned char KeyLength;
	unsigned char KeyIndex;
	unsigned char Group[4][14];
} KEY_WEP;

typedef struct T_WIFI
{
	unsigned char nChannelNum;//信道号
	unsigned char szBssid[12+1];//BSSID
	char szSsid[32+1];//SSID
	unsigned char nDatalink;//数据链路层
	unsigned char szEquipmentIP[15 + 1];//设备IP
	char szServerMask[15+1];// 码
	char szServerGateway[15+1]; // 网关
	char szServerIP[15+1];//服务器的IP 地址
	char szEquipmentPort[5+1];//服务端口
	unsigned char nEncrypType;//加密类型
	KEY_WEP key_wep;
	unsigned char bTcpMonotor;
}T_WIFI;


////////////////////////////////////////////// 函数定义///////////////////////
int HardWareInit(void);
int Wifi_Para_set(T_WIFI t_Wifi);
int Wifi_Para_set_Auto(T_WIFI t_Wifi);
int Wifi_Auto_Init(char * sid1,char * sid2,char * sid3,char * sid4,char * sid5,char * sid6,char * sid7,char * sid8,char * sid9);
int WIFIReset(void);
//-------------不再使用-------start-----------------------------------------------
int ProcessWIFIData(void);
//-------------不再使用-------end-----------------------------------------------
int WIFI_Connect();
int WIFI_Close();
int WIFI_Send(char *szSendData, int nSendLen);
int WIFI_Recv(char *szRecvData, int* nRecvLen);
////////////////////////////////////////////// 函数定义///////////////////////
#endif //__WIFI_H__


