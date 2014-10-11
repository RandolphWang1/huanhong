#include "Main.h"
#include "qrgenerator.h"
#include "aliqr.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t prmutex;
#define ALIPAY_QUERY
int SetMoney();
extern int socket_fd;
extern struct sockaddr_un address;

void querySingle(void);
void *thr_fn(void* arg);
void printTail(char* price);
char* Moneyformat(char* buf);
unsigned int Money2int(char* buf);
void print_logo();
void query24h(void);
char result24h[QRRESULT] = {0};
#ifdef ALIPAY_QUERY
extern unsigned long long query_number; 
extern char qrout_trade_no[];
int SplitStr(char *buff, char *parr[], char *token) 
{
        char *pc = strtok(buff, token); 
        int i;  

        for(i=0; pc != NULL; i++)
        {       
                parr[i] = pc;
                pc = strtok(NULL, token); 
        }       
        
        return i;
}
#endif

static int SetModem()
{
	int ret;
	char tempbuff[40];
	char buff[128];
	int tempDataNum;

	Clear();;

	SetScrFont(FONT20, WHITE);

	TextOut(0, 2, ALIGN_CENTER, "拨号方式");
	if(gTerm.bOut)
	{
		ShowBmpFile(80, 70, "pic/select.bmp");
		ShowBmpFile(80, 110, "pic/select_bk.bmp");
	}
	else
	{
		ShowBmpFile(80, 70, "pic/select_bk.bmp");
		ShowBmpFile(80, 110, "pic/select.bmp");
	}
	TextOutByPixel(130, 80, "1.外线");
 	TextOutByPixel(130, 120, "2.直拨");

   TextOut(0, 8, ALIGN_CENTER, "保持不变请按确认键");

	ret = WaitLimitKey("\x01\x02\x12\x0F", 4, 0);

	if(ret == KEY_CANCEL)
		goto FAILED;
	else if(ret == KEY_1)
		gTerm.bOut = TRUE;
	else if(ret == KEY_2)
		gTerm.bOut = FALSE;

	//选择外线,输入外线号码
	if(gTerm.bOut)
	{
		Clear();
		SetScrFont(FONT20, WHITE);
		TextOut(0, 8, ALIGN_CENTER, "保持不变请按确认键");
		TextOut(2, 3, ALIGN_LEFT, "外线号码:");
		memset(tempbuff, 0, sizeof(tempbuff));
		tempbuff[0] = gTerm.szOutNo;
		SetScrFont(FONT20, RED);
		ret = Input(13, 3, tempbuff, 1, IME_NUMBER, BLACK, GREEN, FALSE, FALSE, FALSE);
		if(ret != OK)
	    	goto FAILED;
		else
		{
			if(tempbuff[0] == 0)
				gTerm.szOutNo = '0';
			else	
				gTerm.szOutNo = tempbuff[0];
		}

		Clear();
		SetScrFont(FONT20, WHITE);
		TextOut(0, 8, ALIGN_CENTER, "保持不变请按确认键");
		TextOut(2, 3, ALIGN_LEFT, "延时秒数:");
		memset(tempbuff, 0, sizeof(tempbuff));
		tempbuff[0] = gTerm.delay_times+0x30;
		SetScrFont(FONT20, RED);
		ret = Input(13, 3, tempbuff, 1, IME_NUMBER, BLACK, GREEN, FALSE, FALSE, FALSE);
		if(ret != OK)
	    		goto FAILED;
		else
		{
			if(tempbuff[0] == 0)
				gTerm.delay_times = 0;
			else	
				gTerm.delay_times = tempbuff[0]-0x30;
		}
	}

	Clear();
	SetScrFont(FONT20, WHITE);
	TextOut(0, 3, ALIGN_CENTER, "主机电话");
	TextOut(0, 8, ALIGN_CENTER, "保持不变请按确认键");
	SetScrFont(FONT20, RED);
	ret = Input(5, 5, gTerm.szTelNo, SIZE_TEL_NO, IME_NUMBER, BLACK, GREEN, FALSE, FALSE, FALSE);
	if(ret != OK)
    		goto FAILED;

	Clear();
	SetScrFont(FONT20, WHITE);
	TextOut(0, 3, ALIGN_CENTER, "TPDU");
	TextOut(0, 8, ALIGN_CENTER, "保持不变请按确认键");
	SetScrFont(FONT20, RED);
	ret = Input(14, 5, gTerm.szTPDU, SIZE_TPDU, IME_NUMBER, BLACK, GREEN, FALSE, FALSE, FALSE);
	if(ret != OK)
    		goto FAILED;

	while(1)
	{
		Clear();
		SetScrFont(FONT20, WHITE);
		TextOut(0, 3, ALIGN_CENTER, "测试数据量大小");
		TextOut(0, 8, ALIGN_CENTER, "保持不变请按确认键");
		memset(buff, 0, sizeof(buff));
		sprintf(buff, "%d", gTerm.uiModemDataNum);
		SetScrFont(FONT20, RED);
		ret = Input(11, 5, buff, 4, IME_NUMBER, BLACK, GREEN, FALSE, FALSE, FALSE);
		if(ret != OK)
	    	goto FAILED;
		if(ret == OK)
		{
			tempDataNum = atoi(buff);
			if(tempDataNum <= 0 || tempDataNum > 9999)
			{
				Clear();
				SetScrFont(FONT20, WHITE);
				TextOut(0, 6, ALIGN_CENTER, "输入内容非法");
				WaitKey(2000);
				continue;
			}
			gTerm.uiModemDataNum = tempDataNum;
		}
		break;
	}

	//保存文件
	if(WriteData("test-term.dat", &gTerm, sizeof(T_TERM), 0) == FALSE)
	{
		Clear();                      
		SetScrFont(FONT20, WHITE);
		TextOut(0, 3, ALIGN_CENTER, "保存文件出错"); 
		TextOut(0, 4, ALIGN_CENTER, "请稍后重试"); 
		FailBeep();
		WaitKey(2000);
    	goto FAILED;
	}

	OkBeep();

	ClearLine(1, 9);
	SetScrFont(FONT20, WHITE);
	TextOut(0, 4, ALIGN_CENTER, "通讯信息设置成功!");
	WaitKey(2000);
	return OK;

FAILED:
	memset(&gTerm, 0, sizeof(T_TERM));
	ReadData("test-term.dat", &gTerm,sizeof(T_TERM), 0);

	return ERROR;
}

