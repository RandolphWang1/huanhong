/***************************************************************************
* Copyright (c) 2007-2010, NEW POS Technology.
* All rights reserved.
*
* 文件名称：Epp.h
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

#ifndef _PINPAD_H_
#define _PINPAD_H_

#ifndef BYTE
#define BYTE unsigned char
#endif

#ifndef WORD
#define WORD unsigned short
#endif

#ifndef DWORD
#define DWORD unsigned long
#endif

#define   TEST_VERSION


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define EPP_PORT PORT_COM1

#define KEY_TYPE_MASTER      0x01
#define KEY_TYPE_MAC         0x02
#define KEY_TYPE_PIN         0x03
#define KEY_TYPE_DUKPT_MAC   0x04
#define KEY_TYPE_DUKPT_PIN   0x05
#define KEY_TYPE_FIXED_MAC   0x10
#define KEY_TYPE_FIXED_PIN   0x11

#define EPPINFO_PRODUCT_NAME    0x00
#define EPPINFO_HW_VER          0x01
#define EPPINFO_HW_CFG          0x02
#define EPPINFO_EPP_SN          0x03
#define EPPINFO_CPU_ID          0x04
#define EPPINFO_BOOT_VER        0x10
#define EPPINFO_KERNEL_VER      0x11
#define EPPINFO_PROTOCOL_VER    0x20


enum EPP_ERROR_CODE
{
    EPP_SUCCESS = 0,
    EPP_RECV_PACKET_ERROR=2001,
    EPP_PORT_OPEN_ERROR,
    EPP_SEND_PACKET_ERROR,  // 03
    EPP_PACKET_LEN_ERROR,
    EPP_RECV_TIMEOUT,  // 05
    EPP_PACKET_LEN_TOO_LONG,
    EPP_CRC_CHECK_ERROR,  // 07
    EPP_OPEN_FILE_ERROR,
    EPP_SEEK_FILE_ERROR,  // 09
    EPP_WRITE_FILE_ERROR,
    EPP_READ_FILE_ERROR,  // 11
    EPP_CONFIGURE_INVALID,
    EPP_CONFIGURE_MAC_ERROR,  // 13
    EPP_NO_PIN,
    EPP_SEND_CMD_ERROR,  // 15
    EPP_RECV_CMD_ERROR,
    EPP_RECV_RET_ERROR,  // 17
    EPP_RECV_LEN_ERROR,
    EPP_MAC_CHECK_ERROR,  // 19
    EPP_AUTHEN_FAILED,
    EPP_INPUT_PARAM_ERROR,  // 21
    EPP_USER_PRESS_CANCEL,
    EPP_INPUT_CMD_ERROR,  // 23
    EPP_INPUT_KEY_INDEX_ERROR,
    EPP_INPUT_MAC_LEN_ERROR, // 25
    EPP_INPUT_MODE_ERROR,
    EPP_KEY_TYPE_ERROR,  // 27
    EPP_KEY_INDEX_ERROR,
    EPP_MASTER_KEY_INDEX_ERROR,  // 29
    EPP_BMP_ERROR,
    EPP_PIN_CFG_LANGE_TYPE_ERROR,  // 31
    EPP_PIN_CFG_SYMBOL_TYPE_ERROR,
    EPP_INPUT_DATA_LEN_ERROR, // 33
};


enum N20_RETURN_CODE{
    RET_ERROR_BASE = 2300,
    RET_PASSWORD_ERROR,  // 01
    RET_BOOT_LEN_ERROR,
    RET_BOOT_HASH_ERROR, // 03
    RET_BOOT_MAC_ERROR,
    RET_NOT_UBSK,  // 05
    RET_CHECK_UBSK_FAILED,
    RET_CHECK_KSK_FAILED,  // 07
    RET_NOT_LCK,
    RET_NOT_AUTH,  // 09
    RET_NOT_LOGON,
    RET_RECV_FAILED,  // 11
    RET_AUTH_CHECK_FAILED,
    RET_MAC_CHECK_FAILED,  // 13
    RET_CRC_CHECK_ERROR,
    RET_KVC_CHECK_ERROR,  // 15
    RET_NOT_APP_FREE_SPACE,
    RET_APP_NOT_EXIST,  // 17
    RET_KEY_TYPE_ERROR,
    RET_KEY_TYPE_NOT_SUPPORT,  // 19
    RET_MAC_LEN_ERROR,
    RET_MAIN_KEY_INDEX_ERROR,  // 21
    RET_MAIN_KEY_NOT_EXIST,
    RET_KEY_INDEX_ERROR,  // 23
    RET_KEY_LEN_ERROR,
    RET_KEY_BE_USE,  // 25
    RET_MAIN_KEY_BE_USE,  // 26
    RET_MAC_ALG_MODE_ERROR,
    RET_KEY_GET_ERROR,
    RET_KEY_OVER_FLOW,  // 29
    RET_KEY_SAME_ERROR,
    RET_KEY_NOT_EXIST,  // 31
    RET_KEY_MAC_CHECK_FAILED,
    RET_KEY_KSN_LEN_ERROR,  // 33
    RET_KEY_BDK_LEN_ERROR,
    RET_USER_PRESS_CANCEL_EXIT,  // 35
    RET_USER_INPUT_TIME_OUT,
    RET_KSN_LEN_RERROR,  // 37
    RET_APP_NUM_OVER_FLOW,
    RET_RW_LENGTH_ERROR,  // 39
    RET_PIN_KEY_TIME_LOCK,
    RET_MAC_KEY_TIME_LOCK,  // 41
    RET_SET_PIN_TIME_OUT_ERROR,
    RET_WRITE_EEPROM_FAILED,  // 43
    RET_RECV_LEN_ERROR,
    RET_PARAM_ERROR  // 45
};




// 打开Pinpad口，在N8110上的串口号为PORT_COM1
int EppOpen();

// 关闭Pinpad口
int EppClose(void);


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
    const BYTE *pbyAkuak, const BYTE *pbyMdtek, const BYTE *pbyMutek);


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
int EppDownloadLckMtek(void);

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
int EppDownloadAik(void);

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
                                 TDEA_NONE       0x02    直接写入明文目标Key
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
int EppDownloadAppKey(BYTE byKeyType, BYTE byMode, BYTE byKeyIndex,
    const BYTE *pbyKeyDataIn, BYTE byKeyLen, BYTE byMasterKeyIndex);



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
  1、 黄俊斌     2010-06-08  V1.0         创建
****************************************************************************/
//int EppDownloadDukptKey(BYTE byKeyIndex,  BYTE *pbyBDK, BYTE *pbyKSN, BYTE byKSNLen);



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
int EppGetRand(BYTE *pbyRandOut);

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
int EppGetMac(BYTE byKeyType, BYTE byKeyIndex, const BYTE *pbyMacDataIn,
                        WORD wMacLen, BYTE byMode, BYTE *pbyMacOut);

