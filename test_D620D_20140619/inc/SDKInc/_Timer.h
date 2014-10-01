#ifndef __TIMER_H_
#define __TIMER_H_


#define YYYY_MM						1	//年-月
#define YYYY_MM_DD						2	//年-月-日
#define MM_DD							3	//月-日
#define HH_MM							4	//时-分
#define HH_MM_SS						5	//时-分-秒
#define YYYY_MM_DD_HH_MM_SS			6	//年-月-日-时-分-秒



/*
 * 时间结构
 */
typedef struct
{
	char century[2+1];							//世纪
	char year[2+1];							//年
	char month[2+1];							//月
	char day[2+1];								//日
	char hour[2+1];							//时
	char minute[2+1];							//分
	char second[2+1];							//秒
	char week[1+1];							//周
}T_DATETIME;




/*
 * function	:	设置时间
 * param1	:	时间结构指针
 * return	:	0	-	执行正常
 				-1	-	执行错误
 */
int SetDateTime(T_DATETIME ptTime);

/*
 * function	:	获取时间
 * param1	:	时间结构指针
 * return	:	0	-	执行正常
 				-1	-	执行错误
 */
int GetDateTime(T_DATETIME *ptTime);

/*
 * function	:	比较日期
 * param1	:	第一个时间结构指针
 * param2	:	第二个时间结构指针
 * return		:	<0,=0,>0
 */
int DateCmp(T_DATETIME *time1, T_DATETIME *time2);

/*
 * function	:	初始化实时时钟
 * return	:	0	-	执行正常
 				-1	-	执行错误
 */
int InitRTC();

/*
 * function	:	等待延时
 * param1	:	等待时间-单位:毫秒
 */
void Wait(ULONG millisecond);

/*
 * function	:	输入时间
 * param1	:	时间缓冲区
 * param1	:	输入类型(类似:YYYY_MM_DD_HH_MM_SS)
 * return	:	0	-	执行正常
 				-1	-	执行错误
 */
int InputTime(int x, int y, T_DATETIME *tTime, int style);

/*
 * function	:	格式化日期
 * param1	:	时间结构指针
 * param2	:	间隔符
 * param3	:	原始时间字符串
 * return		:	无
 */
void FormatDate(T_DATETIME *ptTime, char space, char *data);

/*
 * function	:	格式化时间
 * param1	:	时间结构指针
 * param2	:	间隔符
 * param3	:	原始时间字符串
 * return		:	无
 */
void FormatTime(T_DATETIME *ptTime, char space, char *data);


#endif