static int SetGPRS()
{
	int ret;
	char buff[128];
	int tempDataNum;

	Clear();;
	SetScrFont(FONT20, WHITE);
	TextOut(0, 2, ALIGN_CENTER, "主机IP");
	TextOut(0, 8, ALIGN_CENTER, "保持不变请按确认键");
	SetScrFont(FONT20, RED);
	if(InputIP(8, 5, gTerm.szGprsServerIP) != OK)
    	goto FAILED;
 
	Clear();;
	SetScrFont(FONT20, WHITE);
	TextOut(0, 3, ALIGN_CENTER, "主机端口");
	TextOut(0, 8, ALIGN_CENTER, "保持不变请按确认键");
	SetScrFont(FONT20, RED);
	ret = Input(7, 5, gTerm.szGprsServerPort, 5, IME_NUMBER, BLACK, GREEN, FALSE, FALSE, FALSE);
	if(ret != OK)
    	goto FAILED;

	while(1)
	{
		Clear();
		SetScrFont(FONT20, WHITE);
		TextOut(2, 4, ALIGN_LEFT, "测试数据量大小:");
		TextOut(0, 8, ALIGN_CENTER, "保持不变请按确认键");

		memset(buff, 0, sizeof(buff));
		sprintf(buff, "%d", gTerm.uiGprsDataNum);
		SetScrFont(FONT20, RED);
		ret = Input(18, 4, buff, 4, IME_NUMBER, BLACK, GREEN, FALSE, FALSE, FALSE);
		if(ret != OK)
	    	goto FAILED;
		tempDataNum = atoi(buff);
		if(tempDataNum <= 0 || tempDataNum > 9999)
		{
			Clear();
			SetScrFont(FONT20, WHITE);
			TextOut(0, 4, ALIGN_CENTER, "输入内容非法");
			WaitKey(2000);
			continue;
		}
		gTerm.uiGprsDataNum = tempDataNum;
		DebugOut("test data num:%d\n", gTerm.uiGprsDataNum);
		break;
	}

	//保存文件
	if(WriteData("test-term.dat", &gTerm, sizeof(T_TERM), 0) == FALSE)
	{
		Clear();                      
		SetScrFont(FONT20, WHITE);
		TextOut(0, 3, ALIGN_CENTER, "保存文件出错"); 
		TextOut(0, 4, ALIGN_CENTER, "请稍后重试"); 
		FailBeep();
		WaitKey(2000);
    	goto FAILED;
	}

	OkBeep();
	Clear();
	SetScrFont(FONT20, WHITE);
	TextOut(2, 4, ALIGN_CENTER, "G网设置成功!");
	WaitKey(2000);
	return OK;

FAILED:
	memset(&gTerm, 0, sizeof(T_TERM));
	ReadData("test-term.dat", &gTerm,sizeof(T_TERM), 0);

	return ERROR;
}
static int SetCDMA()
{
	int ret;
	char buff[128];
	int tempDataNum;

	Clear();;
	SetScrFont(FONT20, WHITE);
	TextOut(0, 2, ALIGN_CENTER, "主机IP");
	TextOut(0, 8, ALIGN_CENTER, "保持不变请按确认键");
	SetScrFont(FONT20, RED);
	if(InputIP(8, 5, gTerm.szGprsServerIP) != OK)
    	goto FAILED;
 
	Clear();;
	SetScrFont(FONT20, WHITE);
	TextOut(0, 3, ALIGN_CENTER, "主机端口");
	TextOut(0, 8, ALIGN_CENTER, "保持不变请按确认键");
	SetScrFont(FONT20, RED);
	ret = Input(7, 5, gTerm.szGprsServerPort, 5, IME_NUMBER, BLACK, GREEN, FALSE, FALSE, FALSE);
	if(ret != OK)
    	goto FAILED;

	while(1)
	{
		Clear();
		SetScrFont(FONT20, WHITE);
		TextOut(2, 4, ALIGN_LEFT, "测试数据量大小:");
		TextOut(0, 8, ALIGN_CENTER, "保持不变请按确认键");

		memset(buff, 0, sizeof(buff));
		sprintf(buff, "%d", gTerm.uiGprsDataNum);
		SetScrFont(FONT20, RED);
		ret = Input(18, 4, buff, 4, IME_NUMBER, BLACK, GREEN, FALSE, FALSE, FALSE);
		if(ret != OK)
	    	goto FAILED;
		tempDataNum = atoi(buff);
		if(tempDataNum <= 0 || tempDataNum > 9999)
		{
			Clear();
			SetScrFont(FONT20, WHITE);
			TextOut(0, 4, ALIGN_CENTER, "输入内容非法");
			WaitKey(2000);
			continue;
		}
		gTerm.uiGprsDataNum = tempDataNum;
		DebugOut("test data num:%d\n", gTerm.uiGprsDataNum);
		break;
	}

	//保存文件
	if(WriteData("test-term.dat", &gTerm, sizeof(T_TERM), 0) == FALSE)
	{
		Clear();                      
		SetScrFont(FONT20, WHITE);
		TextOut(0, 3, ALIGN_CENTER, "保存文件出错"); 
		TextOut(0, 4, ALIGN_CENTER, "请稍后重试"); 
		FailBeep();
		WaitKey(2000);
    	goto FAILED;
	}

	OkBeep();
	Clear();
	SetScrFont(FONT20, WHITE);
	TextOut(2, 4, ALIGN_CENTER, "C网设置成功!");
	WaitKey(2000);
	return OK;

FAILED:
	memset(&gTerm, 0, sizeof(T_TERM));
	ReadData("test-term.dat", &gTerm,sizeof(T_TERM), 0);

	return ERROR;
}
static int SetEthernet()
{
	int ret;
	char buff[128] = "";
	int tempDataNum;

	Clear();;
	SetScrFont(FONT20, WHITE);
	TextOut(0, 2, ALIGN_CENTER, "主机IP");
	TextOut(0, 8, ALIGN_CENTER, "保持不变请按确认键");
	SetScrFont(FONT20, RED);
	if(InputIP(8, 5, gTerm.tEthernet.szServerIP) != OK)
    	goto FAILED;

	Clear();;
	SetScrFont(FONT20, WHITE);
	TextOut(0, 3, ALIGN_CENTER, "主机端口");
	TextOut(0, 8, ALIGN_CENTER, "保持不变请按确认键");
	SetScrFont(FONT20, RED);
	ret = Input(7, 5, gTerm.tEthernet.szServerPort, 5, IME_NUMBER, BLACK, GREEN, FALSE, FALSE, FALSE);
	if(ret != OK)
    	goto FAILED;

	Clear();;
	SetScrFont(FONT20, WHITE);
	TextOut(0, 2, ALIGN_CENTER, "本地IP");
	TextOut(0, 8, ALIGN_CENTER, "保持不变请按确认键");
	SetScrFont(FONT20, RED);
	if(InputIP(8, 5, gTerm.tEthernet.szLocalIP) != OK)
    	goto FAILED;

	Clear();;
	SetScrFont(FONT20, WHITE);
	TextOut(0, 2, ALIGN_CENTER, "子网掩码");
	TextOut(0, 8, ALIGN_CENTER, "保持不变请按确认键");
	SetScrFont(FONT20, RED);
	if(InputIP(8, 5, gTerm.tEthernet.szMask) != OK)
    	goto FAILED;

	Clear();;
	SetScrFont(FONT20, WHITE);
	TextOut(0, 2, ALIGN_CENTER, "网关");
	TextOut(0, 8, ALIGN_CENTER, "保持不变请按确认键");
	SetScrFont(FONT20, RED);
	if(InputIP(8, 5, gTerm.tEthernet.szGateway) != OK)
    	goto FAILED;

	while(1)
	{
		Clear();
		SetScrFont(FONT20, WHITE);
		TextOut(2, 4, ALIGN_LEFT, "测试数据量大小:");
		TextOut(0, 8, ALIGN_CENTER, "保持不变请按确认键");

		memset(buff, 0, sizeof(buff));
		sprintf(buff, "%d", gTerm.uiEthDataNum);
		SetScrFont(FONT20, RED);
		ret = Input(18, 4, buff, 4, IME_NUMBER, BLACK, GREEN, FALSE, FALSE, FALSE);
		if(ret != OK)
	    	goto FAILED;
		tempDataNum = atoi(buff);
		if(tempDataNum <= 0 || tempDataNum > 9999)
		{
			Clear();
			SetScrFont(FONT20, WHITE);
			TextOut(0, 4, ALIGN_CENTER, "输入内容非法");
			WaitKey(2000);
			continue;
		}
		gTerm.uiEthDataNum = tempDataNum;
		break;
	}

	Clear();;
	SetScrFont(FONT20, WHITE);
	TextOut(2, 4, ALIGN_CENTER, "初始化以太网...");

	//配置本地IP地址   
	memset(buff, 0, sizeof(buff));	
	strcat(buff, "ifconfig eth0 ");
	strcat(buff, gTerm.tEthernet.szLocalIP);
	strcat(buff, " netmask ");
	strcat(buff, gTerm.tEthernet.szMask);
	strcat(buff, " up");	
	printf("\n%s\n", buff);
	system(buff);	 

	memset(buff, 0, sizeof(buff));	
	strcat(buff, "route del default gw ");
	strcat(buff, gTerm.tEthernet.szGateway);
	printf("\n%s\n", buff);
	system(buff);  

	memset(buff, 0, sizeof(buff));	
	strcat(buff, "route add default gw ");
	strcat(buff, gTerm.tEthernet.szGateway);
	printf("\n%s\n", buff);
	system(buff);

	//保存文件
	if(WriteData("test-term.dat", &gTerm, sizeof(T_TERM), 0) == FALSE)
	{
		Clear();                      
		SetScrFont(FONT20, WHITE);
		TextOut(0, 3, ALIGN_CENTER, "保存文件出错"); 
		TextOut(0, 4, ALIGN_CENTER, "请稍后重试"); 
		FailBeep();
		WaitKey(2000);
    	goto FAILED;
	}

	OkBeep();
	Clear();
	SetScrFont(FONT20, WHITE);
	TextOut(2, 4, ALIGN_CENTER, "以太网设置成功!");
	WaitKey(2000);
	return OK;

FAILED:
	memset(&gTerm, 0, sizeof(T_TERM));
	ReadData("test-term.dat", &gTerm,sizeof(T_TERM), 0);

	return ERROR;
}


