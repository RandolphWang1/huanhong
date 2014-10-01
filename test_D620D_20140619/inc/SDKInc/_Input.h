#ifndef __INPUT_METHOD_H_
#define __INPUT_METHOD_H_


#define IME_TOTAL					3

#define IME_ENGLISH				0
#define IME_NUMBER					1
#define IME_CHINESE				2


#define IME_MAX					100		//最多允许输入的字符数



extern char *input_method_map;


/*
 * function	:	输入框
 * param1	:	屏幕输入x坐标
 * param2	:	屏幕输入y坐标
 * param3	:	输入缓冲区
 * param4	:	输入缓冲区长度
 * param5	:	输入法
 * param6	:	背景颜色
 * param7	:	背景颜色
 * param8	:	是否密码框
 * param9	:	是否必须输满指定长度才能返回
 * param10	:	是否允许切换输入法
 * return	:	成功	:	OK
 *				失败	:	ERROR
 */
int Input(int x, int y, char *buff, int buff_len, int input_method, int bk_color, int font_color,
	BOOL is_secret, BOOL is_full_len, BOOL is_change);

/*
 * function	:	获取当前输入法
 * return	:	当前输入法
 */
int GetInputMethod();

#endif
