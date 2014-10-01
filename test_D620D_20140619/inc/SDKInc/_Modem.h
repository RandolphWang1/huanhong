/***************************************************************************
    Li,Hengbo
    2009-03-10
***************************************************************************/
#ifndef __MODEM_H_
#define __MODEM_H_


#define DIALER_OK						OK
#define DIALER_ERROR					ERROR 



extern int			iModemHandle;
extern char		*p_map;



/*
 * function	:	预拨号
 * param1	:	是否拨外线
 * param2	:	外线号码
 * param3	:	第一个电话号码
 * param4	:	第二个电话号码
 * param5	:	第三个电话号码
 * return		:	无
 */
void PreDial(BOOL is_out, char out_no, char *phone1,
	char *phone2, char *phone3);

/*
 * function	:	挂机(关闭modem)
 * return		:	无
 * info		:	非阻塞模式关闭modem
 */
void DialOff();

/*
 * function	:	挂机(关闭modem)
 * return		:	无
 * info		:	阻塞模式关闭modem
 */
void DialOff_Block();

/*
 * function	:	连接主机
 * param1	:	是否拨外线
 * param2	:	外线号码
 * param3	:	第一个电话号码
 * param4	:	第二个电话号码
 * param5	:	第三个电话号码
 * return		:	成功:	OK
 *				失败:	ERROR
 */
int Modem_Connect(BOOL is_out, char out_no, int delay_times,
	char *phone1, char *phone2, char *phone3);

/*
 * function	:	发送数据
 * param1	:	发送数据缓冲区
 * param2	:	发送数据长度
 * return		:	成功:	OK
 *				失败:	ERROR
 */
int Modem_Send(char *data, int len);

/*
 * function	:	接收数据
 * param1	:	接收数据缓冲区
 * param2	:	接收数据长度
 * return		:	成功:	OK
 *				失败:	ERROR
 */
int Modem_Recv(char *data, int *len);



#endif