// 查看当前ip设置
int ViewIpSet()
{
	UCHAR ucKey;
	char sKeyName[20];

	ClearKbd();
	while(1)
	{
		Clear();
		SetScrFont(FONT20, WHITE);

		//标题
		//ShowBmpFile(0, 25, "pic/title.bmp");
		TextOutByPixel(100, 30, "当前IP设置信息");

		if (gTerm.bAutoIP== TRUE) {
			TextOutByPixel(45, 65, "当前IP模式：自动获取");
		} else {
			TextOutByPixel(45, 65, "当前IP模式：固定IP");
			TextOutByPixel(45, 100, "IP地址：");
			TextOutByPixel(150, 100, gTerm.tEthernet.szLocalIP);
			TextOutByPixel(45, 135, "子网掩码：");
			TextOutByPixel(150, 135, gTerm.tEthernet.szMask);
			TextOutByPixel(45, 170, "网关：");
			TextOutByPixel(150, 170, gTerm.tEthernet.szGateway);
		}
		ucKey = WaitLimitKey("\x12", 1, 0);
		memset(sKeyName, 0, sizeof(sKeyName));
		GetKeyName(ucKey, sKeyName);
		DebugOut("press key: %s\n", sKeyName);

		switch(ucKey)
		{
			case KEY_CANCEL:
				return OK;
				break;
		}
	}
	return OK;
}



