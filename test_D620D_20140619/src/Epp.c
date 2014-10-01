/***************************************************************************
* Copyright (c) 2007-2010, NEW POS Technology.
* All rights reserved.
*
* 文件名称：Epp.c
* 功能描述：   运行于NEW8110上的N20 EPP应用API接口，主要实现密码键盘的基本加解密功能
               // 用于详细说明此程序文件完成的主要功能，与其他模块或函数的接
*              // 口，输出值、取值范围、含义及参数间的控制、顺序、独立或依赖
*              // 等关系
* 其它描述:    // 其它补充内容的说明
*
* 作 者：
* 版本：1.0
* 完成日期：200x年xx月xx日
*
* 版本修改历史信息
*
* 作者 ：黄俊斌
* 版本：1.0
* 修改内容 ：1、创建
* 完成日期：2010年05月17日
****************************************************************************/
#include "Main.h"
#define _PRINT_DEBUG_
#define EPP_PORT PORT_COM1

#define EPP_RECV_PACKET_MAX_LEN 5050

#define  EPP_SEND_BUFFLEN      (8*1024)

#define CMD_CLEAR_ALL_APP_KEY    0x010c
#define CMD_CLEAR_IDLE_LOGO      0x0115

#define CMD_GET_RANDOM_NUMBER        0x0000
#define CMD_GET_SYSTEM_INFO          0x0001
#define CMD_BEEP                     0x0002
#define CMD_LIGHT                    0x0003

#define CMD_DISPLAY_LOGO             0x0100
#define CMD_CLEAR_SCREEN             0x0101
#define CMD_DISPLAY_STRING           0x0102
#define CMD_LOAD_IDLE_LOGO           0x0103
#define CMD_RESTORE_IDLE_LOGO        0x0104

#define CMD_KB_GET_STRING            0x0200

#define CMD_DOWNLOAD_LCK_MTEK        0x0300
#define CMD_DOWNLOAD_AIK_AUTH1       0x0301
#define CMD_DOWNLOAD_AIK_AUTH2       0x0302
#define CMD_DOWNLOAD_APP_INIT_KEY    0x0303
#define CMD_DKEY_AUTH_STEP1          0x0304
#define CMD_DKEY_AUTH_STEP2          0x0305
#define CMD_DOWNLOAD_KEY             0x0306
#define CMD_DOWNLOAD_DUKPT_KEY       0x0307

#define CMD_FORMAT_PED               0x0340
#define CMD_CLEAR_ALL_ONE_KEY        0x0341
#define CMD_CLEAR_ONE_KEY            0x0342

#define CMD_UKEY_AUTH_STEP1          0x0380
#define CMD_UKEY_AUTH_STEP2          0x0381
#define CMD_GET_MAC                  0x0382
#define CMD_GET_PIN                  0x0383
#define CMD_GET_FIXED_MAC            0x0384
#define CMD_GET_FIXED_PIN            0x0385
#define CMD_GET_DUKPT_MAC            0x0386
#define CMD_GET_DUKPT_PIN            0x0387
#define CMD_SET_PIN_TIMEOUT          0x0388
#define CMD_GET_TDEA                 0x0389
#define CMD_SET_TDES_IV              0x038a
// for factory
#define CMD_ENTER_FACTORY_TEST       0xf100
#define CMD_SET_PED_SN               0xf101
#define CMD_DOWNLOAD_UBSK            0xf102
#define CMD_CLEAR_EEPROM             0xf103

#define RET_MODULE_SYSTEM_INFO      0x00
#define RET_MODULE_KEY_BOARD        0x00
#define RET_MODULE_LCD_SCREEN       0x00
#define RET_MODULE_PED              0x00
#define RET_MODULE_COMMUNICATION    0x00
#define RET_MODULE_IC_CARD          0x00
#define RET_MODULE_MAGIC_CARD       0x00
#define RET_MODULE_FACTORY          0x00

#define KEY_INDEX_NUMBER        100

typedef struct _EPP_CONFIGURE_
{
    BYTE abyLck[16];
    BYTE abyMtek[24];
    BYTE abyAppName[32];
    BYTE abyAKDAK[16];
    BYTE abyAKUAK[16];
    BYTE abyMDTEK[24];
    BYTE abyMUTEK[24];
    BYTE byFlag;
    BYTE abyRev[3];
    int  iPort;
    BYTE abyMac[4];
}EPP_CONFIGURE;   // 164 bytes

static BYTE g_EppSendBuff[EPP_SEND_BUFFLEN];
static BYTE g_EppRecvBuff[EPP_SEND_BUFFLEN];

static const WORD g_awEpphalfCrc16CCITT[16]={ /* CRC 半字节余式表 */
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
    0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef
};

static const BYTE g_abyEppDefaultLck[16] = {0xde,0x21,0xcb,0x34,0xb6,0x25,0x34,0x33,0x87,0x98,
    0x9b,0xb5,0x39,0x81,0x42,0x49};

static const BYTE g_abyEppDefaultMtek[24] = {0x1a,0xc4,0xb9,0xc8,0x7b,0xde,0x8c,0x27,0x9b,0x4d,
    0xfe,0x3a,0x64,0x8b,0xcf,0x3f,0x93,0xca,0x02,0x2b,0x02,0x56,0x06,0x12};

static const BYTE g_abyEppDefaultAkdak[16]={0x26,0x90,0x77,0xa3,0x33,0x4d,0x32,0xb1,0x14,0xdb,0x04,0x2d,0x1d,0xa0,0x44,0xdb};

static const BYTE g_abyEppDefaultAkuak[16]={0x98,0xa4,0x9f,0xca,0x31,0x2b,0xcc,0x7e,0xa8,0x01,0x66,0xe1,0x8b,0x2b,0x0c,0xa6};

static const BYTE g_abyEppDefaultMdtek[24]={0x75,0x72,0x4d,0xb9,0x3a,0x73,0xe0,0x44,0x6e,0xf6,
    0xb1,0x7c,0xe8,0x1c,0x5d,0x97,0x0b,0xa4,0x4a,0x79,0x8f,0xc8,0xeb,0x18};

static const BYTE g_abyEppDefaultMutek[24]={0xef,0x02,0xb7,0x68,0x8e,0xe1,0x88,0x24,0x0c,0x50,
    0xa8,0xf0,0x22,0x2d,0xaf,0xe5,0x07,0xdd,0xd9,0x97,0x54,0xa8,0xe0,0x2c};

static const BYTE g_abyDefaultAppName[12] = {0x4e,0x45,0x57,0x20,0x50,0x4f,0x53,0x20,0x41,0x50,0x50,0x00};

static const BYTE g_abyEppMainKey[8] = {0x39, 0xc2, 0xb4, 0xf7, 0xd1, 0xb0, 0xe4, 0x84};

static EPP_CONFIGURE g_sEppConfigure;
static int g_iEppReadConfigureFlag = 0;

static int g_iEppPortNum = 0;
//static int g_iEppPortOpenFlag = 0;

//static BYTE g_byRetError = 0;

static const BYTE g_abySecuryPassword[8] = {0xab,0x87,0x98,0x56,0xcf,0xde,0xa3,0x19};

static const BYTE g_abyIdleLogoFileHead[62] =
{
    0x42, 0x4D, 0x3E, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x28, 0x00,
    0x00, 0x00, 0x7A, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00
};

static const BYTE g_abyNormalLogoFileHead[62] =
{
    0x42, 0x4D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x28, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00
};

static int s_EppSends(const BYTE *SendBuf, DWORD SendLen)
{
    //return portSends(g_iEppPortNum, (BYTE*)SendBuf, SendLen);
    return WriteUart(g_iEppPortNum, (BYTE*)SendBuf, SendLen);
}

static int s_EppRecv(BYTE *RecvBuf, DWORD TimeOutMs, int nRecvLen)
{
    	int nRet = 0;
    	nRet = ReadUart(g_iEppPortNum, (BYTE*)RecvBuf, nRecvLen, TimeOutMs);
	if (nRet <= 0)
	{
		return EPP_RECV_TIMEOUT;
	}
	return nRet;
}

#define _PRINT_DEBUG_
#ifdef  _PRINT_DEBUG_
BYTE g_abySendBuffPrint[10240];
BYTE g_abyRecvBuffPrint[10240];
int g_iSendPrintLen = 0;
int g_iRecvPrintLen = 0;
#endif

/****************************************************************************
  函数名     :  void Crc16CCITT(BYTE *pbyDataIn, DWORD dwDataLen, BYTE abyCrcOut[2])
  描述       :  用移位的方法计算一组数字的16位CRC-CCITT校验值
  输入参数   :  1、BYTE *pbyDataIn : 要进行16位CRC-CCITT计算的数
                2、WORD dwDataLen : pbyDataIn数组的长度最大值65535
  输出参数   :  1、BYTE abyCrcOut[2] : 16位CRC-CCITT计算的结果
  返回值     :  无
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2010-03-30  V1.0         创建
****************************************************************************/
static void s_EppCrc16CCITT(const BYTE *pbyDataIn, DWORD dwDataLen, BYTE abyCrcOut[2])
{
    WORD wCrc = 0;
    BYTE byTemp;
    while (dwDataLen-- != 0)
    {
        byTemp = ((BYTE)(wCrc>>8))>>4;
        wCrc <<= 4;
        wCrc ^= g_awEpphalfCrc16CCITT[byTemp^(*pbyDataIn/16)];
        byTemp = ((BYTE)(wCrc>>8))>>4;
        wCrc <<= 4;
        wCrc ^= g_awEpphalfCrc16CCITT[byTemp^(*pbyDataIn&0x0f)];
        pbyDataIn++;
    }
    abyCrcOut[0] = wCrc/256;
    abyCrcOut[1] = wCrc%256;
}

/****************************************************************************
  函数名     :  int Crc16SendPacket(BYTE *pbySendData, WORD wDataLen, BYTE byCmd)
  描述       :  数据包发送函数，带CRC16-CCITT校验
  输入参数   :  1、BYTE *pbySendData：要发送的数据包
                2、WORD wDataLen：数据包长度
                3、BYTE byCmd：命令码
  输出参数   :  无
  返回值     :  SUCCESS：发送成功 / SEND_PACKET_ERROR：发送错误
                / PACKET_LEN_TOO_LONG
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2010-03-30  V1.0         创建
****************************************************************************/
static int s_EppCrc16SendPacket(WORD wDataLen, WORD wCmd)
{
    int iRet;
    int i = 0;

    if (wDataLen > 8900)
    {
        return EPP_PACKET_LEN_TOO_LONG;
    }
    g_EppSendBuff[0] = 0x02;
    g_EppSendBuff[1] = (BYTE)(wCmd>>8);
    g_EppSendBuff[2] = (BYTE)(wCmd);
    g_EppSendBuff[3] = wDataLen/256;
    g_EppSendBuff[4] = wDataLen%256;
    s_EppCrc16CCITT(&g_EppSendBuff[1], wDataLen+4, &g_EppSendBuff[wDataLen+5]);

	printf("\n");
    for (i = 0; i < wDataLen + 7; i++)
	{
		printf("%02x", g_EppSendBuff[i]);
	}
	printf("\n");
    iRet = s_EppSends(g_EppSendBuff, wDataLen+7);
    if (-1 == iRet || 0 == iRet)
    {
        return EPP_SEND_PACKET_ERROR;
    }
	
#ifdef  _PRINT_DEBUG_
    memcpy(&g_abySendBuffPrint[g_iSendPrintLen], g_EppSendBuff, wDataLen+7);
    g_iSendPrintLen += wDataLen+7;
    memset(&g_abySendBuffPrint[g_iSendPrintLen], 0xff, 4);
    g_iSendPrintLen += 4;
#endif

    return EPP_SUCCESS;
}


