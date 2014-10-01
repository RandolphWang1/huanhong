//////////////////////////////////////////////////////////////////////////////

// 根据格式输出日期
// 2010-11-09

//////////////////////////////////////////////////////////////////////////////
// 功能说明
// 入参
// x: x坐标(以文字为单位)
// y: y坐标(以文字为单位)
// buff: 文本缓冲区(由调用者开辟)
// plen: 缓冲去实际存储字符的个数，不包含'\0'
// mode: 日期格式//参考宏定义(参考值MODE_0 -- MODE_7，其它值默认为MODE_0)
// MODE_X对应的DATE_MODE_X参见宏定义
// bk_color: 背景颜色(NO_BACK_COLOR代表没有背景色)
// font_color: 字体颜色
// font_size: 字体大小
// 出参
// buff : 带回日期
// plen : 实际带回的长度
// 返回值
// 代表错误码

#define ERR_BUFF_NO_EXIST 		-100

#define MODE_0				0
#define MODE_1				1
#define MODE_2				2
#define MODE_3				3
#define MODE_4				4
#define MODE_5				5
#define MODE_6				6
#define MODE_7				7

//////////////////////////////////////////////////////////////////////////////
int InputDate(int x, int y, char * buff, int * plen, int mode, int bk_color,
	int font_color, int font_size);
//////////////////////////////////////////////////////////////////////////////