void SetCommParam()
{
	UCHAR ucKey;
	char sKeyName[20];
	int ret;
#if 0 //def ALIPAY_QUERY
    fd_set rset;
    struct timeval tv;
    int retval;
    int i;
#endif
#if 0 //def ALIPAY_QUERY
    int connection_fd;
    socklen_t address_length;
    int nbytes; 
    char buffer[1024];
    int trade_num;
    char *trade_ptr[100] = {NULL}; 
    char *trade_detail[5] = {NULL}; 
    struct receipt_info pos_receipt;
    char PrintBuff[30];
	T_DATETIME tTime;
	char pos_date[12];
	char pos_time[12];
#endif

#if 0 //def ALIPAY_QUERY
    int maxfd = 0;
    if(socket_fd != 0)
        maxfd = max(maxfd,socket_fd);
#endif

    int err;
    pthread_t ntid;
    pthread_mutex_init(&prmutex, NULL);
    err = pthread_create(&ntid, NULL, thr_fn, NULL);
    if(err != 0)
        printf("!!!! query thread create failure-----\n");
    ClearKbd();
    printf("---------SetCommParam() outside the while loop----\n");
    while(1)
    {
#if 0 //def ALIPAY_QUERY
        FD_ZERO(&rset);
        FD_SET(socket_fd, &rset);
        address_length = sizeof(address);
#endif
		Clear();

        printf("---------SetCommParam() inside the while loop----\n");
		SetScrFont(FONT20, WHITE);


		//标题
		//ShowBmpFile(0, 25, "pic/title.bmp");
		//TextOutByPixel(95, 30, "盈润捷通");
		TextOutByPixel(115, 30, "盈润捷通");
#if 0
		//菜单
		ShowBmpFile(90, 60, "pic/button.bmp");
		TextOutByPixel(105, 65, "1.拨号");

		ShowBmpFile(90, 95, "pic/button.bmp");
		TextOutByPixel(105, 100, "2.G网");
#endif

        ShowBmpFile(90, 75, "pic/button.bmp");
        TextOutByPixel(105, 80, "0.关机");

        ShowBmpFile(90, 110, "pic/button.bmp");
        TextOutByPixel(105, 115, "1.支付宝");

        ShowBmpFile(90, 145, "pic/button.bmp");
        TextOutByPixel(105, 150, "2.逐单查询");

        ShowBmpFile(90, 180, "pic/button.bmp");
        TextOutByPixel(105, 185, "3.日结");
#if 0
        ShowBmpFile(90, 165, "pic/button.bmp");
		TextOutByPixel(105, 170, "4.C网");
		
        ShowBmpFile(90, 200, "pic/button.bmp");
        TextOutByPixel(105, 205, "5.wifi网");
#endif

#if 0 //def ALIPAY_QUERY
        /* Wait up to one seconds. */
        tv.tv_sec = 1;
        tv.tv_usec = 0; 
        retval = select(maxfd+1, &rset, NULL, NULL, &tv);
        //retval = select(maxfd+1, &rset, NULL, NULL, NULL);
        printf("select got return,go before FD_ISSET-----\n");
        if(FD_ISSET(socket_fd, &rset)) {
            if ((connection_fd = accept(socket_fd,
                            (struct sockaddr *) &address,
                            &address_length)) > -1)
            {

    nbytes = read(connection_fd, buffer, 1024);
    buffer[nbytes] = 0;

    printf("MESSAGE FROM ALIPAY: %s\n", buffer);
    //nbytes = snprintf(buffer, 256, "hello from the server");
    //write(connection_fd, buffer, nbytes);
    /* start print out the payment query result */

    trade_num = SplitStr(buffer,trade_ptr,"|");

    //write(tty_data.posfd,alipay_receipt,sizeof(alipay_receipt));
    ///write(tty_data.posfd,"\n",1);
	/* get system time */
	//time(&td);
	//ptr = localtime(&td);
	//strftime(pos_date,sizeof(pos_date),"%Y-%m-%d",ptr);
	//strftime(pos_time,sizeof(pos_time),"%H:%M:%S",ptr);
        GetDateTime(&tTime);
        sprintf(pos_date,"%s%s-%s-%s",tTime.century,tTime.year,tTime.month,tTime.day);
        sprintf(pos_time,"%s:%s:%s",tTime.hour,tTime.minute,tTime.second);
	
    for (i=0; i<trade_num; i++){
       printf("number %d trade:%s\n",i,trade_ptr[i]);
       SplitStr(trade_ptr[i],trade_detail,",");
       memset(pos_receipt.serial_number,0,24);
       memset(pos_receipt.out_trade_no,0,16);
       memset(pos_receipt.trade_no,0,32);
       memset(pos_receipt.total_fee,0,16);

       strcpy(pos_receipt.serial_number,trade_detail[0]);
       strcpy(pos_receipt.out_trade_no,trade_detail[1]);
       strcpy(pos_receipt.trade_no,trade_detail[2]);
       strcpy(pos_receipt.total_fee,trade_detail[3]);
       ///WritePayment(1, &pos_receipt);
      /// write(tty_data.posfd,"\n",1);
       ///write(tty_data.posfd,"\n",1);
START_PRINT:
       if(CheckPrinter() != TRUE)
           printf("the printer is not working well!\n");
       ClearPrintBuff();
       memset(PrintBuff,0,30);
       SetPrintIndent(100);
       SetPrintFont(32);
       strcpy(PrintBuff,"支付宝交易凭条");
       FillPrintBuff(PrintBuff);
	   PrintEmptyLine(2);
	   SetPrintIndent(0);
       SetPrintFont(32);
       strcpy(PrintBuff,"序列号：");
       FillPrintBuff(PrintBuff);
       FillPrintBuff(pos_receipt.serial_number);
	
       strcpy(PrintBuff,"小票号：");
       FillPrintBuff(PrintBuff);
       FillPrintBuff(pos_receipt.out_trade_no);
	
       strcpy(PrintBuff,"日期：");
       FillPrintBuff(PrintBuff);
       FillPrintBuff(pos_date);
	   
       strcpy(PrintBuff,"时间：");
       FillPrintBuff(PrintBuff);
       FillPrintBuff(pos_time);
	   
       strcpy(PrintBuff,"-------------------");
       FillPrintBuff(PrintBuff);	   

       strcpy(PrintBuff,"支付宝当面付");
       FillPrintBuff(PrintBuff);	   

       strcpy(PrintBuff,"交易号：");
       FillPrintBuff(PrintBuff);
       FillPrintBuff(pos_receipt.trade_no);
	   
       strcpy(PrintBuff,"金额：");
       FillPrintBuff(PrintBuff);
       FillPrintBuff(pos_receipt.total_fee);
	   
       strcpy(PrintBuff,"签名 ");
       FillPrintBuff(PrintBuff);
	   
       strcpy(PrintBuff,"本人同意上述交易");
       FillPrintBuff(PrintBuff);

       PrintEmptyLine(3);	 
	     	   	   	   	   
       //开始打印    
        ret =StartPrint();
        DebugOut("print error code:[%d]\n", ret);
        if(ret != 0)
        {   
            if(ret == -1) 
                goto START_PRINT;
            else if(ret == -2) 
                goto end2;
            else if(ret == -3) 
                goto end1;
        }   
        goto normal;
end1:  

        FailBeep(); 
        ClearLine(1, 9);
        TextOut(0, 3, ALIGN_CENTER, "请检查打印机"); 
        TextOut(0, 4, ALIGN_CENTER, "打印失败");
        WaitKey(2000);


        goto normal;
end2:  

        FailBeep(); 
        ClearLine(1, 9);
        TextOut(0, 3, ALIGN_CENTER, "电量不足"); 
        TextOut(0, 4, ALIGN_CENTER, "无法执行打印");
        WaitKey(2000);

normal:
    }
    close(connection_fd);
    }
}
#endif
        printf("go before SetCommParam WaitLimitKey\n");
        ucKey = WaitLimitKey("\x00\x01\x02\x03\x12", 5, 0);
        printf("go after SetCommParam WaitLimitKey\n");
        memset(sKeyName, 0, sizeof(sKeyName));
        GetKeyName(ucKey, sKeyName);
        DebugOut("press key: %s\n", sKeyName);

		switch(ucKey)
		{
			case KEY_CANCEL:
				return;
				break;
                        case KEY_0:
                               Clear();
                               TextOut(0, 3, ALIGN_CENTER, "是否关机?");
                               TextOut(0, 4, ALIGN_CENTER, "1.是   其他键.否"); 
                               if(WaitKey(0) == KEY_1)
                                           ShutDown(); 
                               break;
#if 0
			case KEY_1:
				SetModem();
				break;
			case KEY_2:
				SetGPRS();
				break;
#endif
            case KEY_1:
                //this part should think about more
                if (system("ifconfig eth0") != 0) 
                    if (system("ifconfig ppp0") != 0) {
                        Clear(); 
                        TextOut(0, 5, ALIGN_CENTER, "稍等，正在初始化移动网络...");
                        Wait(2000);
                        return;
                    }
                Clear();
                //TextOut(0, 0, ALIGN_CENTER, "IP 获取方式");
                TextOut(0, 0, ALIGN_CENTER, "欢迎使用支付宝钱包支付");

#if 0
				TextOut(0, 4, ALIGN_LEFT, "  1.自动获取IP");
				TextOut(0, 5, ALIGN_LEFT, "  2.固定设置IP");
				TextOut(0, 6, ALIGN_LEFT, "  3.查看当前ip设置");
				TextOut(0, 7, ALIGN_LEFT, "  4.移动支付");
				ucKey = WaitLimitKey("\x12\x01\x02\x03\x04", 5, 0);
#endif
				TextOut(0, 4, ALIGN_CENTER, "请按OK键输入金额");
				TextOut(0, 5, ALIGN_CENTER, "按CANCEL键或者BACK键返回");
				ucKey = WaitLimitKey("\x12\x0E\x0F", 3, 0);
				if(ucKey == KEY_ENTER)
					SetMoney();
				else if(ucKey == KEY_CANCEL || ucKey == KEY_BACKSPACE)
					return ;
				break;
#if 0
				if(ucKey == KEY_1)
				{
					gTerm.bAutoIP = TRUE;
					if(WriteData("test-term.dat", &gTerm, sizeof(T_TERM), 0) == FALSE)
					{
						Clear();                      
						SetScrFont(FONT20, WHITE);
						TextOut(0, 3, ALIGN_CENTER, "保存文件出错"); 
						TextOut(0, 4, ALIGN_CENTER, "请稍后重试"); 
						FailBeep();
						WaitKey(2000);
				    	//goto FAILED;
					}
					Clear();;
					SetScrFont(FONT20, WHITE);
					TextOut(0, 2, ALIGN_CENTER, "主机IP");
					TextOut(0, 8, ALIGN_CENTER, "保持不变请按确认键");
					SetScrFont(FONT20, RED);
					if(InputIP(8, 5, gTerm.tEthernet.szServerIP) != OK)
						return ;
					
					Clear();;
					SetScrFont(FONT20, WHITE);
					TextOut(0, 3, ALIGN_CENTER, "主机端口");
					TextOut(0, 8, ALIGN_CENTER, "保持不变请按确认键");
					SetScrFont(FONT20, RED);
					ret = Input(7, 5, gTerm.tEthernet.szServerPort, 5, IME_NUMBER, BLACK, GREEN, FALSE, FALSE, FALSE);
					if(ret != OK)
						return ;

					
					system("/usr/udhcp/udhcpc");
					Clear();
					TextOut(0, 5, ALIGN_CENTER, "设置成功");
					WaitKey(3000);
				}
				else if(ucKey == KEY_2)
				{
					gTerm.bAutoIP = FALSE;
					if(WriteData("test-term.dat", &gTerm, sizeof(T_TERM), 0) == FALSE)
					{
						Clear();                      
						SetScrFont(FONT20, WHITE);
						TextOut(0, 3, ALIGN_CENTER, "保存文件出错"); 
						TextOut(0, 4, ALIGN_CENTER, "请稍后重试"); 
						FailBeep();
						WaitKey(2000);
				    	//goto FAILED;
					}
					SetEthernet();
				}
				else if(ucKey == KEY_3)
					ViewIpSet();
				else if(ucKey == KEY_4)
					SetMoney();
				else if(ucKey == KEY_CANCEL)
					return ;
				break;
#endif
            case KEY_2:
                querySingle();
                break;	
            case KEY_3:
                query24h();
                break;
			case KEY_4:
				SetCDMA();
				break;	
            case KEY_5:
                while(1)
                   {
					Clear(); 
					TextOut(0, 3, ALIGN_CENTER, "请选择WiFi连接方式");
					TextOut(0, 4, ALIGN_CENTER, "1.手动设置");
					TextOut(0, 5, ALIGN_CENTER, "2.自动设置");

	                ucKey = WaitLimitKey("\x01\x02\x12", 8, 0);
	                if ('\x01' == ucKey)
	                	{
							gTerm.bAutoWifi = FALSE;
	                     	Set_Wifi();	
	                	}
	                if ('\x02' == ucKey)
	                	{
							gTerm.bAutoWifi = TRUE;
	                     	Set_Wifi_Auto();	
	                	}
	                if ('\x12' == ucKey)
                            break;
				}
                break;

		}
#ifdef ALIPAY_QUERY
#if 0
           if (retval == -1) {
                printf("select function error!\n");
                return ERROR;
           } else {
               continue;
           }
#endif
#endif
	}
}

