#ifndef __FILE_H_
#define __FILE_H_



/*
 * function	:	保存数据到指定文件
 * param1	:	文件名
 * param2	:	要保存的数据串
 * param3	:	要保存的数据大小
 * param4	:	要保存的记录索引
 * return		:	无
 * Info:		:	保存多条记录时，每条记录大小应一致
 */
BOOL WriteData(char *file, void* Buff, int size, int index);

/*
 * function	:	从指定文件读数据
 * param1	:	文件名
 * param2	:	保存读出数据的缓冲区
 * param3	:	要读的数据大小
 * param4	:	要读的记录索引
 * return		:	无
 */
BOOL ReadData(char *file, void* Buff, int size, int Index);


#endif
