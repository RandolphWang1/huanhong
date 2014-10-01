#include "Main.h"



void TestInput()
{
	int ret;
	char buff[200];

	Clear();
	TextOut(0, 1, ALIGN_LEFT, "   请输入:");
	memset(buff, 0, sizeof(buff));
	ret = Input(2, 3, buff, 50, IME_NUMBER, BLACK, GREEN, FALSE, FALSE, TRUE);
	if(ret != OK)
    {
    	DebugOut("ret=[%d]\n", ret);
		return;
	}

	ClearClient();
	TextOut(0, 1, ALIGN_LEFT, "输入内容如下:");
	TextOut(0, 3, ALIGN_LEFT, buff);

	WaitKey(0);
}

void TestInputMoney()
{
	int ret;
	char buff[200];

	ClearClient();
	TextOut(0, 1, ALIGN_LEFT, "   请输入:");
	memset(buff, 0, sizeof(buff));
	ret = InputMoney(2, 3, buff, 50);
	if(ret != OK)
    {
    	DebugOut("ret=[%d]\n", ret);
		return;
	}

	ClearClient();
	TextOut(0, 1, ALIGN_LEFT, "输入内容如下:");
	TextOut(0, 3, ALIGN_LEFT, buff);

	WaitKey(0);
}

void TestInputDate()
{
	int ret;
	char buff[200];
	int len;

	ClearClient();
	TextOut(0, 1, ALIGN_LEFT, "   请输入:");
	memset(buff, 0, sizeof(buff));
	ret = InputDate(2, 3, buff, &len, 0, BLACK, GREEN, FONT24);
	if(ret != OK)
    {
    	DebugOut("ret=[%d]\n", ret);
		return;
	}

	ClearClient();
	TextOut(0, 1, ALIGN_LEFT, "输入内容如下:");
	TextOut(0, 3, ALIGN_LEFT, buff);

	WaitKey(0);
}

void PrintIMG()
{
	int ret;
	int i;

	ClearClient();
	TextOut(0, 4, ALIGN_CENTER, "正在打印...");

	for(i = 0; i < 3; i ++)
	{
		ret = PrintBMP(0, "pic/print.bmp");
		DebugOut("PrintBMP ret:[%d]\n", ret);
		if(ret != OK)
		{
			FailBeep();
			ClearClient();
			TextOut(0, 4, ALIGN_CENTER, "打印失败");
			WaitKey(2000);
		}
	}
}

void CopyData()
{
	Clear();
	TextOut(0, 4, ALIGN_CENTER, "正在压缩...");
	system("cd /usr/local/; tar zcvf sys.tar.gz *");
	Clear();
	TextOut(0, 4, ALIGN_CENTER, "正在拷贝...");
	if(CopyFile2U("/usr/local/sys.tar.gz", "/") != OK)
	{
		FailBeep();
		Clear();
		TextOut(0, 4, ALIGN_CENTER, "没有usb盘或者usb盘没放置好!");
		WaitKey(0);
		return;
	}

	system("cd /usr/local/; rm sys.tar.gz; cd /usr/local/D620D/");

	OkBeep();
	Clear();
	TextOut(0, 3, ALIGN_CENTER, "拷贝成功");
	WaitKey(3000);
}

void OtherTest()
{
	char cKey;
	char pwd[6+1] = "";
	int ret;
	T_IDENTIFY_INFO identify_info;
	char buff[100];
	int i;

	Clear();
	TextOut(0, 2, ALIGN_LEFT, "   请输入密码:");
	SetScrFont(FONT20, RED);
	memset(pwd, 0, sizeof(pwd));
	ret = Input(18, 2, pwd, 6, IME_NUMBER, BLACK, GREEN, TRUE, FALSE, TRUE);
	ReStoreScrFont();
	if(ret != OK)
    {
    	DebugOut("ret=[%d]\n", ret);
		return;
	}

	if(strcmp(pwd, "0"))
	{
    	DebugOut("password error!\n");
		Clear();
		TextOut(0, 3, ALIGN_CENTER, "密码错误");
		FailBeep();
		WaitKey(2000);
		return;
	}

	while(1)
	{
		ClearClient();
		SetScrFont(FONT20, GREEN);
		TextOut(0, 0, ALIGN_CENTER, "其它测试");
		TextOut(0, 2, ALIGN_LEFT, " 1.标准输入法测试");
		TextOut(0, 3, ALIGN_LEFT, " 2.金额输入法测试");
		TextOut(0, 4, ALIGN_LEFT, " 3.日期输入法测试");
		TextOut(0, 5, ALIGN_LEFT, " 4.神思身份证阅读器");
		TextOut(0, 6, ALIGN_LEFT, " 5.DES加密测试");
		TextOut(0, 7, ALIGN_LEFT, " 6.打印图片测试");
		TextOut(0, 8, ALIGN_LEFT, " 7.备份数据");	//拷贝/usr/local/目录下所有压缩包到U盘
		TextOut(0, 9, ALIGN_LEFT, " 8.打印QR二维码");	
		

		cKey = WaitKey(0);

		switch(cKey)
		{
			case KEY_CANCEL:
				ReStoreScrFont();
				return;
				break;
			case KEY_1:
				TestInput();
				break;
			case KEY_2:
				TestInputMoney();
				break;
			case KEY_3:
				TestInputDate();
				break;
			case KEY_4:
				ReadIdentify(&identify_info);
				break;
			case KEY_5:
				memset(buff, 0, sizeof(buff));
				function_des(1, "\x12\x34\x56\x78", "12345678", buff);
				printf("DES result:\n");
				for(i = 0; i < 8; i ++)
					printf("%02x ", buff[i]);
				printf("\n");
				break;
			case KEY_6:
				PrintIMG();
				break;
			case KEY_7:
				CopyData();
				break;
			case KEY_8:
				PrinterQRTest();
				break;
		}
	}

}