int CommTest()
{
	UCHAR ucKey;
	char sKeyName[20];

	ClearKbd();
        printf("+++++++CommTest() outside while loop+++\n");
	while(1)
	{
		Clear();

        printf("+++++++CommTest() inside while loop+++\n");
		SetScrFont(FONT20, WHITE);

		//标题
		//ShowBmpFile(0, 25, "pic/title.bmp");
		TextOutByPixel(100, 30, "通讯测试");

		//菜单
		ShowBmpFile(30, 60, "pic/button.bmp");
		TextOutByPixel(45, 65, "1.拨号测试");

		ShowBmpFile(30, 95, "pic/button.bmp");
		TextOutByPixel(45, 100, "2.G网测试");

		ShowBmpFile(30, 130, "pic/button.bmp");
		TextOutByPixel(45, 135, "3.以太网");
		
		ShowBmpFile(30, 165, "pic/button.bmp");
		TextOutByPixel(45, 170, "4.C网测试");
		
        ShowBmpFile(30, 200, "pic/button.bmp");
        TextOutByPixel(45, 205, "5.wifi测试");

        ShowBmpFile(170, 60, "pic/button.bmp");
        TextOutByPixel(185, 65, "6.参数设置");

		ucKey = WaitLimitKey("\x01\x02\x03\x04\x05\x06\x12", 7, 0);
		memset(sKeyName, 0, sizeof(sKeyName));
		GetKeyName(ucKey, sKeyName);
		DebugOut("press key: %s\n", sKeyName);

		switch(ucKey)
		{
			case KEY_CANCEL:
				return OK;
				break;
			case KEY_1:
				ModemTest();
				break;
			case KEY_2:
				GprsTest();
				break;
			case KEY_3:
				EthernetTest();
				break;
			case KEY_4:
				CDMACommTest();
				break;	
            case KEY_5:
                WifiCommTest();
                break;
            case KEY_6:
                SetCommParam();
                break;

		}
	}
	return OK;
}


int SetMoney()
{
    int ret;
    char buff[128] = "";
    char buffshow[128] = "";
    int tempDataNum;

    memset(buff, 0, sizeof(buff));
    Clear();
    SetScrFont(FONT20, WHITE);
    TextOut(0, 2, ALIGN_CENTER, "请输入金额");
    TextOut(0, 8, ALIGN_CENTER, "输入完成请按OK键");
    SetScrFont(FONT20, RED);
    ret = InputMoney(8, 5, buff,20); 
    if(ret < 0)
    	goto FAILED;

	printf("\nbefore:%s\n", buff);
    Moneyformat(buff);
    printf("\nafter:%s\n", buff);
    pthread_mutex_lock(&prmutex);
    print_logo();
    ret = generator_qrcode_to_bmp(1,buff);
	//system(buff);	 

    OkBeep();
    Clear();
    SetScrFont(FONT20, WHITE);
    if(ret == 1)
        TextOut(2, 4, ALIGN_CENTER, "链接支付宝失败，请检查网络");
    else {
        //TextOut(2, 4, ALIGN_CENTER, "input money OK!");
        TextOut(2, 4, ALIGN_CENTER, "稍等，正在输出二维码...");
        printTail(buff);
    }
    pthread_mutex_unlock(&prmutex);
    WaitKey(1000);
    return OK;

FAILED:

	SetScrFont(FONT20, WHITE);
    if(ret == -1)
        TextOut(2, 6, ALIGN_CENTER, "error input");
    else if (ret == -3) {
        //TextOut(2, 6, ALIGN_CENTER, "cancel");
        return ERROR;
    }
    else 
        TextOut(2, 6, ALIGN_CENTER, "unknow");
	WaitKey(2000);
	printf("SetMoney failed ret;%d\n", ret);
	return ERROR;
}

char* Moneyformat(char* buf)
{   
    int len = strlen(buf);
    if(len > 2) {  // RMB >1.00
    memmove(buf+len-1, buf+len-2,2);
    buf[len-2] = '.';
    } else if (len ==2 ) { //0.09 RMB < 0.99
    memmove(buf+2,buf,2);
    buf[1] = '.';
    buf[0] = '0';
    } else if (len ==1)  {
    memmove(buf+3,buf,1);
    buf[2] = '0';
    buf[1] = '.';
    buf[0] = '0';
    }
    return buf;
}

unsigned int Money2int(char* buf)
{
    char fee[18] = {0};
    int len = strlen(buf);
    int feeint = 0;
    printf("Money2int buf:%s, strlen(buf):%d\n", buf, len);
    if(buf[len-3] == '.'){ //for these have fen like 1.01 or 0.01
        memcpy(fee,buf,len-3);  
        memcpy(fee+len-3,buf+len-2,2);
    } else if(buf[len-2] == '.'){ //for these have jiao like 1.1 or 0.1
        memcpy(fee,buf,len-2);  
        fee[len-2] = buf[len-1];
        fee[len-1] = '0';     //for we must make 1.1 to 110
    } else {
        memcpy(fee,buf,len);
        fee[len] = '0';   
        fee[len + 1]  = '0';  
    }
    feeint = (unsigned int)atoi(fee);
    printf("Money2int fee:%s, feeint:%d\n", fee, feeint);
    return feeint;
}

