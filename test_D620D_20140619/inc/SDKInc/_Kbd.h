#ifndef __KBD_H_
#define __KBD_H_


#define KEY_0       	0x00
#define KEY_1       	0x01
#define KEY_2       	0x02
#define KEY_3       	0x03
#define KEY_4       	0x04
#define KEY_5       	0x05
#define KEY_6       	0x06
#define KEY_7       	0x07
#define KEY_8       	0x08
#define KEY_9       	0x09
#define KEY_F1			0x0A
#define KEY_UP			0x0B
#define KEY_DOWN		0x0C
#define KEY_F2			0x0D
#define KEY_BACKSPACE	0x0E
#define KEY_ENTER		0x0F
#define KEY_ASTERISK	0x10		//*
#define KEY_POUND		0x11		//#
#define KEY_CANCEL		0x12


/*
 * function	:	初始化LCD
 * param1	:	
 * return	:	0	-	初始化成功
 				-1	-	初始化错误
 				-2	-	探测不到按键板
 */
int InitKbd();

#define KEY_LIGHT_LEVEL1		1
#define KEY_LIGHT_LEVEL2		2
#define KEY_LIGHT_LEVEL3		3
/*
 * function	:	设定LCD亮度
 * param1	:	背光亮度
 * return	:	0	-	执行正常
 				-1	-	执行错误
 */
int SetLcdLight(UCHAR bright_level);

/*
 * function	:	设定自动关闭背光时间
 * param1	:	时间(单位-秒)
 * return	:	0	-	执行正常
 				-1	-	执行错误
 */
int SetShutBkLightTime(UCHAR auto_time);

/*
 * function	:	蜂鸣一次
 * param1	:	
 * return	:	0	-	执行正常
 				-1	-	执行错误
 */
int Beep();

/*
 * function	:	允许/禁止 蜂鸣器发声
 * param1	:	开关
 *				TRUE	-	允许
 *				FALSE	-	禁止
 * return	:	0	-	执行正常
 				-1	-	执行错误
 */
int EnableBeep(BOOL on_off);

/*
 * function	:	成功蜂鸣音
 * param1	:	
 * return	:	0	-	执行正常
 				-1	-	执行错误
 */
void OkBeep();

/*
 * function	:	失败蜂鸣音
 * param1	:	
 * return	:	0	-	执行正常
 				-1	-	执行错误
 */
void FailBeep();

/*
 * function	:	指定时间内响应按键
 * param1	:	指定的时间(单位:毫秒)(时间为0表示一直等待)
 * return	:	0	-	执行正常
 				-1	-	错误
 				-2	-	超时
 */
int WaitKey(ULONG lTimeOut);

/*
 * function	:	获取指定按键
 * param1	:	指定的按键值
 * param2	:	指定的时间(单位:毫秒)(时间为0表示一直等待)
 * return	:	0	-	执行正常
 				-1	-	执行错误
 */
char WaitLimitKey(char *buff, int len, ULONG ulTimeOut);

/*
 * function	:	检测是否有按键
 * param1	:	
 * return	:	0	-	执行正常
 				-1	-	执行错误
 */
BOOL KbdHit();

/*
 * function	:	按键事件之后获取键值(此函数需在KbdHit调用成功后方可使用)
 * param1	:	
 * return	:	0	-	执行正常
 				-1	-	执行错误
 */
UCHAR GetKey();

/*
 * function	:	获取按键名字
 * param1	:	键值
 * return	:	0	-	执行正常
 				-1	-	执行错误
 */
int GetKeyName(UCHAR KeyValue, char *KeyName);

/*
 * function	:	允许/禁止 按键
 * param1	:	开关
 *				TRUE	-	允许
 *				FALSE	-	禁止
 * return	:	0	-	执行正常
 				-1	-	执行错误
 */
void EnableKbd(BOOL on_off);

/*
 * function	:	清空键盘缓冲区
 * param1	:
 * return	:	0	-	执行正常
 				-1	-	执行错误
 */
int ClearKbd();

/*
 * function	:	强制关机
 * param1	:
 * return	:	0	-	执行正常
 				其它-	执行错误
 */
int PowerOff();


#endif