/****************************************************************************
  函数名     :  int Crc16RecvPacket(BYTE *pbyRecvData, WORD *pwPacketetLen, DWORD dwTimeoutMs)
  描述       :  数据包接收函数，带CRC16-CCITT校验
  输入参数   :  1、DWORD dwTimeoutMs：接收超时时间，时间单位是毫秒
  输出参数   :  1、BYTE *pbyRecvData：接收到的所有数据，包括包头和校验，传入
                   的pbyRecvData指向的内存不得小于256字节
                2、WORD *pwPacketetLen：接收到的所有数据的长度
  返回值     :  EPP_SUCCESS：接收成功 / EPP_CRC_CHECK_ERROR：数据包CRC校验错误
                / EPP_PACKET_LEN_ERROR：数据包长度错误 / EPP_RECV_TIMEOUT：超时
                / EPP_RECV_PACKET_ERROR：接收错误 /EPP_USER_PRESS_CANCEL
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2010-03-30  V1.0         创建
****************************************************************************/
static int s_EppCrc16RecvPacket(BYTE *pbyRecvData, WORD *pwPacketetLen, DWORD dwTimeoutMs)
{
    BYTE abyCrc[2];
    int i, iRet;
    WORD wPacketLen = 0;

    	iRet = s_EppRecv(pbyRecvData, 3, 1); // timeout = 3ms
	if (EPP_RECV_TIMEOUT == iRet || 0x02 != pbyRecvData[0])
	{
		return EPP_RECV_TIMEOUT;
	}
    	for (i=0; i<4; i++)
    	{
        	iRet = s_EppRecv(&pbyRecvData[i+1], 3, 1);
        	if (EPP_RECV_TIMEOUT == iRet)
		{
            		return EPP_RECV_PACKET_ERROR;
        	}
    	}
    	wPacketLen = pbyRecvData[3]*256 + pbyRecvData[4];
    	*pwPacketetLen = wPacketLen + 7;
    	if (wPacketLen > EPP_RECV_PACKET_MAX_LEN)
	{
        	return EPP_PACKET_LEN_ERROR;
    	}
    	for (i=0; i<wPacketLen+2; i++)
    	{
        	iRet = s_EppRecv(&pbyRecvData[i+5], 3, 1);
        	if (EPP_RECV_TIMEOUT == iRet)
        	{
            		return EPP_RECV_PACKET_ERROR;
        	}
    	}
    	s_EppCrc16CCITT(&pbyRecvData[1], wPacketLen+4, abyCrc);
    	if (0 != memcmp(abyCrc, &pbyRecvData[5+wPacketLen], 2))
	{
        	return EPP_CRC_CHECK_ERROR;
    	}

#ifdef  _PRINT_DEBUG_
        memcpy(&g_abyRecvBuffPrint[g_iRecvPrintLen], pbyRecvData, *pwPacketetLen);
        g_iRecvPrintLen += *pwPacketetLen;
        memset(&g_abyRecvBuffPrint[g_iRecvPrintLen], 0xff, 4);
        g_iRecvPrintLen += 4;
#endif

    return EPP_SUCCESS;
}

/*void s_EppMultiDes(const BYTE *inData, int iMulti, BYTE *outData,
         const BYTE *key, BYTE byKeyLen, BYTE mode)
{
    int i, j;

    for (i=0; i<iMulti; i++)
    {
        j = i<<3;
        TDEA((BYTE*)&inData[j], &outData[j], (BYTE*)key, byKeyLen, mode);
    }
}*/

/*static void s_EppComputeMacCBC(const BYTE *pbyDataIn, int iDataLen,
    const BYTE *pbyKeyIn, BYTE byKeyLen, BYTE *pbyMacOut)
{
    BYTE abyTemp[10];
    int i;

    memset(abyTemp, 0, sizeof(abyTemp));
    for (i=0; i<iDataLen; i++)
    {
        abyTemp[i&0x07] ^= pbyDataIn[i];
        if (0 == ((i+1)&0x07))
        {
            TDEA(abyTemp, pbyMacOut, (BYTE*)pbyKeyIn, byKeyLen, TDEA_ENCRYPT);
            memcpy(abyTemp, pbyMacOut, 8);
        }
    }
    if (0 != (i&0x07))
    {
        TDEA(abyTemp, pbyMacOut, (BYTE*)pbyKeyIn, byKeyLen, TDEA_ENCRYPT);
    }
}
*/
static void s_EppSetDefaultConfig(EPP_CONFIGURE *psCfg)
{
    memcpy(psCfg->abyLck, g_abyEppDefaultLck, 16);
    memcpy(psCfg->abyMtek, g_abyEppDefaultMtek, 24);
    memset(psCfg->abyAppName, 0, 32);
    memcpy(psCfg->abyAppName, g_abyDefaultAppName, 12);
    memcpy(psCfg->abyAKDAK, g_abyEppDefaultAkdak, 16);
    memcpy(psCfg->abyAKUAK, g_abyEppDefaultAkuak, 16);
    memcpy(psCfg->abyMDTEK, g_abyEppDefaultMdtek, 24);
    memcpy(psCfg->abyMUTEK, g_abyEppDefaultMutek, 24);
    psCfg->byFlag = 0x03;
    g_iEppReadConfigureFlag = 1;
}
/*

static int s_EppGetConfig(EPP_CONFIGURE *psCfg)
{
    if (0 == g_iEppReadConfigureFlag)
    {
        s_EppSetDefaultConfig(&g_sEppConfigure);
    }
    memcpy(psCfg, &g_sEppConfigure, sizeof(EPP_CONFIGURE));
    return EPP_SUCCESS;
}

static void s_EppLoadKeyInitial(const BYTE *pbyKsnIn, BYTE byKsnLen,
                      const BYTE *pbyBdkIn, BYTE *pbyKeyOut, BYTE *abyCurKSNOut)
{
    BYTE abyTemp[16], abyCurBdk[16];
    int i;

    memcpy(abyCurBdk, (BYTE*)pbyBdkIn, 16);
    memset(abyCurKSNOut, 0xff, 10);
    memcpy(&abyCurKSNOut[10-byKsnLen], (BYTE*)pbyKsnIn, byKsnLen);
    memset(&abyCurKSNOut[8], 0, 2);
    abyCurKSNOut[7] &= 0xe0;
    TDEA(abyCurKSNOut, pbyKeyOut, abyCurBdk, 16, TDEA_ENCRYPT);
    for (i=0; i<4; i++)
    {
        abyTemp[i] = abyCurBdk[i]^0xc0;
        abyTemp[i+4] = abyCurBdk[i+4];
        abyTemp[i+8] = abyCurBdk[i+8]^0xc0;
        abyTemp[i+12] = abyCurBdk[i+12];
    }
    TDEA(abyCurKSNOut, &pbyKeyOut[8], abyTemp, 16, TDEA_ENCRYPT);
}
*/
static void s_EppPortClear(void)
{
    char szBuff[10240];
    DWORD dwLen = 0;
    //portReset(g_iEppPortNum);
    ReadUart(g_iEppPortNum, szBuff, dwLen, 1);
}

static void s_EppCheckCfg(void)
{
    if (0 == g_iEppReadConfigureFlag)
    {
        s_EppSetDefaultConfig(&g_sEppConfigure);
    }
}


int EppClose(void)
{
    //if (0 != g_iEppPortOpenFlag)
    {
       // g_iEppPortOpenFlag = 0;
        return SERIAL_Close(g_iEppPortNum);
    }
    return 0;
}

int EppOpen(void)
{
    int iHandle = 0;

    SwitchCom0_Pinpad();
    //g_iEppPortOpenFlag = 0;
    if((iHandle = SERIAL_Open("/dev/ttyS0", 115200)) < 0)
    {
        //g_iEppPortOpenFlag = 0;
        printf("Open Pinpad RS232 error!");
        FailBeep();
        TextOut(0, 3, ALIGN_CENTER, "串口打不开!");
        Wait(2000);
        return ERROR;
    }
    //g_iEppPortOpenFlag = 1;
    printf("打开串口成功\n");
    g_iEppPortNum = iHandle;
    return iHandle;
}

/****************************************************************************
  函数名     :  int EppInit(const BYTE *pbyLck, const BYTE *pbyMtek, const BYTE *pbyAppName,
                      const BYTE *pbyAkdak, const BYTE *pbyAkuak, const BYTE *pbyMdtek,
                      const BYTE *pbyMutek)
  描述       :  修改当前密钥配置，如果不使用默认配置，每次设备启动都要初始化配置
  输入参数   :  1、const BYTE *pbyLck ：16字节密钥
                2、const BYTE *pbyMtek ：24字节密钥
                3、const BYTE *pbyAppName ：最长16字节的应用名
                4、const BYTE *pbyAkdak ：16字节密钥
                5、const BYTE *pbyAkuak ：16字节密钥
                6、const BYTE *pbyMdtek ：24字节密钥
                7、const BYTE *pbyMutek ：24字节密钥
  输出参数   :  无
  返回值     :  EPP_SUCCESS
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2010-12-11  V1.0         创建
****************************************************************************/
int EppInit(const BYTE *pbyLck, const BYTE *pbyMtek, const BYTE *pbyAppName, const BYTE *pbyAkdak,
    const BYTE *pbyAkuak, const BYTE *pbyMdtek, const BYTE *pbyMutek)
{
    memcpy(g_sEppConfigure.abyLck, pbyLck, 16);
    memcpy(g_sEppConfigure.abyMtek, pbyMtek, 24);
    memset(g_sEppConfigure.abyAppName, 0, 32);
    strcpy((char*)g_sEppConfigure.abyAppName, (char*)pbyAppName);
    memcpy(g_sEppConfigure.abyAKDAK, pbyAkdak, 16);
    memcpy(g_sEppConfigure.abyAKUAK, pbyAkuak, 16);
    memcpy(g_sEppConfigure.abyMDTEK, pbyMdtek, 24);
    memcpy(g_sEppConfigure.abyMUTEK, pbyMutek, 24);
    g_iEppReadConfigureFlag = 1;
    return EPP_SUCCESS;
}

/****************************************************************************
  函数名     :  int EppDownloadLckMtek(void)
  描述       :  将当前配置中的LCK和MTEK下载到N20上
  输入参数   :  无
  输出参数   :  无
  返回值     :  EPP_SUCCESS / EPP_CRC_CHECK_ERROR / EPP_SEND_CMD_ERROR
                / EPP_PACKET_LEN_ERROR / EPP_RECV_TIMEOUT
                / EPP_RECV_PACKET_ERROR / EPP_USER_PRESS_CANCEL
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2010-12-11  V1.0         创建
****************************************************************************/
int EppDownloadLckMtek(void)
{
    int iRet;
    WORD wRecvLen;

    s_EppPortClear();
    s_EppCheckCfg();

    memcpy(&g_EppSendBuff[5], g_sEppConfigure.abyLck, 16);
    memcpy(&g_EppSendBuff[21], g_sEppConfigure.abyMtek, 24);
    iRet = s_EppCrc16SendPacket(40, CMD_DOWNLOAD_LCK_MTEK);
    if (EPP_SUCCESS != iRet)
    {//lcdPrintf("as011"); GetKey();
        return EPP_SEND_CMD_ERROR;
    }
    memset(g_EppRecvBuff, 0x0, 100);
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 100000);
    if (EPP_SUCCESS != iRet)
    {//lcdPrintf("as012"); GetKey();
        return iRet;
    }
    if (0 != g_EppRecvBuff[8])
    {
        return (RET_ERROR_BASE+g_EppRecvBuff[8]);
    }
    return EPP_SUCCESS;
}