void printTail(char* price)
{
    int ret = 0;
    char printBuff[50];
START_PRINT:
    if(CheckPrinter() != TRUE) {
        printf("the printer is not working well!\n");
        goto end1;
    }
    ClearPrintBuff();
    SetPrintIndent(0);
    SetPrintFont(32);
    sprintf(printBuff,"序列号:%lld",query_number);
    FillPrintBuff(printBuff);
    if(strlen(qrout_trade_no) > 0) {
        strcpy(printBuff,"商户订单号:");
        strcat(printBuff,qrout_trade_no);
        FillPrintBuff(printBuff);
        memset(qrout_trade_no,0, 65);
    }
    PrintEmptyLine(1);
    SetPrintIndent(0);
    SetPrintFont(32);
    strcpy(printBuff,"盈润捷通     金额:");
    strcat(printBuff,price);
    FillPrintBuff(printBuff);
    PrintEmptyLine(2);

    



    //开始打印    
    ret =StartPrint();
    DebugOut("print error code:[%d]\n", ret);
    if(ret != 0)
    {   
        if(ret == -1) 
            goto START_PRINT;
        else if(ret == -2) 
            goto end2;
        else if(ret == -3) 
            goto end1;
    }   
    return;
end1:  

    FailBeep(); 
    ClearLine(1, 9);
    TextOut(0, 3, ALIGN_CENTER, "请检查打印机"); 
    TextOut(0, 4, ALIGN_CENTER, "打印失败");
    WaitKey(2000);
    return;

end2:  

    FailBeep(); 
    ClearLine(1, 9);
    TextOut(0, 3, ALIGN_CENTER, "电量不足"); 
    TextOut(0, 4, ALIGN_CENTER, "无法执行打印");
    WaitKey(2000);
    return;
}
#ifdef ALIPAY_GOLDENLAKE
void print_logo()
{
    char PrintBuff[40];
    int ret = 0;
START_PRINT:
    if(CheckPrinter() != TRUE) {
        printf("the printer is not working well!\n");
        goto end1;
    }
    ClearPrintBuff();
    PrintEmptyLine(2);
    memset(PrintBuff,0,sizeof(PrintBuff));
    SetPrintIndent(0);
    SetPrintFont(48);
    strcpy(PrintBuff,"   金湖茶餐厅");
    FillPrintBuff(PrintBuff);
    SetPrintFont(48);
    strcpy(PrintBuff,"    GL Cafe");
    FillPrintBuff(PrintBuff);
    SetPrintFont(24);
    strcpy(PrintBuff,"  北三环东路36号环贸中心D栋1层");
    FillPrintBuff(PrintBuff);
    strcpy(PrintBuff,"      订餐电话：58257262");
    FillPrintBuff(PrintBuff);
    strcpy(PrintBuff,"-----------------------------------");
    FillPrintBuff(PrintBuff);
    PrintEmptyLine(2);
    SetPrintFont(32);
    strcpy(PrintBuff,"    支付宝钱包支付");
    FillPrintBuff(PrintBuff);
 



    //开始打印    
    ret =StartPrint();
    DebugOut("print error code:[%d]\n", ret);
    if(ret != 0)
    {   
        if(ret == -1) 
            goto START_PRINT;
        else if(ret == -2) 
            goto end2;
        else if(ret == -3) 
            goto end1;
    }   
    return;
end1:  

    FailBeep(); 
    ClearLine(1, 9);
    TextOut(0, 3, ALIGN_CENTER, "请检查打印机"); 
    TextOut(0, 4, ALIGN_CENTER, "打印失败");
    WaitKey(2000);
    return;

end2:  

    FailBeep(); 
    ClearLine(1, 9);
    TextOut(0, 3, ALIGN_CENTER, "电量不足"); 
    TextOut(0, 4, ALIGN_CENTER, "无法执行打印");
    WaitKey(2000);
    return;
}
#endif

#ifdef ALIPAY_JIUYI
void print_logo()
{
    char PrintBuff[40];
    int ret = 0;
START_PRINT:
    if(CheckPrinter() != TRUE) {
        printf("the printer is not working well!\n");
        goto end1;
    }
    ClearPrintBuff();
    PrintEmptyLine(2);
    memset(PrintBuff,0,sizeof(PrintBuff));
    SetPrintIndent(0);
    SetPrintFont(48);
    strcpy(PrintBuff,"   九亿粥饼店");
    FillPrintBuff(PrintBuff);
    SetPrintFont(48);
    strcpy(PrintBuff,"     JIU YI");
    FillPrintBuff(PrintBuff);
    SetPrintFont(24);
    strcpy(PrintBuff,"  北京市石景山区实兴大街5号");
    FillPrintBuff(PrintBuff);
    strcpy(PrintBuff,"      订餐电话：88939699");
    FillPrintBuff(PrintBuff);
    strcpy(PrintBuff,"-----------------------------------");
    FillPrintBuff(PrintBuff);
    PrintEmptyLine(2);
    SetPrintFont(32);
    strcpy(PrintBuff,"    支付宝钱包支付");
    FillPrintBuff(PrintBuff);
 



    //开始打印    
    ret =StartPrint();
    DebugOut("print error code:[%d]\n", ret);
    if(ret != 0)
    {   
        if(ret == -1) 
            goto START_PRINT;
        else if(ret == -2) 
            goto end2;
        else if(ret == -3) 
            goto end1;
    }   
    return;
end1:  

    FailBeep(); 
    ClearLine(1, 9);
    TextOut(0, 3, ALIGN_CENTER, "请检查打印机"); 
    TextOut(0, 4, ALIGN_CENTER, "打印失败");
    WaitKey(2000);
    return;

end2:  

    FailBeep(); 
    ClearLine(1, 9);
    TextOut(0, 3, ALIGN_CENTER, "电量不足"); 
    TextOut(0, 4, ALIGN_CENTER, "无法执行打印");
    WaitKey(2000);
    return;
}
#endif

