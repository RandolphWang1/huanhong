#ifndef __FUNC_H_
#define __FUNC_H_



/*
 * function	:	设置为调试模式(否则不打印调试信息)
 * param1	:	模式(TRUE/FALSE)
 * return		:	无
 */
void SetDebug(BOOL debug);

/*
 * function	:	打印调试信息
 * return		:	无
 */
void sdkPrintf(const char *format, ...);

/*
 * function	:	打印调试信息-带时间前缀
 * return		:	无
 */
void DebugOut(const char *format, ...);

/*
 * function	:	打印调试信息(十六进制)
 * param1	:	内容
 * param2	:	长度
 * return		:	无
 */
void printf_x(char *title, char *data, int len);

/*
 * function	:	打印调试信息(原始字符串)
 * param1	:	标题
 * param2	:	内容
 * param3	:	长度
 * return		:	无
 */
void printf_s(char *title, char *data, int len);

/*
 * function	:	菜单
 * param1	:	标题
 * param2	:	菜单内容
 * param3	:	子菜单数目
 * param4	:	子菜单缩进
 */
int MenuDlg(char *title, char **menu, int item_count, char *indent);

/*
 * function	:	获取SDK版本号
 * param1	:	返回版本号的缓冲区
 */
void GetSDKVersion(char *version);

/*
 * function	:	初始化SDK
 */
void InitSDK();

/*
 * function	:	过滤掉指定字符后的长度
 * param1	:	字符串
 * param1	:	指定字符
 * return	:	长度
 */
int strlen_filt(char *buff, char filt_ch);

/*
 * function	:	过滤掉IP地址中的空格和0
 * param1	:	IP地址
 */
void FiltIP(char *buff);

/*
 * function	:	输入IP
 * param1	:	x坐标
 * param2	:	y坐标
 * param3	:	缓冲区
 */
int InputIP(int x, int y, char *buff);

/*
 * function	:	查询子字符串
 * param1	:	母串
 * param2	:	母串长度
 * param3	:	目标子串
 * param4	:	子串长度
 * return		:	字符串起始位置
 */
int FindStr(char *Buffer, int BufLen, char *Target, int TargetLen);

int OpenUart();
int ReadUart(int iHandle, unsigned char * pucOutData, unsigned short uiOutLen, ULONG ulTimeOut);
int WriteUart(int iHandle,unsigned char * pucInData,unsigned short uiInLen);

/*
 * function	:	创建状态栏
 * param1	:	
 * return		:	无
 */
int CreateStatusBar(int font_color);

/*
 * function	:	启动状态栏
 * param1	:	
 * return		:	无
 */
void OpenStatusBar();

/*
 * function	:	关闭状态栏
 * param1	:	
 * return		:	无
 */
void CloseStatusBar();

void ShutDown();

/*
 * function	:	修改参数
 * param1	:	要修改的参数
 * param2	:	长度
 * param3	:	前缀标题
 * return		:	无
 */
void ModifyData(char *value, int value_len, char *title);

/*
 * function	:	格式化金额字符串
 * param1	:	前缀
 * param2	:	金额字符串
 * param3	:	后缀
 * param4	:	格式化后的结果
 * return		:	无
 */
void FormatMoney(char *prefix, char *money, char *suffix, char *result);

/*
 * function	:	计算日期是否在某区间之内
 * param1	:	日期
 * param2	:	起始日期
 * param3	:	结束日期
 * return		:	在此区间内		:	TURE
 *				不在该区间内	:	FALSE
 */
BOOL CheckTime(T_DATETIME time, char *start, char *end);

/*
 * function	:	输入金额
 * param1	:	屏幕输入X坐标
 * param2	:	屏幕输入Y坐标
 * param3	:	输入缓冲区
 * param4	:	缓冲区大小
 * return	:	成功			:	输入长度
 *				错误			:	-1
 *				取消			:	-3
*/
int InputMoney(int x, int y, char* buff, int size);

/*
 * function	:	字符串左对齐转换为右对齐(补0)
 * param1	:	字符串
 * param2	:	字符串长度
 * return		:	无
 */
void ChangeLeftToRight(char *data, int len);

/*
 * function	:	从U盘拷贝文件
 * param1	:	POS路径
 * param2	:	U盘路径
 * return		:	无
 */
int GetFileFromU(char *pos_path, char *u_path);

/*
 * function	:	拷贝文件至U盘
 * param1	:	POS路径
 * param2	:	U盘路径
 * return		:	无
 */
int CopyFile2U(char *pos_path, char *u_path);

void Fit_Length_by_LeadChar(CHAR *src,   INT maxlen,   CHAR cPadChr);


/*
Buffer:目标串
BufLen:目标串长度
Target:查找串
TargetLen:查找串长度
*/
int FindString(char *Buffer, int BufLen, char *Target, int TargetLen);

#endif