#ifdef   TEST_VERSION
int EppGetMacExt(BYTE byKeyType, BYTE byKeyIndex, const BYTE *pbyMacDataIn,
                        WORD wMacLen, BYTE byMode, BYTE *pbyMacOut, DWORD *pdwOut);
#endif


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
int EppGetPin(BYTE byKeyType, BYTE byKeyIndex, BYTE byDisplayMode,
            BYTE byMode, const BYTE *pbyCardInfo, const BYTE *pbyLenInfo, BYTE *pbyPinBlockOut);


/****************************************************************************
  函数名     :  int EppGetTdea(WORD wMode, BYTE byKeyType, BYTE  byKeyIndex, WORD wDataLen,
                   const BYTE *pbyDataIn, BYTE *pbyDataOut)
  描述       :  TDEA运算API
  输入参数   :  1、WORD wMode  ：算法模式
                                TDEA_ENCRYPT  加密运算
                                TDEA_DECRYPT  解密运算
                                TDEA_MODE_ECB=0x0000---使用ECB模式
                                TDEA_MODE_CBC=0x0100---使用CBC模式
                                请参考8110的SDK宏定义
                2、BYTE byKeyType ：密钥类型。
                                0x01：主密钥；
                                0x02：MAC密钥；
                                0x03：PIN密钥；
                                0x04：DUKPT MAC密钥；暂不支持
                                0x05：DUKPT PIN密钥；暂不支持
                                0x10：Fixed MAC密钥；
                                0x11：Fixed PIN密钥；
                3、BYTE byKeyIndex ：MAC密钥索引，如果是MAC或者Fixed MAC可取值[1,100]，
                                     如果是DUKPT MAC则可取值[1,32]
                4、WORD wDataLen ：MAC数据长度，必须是8的整数倍，最小值为8，最大值为2024
                5、const BYTE *pbyDataIn ：参与运算的数据
  输出参数   :  1、BYTE *pbyDataOut ：运算结果
  返回值     :  EPP_SUCCESS/EPP_SEND_CMD_ERROR/EPP_RECV_CMD_ERROR/EPP_RECV_RET_ERROR/
                EPP_RECV_LEN_ERROR/EPP_AUTHEN_FAILED/
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2010-12-11  V1.0         创建
****************************************************************************/
int EppGetTdea(WORD wMode, BYTE byKeyType, BYTE  byKeyIndex, WORD wDataLen,
                   const BYTE *pbyDataIn, BYTE *pbyDataOut);


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
int EppSetPinInputTimeout(WORD wTimeout100Ms);

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
int EppSetTdesIv(const BYTE *pbyIvData);

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
int EppClearAKey(BYTE byKeyType, BYTE byKeyIndex);

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
int EppClearOneAppKey(void);

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
int EppFormatPed(void);

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
int EppSetIdleLogo(const BYTE *pbyBmpIdleLogoIn);


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
int EppRestoreDefaultIdleLogo(void);


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
int EppDisplayLogo(const BYTE *pbyBmpIdleLogoIn);

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
int EppClearScreen(void);

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
int EppDisplayString(int X, int Y, int iMode, const void *str, int iStrLen);

// 工厂测试接口，应用程序使用不到
int EppEnterFactoryTest(BYTE bySpeed);

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
  输出参数   :  1、void *pvInfoOut ：16字节的输出信息
  返回值     :
  修改历史   :
      修改人     修改时间    修改版本号   修改原因
  1、 黄俊斌     2010-12-11  V1.0         创建
****************************************************************************/
int EppGetSystemInfo(BYTE byType, void *pvInfoOut);

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
int EppBeep(DWORD dwFrequency, DWORD dwTimeMs);

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
int EppLight(DWORD dwMode, DWORD dwTimeMs);










#ifdef __cplusplus
}
#endif /* __cplusplus */









#endif