/****************************************************************************
  函数名     :  int EppDownloadAik(void)
  描述       :  将当前配置中AIK下载到N20上。AIK包括AppName和AKDAK/AKUAK/MDTEK/MUTEK等4组密钥。
  输入参数   :  无
  输出参数   :  无
  返回值     :  EPP_SUCCESS / EPP_CRC_CHECK_ERROR / EPP_SEND_CMD_ERROR
                / EPP_PACKET_LEN_ERROR / EPP_RECV_TIMEOUT / EPP_RECV_LEN_ERROR
                / EPP_RECV_PACKET_ERROR / EPP_USER_PRESS_CANCEL / EPP_AUTHEN_FAILED
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2010-12-11  V1.0         创建
****************************************************************************/
/*int EppDownloadAik(void)
{
    int iRet;
    WORD wRecvLen;
    BYTE abyRand[10], abyTek[64], abyMac[8];

    s_EppPortClear();
    s_EppCheckCfg();

    PedGetRandom(&g_EppSendBuff[5]);
    memcpy(abyRand, &g_EppSendBuff[5], 8);
    iRet = s_EppCrc16SendPacket(8, CMD_DOWNLOAD_AIK_AUTH1);
    if (EPP_SUCCESS != iRet)
    {
        return EPP_SEND_CMD_ERROR;
    }
    memset(g_EppRecvBuff, 0x0, 100);
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 3000);
    if (EPP_SUCCESS != iRet)
    {
        return iRet;
    }
    if (0 != g_EppRecvBuff[8])
    {
        return (RET_ERROR_BASE+g_EppRecvBuff[8]);
    }
    if (75 != wRecvLen)
    {
        return EPP_RECV_LEN_ERROR;
    }
    TDEA(&g_EppRecvBuff[9], &g_EppSendBuff[20], g_sEppConfigure.abyLck, 16, TDEA_DECRYPT);
    if (0 != memcmp(abyRand,&g_EppSendBuff[20],8))
    {
        return EPP_AUTHEN_FAILED;
    }

    s_EppMultiDes(&g_EppRecvBuff[25], 6, abyTek, g_sEppConfigure.abyMtek, 24, TDEA_DECRYPT);

    TDEA(&g_EppRecvBuff[17], &g_EppSendBuff[9], g_sEppConfigure.abyLck, 16, TDEA_ENCRYPT);
    g_EppSendBuff[5] = 0;
    g_EppSendBuff[6] = 0;
    g_EppSendBuff[7] = RET_MODULE_PED;
    g_EppSendBuff[8] = 0;
    iRet = s_EppCrc16SendPacket(12, CMD_DOWNLOAD_AIK_AUTH2);
    if (EPP_SUCCESS != iRet)
    {
        return EPP_SEND_CMD_ERROR;
    }
    memset(g_EppRecvBuff, 0x0, 100);
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 3000);
    if (EPP_SUCCESS != iRet)
    {
        return iRet;
    }
    if (0 != g_EppRecvBuff[8])
    {
        return (RET_ERROR_BASE+g_EppRecvBuff[8]);
    }
    if (11 != wRecvLen)
    {
        return EPP_RECV_LEN_ERROR;
    }

    memset(g_EppRecvBuff, 0, 1024);
    memcpy(g_EppRecvBuff, g_sEppConfigure.abyAppName, 16);
    memcpy(&g_EppRecvBuff[32], g_sEppConfigure.abyAKDAK, 16);
    TDEA(&g_EppRecvBuff[500], &g_EppRecvBuff[48], g_sEppConfigure.abyAKDAK, 16, TDEA_ENCRYPT);
    memcpy(&g_EppRecvBuff[50], g_sEppConfigure.abyAKUAK, 16);
    TDEA(&g_EppRecvBuff[500], &g_EppRecvBuff[66], g_sEppConfigure.abyAKUAK, 16, TDEA_ENCRYPT);
    memcpy(&g_EppRecvBuff[68], g_sEppConfigure.abyMDTEK, 24);
    TDEA(&g_EppRecvBuff[500], &g_EppRecvBuff[92], g_sEppConfigure.abyMDTEK, 24, TDEA_ENCRYPT);
    memcpy(&g_EppRecvBuff[94], g_sEppConfigure.abyMUTEK, 24);
    TDEA(&g_EppRecvBuff[500], &g_EppRecvBuff[118], g_sEppConfigure.abyMUTEK, 24, TDEA_ENCRYPT);
    s_EppMultiDes(g_EppRecvBuff, 15, &g_EppSendBuff[5], abyTek, 24, TDEA_ENCRYPT);

    s_EppComputeMacCBC(&g_EppSendBuff[5], 120, &abyTek[24], 24, &g_EppSendBuff[125]);
    memcpy(abyMac, &g_EppSendBuff[125], 8);

    iRet = s_EppCrc16SendPacket(124, CMD_DOWNLOAD_APP_INIT_KEY);
    if (EPP_SUCCESS != iRet)
    {
        return EPP_SEND_CMD_ERROR;
    }

    memset(g_EppRecvBuff, 0x0, 100);
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 13000);
    if (EPP_SUCCESS != iRet)
    {
        return iRet;
    }
    if (0 != g_EppRecvBuff[8])
    {
        return (RET_ERROR_BASE+g_EppRecvBuff[8]);
    }
    if (11 != wRecvLen)
    {
        return EPP_RECV_LEN_ERROR;
    }
    return EPP_SUCCESS;
}
*/

/****************************************************************************
  函数名     :  int EppDownloadAppKey(BYTE byKeyType, BYTE byMode, BYTE byKeyIndex,
                     const BYTE *pbyKeyDataIn, BYTE byKeyLen, BYTE byMasterKeyIndex)
  描述       :  应用密钥下载
  输入参数   :  1、BYTE byKeyType ：密钥类型
                            0x01：主密钥；
                            0x02：MAC密钥；
                            0x03：PIN密钥；
                            0x10：Fixed MAC密钥；
                            0x11：Fixed PIN密钥；
                2、BYTE byKeyIndex ：密钥索引[1，100]
                3、BYTE byMode ：
                         Bit0~3  TDEA_DECRYPT    0x00    采用解密方法得到明文目标Key
                                 TDEA_NONE       0x02    直接写入明文目标Key(对MAC key和PIN key无效)
                         Bit4~7  PARITY_NONE     0x00    对(解密后的)Key明文无校验
                                 PARITY_ODD      0x10    对(解密后的)Key明文进行奇校验
                                 PARITY_EVEN     0x20    对(解密后的)Key明文进行偶校验
                4、const BYTE *pbyKeyDataIn ：密钥内容
                5、BYTE byKeyLen ：密钥长度，可取值8/16/24
                6、BYTE byMasterKeyIndex ：主密钥索引[1，100]，只有密钥类型是MAC密钥或者
                        PIN密钥的时候，该参数才有效
  输出参数   :  无
  返回值     :  EPP_SUCCESS / EPP_CRC_CHECK_ERROR / EPP_SEND_CMD_ERROR / EPP_KEY_TYPE_ERROR
                / EPP_PACKET_LEN_ERROR / EPP_RECV_TIMEOUT / EPP_RECV_LEN_ERROR
                / EPP_RECV_PACKET_ERROR / EPP_USER_PRESS_CANCEL
                / EPP_MASTER_KEY_INDEX_ERROR / EPP_KEY_INDEX_ERROR / EPP_INPUT_DATA_LEN_ERROR
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2010-12-11  V1.0         创建
****************************************************************************/
/*int EppDownloadAppKey(BYTE byKeyType, BYTE byMode, BYTE byKeyIndex,
    const BYTE *pbyKeyDataIn, BYTE byKeyLen, BYTE byMasterKeyIndex)
{
    int iRet, k;
    BYTE abyRand[10], abyTek[64];
    WORD wRecvLen;

    if ((KEY_TYPE_MASTER!=byKeyType) && (KEY_TYPE_MAC!=byKeyType)
        && (KEY_TYPE_PIN!=byKeyType) && (KEY_TYPE_FIXED_MAC!=byKeyType)
        && (KEY_TYPE_FIXED_PIN!=byKeyType))
    {
        return EPP_KEY_TYPE_ERROR;
    }
    if ((KEY_TYPE_MASTER!=byKeyType) && ((byMasterKeyIndex>KEY_INDEX_NUMBER)||(0==byMasterKeyIndex)))
    {
        return EPP_MASTER_KEY_INDEX_ERROR;
    }
    if ((byKeyIndex>KEY_INDEX_NUMBER) || (0==byKeyIndex))
    {
        return EPP_KEY_INDEX_ERROR;
    }
    if ((byKeyLen!=8) && (byKeyLen!=16) && (byKeyLen!=24))
    {
        return EPP_INPUT_DATA_LEN_ERROR;
    }

    s_EppPortClear();
    s_EppCheckCfg();

    memset(g_EppSendBuff, 0, 1024);
    memcpy(&g_EppSendBuff[5], g_sEppConfigure.abyAppName, 16);
    PedGetRandom(&g_EppSendBuff[37]);
//    memset(&g_EppSendBuff[37], 0x31, 8);
    memcpy(abyRand, &g_EppSendBuff[37], 8);
    iRet = s_EppCrc16SendPacket(40, CMD_DKEY_AUTH_STEP1);
    if (EPP_SUCCESS != iRet)
    {//lcdPrintf("as031"); GetKey();
        return EPP_SEND_CMD_ERROR;
    }
    memset(g_EppRecvBuff, 0x0, 100);
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 3000);
    if (EPP_SUCCESS != iRet)
    {//lcdPrintf("as032=%s,",g_sEppConfigure.abyAppName); GetKey();
        return iRet;
    }
    if (0 != g_EppRecvBuff[8])
    {//lcdPrintf("as033"); GetKey();
        return (RET_ERROR_BASE+g_EppRecvBuff[8]);
    }
    if (75 != wRecvLen)
    {//lcdPrintf("as034"); GetKey();
        return EPP_RECV_LEN_ERROR;
    }
    TDEA(&g_EppRecvBuff[9], &g_EppSendBuff[20], g_sEppConfigure.abyAKDAK, 16, TDEA_DECRYPT);
    if (0 != memcmp(abyRand,&g_EppSendBuff[20],8))
    {
*//*    lcdCls();
    lcdDisplay(0,0,0,"as035=%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,",
        abyRand[0], abyRand[1], abyRand[2], abyRand[3], abyRand[4], abyRand[5],
        abyRand[6], abyRand[7]);
    lcdDisplay(0,2,0,"%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,",
        g_EppSendBuff[20], g_EppSendBuff[21], g_EppSendBuff[22], g_EppSendBuff[23],
        g_EppSendBuff[24], g_EppSendBuff[25], g_EppSendBuff[26], g_EppSendBuff[27]);
    lcdDisplay(0,4,0,"%02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x,",
        g_EppRecvBuff[9], g_EppRecvBuff[10], g_EppRecvBuff[11], g_EppRecvBuff[12],
        g_EppRecvBuff[13], g_EppRecvBuff[14],
        g_EppRecvBuff[15], g_EppRecvBuff[16]);
    GetKey();*/
  /*      return EPP_AUTHEN_FAILED;
    }

    s_EppMultiDes(&g_EppRecvBuff[25], 6, abyTek, g_sEppConfigure.abyMDTEK, 24, TDEA_DECRYPT);

    TDEA(&g_EppRecvBuff[17], &g_EppSendBuff[9], g_sEppConfigure.abyAKDAK, 16, TDEA_ENCRYPT);
    g_EppSendBuff[5] = 0;
    g_EppSendBuff[6] = 0;
    g_EppSendBuff[7] = RET_MODULE_PED;
    g_EppSendBuff[8] = 0;
    iRet = s_EppCrc16SendPacket(12, CMD_DKEY_AUTH_STEP2);
    if (EPP_SUCCESS != iRet)
    {//lcdPrintf("as036"); GetKey();
        return EPP_SEND_CMD_ERROR;
    }
    memset(g_EppRecvBuff, 0x0, 100);
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 3000);
    if (EPP_SUCCESS != iRet)
    {//lcdPrintf("as037"); GetKey();
        return iRet;
    }
    if (0 != g_EppRecvBuff[8])
    {//lcdPrintf("as038"); GetKey();
        return (RET_ERROR_BASE+g_EppRecvBuff[8]);
    }
    if (11 != wRecvLen)
    {//lcdPrintf("as039"); GetKey();
        return EPP_RECV_LEN_ERROR;
    }

    memset(g_EppRecvBuff, 0, 1024);
    g_EppRecvBuff[0] = byKeyType;  // Key type = fixed mac key
    g_EppRecvBuff[1] = byMode;  //
    g_EppRecvBuff[2] = byMasterKeyIndex;  //
    g_EppRecvBuff[3] = byKeyIndex;  // Key Index = 1;
    g_EppRecvBuff[4] = byKeyLen;  // Key Length=16
    memset(&g_EppRecvBuff[5], 0x13, byKeyLen+4);
    memcpy(&g_EppRecvBuff[5], pbyKeyDataIn, byKeyLen);
    k = byKeyLen/8;
    k++;
    s_EppMultiDes(g_EppRecvBuff, k, &g_EppSendBuff[5], abyTek, 24, TDEA_ENCRYPT);

    s_EppComputeMacCBC(&g_EppSendBuff[5], byKeyLen+8, &abyTek[24], 24, &g_EppSendBuff[byKeyLen+13]);

    iRet = s_EppCrc16SendPacket(byKeyLen+12, CMD_DOWNLOAD_KEY);
    if (EPP_SUCCESS != iRet)
    {//lcdPrintf("as03a"); GetKey();
        return EPP_SEND_CMD_ERROR;
    }

    memset(g_EppRecvBuff, 0x0, 100);
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 30000);
    if (EPP_SUCCESS != iRet)
    {//lcdPrintf("as03b"); GetKey();
        return iRet;
    }
    if (0 != g_EppRecvBuff[8])
    {//lcdPrintf("as03c"); GetKey();
        return (RET_ERROR_BASE+g_EppRecvBuff[8]);
    }
    if (11 != wRecvLen)
    {//lcdPrintf("as03d"); GetKey();
        return EPP_RECV_LEN_ERROR;
    }
    return EPP_SUCCESS;
}
*/
/****************************************************************************
  函数名     :  int EppDownloadDukptKey(BYTE byKeyIndex,  BYTE *pbyBDK, BYTE *pbyKSN,
                    BYTE byKSNLen)
  描述       :  应用密钥下载
  输入参数   :  1、BYTE byKeyIndex ：密钥索引，取值范围[1,32]
                2、BYTE *pbyBDK ：16字节BDK输入，未被发散的BDK
                3、BYTE *pbyKSN ：KSN内容
                4、BYTE byKSNLen ：KSN长度[1,10]
  输出参数   :  无
  返回值     :  EPP_SUCCESS/EPP_OPEN_FILE_ERROR/EPP_SEEK_FILE_ERROR/EPP_READ_FILE_ERROR/
                EPP_CONFIGURE_INVALID/EPP_CONFIGURE_MAC_ERROR/EPP_SEND_CMD_ERROR/
                EPP_RECV_CMD_ERROR/EPP_RECV_RET_ERROR/EPP_RECV_LEN_ERROR/EPP_AUTHEN_FAILED
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2010-12-11  V1.0         创建
****************************************************************************/
/*int EppDownloadDukptKey(BYTE byKeyIndex,  BYTE *pbyBDK, BYTE *pbyKSN,
       BYTE byKSNLen)
{
    int iRet;
    BYTE abyRand[10], abyTek[64], abyInitKey[24], abyCurKsn[16];
    WORD wRecvLen;

    s_EppPortClear();

    s_EppCheckCfg();


    memset(g_EppSendBuff, 0, 1024);
    memcpy(&g_EppSendBuff[5], g_sEppConfigure.abyAppName, 16);
    PedGetRandom(&g_EppSendBuff[37]);
    memcpy(abyRand, &g_EppSendBuff[37], 8);
    iRet = s_EppCrc16SendPacket(40, CMD_DKEY_AUTH_STEP1);
    if (EPP_SUCCESS != iRet)
    {
        return EPP_SEND_CMD_ERROR;
    }
    memset(g_EppRecvBuff, 0x0, 100);
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 3000);
    if (EPP_SUCCESS != iRet)
    {
        return iRet;
    }
    if (0 != g_EppRecvBuff[5])
    {
        return (RET_ERROR_BASE+g_EppRecvBuff[5]);
    }
    if ((72!=wRecvLen) && (48!=wRecvLen))
    {
        return EPP_RECV_LEN_ERROR;
    }
    TDEA(&g_EppRecvBuff[6], &g_EppSendBuff[20], g_sEppConfigure.abyAKDAK, 16, TDEA_DECRYPT);
    if (0 != memcmp(abyRand,&g_EppSendBuff[20],8))
    {
        return EPP_AUTHEN_FAILED;
    }

	if (72 == wRecvLen)
    {
	    s_EppMultiDes(&g_EppRecvBuff[22], 6, abyTek, g_sEppConfigure.abyMDTEK, 24, TDEA_DECRYPT);
    }
	else
    {
	    s_EppMultiDes(&g_EppRecvBuff[22], 3, abyTek, g_sEppConfigure.abyMDTEK, 24, TDEA_DECRYPT);
		memcpy(&abyTek[24], abyTek, 24);
    }

    TDEA(&g_EppRecvBuff[14], &g_EppSendBuff[6], g_sEppConfigure.abyAKDAK, 16, TDEA_ENCRYPT);
    g_EppSendBuff[5] = 0;
    iRet = s_EppCrc16SendPacket(9, CMD_DKEY_AUTH_STEP2);
    if (EPP_SUCCESS != iRet)
    {
        return EPP_SEND_CMD_ERROR;
    }
    memset(g_EppRecvBuff, 0x0, 100);
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 3000);
    if (EPP_SUCCESS != iRet)
    {
        return iRet;
    }
    if (0 != g_EppRecvBuff[5])
    {
        return (RET_ERROR_BASE+g_EppRecvBuff[5]);
    }
    if (8 != wRecvLen)
    {
        return EPP_RECV_LEN_ERROR;
    }

    s_EppLoadKeyInitial(pbyKSN, byKSNLen, pbyBDK, abyInitKey, abyCurKsn);
    memset(g_EppRecvBuff, 0, 1024);
    g_EppRecvBuff[0] = byKeyIndex-1;
    g_EppRecvBuff[1] = 8;  //
    memcpy(&g_EppRecvBuff[2], abyCurKsn, 8);
    s_EppCrc16CCITT(&g_EppRecvBuff[2], 10, &g_EppRecvBuff[12]);
	g_EppRecvBuff[14] = 16;
    memcpy(&g_EppRecvBuff[15], abyInitKey, 16);

    s_EppMultiDes(g_EppRecvBuff, 5, &g_EppSendBuff[5], abyTek, 24, TDEA_ENCRYPT);

    s_EppComputeMacCBC(&g_EppSendBuff[5], 40, &abyTek[24], 24, &g_EppSendBuff[45]);

    iRet = s_EppCrc16SendPacket(44, CMD_DOWNLOAD_DUKPT_KEY);
    if (EPP_SUCCESS != iRet)
    {
        return EPP_SEND_CMD_ERROR;
    }

    memset(g_EppRecvBuff, 0x0, 100);
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 30000);
    if (EPP_SUCCESS != iRet)
    {
        return iRet;
    }
    if (0 != g_EppRecvBuff[5])
    {
        return (RET_ERROR_BASE+g_EppRecvBuff[5]);
    }
    if (8 != wRecvLen)
    {
        return EPP_RECV_LEN_ERROR;
    }
    return EPP_SUCCESS;
}
*/


