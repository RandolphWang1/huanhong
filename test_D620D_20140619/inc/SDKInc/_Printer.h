#ifndef __PRINTER_H_
#define __PRINTER_H_


#define FONT32	32
#define FONT48	48
#define FONT64	64

//QR码纠错等级
#define QR_ECLEVEL_L    0   //7%
#define QR_ECLEVEL_M    1   //15%
#define QR_ECLEVEL_Q    2   //25%
#define QR_ECLEVEL_H    3   //30%

//QR码编码模式
#define QR_MODE_NUM     0   //数字模式
#define QR_MODE_AN      1   //字母-数字模式
#define QR_MODE_8       2   //8位数据模式
#define QR_MODE_HANZI   3   //汉字模式


/*
 * function	:	初始化打印机
 * param1	:	
 * return	:	0	-	执行正常
 				-1	-	执行错误
 */
int InitPrinter();

/*
 * function	:	设置打印字体
 * param1	:	字体(16,24,32,48,64)
 */
void SetPrintFont(int font);

/*
 * function	:	设置打印字体
 * param1	:	行间距
 * param2	:	列间距
 */
void SetPrintSpace(int row, int col);

/*
 * function	:	设置打印字体
 * return	:	0	-	正常状态
 				-1	-	无纸状态
 */
int GetPrinterStatus();

/*
 * function	:	设置打印机的左边界(以像素点为单位)
 * param1	:	像素数量
 */
void SetPrintIndent(int indent);

/*
 * function	:	依照先前已设置的字体、间距和左边界值，支持格式化字符串的打印
 * return	:	0	-	执行正常
 				-1	-	执行错误
 */
int FillPrintBuff(char *format,...);

/*
 * function	:	打印缓冲区内容
 * return	:	0	-	执行正常
 				-1	-	执行错误
 */
int StartPrint();

/*
 * function	:	清空打印机缓冲区内容
 */
void ClearPrintBuff();

/*
 * function	:	检测打印机
 * return		:	正常		:	TRUE
 *				不正常	:	FALSE
 */
BOOL CheckPrinter();

/*
 * function	:	打印空行
 * param1	:	行数
 * return		:	成功:	TRUE
 *				失败:	FALSE
 */
BOOL PrintEmptyLine(int row);

/*
 * function	:	打印BMP图片
 * param1	:	x坐标，0-47，图片的左右位置，0为最左侧，47为最右侧，单位是毫米。
 * param2	:	指定BMP文件路径和文件名。
 * return		:0:ok
	-1:no paper.
	-2:invalid bmp position
	-3:can't find bmp file
	-4,-5,-6,-8:open bmp file error
	-7:oversize
	-9,-10:system error
	-11:print fail.
	-12:invalid mode.
	备注:	1.本函数直接打印单色BMP图片，不要再执行PrnStart。
			2.最大图片尺寸限定为宽384X高200点。
 */
int PrintBMP(unsigned x,char *bmp_file);

//
/**********************************************************
函数：	PrintQR
说明：	打印QR条码基础框架(for test).目前仅支持版本10.纠错等级M.
输入：  QR_version  :QR码版本.1-40.
        ec_level    :纠错等级,参考app_printer.h定义:
                        QR_ECLEVEL_L    0   //7%
                        QR_ECLEVEL_M    1   //15%
                        QR_ECLEVEL_Q    2   //25%
                        QR_ECLEVEL_H    3   //30%
        enc_mode    :编码方式,参考app_printer.h:
                        QR_MODE_NUM     0   //数字模式
                        QR_MODE_AN      1   //字母-数字模式
                        QR_MODE_8       2   //8位数据模式
                        QR_MODE_HANZI   3   //汉字模式
        *str        :输入数据
        
        x           :水平偏移的点数.以1个点(0.125mm)为单位进行偏移.偏移量必须使得剩余空间可以容纳二维码.
                        计算方法:x+QR码边长<=384.
       scale       :放大倍数,取值2-8,=放大2-8倍.推荐放大3-5倍.
返回：  -1:无纸
        -2:错误的QR码版本或纠错等级
        -3:错误的放大倍数
        -4:QR码坐标/QR码大小超范围
        -5:无效bat_level
        -6:打印失败
        0:ok.
***********************************************************/
int PrintQR(int QR_version,int ec_level,int enc_mode,char *str,int x,int scale);

/**********************************************************
*函数：	MoveDotLine
*说明： 打印机走纸pixel个像素点,每dot=0.0625mm(1mm=16 pixels).如果遇见黑标,越过黑标后会停止走纸.
*输入： nStep:最大走纸范围
*返回： 0:走纸正常(如果设定了检测黑标,为检测到黑标，且已经越过黑标).
*       -1:无纸(无论是否设置检测黑标)
*       -2:有纸,启用了黑标功能但没找到黑标
*       -3:有纸,启用了黑标功能但黑标过大,没有在20mm内走出黑标.
*       -4:电量低
***********************************************************/
int MoveDotLine(int nStep);

/**********************************************************
*函数：	BmSwitch
*说明：	
*输入：	nBm:0=禁止黑标打印.1=启动黑标打印.
*返回： -1:执行失败。
*       0:执行成功。
***********************************************************/
int BmSwitch(int nBm);

#endif

