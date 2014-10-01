#ifndef __PINPAD_RONGCI_H_
#define __PINPAD_RONGCI_H_


#define PINPAD_DEVICE					"/dev/ttyS0"

#define PINPAD_TIMEOUT					1000

#define PINPAD_MODE_DES					1		//DES加密模式
#define PINPAD_MODE_3DES				2		//3DES加密模式

#define SUCCESS_PINPAD					0		//成功
#define ERROR_PINPAD					-1		//错误
#define ERROR_PINPAD_OPEN				-2		//打开设备失败
#define ERROR_PINPAD_TIMEOUT			-3		//超时
#define ERROR_PINPAD_WRITE				-4		//写设备失败
#define ERROR_PINPAD_READ				-5		//读设备失败
#define ERROR_PINPAD_PARAM				-6		//参数错误


/*
 * function	:	打开密码键盘
 * return	:	>0	=	成功
 *				其它			=	失败
 */
int PinpadOpen();

/*
 * function	:	关闭密码键盘
 * param1	:	设备句柄
 * return	:	SUCCESS_PINPAD	=	成功
 *				其它			=	失败
 */
int PinpadClose(int handle);

/*
 * function	:	获取密码键盘ID
 * param1	:	设备句柄
 * param2	:	输出内容
 * return	:	SUCCESS_PINPAD	=	成功
 *				其它			=	失败
 */
int PinpadGetID(int handle, char *out_buff);

/*
 * function	:	在密码键盘屏幕上显示文字
 * param1	:	设备句柄
 * param2	:	行
 * param3	:	显示的文本
 * return	:	SUCCESS_PINPAD	=	成功
 *				其它			=	失败
 */
int PinpadShowText(int handle, int row, char *text_buff);

/*
 * function	:	清除LCD
 * param1	:	设备句柄
 * return	:	SUCCESS_PINPAD	=	成功
 *				其它			=	失败
 */
int PinpadClearLCD(int handle);

/*
 * function	:	设置加密模式
 * param1	:	设备句柄
 * param2	:	加密模式   1-des 2-3des
 * return	:	SUCCESS_PINPAD	=	成功
 *				其它			=	失败
 */
int PinpadSetMode(int handle, int mode);

/*
 * function	:	设置主密钥
 * param1	:	设备句柄
 * param2	:	主密钥号 0-15
 * param3	:	主密钥
 * return	:	SUCCESS_PINPAD	=	成功
 *				其它			=	失败
 */
int PinpadSetPriKey(int handle, UINT key_id, UCHAR *key_buff);

/*
 * function	:	设置工作密钥
 * param1	:	设备句柄
 * param2	:	主密钥号   0-15
 * param3	:	工作密钥号 0-3
 * param4	:	主密钥
 * return	:	SUCCESS_PINPAD	=	成功
 *				其它			=	失败
 */
int PinpadSetWorkKey(int handle, UINT prikey_id, UINT workkey_id, UCHAR *key_buff);

/*
 * function	:	激活工作密钥
 * param1	:	设备句柄
 * param2	:	主密钥号   0-15
 * param3	:	工作密钥号 0-3
 * return	:	SUCCESS_PINPAD	=	成功
 *				其它			=	失败
 */
int PinpadActiveWorkKey(int handle, UINT prikey_id, UINT workkey_id);

/*
 * function	:	输入密码
 * param1	:	设备句柄
 * param2	:	输入模式1
 *				0-请输入密码   有语音 输入密码显示*
 *				1-请再输入一次 有语音 输入密码显示*
 *				2-请输入密码   无语音 输入密码显示*
 *				3-请再输入一次 无语音 输入密码显示*
 * param3	:	输入模式2
 *				0-按确认键或到达指定的长度
 *				1-按确认键
 *				2-到达指定的长度
 *				3-到达指定的长度并按确认键
 * param4	:	12位卡号
 * param5	:	输出内容
 * return	:	SUCCESS_PINPAD	=	成功
 *				其它			=	失败
 */
int PinpadInputPwd(int handle, UINT mode1, UINT mode2, char *card_no, char *out_buff);

/*
 * function	:	计算MAC
 * param1	:	设备句柄
 * param2	:	参与计算的字符串
 * param3	:	参与计算的字符串长度
 * param4	:	计算出的MAC
 * return	:	SUCCESS_PINPAD	=	成功
 *				其它			=	失败
 */
int PinpadCalcMac(int handle, char *in, UINT length, char *out);

/*
 * function	:	数据加解密
 * param1	:	设备句柄
 * param2	:	参与计算的字符串
 * param3	:	计算出的结果
 * param4	:	加密/解密    0/1
 * return	:	SUCCESS_PINPAD	=	成功
 *				其它			=	失败
 */
int PinpadEncrypt(int handle, char *in, char *out, int mode);



#endif