/****************************************************************************
  函数名     :  int EppGetMac(BYTE byKeyType, BYTE byKeyIndex, const BYTE *pbyMacDataIn,
                        WORD wMacLen, BYTE byMode, BYTE *pbyMacOut)
  描述       :  MAC运算API
  输入参数   :  1、BYTE byKeyType ：密钥类型。KEY_TYPE_MAC/KEY_TYPE_FIXED_MAC/KEY_TYPE_DUKPT
                2、BYTE byKeyIndex ：MAC密钥索引，如果是MAC或者Fixed MAC可取值[1,100]，
                                     如果是DUKPT MAC则可取值[1,32]
                3、const BYTE *pbyMacDataIn ：参与运算的MAC数据
                4、WORD wMacLen ：MAC数据长度，必须是8的整数倍，最小值为8，最大值为2024
                5、BYTE byMode ：运算算法选择
                                     0x00	算法1
                                     0x01	算法2
                                     0x02	EMV2000算法
                                     0x03	中国银联算法
  输出参数   :  1、BYTE *pbyMacOut ：8字节MAC运算结果，如果命令码是DUKPT_MAC_KEY，则前8个字节
                   是MAC计算结果，后10个字节是当前KSN。
  返回值     :  EPP_SUCCESS/EPP_SEND_CMD_ERROR/EPP_RECV_CMD_ERROR/EPP_RECV_RET_ERROR/
                EPP_RECV_LEN_ERROR/EPP_AUTHEN_FAILED/
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2010-12-11  V1.0         创建
****************************************************************************/
// INT32  PedGetMac(UINT16 MacKeyID, UINT8 *DataIn, UINT16 InLen, INT32 Mode, UINT8 *MacOut)
/*int EppGetMac(BYTE byKeyType, BYTE byKeyIndex, const BYTE *pbyMacDataIn,
                        WORD wMacLen, BYTE byMode, BYTE *pbyMacOut)
{
    int iRet, i;
    BYTE abyRand[10], abyTek[64], abyTemp[8];
    WORD wRecvLen;

    s_EppPortClear();
    if ((KEY_TYPE_MAC!=byKeyType) && (KEY_TYPE_FIXED_MAC!=byKeyType) && (KEY_TYPE_DUKPT_MAC!=byKeyType))
    {
        return EPP_INPUT_CMD_ERROR;
    }
    if ((0==byKeyIndex) || (byKeyIndex>100))
    {
        return EPP_INPUT_KEY_INDEX_ERROR;
    }

    if ((0!=(wMacLen%8)) || (0==wMacLen) || (wMacLen>2024))
    {
        return EPP_INPUT_MAC_LEN_ERROR;
    }
    if (byMode > 0x03)
    {
        return EPP_INPUT_MODE_ERROR;
    }
    if ((NULL==pbyMacDataIn) || (NULL==pbyMacOut))
    {
        return EPP_INPUT_PARAM_ERROR;
    }

    s_EppCheckCfg();


    memset(g_EppSendBuff, 0, 1024);
    memcpy(&g_EppSendBuff[5], g_sEppConfigure.abyAppName, 16);
    PedGetRandom(&g_EppSendBuff[37]);
    memcpy(abyRand, &g_EppSendBuff[37], 8);
    iRet = s_EppCrc16SendPacket(40, CMD_UKEY_AUTH_STEP1);
    if (EPP_SUCCESS != iRet)
    {//lcdPrintf("as021"); GetKey();
        return EPP_SEND_CMD_ERROR;
    }
    memset(g_EppRecvBuff, 0x0, 100);
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 3000);
    if (EPP_SUCCESS != iRet)
    {//lcdPrintf("as022"); GetKey();
        return iRet;
    }
    if (0 != g_EppRecvBuff[8])
    {//lcdPrintf("as023"); GetKey();
        return (RET_ERROR_BASE+g_EppRecvBuff[8]);
    }
    if (75 != wRecvLen)
    {//lcdPrintf("as024"); GetKey();
        return EPP_RECV_LEN_ERROR;
    }
    TDEA(&g_EppRecvBuff[9], &g_EppSendBuff[20], g_sEppConfigure.abyAKUAK, 16, TDEA_DECRYPT);
    if (0 != memcmp(abyRand,&g_EppSendBuff[20],8))
    {//lcdPrintf("as025"); GetKey();
        return EPP_AUTHEN_FAILED;
    }

	s_EppMultiDes(&g_EppRecvBuff[25], 6, abyTek, g_sEppConfigure.abyMUTEK, 24, TDEA_DECRYPT);

    TDEA(&g_EppRecvBuff[17], &g_EppSendBuff[9], g_sEppConfigure.abyAKUAK, 16, TDEA_ENCRYPT);
    g_EppSendBuff[5] = 0;
    g_EppSendBuff[6] = 0;
    g_EppSendBuff[7] = RET_MODULE_PED;
    g_EppSendBuff[8] = 0;
    iRet = s_EppCrc16SendPacket(12, CMD_UKEY_AUTH_STEP2);
    if (EPP_SUCCESS != iRet)
    {//lcdPrintf("as026"); GetKey();
        return EPP_SEND_CMD_ERROR;
    }
    memset(g_EppRecvBuff, 0x0, 100);
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 3000);
    if (EPP_SUCCESS != iRet)
    {//lcdPrintf("as027=%d,", iRet); GetKey();
        return iRet;
    }
    if (0 != g_EppRecvBuff[8])
    {//lcdPrintf("as028"); GetKey();
        return (RET_ERROR_BASE+g_EppRecvBuff[8]);
    }
    if (11 != wRecvLen)
    {//lcdPrintf("as029"); GetKey();
        return EPP_RECV_LEN_ERROR;
    }

    memset(g_EppRecvBuff, 0, 1024);

    g_EppRecvBuff[0] = byKeyIndex;
    g_EppRecvBuff[1] = byMode;  //abyTemp

    // 20110412 0935 黄俊斌 增加，为了提速  begin
    if ((MAC_MODE_2==byMode) || (MAC_MODE_CUP==byMode))
    {
        memset(abyTemp, 0, 8);
        for (i=0; i<wMacLen; i++)
        {
            abyTemp[i&0x07] ^= pbyMacDataIn[i];
        }
        memcpy(&g_EppRecvBuff[2], abyTemp, 8);
        wMacLen = 8;
    }
    else
    {
    // 20110412 0935 黄俊斌 增加  end

        memcpy(&g_EppRecvBuff[2], pbyMacDataIn, wMacLen);
    }
//    memset(&g_EppRecvBuff[wMacLen+2], 0x33, 8);
    PedGetRandom(&g_EppRecvBuff[wMacLen+2]);
    s_EppMultiDes(g_EppRecvBuff, (wMacLen+8)/8, &g_EppSendBuff[5], abyTek, 24, TDEA_ENCRYPT);
    s_EppComputeMacCBC(&g_EppSendBuff[5], wMacLen+8, &abyTek[24], 24, &g_EppSendBuff[wMacLen+13]);
    switch (byKeyType)
    {
    case KEY_TYPE_MAC:
        iRet = s_EppCrc16SendPacket(wMacLen+12, CMD_GET_MAC);
        break;
    case KEY_TYPE_FIXED_MAC:
        iRet = s_EppCrc16SendPacket(wMacLen+12, CMD_GET_FIXED_MAC);
        break;
    case KEY_TYPE_DUKPT_MAC:
        iRet = s_EppCrc16SendPacket(wMacLen+12, CMD_GET_DUKPT_MAC);
        break;
    }
    if (EPP_SUCCESS != iRet)
    {//lcdPrintf("as02a"); GetKey();
        return EPP_SEND_CMD_ERROR;
    }

    memset(g_EppRecvBuff, 0x0, 100);
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 30000);
    if (EPP_SUCCESS != iRet)
    {//lcdPrintf("as02b"); GetKey();
        return iRet;
    }
    if (KEY_TYPE_DUKPT_MAC == byKeyType)
    {
        if (35 != wRecvLen)
        {//lcdPrintf("as02f"); GetKey();
            return EPP_RECV_LEN_ERROR;
        }
        s_EppComputeMacCBC(&g_EppRecvBuff[5], 24, &abyTek[24], 24, &g_EppSendBuff[200]);
        if (0 != memcmp(&g_EppSendBuff[200],&g_EppRecvBuff[29],4))
        {//lcdPrintf("as02g"); GetKey();
            return EPP_MAC_CHECK_ERROR;
        }
        s_EppMultiDes(&g_EppRecvBuff[5], 3, &g_EppSendBuff[200], abyTek, 24, TDEA_DECRYPT);
        if (0 != g_EppSendBuff[203])
        {//lcdPrintf("as02h"); GetKey();
            return (RET_ERROR_BASE+g_EppSendBuff[203]);
        }
        memcpy(pbyMacOut, &g_EppSendBuff[204], 18);
        return EPP_SUCCESS;
    }
    else
    {
        if (27 != wRecvLen)
        {//lcdPrintf("as02c=%d,%d,", wRecvLen, g_EppRecvBuff[5]); GetKey();
            return EPP_RECV_LEN_ERROR;
        }
        s_EppComputeMacCBC(&g_EppRecvBuff[5], 16, &abyTek[24], 24, &g_EppSendBuff[200]);
        if (0 != memcmp(&g_EppSendBuff[200],&g_EppRecvBuff[21],4))
        {//lcdPrintf("as02d"); GetKey();
            return EPP_MAC_CHECK_ERROR;
        }
        s_EppMultiDes(&g_EppRecvBuff[5], 2, &g_EppSendBuff[200], abyTek, 24, TDEA_DECRYPT);
        if (0 != g_EppSendBuff[203])
        {//lcdPrintf("as02e=%02x",g_EppSendBuff[203]); GetKey();
            return (RET_ERROR_BASE+g_EppSendBuff[203]);
        }
        memcpy(pbyMacOut, &g_EppSendBuff[204], 8);
        return EPP_SUCCESS;
    }
}
*/
/*#ifdef   TEST_VERSION
int EppGetMacExt(BYTE byKeyType, BYTE byKeyIndex, const BYTE *pbyMacDataIn,
                        WORD wMacLen, BYTE byMode, BYTE *pbyMacOut, DWORD *pdwOut)
{
    int iRet;
    BYTE abyRand[10], abyTek[64];
    WORD wRecvLen;

    s_EppPortClear();

    sysTimerSet(1, 20000);
    if ((KEY_TYPE_MAC!=byKeyType) && (KEY_TYPE_FIXED_MAC!=byKeyType) && (KEY_TYPE_DUKPT_MAC!=byKeyType))
    {
        return EPP_INPUT_CMD_ERROR;
    }
    if ((0==byKeyIndex) || (byKeyIndex>100))
    {
        return EPP_INPUT_KEY_INDEX_ERROR;
    }
    if ((0!=(wMacLen%8)) || (0==wMacLen) || (wMacLen>2024))
    {
        return EPP_INPUT_MAC_LEN_ERROR;
    }
    if (byMode > 0x03)
    {
        return EPP_INPUT_MODE_ERROR;
    }
    if ((NULL==pbyMacDataIn) || (NULL==pbyMacOut))
    {
        return EPP_INPUT_PARAM_ERROR;
    }

    s_EppCheckCfg();


    memset(g_EppSendBuff, 0, 1024);
    memcpy(&g_EppSendBuff[5], g_sEppConfigure.abyAppName, 16);
    PedGetRandom(&g_EppSendBuff[37]);
    memcpy(abyRand, &g_EppSendBuff[37], 8);
    iRet = s_EppCrc16SendPacket(40, CMD_UKEY_AUTH_STEP1);
    if (EPP_SUCCESS != iRet)
    {//lcdPrintf("as021"); GetKeyBlockMs(12000);
        return EPP_SEND_CMD_ERROR;
    }
    memset(g_EppRecvBuff, 0x0, 100);
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 3000);
    if (EPP_SUCCESS != iRet)
    {//lcdPrintf("as022=%x,", iRet); GetKeyBlockMs(12000);
        return iRet;
    }
    if (0 != g_EppRecvBuff[8])
    {//lcdPrintf("as023=%x,",g_EppRecvBuff[5]); GetKeyBlockMs(12000);
        return (RET_ERROR_BASE+g_EppRecvBuff[8]);
    }
    if (75 != wRecvLen)
    {//lcdPrintf("as024"); GetKeyBlockMs(12000);
        return EPP_RECV_LEN_ERROR;
    }
    TDEA(&g_EppRecvBuff[9], &g_EppSendBuff[20], g_sEppConfigure.abyAKUAK, 16, TDEA_DECRYPT);
    if (0 != memcmp(abyRand,&g_EppSendBuff[20],8))
    {//lcdPrintf("as025"); GetKeyBlockMs(12000);
        return EPP_AUTHEN_FAILED;
    }


    s_EppMultiDes(&g_EppRecvBuff[25], 6, abyTek, g_sEppConfigure.abyMUTEK, 24, TDEA_DECRYPT);


    TDEA(&g_EppRecvBuff[17], &g_EppSendBuff[9], g_sEppConfigure.abyAKUAK, 16, TDEA_ENCRYPT);
    g_EppSendBuff[5] = 0;
    g_EppSendBuff[6] = 0;
    g_EppSendBuff[7] = RET_MODULE_PED;
    g_EppSendBuff[8] = 0;
    iRet = s_EppCrc16SendPacket(12, CMD_UKEY_AUTH_STEP2);
    if (EPP_SUCCESS != iRet)
    {//lcdPrintf("as026"); GetKeyBlockMs(12000);
        return EPP_SEND_CMD_ERROR;
    }
    memset(g_EppRecvBuff, 0x0, 100);
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 3000);
    if (EPP_SUCCESS != iRet)
    {//lcdPrintf("as027=%d,", iRet); GetKeyBlockMs(12000);
        return iRet;
    }
    if (0 != g_EppRecvBuff[8])
    {//lcdPrintf("as028"); GetKeyBlockMs(12000);
        return (RET_ERROR_BASE+g_EppRecvBuff[8]);
    }
    if (11 != wRecvLen)
    {//lcdPrintf("as029"); GetKeyBlockMs(12000);
        return EPP_RECV_LEN_ERROR;
    }

    pdwOut[0] = 20000 - sysTimerCheck(1);
    sysTimerSet(1, 20000);

    memset(g_EppRecvBuff, 0, 1024);

    g_EppRecvBuff[0] = byKeyIndex;
    g_EppRecvBuff[1] = byMode;  //
    memcpy(&g_EppRecvBuff[2], pbyMacDataIn, wMacLen);
//    memset(&g_EppRecvBuff[wMacLen+2], 0x33, 8);
    PedGetRandom(&g_EppRecvBuff[wMacLen+2]);
    s_EppMultiDes(g_EppRecvBuff, (wMacLen+8)/8, &g_EppSendBuff[5], abyTek, 24, TDEA_ENCRYPT);
    s_EppComputeMacCBC(&g_EppSendBuff[5], wMacLen+8, &abyTek[24], 24, &g_EppSendBuff[wMacLen+13]);
    switch (byKeyType)
    {
    case KEY_TYPE_MAC:
        iRet = s_EppCrc16SendPacket(wMacLen+12, CMD_GET_MAC);
        break;
    case KEY_TYPE_FIXED_MAC:
        iRet = s_EppCrc16SendPacket(wMacLen+12, CMD_GET_FIXED_MAC);
        break;
    case KEY_TYPE_DUKPT_MAC:
        iRet = s_EppCrc16SendPacket(wMacLen+12, CMD_GET_DUKPT_MAC);
        break;
    }
    if (EPP_SUCCESS != iRet)
    {//lcdPrintf("as02a"); GetKeyBlockMs(12000);
        return EPP_SEND_CMD_ERROR;
    }

    memset(g_EppRecvBuff, 0x0, 100);
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 30000);
    if (EPP_SUCCESS != iRet)
    {//lcdPrintf("as02b"); GetKeyBlockMs(12000);
        return iRet;
    }
    if (KEY_TYPE_DUKPT_MAC == byKeyType)
    {
        if (35 != wRecvLen)
        {//lcdPrintf("as02f"); GetKeyBlockMs(12000);
            return EPP_RECV_LEN_ERROR;
        }
        s_EppComputeMacCBC(&g_EppRecvBuff[5], 24, &abyTek[24], 24, &g_EppSendBuff[200]);
        if (0 != memcmp(&g_EppSendBuff[200],&g_EppRecvBuff[29],4))
        {//lcdPrintf("as02g"); GetKeyBlockMs(12000);
            return EPP_MAC_CHECK_ERROR;
        }
        s_EppMultiDes(&g_EppRecvBuff[5], 3, &g_EppSendBuff[200], abyTek, 24, TDEA_DECRYPT);
        if (0 != g_EppSendBuff[203])
        {//lcdPrintf("as02h=%02x,",g_EppSendBuff[200]); GetKeyBlockMs(12000);
            return (RET_ERROR_BASE+g_EppSendBuff[203]);
        }
        memcpy(pbyMacOut, &g_EppSendBuff[204], 18);
        pdwOut[1] = 20000 - sysTimerCheck(1);
        return EPP_SUCCESS;
    }
    else
    {
        if (27 != wRecvLen)
        {//lcdPrintf("as02c=%d,%d,", wRecvLen, g_EppRecvBuff[5]); GetKeyBlockMs(12000);
            return EPP_RECV_LEN_ERROR;
        }
        s_EppComputeMacCBC(&g_EppRecvBuff[5], 16, &abyTek[24], 24, &g_EppSendBuff[200]);
        if (0 != memcmp(&g_EppSendBuff[200],&g_EppRecvBuff[21],4))
        {//lcdPrintf("as02d"); GetKeyBlockMs(12000);
            return EPP_MAC_CHECK_ERROR;
        }
        s_EppMultiDes(&g_EppRecvBuff[5], 2, &g_EppSendBuff[200], abyTek, 24, TDEA_DECRYPT);
        if (0 != g_EppSendBuff[203])
        {//lcdPrintf("as02e=%02x",g_EppSendBuff[203]); GetKeyBlockMs(12000);
            return (RET_ERROR_BASE+g_EppSendBuff[203]);
        }
        memcpy(pbyMacOut, &g_EppSendBuff[204], 8);
        pdwOut[1] = 20000 - sysTimerCheck(1);
        return EPP_SUCCESS;
    }
}
#endif
*/
/****************************************************************************
  函数名     :  int EppGetPin(BYTE byKeyType, BYTE byKeyIndex, BYTE byDisplayMode,
            BYTE byMode, const BYTE *pbyCardInfo, const BYTE *pbyLenInfo, BYTE *pbyPinBlockOut)
  描述       :  MAC运算API
  输入参数   :  1、BYTE byKeyType ：密钥类型。KEY_TYPE_PIN/KEY_TYPE_FIXED_PIN/KEY_TYPE_DUKPT
                2、BYTE byKeyIndex ：密钥索引，如果是PIN或者Fixed PIN可取值[1,100]，
                                     如果是DUKPT PIN则可取值[1,32]
                3、BYTE byDisplayMode ：PIN显示模式，0表示从左到右，1表示从右到左，2表示居中。
                4、BYTE byMode ：运算算法选择，
                                        0x00表示选择“Format 0模式”算法，
                                        0x01表示选择“Format EMV模式”算法，
                                        0x0a表示选择“Format EPS模式”算法，
                5、const BYTE *pbyCardInfo ：16卡号信息，缓冲长度必须不小于16字节
                6、const BYTE *pbyLenInfo ：允许输入的PIN密钥长度枚举值字符串，长度可取值0,4~12，分
                            别用字符'0'，'4'，……，'9'，'a'，'b'，'c'表示。例如填入数据
                            "0456a\0\0\0\0\0\0\0\0"，则表示支持长度为0、4、5、6和10的PIN输入。
                            缓冲长度不小于13字节。
  输出参数   :  1、BYTE *pbyPinBlockOut ：缓冲长度最小为19字节，前8字节是PIN运算结果；第九个
                   字节表示用户是否输入PIN，如果是0则表示没输入PIN，1表示有输入PIN；
                   如果是DUKPT，则接下来9~18字节是当前KSN
  返回值     :  EPP_SUCCESS/EPP_SEND_CMD_ERROR/EPP_RECV_CMD_ERROR/EPP_RECV_RET_ERROR/
                EPP_RECV_LEN_ERROR/EPP_AUTHEN_FAILED/0x0d:No PIN
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2010-12-11  V1.0         创建
****************************************************************************/
// INT32  PedGetPin(UINT16 PinKeyID, UINT8 *ExpectPinLen, UINT8 *CardNo, INT32 Mode, UINT8 *PinBlock)
/*int EppGetPin(BYTE byKeyType, BYTE byKeyIndex, BYTE byDisplayMode,
            BYTE byMode, const BYTE *pbyCardInfo, const BYTE *pbyLenInfo, BYTE *pbyPinBlockOut)
{
    int iRet;
    BYTE abyRand[10], abyTek[64];
    WORD wRecvLen, wCmd = 0;

    s_EppPortClear();

    if ((KEY_TYPE_PIN!=byKeyType) && (KEY_TYPE_FIXED_PIN!=byKeyType) && (KEY_TYPE_DUKPT_PIN!=byKeyType))
    {
        return EPP_INPUT_CMD_ERROR;
    }
    if ((0==byKeyIndex) || (byKeyIndex>100))
    {
        return EPP_INPUT_KEY_INDEX_ERROR;
    }

    if ((0x00!=byMode) && (0x0a!=byMode) && (0x01!=byMode))
    {//lcdPrintf("cs00o   "); GetKey();
        return EPP_INPUT_MODE_ERROR;
    }
    if ((NULL==pbyCardInfo) || (NULL==pbyLenInfo) || (NULL==pbyPinBlockOut))
    {//lcdPrintf("cs00n   "); GetKey();
        return EPP_INPUT_PARAM_ERROR;
    }
    if ((pbyLenInfo[0]<'0') || ((pbyLenInfo[0]>'9')&&(pbyLenInfo[0]<'a')) || (pbyLenInfo[0]>'c'))
    {//lcdPrintf("cs00n   "); GetKey();
        return EPP_INPUT_PARAM_ERROR;
    }

    s_EppCheckCfg();


    memset(g_EppSendBuff, 0, 1024);
    memcpy(&g_EppSendBuff[5], g_sEppConfigure.abyAppName, 16);
    PedGetRandom(&g_EppSendBuff[37]);
    memcpy(abyRand, &g_EppSendBuff[37], 8);
    iRet = s_EppCrc16SendPacket(40, CMD_UKEY_AUTH_STEP1);
    if (EPP_SUCCESS != iRet)
    {//lcdPrintf("cs00l   "); GetKey();
        return EPP_SEND_CMD_ERROR;
    }
    memset(g_EppRecvBuff, 0x0, 100);
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 3000);
    if (EPP_SUCCESS != iRet)
    {//lcdPrintf("cs00k   "); GetKey();
        return iRet;
    }
    if (0 != g_EppRecvBuff[8])
    {//lcdPrintf("cs00j   "); GetKey();
        return (RET_ERROR_BASE+g_EppRecvBuff[8]);
    }
    if (75 != wRecvLen)
    {//lcdPrintf("cs00i   "); GetKey();
        return EPP_RECV_LEN_ERROR;
    }
    TDEA(&g_EppRecvBuff[9], &g_EppSendBuff[20], g_sEppConfigure.abyAKUAK, 16, TDEA_DECRYPT);
    if (0 != memcmp(abyRand,&g_EppSendBuff[20],8))
    {//lcdPrintf("cs00f   "); GetKey();
        return EPP_AUTHEN_FAILED;
    }

    s_EppMultiDes(&g_EppRecvBuff[25], 6, abyTek, g_sEppConfigure.abyMUTEK, 24, TDEA_DECRYPT);

    TDEA(&g_EppRecvBuff[17], &g_EppSendBuff[9], g_sEppConfigure.abyAKUAK, 16, TDEA_ENCRYPT);
    g_EppSendBuff[5] = 0;
    g_EppSendBuff[6] = 0;
    g_EppSendBuff[7] = RET_MODULE_PED;
    g_EppSendBuff[8] = 0;
    iRet = s_EppCrc16SendPacket(12, CMD_UKEY_AUTH_STEP2);
    if (EPP_SUCCESS != iRet)
    {//lcdPrintf("cs00e   "); GetKey();
        return EPP_SEND_CMD_ERROR;
    }
    memset(g_EppRecvBuff, 0x0, 100);
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 3000);
    if (EPP_SUCCESS != iRet)
    {//lcdPrintf("cs00c   "); GetKey();
        return iRet;
    }
    if (0 != g_EppRecvBuff[8])
    {//lcdPrintf("cs00d=%d   ", g_EppRecvBuff[5]); GetKey();
        return (RET_ERROR_BASE+g_EppRecvBuff[8]);
    }
    if (11 != wRecvLen)
    {//lcdPrintf("cs00b   "); GetKey();
        return EPP_RECV_LEN_ERROR;
    }

    memset(g_EppRecvBuff, 0, 1024);
    g_EppRecvBuff[0] = byKeyIndex;
    g_EppRecvBuff[1] = byMode;  //
    memcpy(&g_EppRecvBuff[2], pbyCardInfo, 16);
    memcpy(&g_EppRecvBuff[18], pbyLenInfo, 13);
    g_EppRecvBuff[31] = byDisplayMode;
    PedGetRandom(&g_EppRecvBuff[32]);
    s_EppMultiDes(g_EppRecvBuff, 5, &g_EppSendBuff[5], abyTek, 24, TDEA_ENCRYPT);
    s_EppComputeMacCBC(&g_EppSendBuff[5], 40, &abyTek[24], 24, &g_EppSendBuff[45]);

    switch (byKeyType)
    {
    case KEY_TYPE_PIN:
        wCmd = CMD_GET_PIN;
        break;
    case KEY_TYPE_FIXED_PIN:
        wCmd = CMD_GET_FIXED_PIN;
        break;
    case KEY_TYPE_DUKPT_PIN:
        wCmd = CMD_GET_DUKPT_PIN;
        break;
    }
    iRet = s_EppCrc16SendPacket(44, wCmd);
    if (EPP_SUCCESS != iRet)
    {//lcdPrintf("cs00a   "); GetKey();
        return EPP_SEND_CMD_ERROR;
    }

    memset(g_EppRecvBuff, 0x0, 100);
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 300000);
    if (EPP_SUCCESS != iRet)
    {//lcdPrintf("cs008   "); GetKey();
        return iRet;
    }

    if (KEY_TYPE_DUKPT_PIN == byKeyType)
    {
        if (35 != wRecvLen)
        {//lcdPrintf("cs008   "); GetKey();
            return EPP_RECV_LEN_ERROR;
        }
        s_EppComputeMacCBC(&g_EppRecvBuff[5], 24, &abyTek[24], 24, &g_EppSendBuff[200]);
        if (0 != memcmp(&g_EppSendBuff[200],&g_EppRecvBuff[29],4))
        {//lcdPrintf("cs007   "); GetKey();
            return EPP_MAC_CHECK_ERROR;
        }
        s_EppMultiDes(&g_EppRecvBuff[5], 3, &g_EppSendBuff[200], abyTek, 24, TDEA_DECRYPT);
        if (0 != g_EppSendBuff[203])
        {//lcdPrintf("cs006=%d,", g_EppSendBuff[200]); GetKey();
            return (RET_ERROR_BASE+g_EppSendBuff[203]);
        }
        if (0 == g_EppSendBuff[212])
        {//lcdPrintf("cs005   "); GetKey();
            return EPP_NO_PIN;
        }
        memcpy(pbyPinBlockOut, &g_EppSendBuff[204], 8);
        memcpy(&pbyPinBlockOut[8], &g_EppSendBuff[213], 10);
    }
    else
    {
        if (27 != wRecvLen)
        {//lcdPrintf("cs004   "); GetKey();
            return EPP_RECV_LEN_ERROR;
        }
        s_EppComputeMacCBC(&g_EppRecvBuff[5], 16, &abyTek[24], 24, &g_EppSendBuff[200]);
        if (0 != memcmp(&g_EppSendBuff[200],&g_EppRecvBuff[21],4))
        {//lcdPrintf("cs003   "); GetKey();
            return EPP_MAC_CHECK_ERROR;
        }
        s_EppMultiDes(&g_EppRecvBuff[5], 2, &g_EppSendBuff[200], abyTek, 24, TDEA_DECRYPT);
        if (0 != g_EppSendBuff[203])
        {//lcdPrintf("cs002=%d,   ", g_EppSendBuff[200]); GetKey();
            return (RET_ERROR_BASE+g_EppSendBuff[203]);
        }
        if (0 == g_EppSendBuff[212])
        {//lcdPrintf("cs001   "); GetKey();
            return EPP_NO_PIN;
        }
        memcpy(pbyPinBlockOut, &g_EppSendBuff[204], 8);
    }
    return EPP_SUCCESS;
}
*/
/****************************************************************************
  函数名     :  int EppGetTdea(WORD wMode, BYTE byKeyType, BYTE  byKeyIndex, WORD wDataLen,
                   const BYTE *pbyDataIn, BYTE *pbyDataOut)
  描述       :  TDEA运算API
  输入参数   :  1、WORD wMode  ：算法模式
                                TDEA_ENCRYPT  加密运算
                                TDEA_DECRYPT  解密运算
                                TDEA_MODE_ECB---使用ECB模式
                                TDEA_MODE_CBC---使用CBC模式
                                请参考8110的SDK宏定义
                2、BYTE byKeyType ：密钥类型。
                                0x01：主密钥；
                                0x02：MAC密钥；
                                0x03：PIN密钥；
                                0x04：DUKPT MAC密钥；暂不支持
                                0x05：DUKPT PIN密钥；暂不支持
                                0x10：Fixed MAC密钥；
                                0x11：Fixed PIN密钥；
                3、BYTE byKeyIndex ：密钥索引，可取值[1,100]
                4、WORD wDataLen ：数据长度，必须是8的整数倍，最小值为8，最大值为96
                5、const BYTE *pbyDataIn ：参与运算的数据
  输出参数   :  1、BYTE *pbyDataOut ：运算结果
  返回值     :  EPP_SUCCESS/EPP_SEND_CMD_ERROR/EPP_RECV_CMD_ERROR/EPP_RECV_RET_ERROR/
                EPP_RECV_LEN_ERROR/EPP_AUTHEN_FAILED/
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2010-12-11  V1.0         创建
****************************************************************************/
/*int EppGetTdea(WORD wMode, BYTE byKeyType, BYTE  byKeyIndex, WORD wDataLen,
                   const BYTE *pbyDataIn, BYTE *pbyDataOut)
{
    int iRet;
    BYTE abyRand[10], abyTek[64];
    WORD wRecvLen;

    s_EppPortClear();
    if (   (KEY_TYPE_MAC!=byKeyType)       && (KEY_TYPE_PIN!=byKeyType)
        && (KEY_TYPE_FIXED_MAC!=byKeyType) && (KEY_TYPE_FIXED_PIN!=byKeyType)
//        && (KEY_TYPE_DUKPT_MAC!=byKeyType) && (KEY_TYPE_DUKPT_PIN!=byKeyType)
        && (KEY_TYPE_MASTER!=byKeyType))
    {
        return EPP_INPUT_CMD_ERROR;
    }
    if ((0==byKeyIndex) || (byKeyIndex>100))
    {
        return EPP_INPUT_KEY_INDEX_ERROR;
    }

    if ((0!=(wDataLen%8)) || (0==wDataLen) || (wDataLen>96))
    {
        return EPP_INPUT_DATA_LEN_ERROR;
    }

    s_EppCheckCfg();

    memset(g_EppSendBuff, 0, 1024);
    memcpy(&g_EppSendBuff[5], g_sEppConfigure.abyAppName, 16);
    PedGetRandom(&g_EppSendBuff[37]);
    memcpy(abyRand, &g_EppSendBuff[37], 8);
    iRet = s_EppCrc16SendPacket(40, CMD_UKEY_AUTH_STEP1);
    if (EPP_SUCCESS != iRet)
    {//lcdPrintf("as121"); GetKey();
        return EPP_SEND_CMD_ERROR;
    }
    memset(g_EppRecvBuff, 0x0, 100);
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 3000);
    if (EPP_SUCCESS != iRet)
    {//lcdPrintf("as122"); GetKey();
        return iRet;
    }
    if (0 != g_EppRecvBuff[8])
    {//lcdPrintf("as123"); GetKey();
        return (RET_ERROR_BASE+g_EppRecvBuff[8]);
    }
    if (75 != wRecvLen)
    {//lcdPrintf("as124"); GetKey();
        return EPP_RECV_LEN_ERROR;
    }
    TDEA(&g_EppRecvBuff[9], &g_EppSendBuff[20], g_sEppConfigure.abyAKUAK, 16, TDEA_DECRYPT);
    if (0 != memcmp(abyRand,&g_EppSendBuff[20],8))
    {//lcdPrintf("as125"); GetKey();
        return EPP_AUTHEN_FAILED;
    }

	s_EppMultiDes(&g_EppRecvBuff[25], 6, abyTek, g_sEppConfigure.abyMUTEK, 24, TDEA_DECRYPT);

    TDEA(&g_EppRecvBuff[17], &g_EppSendBuff[9], g_sEppConfigure.abyAKUAK, 16, TDEA_ENCRYPT);
    g_EppSendBuff[5] = 0;
    g_EppSendBuff[6] = 0;
    g_EppSendBuff[7] = RET_MODULE_PED;
    g_EppSendBuff[8] = 0;
    iRet = s_EppCrc16SendPacket(12, CMD_UKEY_AUTH_STEP2);
    if (EPP_SUCCESS != iRet)
    {//lcdPrintf("as126"); GetKey();
        return EPP_SEND_CMD_ERROR;
    }
    memset(g_EppRecvBuff, 0x0, 100);
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 3000);
    if (EPP_SUCCESS != iRet)
    {//lcdPrintf("as127=%d,", iRet); GetKey();
        return iRet;
    }
    if (0 != g_EppRecvBuff[8])
    {//lcdPrintf("as128"); GetKey();
        return (RET_ERROR_BASE+g_EppRecvBuff[8]);
    }
    if (11 != wRecvLen)
    {//lcdPrintf("as129"); GetKey();
        return EPP_RECV_LEN_ERROR;
    }

    memset(g_EppRecvBuff, 0, 1024);

    g_EppRecvBuff[0] = byKeyType;
    g_EppRecvBuff[1] = wMode>>8;
    g_EppRecvBuff[2] = wMode;
    g_EppRecvBuff[3] = byKeyIndex;
    g_EppRecvBuff[4] = 0;
    memcpy(&g_EppRecvBuff[5], pbyDataIn, wDataLen);
//    memset(&g_EppRecvBuff[5+wDataLen], 0x33, 8);
    PedGetRandom(&g_EppRecvBuff[5+wDataLen]);
    s_EppMultiDes(g_EppRecvBuff, (wDataLen+8)/8, &g_EppSendBuff[5], abyTek, 24, TDEA_ENCRYPT);
    s_EppComputeMacCBC(&g_EppSendBuff[5], wDataLen+8, &abyTek[24], 24, &g_EppSendBuff[wDataLen+13]);

    iRet = s_EppCrc16SendPacket(wDataLen+12, CMD_GET_TDEA);
    if (EPP_SUCCESS != iRet)
    {//lcdPrintf("as12a"); GetKey();
        return EPP_SEND_CMD_ERROR;
    }

    memset(g_EppRecvBuff, 0x0, 100);
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 30000);
    if (EPP_SUCCESS != iRet)
    {//lcdPrintf("as12b"); GetKey();
        return iRet;
    }

    s_EppComputeMacCBC(&g_EppRecvBuff[5], wRecvLen-11, &abyTek[24], 24, &g_EppSendBuff[200]);
    if (0 != memcmp(&g_EppSendBuff[200],&g_EppRecvBuff[wRecvLen-6],4))
    {//lcdPrintf("as12d"); GetKey();
        return EPP_MAC_CHECK_ERROR;
    }
    s_EppMultiDes(&g_EppRecvBuff[5], (wRecvLen-11)/8, &g_EppSendBuff[200], abyTek, 24, TDEA_DECRYPT);
    if (0 != g_EppSendBuff[203])
    {//lcdPrintf("as12e=%02x, %d,",g_EppSendBuff[203], wRecvLen); GetKey();
        return (RET_ERROR_BASE+g_EppSendBuff[203]);
    }
    memcpy(pbyDataOut, &g_EppSendBuff[204], wDataLen);
    return EPP_SUCCESS;
}
*/
/****************************************************************************
  函数名     :  int EppSetPinInputTimeout(WORD wTimeout100Ms)
  描述       :  设置PIN输入超时时间长度
  输入参数   :  1、WORD wTimeout100Ms : 超时时间，单位100毫秒，最大值1200（120秒）。
                   如果取值0，则表示使用默认值120秒。
  输出参数   :  无
  返回值     :
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2010-12-11  V1.0         创建
****************************************************************************/
int EppSetPinInputTimeout(WORD wTimeout100Ms)
{
    int iRet;
    WORD wRecvLen;

    s_EppPortClear();
    if (wTimeout100Ms > 1200)
    {
        return EPP_INPUT_PARAM_ERROR;
    }

    g_EppSendBuff[5] = (BYTE)(wTimeout100Ms>>8);
    g_EppSendBuff[6] = (BYTE)(wTimeout100Ms);

    iRet = s_EppCrc16SendPacket(2, CMD_SET_PIN_TIMEOUT);
    if (EPP_SUCCESS != iRet)
    {
        return EPP_SEND_CMD_ERROR;
    }
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 1000);
    if (EPP_SUCCESS != iRet)
    {
        return iRet;
    }
    if (0 != g_EppRecvBuff[8])
    {
        return (RET_ERROR_BASE+g_EppRecvBuff[8]);
    }
    if (11 != wRecvLen)
    {
        return EPP_RECV_LEN_ERROR;
    }
    return EPP_SUCCESS;
}