//move Query print to a thread. to avoid missing key issue.
#ifdef ALIPAY_QUERY
void *thr_fn(void* arg)
{
    int ret = 0;
#ifdef ALIPAY_QUERY
    fd_set rset;
    struct timeval tv;
    int retval;
    int i;
#endif
#ifdef ALIPAY_QUERY
    int connection_fd;
    socklen_t address_length;
    int nbytes; 
    char buffer[1024];
    int trade_num;
    char *trade_ptr[100] = {NULL}; 
    char *trade_detail[5] = {NULL}; 
    struct receipt_info pos_receipt;
    char PrintBuff[30];
    T_DATETIME tTime;
    char pos_date[12];
    char pos_time[12];
#endif

#ifdef ALIPAY_QUERY
    int maxfd = 0;
    if(socket_fd != 0)
        maxfd = max(maxfd,socket_fd);
#endif
    while(1) {
        FD_ZERO(&rset);
        FD_SET(socket_fd, &rset);
        address_length = sizeof(address);

        /* Wait up to one seconds. */
        tv.tv_sec = 1;
        tv.tv_usec = 0; 
        retval = select(maxfd+1, &rset, NULL, NULL, &tv);
        //retval = select(maxfd+1, &rset, NULL, NULL, NULL);
        printf("select got return,go before FD_ISSET-----\n");
        if(FD_ISSET(socket_fd, &rset)) {
            if ((connection_fd = accept(socket_fd,
                            (struct sockaddr *) &address,
                            &address_length)) > -1)
            {

                nbytes = read(connection_fd, buffer, 1024);
                buffer[nbytes] = 0;

                printf("MESSAGE FROM ALIPAY: %s\n", buffer);
                //nbytes = snprintf(buffer, 256, "hello from the server");
                //write(connection_fd, buffer, nbytes);
                /* start print out the payment query result */

                trade_num = SplitStr(buffer,trade_ptr,"|");

                //write(tty_data.posfd,alipay_receipt,sizeof(alipay_receipt));
                ///write(tty_data.posfd,"\n",1);
                /* get system time */
                //time(&td);
                //ptr = localtime(&td);
                //strftime(pos_date,sizeof(pos_date),"%Y-%m-%d",ptr);
                //strftime(pos_time,sizeof(pos_time),"%H:%M:%S",ptr);
                GetDateTime(&tTime);
                sprintf(pos_date,"%s%s-%s-%s",tTime.century,tTime.year,tTime.month,tTime.day);
                sprintf(pos_time,"%s:%s:%s",tTime.hour,tTime.minute,tTime.second);

                for (i=0; i<trade_num; i++){
                    printf("number %d trade:%s\n",i,trade_ptr[i]);
                    SplitStr(trade_ptr[i],trade_detail,",");
                    memset(pos_receipt.serial_number,0,24);
                    memset(pos_receipt.out_trade_no,0,16);
                    memset(pos_receipt.trade_no,0,32);
                    memset(pos_receipt.total_fee,0,16);

                    strcpy(pos_receipt.serial_number,trade_detail[0]);
                    strcpy(pos_receipt.out_trade_no,trade_detail[1]);
                    strcpy(pos_receipt.trade_no,trade_detail[2]);
                    strcpy(pos_receipt.total_fee,trade_detail[3]);
                    ///WritePayment(1, &pos_receipt);
                    /// write(tty_data.posfd,"\n",1);
                    ///write(tty_data.posfd,"\n",1);
                    pthread_mutex_lock(&prmutex);
START_PRINT:
                    if(CheckPrinter() != TRUE) {
                        printf("the printer is not working well!\n");
                        pthread_mutex_unlock(&prmutex);
                        goto end1;
                    }
                    ClearPrintBuff();
                    memset(PrintBuff,0,30);
                    SetPrintIndent(100);
                    SetPrintFont(32);
                    strcpy(PrintBuff,"支付宝交易凭条");
                    FillPrintBuff(PrintBuff);
                    PrintEmptyLine(2);
                    SetPrintIndent(0);
                    SetPrintFont(32);
                    strcpy(PrintBuff,"序列号：");
                    FillPrintBuff(PrintBuff);
                    FillPrintBuff(pos_receipt.serial_number);

                    strcpy(PrintBuff,"商户订单号：");
                    FillPrintBuff(PrintBuff);
                    FillPrintBuff(pos_receipt.out_trade_no);

                    strcpy(PrintBuff,"日期：");
                    FillPrintBuff(PrintBuff);
                    FillPrintBuff(pos_date);

                    strcpy(PrintBuff,"时间：");
                    FillPrintBuff(PrintBuff);
                    FillPrintBuff(pos_time);

                    strcpy(PrintBuff,"-------------------");
                    FillPrintBuff(PrintBuff);	   

                    strcpy(PrintBuff,"支付宝当面付");
                    FillPrintBuff(PrintBuff);	   

                    strcpy(PrintBuff,"交易号：");
                    FillPrintBuff(PrintBuff);
                    FillPrintBuff(pos_receipt.trade_no);

                    strcpy(PrintBuff,"金额：");
                    FillPrintBuff(PrintBuff);
                    FillPrintBuff(pos_receipt.total_fee);

                    strcpy(PrintBuff,"签名 ");
                    FillPrintBuff(PrintBuff);

                    strcpy(PrintBuff,"本人同意上述交易");
                    FillPrintBuff(PrintBuff);

                    PrintEmptyLine(3);	 

                    //开始打印    
                    ret =StartPrint();
                    pthread_mutex_unlock(&prmutex);
                    DebugOut("print error code:[%d]\n", ret);
                    if(ret != 0)
                    {   
                        if(ret == -1) 
                            goto START_PRINT;
                        else if(ret == -2) 
                            goto end2;
                        else if(ret == -3) 
                            goto end1;
                    }   
                    goto normal;
end1:  

                    FailBeep(); 
                    ClearLine(1, 9);
                    TextOut(0, 3, ALIGN_CENTER, "请检查打印机"); 
                    TextOut(0, 4, ALIGN_CENTER, "打印失败");
                    WaitKey(2000);


                    goto normal;
end2:  

                    FailBeep(); 
                    ClearLine(1, 9);
                    TextOut(0, 3, ALIGN_CENTER, "电量不足"); 
                    TextOut(0, 4, ALIGN_CENTER, "无法执行打印");
                    WaitKey(2000);

normal:
                }
                close(connection_fd);
            }
        }
    }
    return NULL;
}
#endif


void querySingle(void)
{
    char prefix[12] = {0};
    unsigned long long prefixint;
    char hmno[7] = {0};
    char queryNo[18] = {0};
    int ret = 0;
    Clear();
    TextOut(0, 2, ALIGN_LEFT, "请输入交易单上序列号的后6位");
    TextOut(0, 3, ALIGN_LEFT, "查询当日交易");
    getSNoPre(prefix);
    printf("queryNO prefixint:\n");   
    printf("queryNO prefix:%s \n", prefix);   
    //sprintf(prefix, "%lld\0", prefixint);
    //TextOut(0, 5, ALIGN_LEFT, prefix);
    memcpy(queryNo,prefix,11);
    ret = Input(0,5,queryNo,17,IME_NUMBER,WHITE, RED,FALSE,TRUE,FALSE);
    if(ret != OK)
        return;
    //memcpy(queryNo,prefix,11);
    //memcpy(queryNo+11,hmno,6);
    printf("queryNo:%s\n",queryNo);
    ret = alipay_query_single((unsigned long long)atoll(queryNo));
    if(ret)
    {
        char PrintBuff[30];
        Clear();
        TextOut(0, 2, ALIGN_CENTER, "该单交易已成功");
        TextOut(0, 3, ALIGN_CENTER, queryNo); 
START_PRINT:
        pthread_mutex_lock(&prmutex);
        if(CheckPrinter() != TRUE) {
            printf("the printer is not working well!\n");
            pthread_mutex_unlock(&prmutex);
            goto end1;
        }
        ClearPrintBuff();
        memset(PrintBuff,0,30);
        SetPrintIndent(100);
        SetPrintFont(32);
        strcpy(PrintBuff,"以下交易确已成功");
        FillPrintBuff(PrintBuff);
        strcpy(PrintBuff,"序列号:");
        FillPrintBuff(PrintBuff);
        strcpy(PrintBuff,queryNo);
        FillPrintBuff(PrintBuff);	   
        PrintEmptyLine(3);	 
        ret =StartPrint();
        pthread_mutex_unlock(&prmutex);
        DebugOut("print error code:[%d]\n", ret);
        if(ret != 0)
        {   
            if(ret == -1) 
                goto START_PRINT;
            else if(ret == -2) 
                goto end2;
            else if(ret == -3) 
                goto end1;
        }   
        goto normal;
end1:  

        FailBeep(); 
        ClearLine(1, 9);
        TextOut(0, 3, ALIGN_CENTER, "请检查打印机"); 
        TextOut(0, 4, ALIGN_CENTER, "打印失败");
        WaitKey(2000);

        return ERROR;
end2:  

        FailBeep(); 
        ClearLine(1, 9);
        TextOut(0, 3, ALIGN_CENTER, "电量不足"); 
        TextOut(0, 4, ALIGN_CENTER, "无法执行打印");
        WaitKey(2000);

        return ERROR;

normal:
    } else {
        Clear();
        TextOut(0, 2, ALIGN_CENTER, "该单交易失败");
        WaitKey(2000);
    }

}


