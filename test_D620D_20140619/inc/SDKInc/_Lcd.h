#ifndef __LCD_H_
#define __LCD_H_

#define	BLACK		0x0000		//黑色
#define	BLUE		0x001F		//蓝色
#define	RED			0xF800		//红色
#define	GREEN		0x07E0		//绿色
#define CYAN		0x07FF		//浅蓝色
#define MAGENTA		0xF81F		//粉色
#define YELLOW		0xFFE0		//黄色
#define WHITE		0XFFFF		//白色


#define FONT12		12
#define FONT16		16
#define FONT20		20
#define FONT24		24

#define SCREEN_LEFT					0
#define SCREEN_RIGHT				319
#define SCREEN_TOP					0
#define SCREEN_BOTTOM				239


#define SCREEN_WIDTH				320
#define SCREEN_HEIGHT				240

#define ALIGN_LEFT			0
#define ALIGN_CENTER		1
#define ALIGN_RIGHT		2


/*
 * function	:	初始化LCD
 * return	:	0	-	执行正常
 				-1	-	执行错误
 */
int InitLcd();

/*
 * function	:	设定屏幕字体大小与字体颜色
 * param1	:	字体大小(12,16,20,24)
 * param2	:	字体颜色
 * return	:	0	-	执行正常
 				-1	-	执行错误
 */
void SetScrFont(int font_size, int font_color);

/*
 * function	:	获取屏幕字体大小与字体颜色
 * param1	:	字体大小
 * param2	:	字体颜色
 */
void GetScrFont(int *font_size, int *font_color);

/*
 * function	:	恢复上一次设定的屏幕字体大小与字体颜色
 * return	:	0	-	执行正常
 				-1	-	执行错误
 */
void ReStoreScrFont();

/*
 * function	:	显示文本
 * param1	:	x坐标(以文字为单位)
 * param2	:	y坐标(以文字为单位)
 * param3	:	对齐方式
 * param4	:	文本缓冲区
 * return	:	0	-	执行正常
 				-1	-	执行错误
 */
int TextOut(int x, int y, int align_type, char *string);

/*
 * function	:	填充模式显示文本
 * param1	:	x坐标(以文字为单位)
 * param2	:	y坐标(以文字为单位)
 * param3	:	填充背景颜色
 * param4	:	字体颜色
 * param5	:	文本缓冲区
 * return	:	0	-	执行正常
 				-1	-	执行错误
 */
int TextOutFill(int x, int y, int align_type, int bk_color, int font_color, char *string);

/*
 * function	:	显示文本
 * param1	:	x坐标(以像素点为单位)
 * param2	:	y坐标(以像素点为单位)
 * param3	:	文本缓冲区
 * return	:	0	-	执行正常
 				-1	-	执行错误
 */
int TextOutByPixel(int x, int y, char *string);

/*
 * function	:	填充模式显示文本
 * param1	:	x坐标(0-319)
 * param2	:	y坐标(1-239)
 * param3	:	填充背景颜色
 * param4	:	字体颜色
 * param5	:	文本缓冲区
 * return	:	0	-	执行正常
 				-1	-	执行错误
 */
int TextOutFillByPixel(int x, int y, int bk_color, int font_color, char *string);

/*
 * function	:	显示文本(支持多行)
 * param1	:	x坐标(以文字为单位)
 * param2	:	y坐标(以文字为单位)
 * param3	:	文本缓冲区
 * return	:	0	-	执行正常
 				-1	-	执行错误
 */
int TextOutMultiLine(int x, int y, char *string);

/*
 * function	:	清除全屏
 */
void Clear();

/*
 * function	:	清除矩形屏幕区域
 * param1	:	矩形左边横坐标
 * param2	:	矩形右边横坐标
 * param3	:	矩形左边纵坐标
 * param4	:	矩形右边纵坐标
 */
void ClearRect(int x1, int x2, int y1, int y2);

/*
 * function	:	清除指定行
 * param1	:	起始行
 * param2	:	行数
 */
void ClearLine(int start_line, int line_num);

/*
 * function	:	进入设定墙纸模式
 * param1	:	模式开关 1-进入 0-退出
 * return	;
 */
void SetWallPaper(int mode);

/*
 * function	:	以指定颜色显示背景
 * param1	:	颜色
 * return	:	0	-	执行正常
 				-1	-	执行错误
 */
int ShowBkGround(int color);

/*
 * function	:	以指定颜色显示像素点
 * param1	:	x坐标
 * param2	:	y坐标
 * param3	:	颜色
 * return	:	0	-	执行正常
 				-1	-	执行错误
 */
int ShowPixel(int x, int y, int color);

/*
 * function	:	显示数组形式图片
 * param1	:	x坐标
 * param2	:	y坐标
 * param3	:	行长度
 * param4	:	列长度
 * param5	:	图片数据数组
 * return	:	0	-	执行正常
 				-1	-	执行错误
 */
int ShowBmpData(int x, int y, int x_size, int y_size, const unsigned char *pic);

/*
 * function	:	显示FLASH上的图片
 * param1	:	x坐标
 * param2	:	y坐标
 * param3	:	图片文件名(全名)
 * return	:	0	-	执行正常
 				-1	-	执行错误
 */
int ShowBmpFile(int x, int y, char *bmp_file);

/*
 * function	:	绘制矩形
 * param1	:	矩形左边横坐标
 * param2	:	矩形右边横坐标
 * param3	:	矩形左边纵坐标
 * param4	:	矩形右边纵坐标
 * param5	:	颜色
 * return	:	0	-	执行正常
 				-1	-	执行错误
 */
int DrawRect(int x1, int x2, int y1, int y2, int color);

/*
 * function	:	填充矩形
 * param1	:	矩形左边横坐标
 * param2	:	矩形右边横坐标
 * param3	:	矩形左边纵坐标
 * param4	:	矩形右边纵坐标
 * param5	:	颜色
 * return	:	0	-	执行正常
 				-1	-	执行错误
 */
int FillRect(int x1, int x2, int y1, int y2, int color);
//
int TextOut1(int x, int y, char *string, int align_type);

int PutsXY(int x, int y, char *string);

void ClearDynamic();
#endif