/****************************************************************************
  函数名     :  int EppSetTdesIv(const BYTE *pbyIvData)
  描述       :  设置Tdes初始向量，该向量只对CBC算法有效
  输入参数   :  1、const BYTE *pbyIvData : 初始向量，8字节。
  输出参数   :  无
  返回值     :
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2010-12-11  V1.0         创建
****************************************************************************/
int EppSetTdesIv(const BYTE *pbyIvData)
{
    int iRet;
    WORD wRecvLen;

    s_EppPortClear();
    memcpy(&g_EppSendBuff[5], pbyIvData, 8);
    iRet = s_EppCrc16SendPacket(8, CMD_SET_TDES_IV);
    if (EPP_SUCCESS != iRet)
    {
        return EPP_SEND_CMD_ERROR;
    }
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 1000);
    if (EPP_SUCCESS != iRet)
    {
        return iRet;
    }
    if (0 != g_EppRecvBuff[8])
    {
        return (RET_ERROR_BASE+g_EppRecvBuff[8]);
    }
    if (11 != wRecvLen)
    {
        return EPP_RECV_LEN_ERROR;
    }
    return EPP_SUCCESS;
}

/****************************************************************************
  函数名     :  int EppGetRand(BYTE *pbyRandOut)
  描述       :  获取随机数
  输入参数   :  无
  输出参数   :  1、BYTE *pbyRandOut ：随机数，8字节
  返回值     :
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2010-12-11  V1.0         创建
****************************************************************************/
int EppGetRand(BYTE *pbyRandOut)
{
    int iRet;
    WORD wRecvLen;
    s_EppPortClear();

    if (NULL == pbyRandOut)
    {
        return EPP_INPUT_PARAM_ERROR;
    }

    iRet = s_EppCrc16SendPacket(0, CMD_GET_RANDOM_NUMBER);
    if (EPP_SUCCESS != iRet)
    {
        return EPP_SEND_CMD_ERROR;
    }
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 3000);
    if (EPP_SUCCESS != iRet)
    {
        return iRet;
    }
    if (0 != g_EppRecvBuff[8])
    {
        return (RET_ERROR_BASE+g_EppRecvBuff[8]);
    }
    if (19 != wRecvLen)
    {
        return EPP_RECV_RET_ERROR;
    }
    memcpy(pbyRandOut, &g_EppRecvBuff[9], 8);
    return EPP_SUCCESS;
}

