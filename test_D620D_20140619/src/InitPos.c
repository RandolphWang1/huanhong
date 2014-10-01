#include "Main.h"

//对POS关键模块进行初始化检测
INT InitPOS(void)
{
	int ret;
	char buff[100];
	char ckey;
	char szBuff[100];
	//int handle = 0;
	
	InitSDK();
	Clear();

	memset(buff, 0, sizeof(buff));
	GetSDKVersion(buff);
	printf("SDK VERSION: [%s]\n", buff);
	printf("APP VERSION: [%s]\n", APP_VERSION);

	SetScrFont(FONT20, WHITE);

	//初始化控制器
	DebugOut("初始化control...\n");
	ret = InitControl();
	//电池有问题
	if(ret == -2)
	{
		DebugOut("初始化时钟...\n");
		if(InitRTC() != OK)
		{
			SetScrFont(FONT24, RED);
			TextOut(0, 1, ALIGN_LEFT, "   纽扣电池异常!");
			DebugOut("初始化control-纽扣电池异常!\n");
			Wait(1000);
			return ERROR;
		}
	}
	else if(ret != OK)
	{
		SetScrFont(FONT24, RED);
		TextOut(0, 1, ALIGN_LEFT, "   初始化control错误!");
		DebugOut("初始化control错误!\n");
		Wait(1000);
		return ERROR;
	}
	else
		DebugOut("初始化control成功!\n");

	//PowerOnMC55();
	M35_power_on();

	DebugOut("初始化LCD...\n");
	//初始化LCD
	if(InitLcd() != OK)
	{
		FailBeep();
		SetScrFont(FONT24, RED);
		TextOut(0, 1, ALIGN_LEFT, "    LCD初始化错误!");
		DebugOut("LCD初始化错误!\n");
		Wait(1000);
		return ERROR;
	}

	//设定壁纸
	SetWallPaper(1);
	//ShowBmpFile(0, 0, "pic/wallpaper1.bmp");
	ShowBmpFile(0, 0, "pic/main_bk.bmp");
	SetWallPaper(0);

	TextOut(0, 1, ALIGN_LEFT, "    初始化LCD...");
	Wait(500);
	TextOut(0, 2, ALIGN_LEFT, "    LCD初始化成功!");
	DebugOut("LCD初始化成功!\n");

	TextOut(0, 3, ALIGN_LEFT, "    初始化键盘...");
	//初始化键盘
	DebugOut("初始化键盘...\n");
	Wait(500);
	if(InitKbd() != OK)
	{
		FailBeep();
		SetScrFont(FONT24, RED);
		TextOut(0, 4, ALIGN_LEFT, "    键盘初始化错误!");
		DebugOut("键盘初始化错误!\n");
		return ERROR;
	}

	//使能蜂鸣声
	//EnableBeep(TRUE);

	TextOut(0, 4, ALIGN_LEFT, "    键盘初始化成功!");
	DebugOut("键盘初始化成功!\n");

	//设定LCD亮度
	SetLcdLight(3);
	SetShutBkLightTime(0);

	TextOut(0, 5, ALIGN_LEFT, "    初始化打印机...");
	//初始化打印机
	DebugOut("初始化打印机...\n");
	Wait(500);
	ret = InitPrinter();
	DebugOut("InitPrinter ret:[%d]\n", ret);
	if(ret < 0)
	{ 
		FailBeep();
		SetScrFont(FONT24, RED);
		TextOut(0, 6, ALIGN_LEFT, "    打印机初始化错误!");
		DebugOut("打印机初始化错误!\n");
	}
	else
	{
		TextOut(0, 6, ALIGN_LEFT, "    打印机初始化成功!");
		DebugOut("打印机初始化成功!\n");
	}
/*
	SwitchCom2_Rf(1);
	TextOut(0, 7, ALIGN_LEFT, "    初始化非接...");
	DebugOut("初始化非接...\n");
	if(-1 == FM17dcInit(2, 19200))
	{
     		FM17dcExit(iM1Handle);
            gTerm.bIsDC = TRUE;
			TextOut(0, 8, ALIGN_LEFT, "    非接初始化成功!");
			DebugOut("非接初始化成功!\n");
	}else if (OK != FM17dcInit(2, 19200))
    {
        gTerm.bIsDC = FALSE;
		TextOut(0, 8, ALIGN_LEFT, "   非接初始化成功!");
		DebugOut("非接初始化成功!\n");
	}
	*/
	//初始化扫描头
	switch_CTSRTS(0);   //CTS/RTS暂无别的模块使用.初始化执行一次即可.
	
	Clear(); 
	TextOut(0, 4, ALIGN_CENTER, "是否初始化扫描头?");
    	ckey=WaitKey(0);
	
    	//ckey= KEY_ENTER;
    	if(ckey== KEY_ENTER)
    	{
    		switch_COM2(0);     //如果有外接清华非接模块和扫描循环调用,则每次扫描前都执行该句.

	    	Clear();
///////////////////////////////////////////////////////////////////////
		/*memset(buff, 0, sizeof(buff));
		handle = SERIAL_Open("/dev/ttyS2", 9600);
		SERIAL_Read(handle, buff,100);
		*/
///////////////////////////////////////////////////////////////////////
		ret=se955_init() ;
		if(ret<0)
		{
		    FailBeep();
			SetScrFont(FONT24, RED);
			TextOut(0, 4, ALIGN_LEFT, "扫描头没有安装,或者通道1被占用");
			DebugOut("error code:[%d]\n",ret);
			iScanHandle=FALSE;
			WaitKey(2000);
		}
		else
		{
			iScanHandle=TRUE;
			DebugOut("error code:[%d]\n",ret);
		}
    	}
	else
	{
		iScanHandle=FALSE;
	}
	SetScrFont(FONT20, WHITE);
	Clear();
	//读取配置文件
	TextOut(0, 7, ALIGN_LEFT, "    读取配置文件...");
	DebugOut("读取配置文件\n");
	Wait(500);
   	memset(&gTerm, 0,sizeof(gTerm));
	if(!ReadData("test-term.dat", &gTerm,sizeof(T_TERM), 0))
	{
		TextOut(0, 8, ALIGN_LEFT, "    初始化配置文件..."); 
		DebugOut("初始化配置文件...\n");
		InitTerm();
		Wait(1000);
		TextOut(0, 9, ALIGN_LEFT, "    初始化配置文件成功!"); 
		DebugOut("初始化配置文件成功!\n");
	}
	else
	{
		TextOut(0, 8, ALIGN_LEFT, "    读取配置文件成功!"); 
		DebugOut("读取配置文件成功!\n");
	}

	memset(&gSMS, 0,sizeof(T_SMS));
	if(!ReadData("test-sms.dat", &gSMS,sizeof(T_SMS), 0))
	{
		TextOut(0, 8, ALIGN_LEFT, "    初始化配置文件..."); 
		DebugOut("初始化配置文件...\n");
		InitSMS();
		Wait(1000);
		TextOut(0, 9, ALIGN_LEFT, "    初始化配置文件成功!"); 
		DebugOut("初始化配置文件成功!\n");
	}
	else
	{
		DebugOut("读取配置文件成功!\n");
	}
	

	if(gTerm.bAutoIP == TRUE)
	{
		system("/usr/udhcp/udhcpc");
	} 
	else 
	{
	
		//TextOutByPixel(45, 65, "当前IP模式：固定IP");
		memset(szBuff, 0, sizeof(szBuff));
		strcat(szBuff, "ifconfig eth0 ");
		strcat(szBuff, gTerm.tEthernet.szLocalIP);
		strcat(szBuff, " netmask ");
		strcat(szBuff, gTerm.tEthernet.szMask);
		//strcat(szBuff, " up");	
		DebugOut("\n%s\n", szBuff);
		system(szBuff);	

		//关闭网关
		memset(szBuff, 0, sizeof(szBuff));
		strcat(szBuff, "route del default gw ");
		strcat(szBuff, gTerm.tEthernet.szGateway);
		DebugOut("\n%s\n", szBuff);
		system(szBuff);

		//重启网关
		memset(szBuff, 0, sizeof(szBuff));
		strcat(szBuff, "route add default gw ");
		strcat(szBuff, gTerm.tEthernet.szGateway);
		DebugOut("\n%s\n", szBuff);
		system(szBuff);  

		memset(szBuff, 0, sizeof(szBuff));
		strcat(szBuff, "ifconfig eth0 down && ifconfig eth0 up ");
		DebugOut("\n%s\n", szBuff);
		system(szBuff);	 
	}
	TextOut(0, 8, ALIGN_CENTER, "以太网配置完成!");


	return OK;
}

