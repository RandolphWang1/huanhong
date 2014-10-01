#include "Main.h"


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

	ClearKbd();
	while(1)
	{
		Clear();

		SetScrFont(FONT20, WHITE);

		//标题
		//ShowBmpFile(0, 25, "pic/title.bmp");
		TextOutByPixel(95, 30, "通讯参数设置");

		//菜单
		ShowBmpFile(90, 60, "pic/button.bmp");
		TextOutByPixel(105, 65, "1.拨号");

		ShowBmpFile(90, 95, "pic/button.bmp");
		TextOutByPixel(105, 100, "2.G网");

		ShowBmpFile(90, 130, "pic/button.bmp");
		TextOutByPixel(105, 135, "3.以太网");

        ShowBmpFile(90, 165, "pic/button.bmp");
		TextOutByPixel(105, 170, "4.C网");
		
        ShowBmpFile(90, 200, "pic/button.bmp");
        TextOutByPixel(105, 205, "5.wifi网");

                ucKey = WaitLimitKey("\x01\x02\x03\x04\x05\x12", 6, 0);
		memset(sKeyName, 0, sizeof(sKeyName));
		GetKeyName(ucKey, sKeyName);
		DebugOut("press key: %s\n", sKeyName);

		switch(ucKey)
		{
			case KEY_CANCEL:
				return;
				break;
			case KEY_1:
				SetModem();
				break;
			case KEY_2:
				SetGPRS();
				break;
			case KEY_3:
				Clear();
				TextOut(0, 0, ALIGN_CENTER, "IP 获取方式");
				
				TextOut(0, 4, ALIGN_LEFT, "  1.自动获取IP");
				TextOut(0, 5, ALIGN_LEFT, "  2.固定设置IP");
				TextOut(0, 6, ALIGN_LEFT, "  3.查看当前ip设置");
				ucKey = WaitLimitKey("\x12\x01\x02\x03", 4, 0);
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
				else if(ucKey == KEY_CANCEL)
					return ;
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
	}
}

int CommTest()
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