/****************************************************************************
  函数名     :  int EppClearAKey(BYTE byKeyType, BYTE byKeyIndex)
  描述       :  清除某个密钥
  输入参数   :  1、BYTE byKeyType ：密钥类型
                                0x01：主密钥；
                                0x02：MAC密钥；
                                0x03：PIN密钥；
                                0x10：Fixed MAC密钥；
                                0x11：Fixed PIN密钥；
                2、BYTE byKeyIndex ：密钥索引
  输出参数   :  无
  返回值     :
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2010-12-11  V1.0         创建
****************************************************************************/
int EppClearAKey(BYTE byKeyType, BYTE byKeyIndex)
{
    int iRet;
    WORD wRecvLen;

    if (   (KEY_TYPE_MAC!=byKeyType)       && (KEY_TYPE_PIN!=byKeyType)
        && (KEY_TYPE_FIXED_MAC!=byKeyType) && (KEY_TYPE_FIXED_PIN!=byKeyType)
//        && (KEY_TYPE_DUKPT_MAC!=byKeyType) && (KEY_TYPE_DUKPT_PIN!=byKeyType)
        && (KEY_TYPE_MASTER!=byKeyType))
    {
        return EPP_INPUT_CMD_ERROR;
    }
    if ((byKeyIndex>KEY_INDEX_NUMBER)||(0==byKeyIndex))
    {
        return EPP_KEY_INDEX_ERROR;
    }

    s_EppPortClear();
    memset(g_EppSendBuff, 0, 1024);
    g_sEppConfigure.abyAppName[31] = 0;
    strcpy((char*)(&g_EppSendBuff[5]), (char*)g_sEppConfigure.abyAppName);
    switch (byKeyType)
    {
    case KEY_TYPE_MAC:
        g_EppSendBuff[37] = 1;
        break;
    case KEY_TYPE_PIN:
        g_EppSendBuff[37] = 2;
        break;
    case KEY_TYPE_FIXED_MAC:
        g_EppSendBuff[37] = 3;
        break;
    case KEY_TYPE_FIXED_PIN:
        g_EppSendBuff[37] = 4;
        break;
    case KEY_TYPE_MASTER:
        g_EppSendBuff[37] = 0;
        break;
    }
    g_EppSendBuff[38] = byKeyIndex;

    iRet = s_EppCrc16SendPacket(34, CMD_CLEAR_ONE_KEY);
    if (EPP_SUCCESS != iRet)
    {
        return EPP_SEND_CMD_ERROR;
    }
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 1000);
    if (EPP_SUCCESS != iRet)
    {
        return iRet;
    }
    if (0 != g_EppRecvBuff[8])
    {
        return (RET_ERROR_BASE+g_EppRecvBuff[8]);
    }
    if (11 != wRecvLen)
    {
        return EPP_RECV_LEN_ERROR;
    }
    return EPP_SUCCESS;
}

