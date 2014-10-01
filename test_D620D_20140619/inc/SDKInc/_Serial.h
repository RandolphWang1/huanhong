#ifndef __SERIAL_H__
#define __SERIAL_H__

extern char pinpad_device[20];



/*
 * function	:	打开串口
 * param1	:	串口号
 * param2	:	波特率
 * return		:	成功:	串口句柄
 *				失败:	-1
 */
int SERIAL_Open(char *pcPort,int iSpeed);

/*
 * function	:	从串口读数据
 * param1	:	串口句柄
 * param2	:	读数据缓冲区
 * param3	:	缓冲区长度
 * return		:	读到的长度
 */
int SERIAL_Read(int iHandle,unsigned char *pucOutData,unsigned short uiOutLen);

/*
 * function	:	向串口写数据
 * param1	:	串口句柄
 * param2	:	将要写的数据
 * param3	:	缓冲区长度
 * return		:	成功:	写入长度
 *				失败:	-1
 */
int SERIAL_Write(int iHandle,unsigned char *pucInData,unsigned short uiInLen);

/*
 * function	:	关闭串口
 * param1	:	串口句柄
 * return		:	0
 */
int SERIAL_Close(int iHandle);


#endif