void query24h(void)
{
    int ret = 0, i;
    int trade_num;
    char trade_numstr[64] = {0};
    char *trade_ptr[2000] = {NULL}; 
    char *trade_detail[5] = {NULL}; 
    char pos_date[12];
    char pos_time[12];
    T_DATETIME tTime;
    struct receipt_info pos_receipt;
    char PrintBuff[100];
    unsigned int total24h_fee = 0;
    char total24h_feestr[16] = {0};
    Clear();
    TextOut(0, 3, ALIGN_LEFT, "查询近24小时成功交易");
    ret = alipay_query_24h(result24h);

    trade_num = SplitStr(result24h,trade_ptr,"|");

    pthread_mutex_lock(&prmutex);
START_PRINT:
    if(CheckPrinter() != TRUE) {
        printf("the printer is not working well!\n");
        pthread_mutex_unlock(&prmutex);
        goto end1;
    }
    ClearPrintBuff();
    memset(PrintBuff,0,sizeof(PrintBuff));
    SetPrintIndent(0);
    SetPrintFont(24);

#ifdef ALIPAY_GOLDENLAKE
    strcpy(PrintBuff,"北京金湖餐饮有限公司金湖环贸店");
#endif
#ifdef ALIPAY_JIUYI
    strcpy(PrintBuff,"北京九亿升辉餐饮管理有限公司");
#endif

    FillPrintBuff(PrintBuff);	   
    GetDateTime(&tTime);
    sprintf(pos_date,"%s%s-%s-%s",tTime.century,tTime.year,tTime.month,tTime.day);
    sprintf(pos_time,"%s:%s:%s",tTime.hour,tTime.minute,tTime.second);
    strcpy(PrintBuff,"日期时间：");
    strcat(PrintBuff,pos_date);
    strcat(PrintBuff," ");
    strcat(PrintBuff,pos_time);
    FillPrintBuff(PrintBuff);
    PrintEmptyLine(1);	 
    strcpy(PrintBuff,"------------------");
    FillPrintBuff(PrintBuff);

    ret = StartPrint();
    DebugOut("print error code:[%d]\n", ret);
    if(ret != 0) {   
        if(ret == -1)
            goto START_PRINT;
        else if(ret == -2)
            goto end2;
        else if(ret == -3)
            goto end1;
    }
    ClearPrintBuff();
    memset(PrintBuff,0,sizeof(PrintBuff));
    SetPrintIndent(0);
    SetPrintFont(24);

    for (i=0; i<trade_num; i++){
        printf("number %d trade:%s\n",i,trade_ptr[i]);
        SplitStr(trade_ptr[i],trade_detail,",");
        memset(pos_receipt.serial_number,0,24);
        memset(pos_receipt.out_trade_no,0,16);
        memset(pos_receipt.trade_no,0,32);
        memset(pos_receipt.total_fee,0,16);

        strcpy(pos_receipt.serial_number,trade_detail[0]);
        strcpy(pos_receipt.out_trade_no,trade_detail[1]);
        strcpy(pos_receipt.trade_no,trade_detail[2]);
        strcpy(pos_receipt.total_fee,trade_detail[3]);

        total24h_fee += Money2int(trade_detail[3]);

        printf("total24h_fee:%d", total24h_fee);
        strcpy(PrintBuff,"序列号：");
        strcat(PrintBuff, pos_receipt.serial_number);
        FillPrintBuff(PrintBuff);	   
        strcpy(PrintBuff,"商户订单号：");
        strcat(PrintBuff, pos_receipt.out_trade_no);
        FillPrintBuff(PrintBuff);	   
        strcpy(PrintBuff,"交易号：");
        strcat(PrintBuff,  pos_receipt.trade_no);
        FillPrintBuff(PrintBuff);	   
        strcpy(PrintBuff,"金额：");
        strcat(PrintBuff, pos_receipt.total_fee);
        FillPrintBuff(PrintBuff);	   
        strcpy(PrintBuff,"------------------");
        FillPrintBuff(PrintBuff);
        if(i%5 == 0)  {
            //because of FillPrintBuff may overflow so print segmentlly
            ret = StartPrint();
            DebugOut("print error code:[%d]\n", ret);
            if(ret != 0) {   
                if(ret == -1) 
                    goto START_PRINT;
                else if(ret == -2)
                    goto end2;
                else if(ret == -3)
                    goto end1;
            }
            ClearPrintBuff();
            memset(PrintBuff,0,sizeof(PrintBuff));
            SetPrintIndent(0);
            SetPrintFont(24);
        }
    }
    
    strcpy(PrintBuff,"=====================");
    FillPrintBuff(PrintBuff);
    ret = StartPrint();
    DebugOut("print error code:[%d]\n", ret);
    printf("print error code:[%d]\n", ret);
    if(ret != 0) {   
        if(ret == -1)
            goto START_PRINT;
        else if(ret == -2)
             goto end2;
        else if(ret == -3)
             goto end1;
    }
    ClearPrintBuff();
    memset(PrintBuff,0,sizeof(PrintBuff));
    SetPrintIndent(0);
    SetPrintFont(32);

    sprintf(total24h_feestr,"%d", total24h_fee);
    printf("\nbefore:%s\n", total24h_feestr);
    Moneyformat(total24h_feestr);
    printf("\nafter:%s\n", total24h_feestr);
    printf("total24h_feestr:%s", total24h_feestr);
    strcpy(PrintBuff,"总金额：");
    strcat(PrintBuff, total24h_feestr);
    FillPrintBuff(PrintBuff);	   
    //memset(PrintBuff,0,sizeof(PrintBuff));
    sprintf(trade_numstr, "总单数:%d", trade_num);
    strcpy(PrintBuff, trade_numstr);
    FillPrintBuff(PrintBuff);	   
    PrintEmptyLine(3);	 
    ret = StartPrint();
    pthread_mutex_unlock(&prmutex);
    DebugOut("print error code:[%d]\n", ret);
    if(ret != 0) 
    {
        if(ret == -1)
            goto START_PRINT;
        else if(ret == -2)
             goto end2;
        else if(ret == -3)
             goto end1;
    }   
    goto normal;
end1:  
    FailBeep(); 
    ClearLine(1, 9);
    TextOut(0, 3, ALIGN_CENTER, "请检查打印机"); 
    TextOut(0, 4, ALIGN_CENTER, "打印失败");
    WaitKey(2000);
    
    return ERROR;
end2:  

    FailBeep(); 
    ClearLine(1, 9);
    TextOut(0, 3, ALIGN_CENTER, "电量不足"); 
    TextOut(0, 4, ALIGN_CENTER, "无法执行打印"); 
    WaitKey(2000);

    return ERROR;

normal:
}