/****************************************************************************
  函数名     :  int EppClearOneAppKey(void)
  描述       :  清除当前应用的所有密钥
  输入参数   :  无
  输出参数   :  无
  返回值     :
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2010-12-11  V1.0         创建
****************************************************************************/
int EppClearOneAppKey(void)
{
    int iRet;
    WORD wRecvLen;

    s_EppPortClear();
    memset(g_EppSendBuff, 0, 1024);
    g_sEppConfigure.abyAppName[31] = 0;
    strcpy((char*)(&g_EppSendBuff[5]), (char*)g_sEppConfigure.abyAppName);
//    memcpy(&g_EppSendBuff[37], g_abySecuryPassword, 8);

    iRet = s_EppCrc16SendPacket(32, CMD_CLEAR_ALL_ONE_KEY);
    if (EPP_SUCCESS != iRet)
    {
        return EPP_SEND_CMD_ERROR;
    }
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 10000);
    if (EPP_SUCCESS != iRet)
    {
        return iRet;
    }
    if (0 != g_EppRecvBuff[8])
    {
        return (RET_ERROR_BASE+g_EppRecvBuff[8]);
    }
    if (11 != wRecvLen)
    {
        return EPP_RECV_LEN_ERROR;
    }
    return EPP_SUCCESS;
}

/****************************************************************************
  函数名     :  int EppFormatPed(void)
  描述       :  清除密码键盘内的所有PED敏感数据，SIEK除外
  输入参数   :  无
  输出参数   :  无
  返回值     :
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2010-12-11  V1.0         创建
****************************************************************************/
int EppFormatPed(void)
{
    int iRet;
    WORD wRecvLen;

    s_EppPortClear();

    iRet = s_EppCrc16SendPacket(0, CMD_FORMAT_PED);
    if (EPP_SUCCESS != iRet)
    {
        return EPP_SEND_CMD_ERROR;
    }
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 20000);
    if (EPP_SUCCESS != iRet)
    {
        return iRet;
    }
    if (0 != g_EppRecvBuff[8])
    {
        return (RET_ERROR_BASE+g_EppRecvBuff[8]);
    }
    if (11 != wRecvLen)
    {
        return EPP_RECV_LEN_ERROR;
    }
    return EPP_SUCCESS;
}

/****************************************************************************
  函数名     :  int EppSetIdleLogo(BYTE *pbyBmpIdleLogoIn)
  描述       :  设置IDLE LOGO
  输入参数   :  1、BYTE *pbyBmpIdleLogoIn ：分辨率为122*32的单色BMP文件的数据内容
  输出参数   :  无
  返回值     :
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2010-12-11  V1.0         创建
****************************************************************************/
int EppSetIdleLogo(const BYTE *pbyBmpIdleLogoIn)
{
    int iRet;
    WORD wRecvLen;

    if (0 != memcmp(pbyBmpIdleLogoIn,g_abyIdleLogoFileHead,62))
    {
        return EPP_BMP_ERROR;
    }
    s_EppPortClear();

    memcpy(&g_EppSendBuff[5], pbyBmpIdleLogoIn, 574);

    iRet = s_EppCrc16SendPacket(574, CMD_LOAD_IDLE_LOGO);
    if (EPP_SUCCESS != iRet)
    {
        return EPP_SEND_CMD_ERROR;
    }
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 1000);
    if (EPP_SUCCESS != iRet)
    {
        return iRet;
    }
    if (0 != g_EppRecvBuff[8])
    {
        return (RET_ERROR_BASE+g_EppRecvBuff[8]);
    }
    if (11 != wRecvLen)
    {
        return EPP_RECV_LEN_ERROR;
    }
    return EPP_SUCCESS;
}

/****************************************************************************
  函数名     :  int EppRestoreDefaultIdleLogo(void)
  描述       :  清除现有IDLE LOGO，恢复默认IDLE LOGO
  输入参数   :  无
  输出参数   :  无
  返回值     :
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2010-12-11  V1.0         创建
****************************************************************************/
int EppRestoreDefaultIdleLogo(void)
{
    int iRet;
    WORD wRecvLen;

    s_EppPortClear();
    g_EppSendBuff[5] = 0x3e;

    iRet = s_EppCrc16SendPacket(1, CMD_RESTORE_IDLE_LOGO);
    if (EPP_SUCCESS != iRet)
    {
        return EPP_SEND_CMD_ERROR;
    }
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 1000);
    if (EPP_SUCCESS != iRet)
    {
        return iRet;
    }
    if (0 != g_EppRecvBuff[8])
    {
        return (RET_ERROR_BASE+g_EppRecvBuff[8]);
    }
    if (11 != wRecvLen)
    {
        return EPP_RECV_LEN_ERROR;
    }
    return EPP_SUCCESS;
}


/****************************************************************************
  函数名     :  int EppDisplayLogo(BYTE *pbyBmpIdleLogoIn)
  描述       :  在液晶屏上显示图标信息
  输入参数   :  1、BYTE *pbyBmpIdleLogoIn ：
  输出参数   :  无
  返回值     :
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2010-12-11  V1.0         创建
****************************************************************************/
int EppDisplayLogo(const BYTE *pbyBmpIdleLogoIn)
{
    int iRet;
    WORD wRecvLen, wSendLength;
    BYTE abyBuff[64];

    memcpy(abyBuff, pbyBmpIdleLogoIn, 62);
    wSendLength = abyBuff[3];
    wSendLength <<= 8;
    wSendLength += abyBuff[2];
    abyBuff[2] = 0;
    abyBuff[3] = 0;
    if (wSendLength > 574)
    {
        return EPP_BMP_ERROR;
    }
    if ((0==abyBuff[18])||(abyBuff[18]>122))
    {
        return EPP_BMP_ERROR;
    }
    abyBuff[18] = 0;
    if ((8!=abyBuff[22])&&(16!=abyBuff[22])&&(24!=abyBuff[22])&&(32!=abyBuff[22]))
    {
        return EPP_BMP_ERROR;
    }
    abyBuff[22] = 0;

    wRecvLen = pbyBmpIdleLogoIn[35];
    wRecvLen <<= 8;
    wRecvLen += pbyBmpIdleLogoIn[34];
    if (wRecvLen > 512)
    {
        return EPP_BMP_ERROR;
    }
    abyBuff[34] = 0;
    abyBuff[35] = 0;
    if (0 != memcmp(abyBuff,g_abyNormalLogoFileHead,62))
    {
        return EPP_BMP_ERROR;
    }
    s_EppPortClear();
    g_EppSendBuff[5] = 0;
    g_EppSendBuff[6] = 0;
    g_EppSendBuff[7] = 0;
    g_EppSendBuff[8] = 0;
    memcpy(&g_EppSendBuff[9], pbyBmpIdleLogoIn, wSendLength);

    iRet = s_EppCrc16SendPacket(wSendLength+4, CMD_DISPLAY_LOGO);
    if (EPP_SUCCESS != iRet)
    {
        return EPP_SEND_CMD_ERROR;
    }
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 1000);
    if (EPP_SUCCESS != iRet)
    {
        return iRet;
    }
    if (0 != g_EppRecvBuff[8])
    {
        return (RET_ERROR_BASE+g_EppRecvBuff[8]);
    }
    if (11 != wRecvLen)
    {
        return EPP_RECV_LEN_ERROR;
    }
    return EPP_SUCCESS;
}

/****************************************************************************
  函数名     :  int EppDisplayString(int X, int Y, int iMode, const void *str, int iStrLen)
  描述       :  在屏幕上显示字符串，目前n20只能显示ASCII字符
  输入参数   :  1、int X ：显示位置x
                2、int Y ：显示位置y
                3、int iMode ：显示字符的大小8:6x8;16:8x16
                4、const void *str ：显示的字符串
                5、int iStrLen ：字符串的长度
  输出参数   :  无
  返回值     :
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2010-12-15  V1.0         创建
****************************************************************************/
int EppDisplayString(int X, int Y, int iMode, const void *str, int iStrLen)
{
    int iRet;
    WORD wRecvLen;

    s_EppPortClear();

    if (NULL == str)
    {
        return EPP_INPUT_PARAM_ERROR;
    }
    g_EppSendBuff[5] = (BYTE)(X>>8);
    g_EppSendBuff[6] = (BYTE)X;
    g_EppSendBuff[7] = (BYTE)(Y>>8);
    g_EppSendBuff[8] = (BYTE)Y;
    g_EppSendBuff[9] = (BYTE)(iMode>>8);
    g_EppSendBuff[10] = (BYTE)iMode;
    memcpy(&g_EppSendBuff[11], str, iStrLen);

    iRet = s_EppCrc16SendPacket(6+iStrLen, CMD_DISPLAY_STRING);
    if (EPP_SUCCESS != iRet)
    {
        return EPP_SEND_CMD_ERROR;
    }
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 3000);
    if (EPP_SUCCESS != iRet)
    {
        return iRet;
    }
    if (0 != g_EppRecvBuff[8])
    {
        return (RET_ERROR_BASE+g_EppRecvBuff[8]);
    }
    if (11 != wRecvLen)
    {
        return EPP_RECV_RET_ERROR;
    }
    return EPP_SUCCESS;
}

/****************************************************************************
  函数名     :  int EppClearScreen(void)
  描述       :  清除屏幕显示。
  输入参数   :  无
  输出参数   :  无
  返回值     :
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2010-12-15  V1.0         创建
****************************************************************************/
int EppClearScreen(void)
{
    int iRet;
    WORD wRecvLen;

    s_EppPortClear();


    iRet = s_EppCrc16SendPacket(0, CMD_CLEAR_SCREEN);
    if (EPP_SUCCESS != iRet)
    {
        return EPP_SEND_CMD_ERROR;
    }
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 3000);
    if (EPP_SUCCESS != iRet)
    {
        return iRet;
    }
    if (0 != g_EppRecvBuff[8])
    {
        return (RET_ERROR_BASE+g_EppRecvBuff[8]);
    }
    if (11 != wRecvLen)
    {
        return EPP_RECV_RET_ERROR;
    }
    return EPP_SUCCESS;
}

int EppEnterFactoryTest(BYTE bySpeed)
{
    int iRet;

    s_EppPortClear();
    g_EppSendBuff[5] = bySpeed;

    memcpy(&g_EppSendBuff[6], "\xab\x87\x98\x56\xcf\xde\xa3\x19", 8);

    iRet = s_EppCrc16SendPacket(9, CMD_ENTER_FACTORY_TEST);
    if (EPP_SUCCESS != iRet)
    {
        return EPP_SEND_CMD_ERROR;
    }
    return EPP_SUCCESS;
}


/****************************************************************************
  函数名     :  int EppGetSystemInfo(BYTE byType, void *pvInfoOut)
  描述       :  读取Epp系统信息
  输入参数   :  1、BYTE byType ：
                          查询类别；
                          0x00产品名称；
                          0x01硬件版本；
                          0x02硬件配置；
                          0x03 机器序列号；
                          0x04 CPU SN；
                          0x10 boot版本；
                          0x11 Kernel版本；
                          0x20 查询协议版本；
  输出参数   :  1、void *pvInfoOut ：16字节的输出信息，缓冲不得小于17字节
  返回值     :
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2010-12-11  V1.0         创建
****************************************************************************/
int EppGetSystemInfo(BYTE byType, void *pvInfoOut)
{
    int iRet;
    WORD wRecvLen;

    s_EppPortClear();
    g_EppSendBuff[5] = byType;

    iRet = s_EppCrc16SendPacket(1, CMD_GET_SYSTEM_INFO);
    if (EPP_SUCCESS != iRet)
    {
        return EPP_SEND_CMD_ERROR;
    }
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 1000);
    if (EPP_SUCCESS != iRet)
    {
        return iRet;
    }
    if (0 != g_EppRecvBuff[8])
    {
        return (RET_ERROR_BASE+g_EppRecvBuff[8]);
    }
    if (27 != wRecvLen)
    {
        return EPP_RECV_LEN_ERROR;
    }
    memcpy(pvInfoOut, &g_EppRecvBuff[9], 16);
    memset(pvInfoOut+16, 0, 1);
    return EPP_SUCCESS;
}

/****************************************************************************
  函数名     :  int EppBeep(DWORD dwFrequency, DWORD dwTimeMs)
  描述       :  让Epp蜂鸣器鸣叫
  输入参数   :  1、DWORD dwFrequency ：
                        频率控制参数。
                        (1)如果取值从0到6，则对应频率是1680, 1834, 2020, 2127, 2380, 2700, 2900；
                        (2)如果取值大于等于7，则按该值来设置频率，例如该值是1500，则频率是1500。
                        说明：该参数对于N20无意义，N20仅支持固定的2500hz的频率。
                  2、DWORD dwTimeMs ：鸣叫时间长度，单位毫秒
  输出参数   :  无
  返回值     :
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2010-12-11  V1.0         创建
****************************************************************************/
int EppBeep(DWORD dwFrequency, DWORD dwTimeMs)
{
    int iRet;
    WORD wRecvLen;

    s_EppPortClear();
    g_EppSendBuff[5]  = (BYTE)(dwFrequency>>24);
    g_EppSendBuff[6]  = (BYTE)(dwFrequency>>16);
    g_EppSendBuff[7]  = (BYTE)(dwFrequency>>8);
    g_EppSendBuff[8]  = (BYTE) dwFrequency;
    g_EppSendBuff[9]  = (BYTE)(dwTimeMs>>24);
    g_EppSendBuff[10] = (BYTE)(dwTimeMs>>16);
    g_EppSendBuff[11] = (BYTE)(dwTimeMs>>8);
    g_EppSendBuff[12] = (BYTE) dwTimeMs;

    iRet = s_EppCrc16SendPacket(8, CMD_BEEP);
    if (EPP_SUCCESS != iRet)
    {
        return EPP_SEND_CMD_ERROR;
    }
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 1000);
    if (EPP_SUCCESS != iRet)
    {
        return iRet;
    }
    if (0 != g_EppRecvBuff[8])
    {
        return (RET_ERROR_BASE+g_EppRecvBuff[8]);
    }
    if (11 != wRecvLen)
    {
        return EPP_RECV_LEN_ERROR;
    }
    return EPP_SUCCESS;
}

/****************************************************************************
  函数名     :  int EppLight(DWORD dwMode, DWORD dwTimeMs)
  描述       :  Epp背光控制
  输入参数   :  1、DWORD dwMode ：背光（指示灯）控制模式
                            1  背光（指示灯）定时（按指定的时间长度亮着）模式
                            2  背光（指示灯）常开（一直打开）模式
                            3  背光（指示灯）常关（一直关闭）模式
                2、DWORD dwTimeMs ：
                              背光控制时间长度，单位为毫秒。
                              该参数只有在模式1的时候才有效。
  输出参数   :  无
  返回值     :
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2010-12-11  V1.0         创建
****************************************************************************/
int EppLight(DWORD dwMode, DWORD dwTimeMs)
{
    int iRet;
    WORD wRecvLen;

    s_EppPortClear();
    g_EppSendBuff[5]  = (BYTE)(dwMode>>24);
    g_EppSendBuff[6]  = (BYTE)(dwMode>>16);
    g_EppSendBuff[7]  = (BYTE)(dwMode>>8);
    g_EppSendBuff[8]  = (BYTE) dwMode;
    g_EppSendBuff[9]  = (BYTE)(dwTimeMs>>24);
    g_EppSendBuff[10] = (BYTE)(dwTimeMs>>16);
    g_EppSendBuff[11] = (BYTE)(dwTimeMs>>8);
    g_EppSendBuff[12] = (BYTE) dwTimeMs;

    iRet = s_EppCrc16SendPacket(8, CMD_LIGHT);
    if (EPP_SUCCESS != iRet)
    {
        return EPP_SEND_CMD_ERROR;
    }
    iRet = s_EppCrc16RecvPacket(g_EppRecvBuff, &wRecvLen, 1000);
    if (EPP_SUCCESS != iRet)
    {
        return iRet;
    }
    if (0 != g_EppRecvBuff[8])
    {
        return (RET_ERROR_BASE+g_EppRecvBuff[8]);
    }
    if (11 != wRecvLen)
    {
        return EPP_RECV_LEN_ERROR;
    }
    return EPP_SUCCESS;
}





