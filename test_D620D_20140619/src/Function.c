#include "Main.h"
#include <sys/wait.h>
#include <sys/mman.h>
#include "qrgenerator.h"


void UpdateTms()
{
	char ckey;
	system("cd /usr/local/D620D/;./tms");
	Clear();
	PutsXY(0, 4, "请手动重启POS...");
	PutsXY(0, 8, "提示远程更新后请重启POS");
	ckey =GetKey("\x12\x0f");
        if (ckey == KEY_CANCEL)
        	return ;
	ClearLine(1,9);
	while(1)
	{
		PutsXY(0, 4, "请手动重启POS...");
	}
}


void UpdateAppFromU()
{
	char cKey;

	Clear();
	TextOut(0, 3, ALIGN_CENTER, "更新应用程序");
	TextOut(0, 5, ALIGN_CENTER, "【确定】/【取消】");

	cKey = WaitLimitKey("\x12\x0F", 2, 0);
	if(cKey == KEY_CANCEL)
		return;

	system("rm D620D.tar.gz");
	Clear();
	TextOut(0, 4, ALIGN_CENTER, "正在更新...");
	if(GetFileFromU("/usr/local/", "D620D.tar.gz") == OK)
		system("tar zxvf /usr/local/D620D.tar.gz");
	else
	{
		FailBeep();
		Clear();
		TextOut(0, 4, ALIGN_CENTER, "没有usb盘或者usb盘没放置好!");
		WaitKey(0);
		return;
	}

	OkBeep();
	Clear();
	TextOut(0, 3, ALIGN_CENTER, "更新成功");
	TextOut(0, 4, ALIGN_CENTER, "请重新启动POS");
	while(1);
	return;
}

int OtherFunc()
{
	UCHAR ucKey;
	ClearKbd();
	while(1)
	{
		Clear();

		SetScrFont(FONT20, WHITE);

		//菜单
		ShowBmpFile(90, 60, "pic/button.bmp");
		TextOutByPixel(100, 65, "1.软件版本");

		ShowBmpFile(90, 95, "pic/button.bmp");
		TextOutByPixel(100, 100, "2.软件更新");

		ShowBmpFile(90, 130, "pic/button.bmp");
		TextOutByPixel(100, 135, "3.短信");



		ucKey = WaitLimitKey("\x01\x02\x12\x03",4, 0);

		switch(ucKey)
		{
			case KEY_CANCEL:
			case 0:
				return OK;
				break;
			case 1:
				Clear();
				SetScrFont(FONT20, WHITE);
				TextOut(0, 4, ALIGN_CENTER, "D620D功能测试程序");
				TextOut(0, 6, ALIGN_CENTER, APP_VERSION);
				WaitKey(0);
				break;
			case 2:
				UpdateAppFromU();
				break;
			case 3:
				//SMS_Demo();
                                generator_qrcode_to_bmp(1,"1.00"); 
				break;

		}
	}
	return OK;
}

void ClearClient()
{
	Clear();
	FillRect(0, 319, 0, 227, BLACK);
	DrawRect(0, 319, 0, 227, GREEN);
}

void ScanBarCode()
{
	char code_type[2];
	char code_data[500];
	char buff[500];
	int ret;

	if(!iScanHandle)
	{
		Clear();
		TextOut(0, 3, ALIGN_CENTER, "没有安装扫描头,或者初始化失败");
		WaitKey(5000);
		return;
	}
	switch_COM2(0);
	//switch_CTSRTS(0);
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

	while(1)
	{
	    Clear(); 
		TextOut(0, 3, ALIGN_CENTER, "请扫描...");
		memset(buff, 0, sizeof(buff));
		if(se955_scan(buff, 50) <= 0)
		{
		       Clear(); 
			TextOut(0, 3, ALIGN_CENTER, "扫描超时");
			WaitKey(1000);
			goto SELECT;
		}
	    Clear();
		code_type[0] = buff[0];

		memset(code_data, 0, sizeof(code_data));
		strcpy(code_data, buff+1);

		memset(buff, 0, sizeof(buff));
		sprintf(buff, "  条码类型: %02x", code_type[0]);
		TextOut(0, 3, ALIGN_LEFT, buff);

		memset(buff, 0, sizeof(buff));
		sprintf(buff, "  条码内容: %s", code_data);
		DebugOut("%s\n",buff);
		TextOut(0,4, ALIGN_LEFT, buff);
SELECT:
		TextOut(0, 8, ALIGN_CENTER, "【OK】键继续扫描");
		TextOut(0, 9, ALIGN_CENTER, "【CANCEL】键退出");
		if(WaitLimitKey("\x0F\x12", 2, 0) == KEY_CANCEL)
			break;
	}
}


int SMS_Demo()
{
	UCHAR ucKey;
	int i;
	ClearKbd();
	while(1)
	{
		Clear();
		TextOut(0,0,ALIGN_CENTER,"短信");
		SetScrFont(FONT20, WHITE);
		
		//菜单
		ShowBmpFile(90, 60, "pic/button.bmp");
		TextOutByPixel(100, 65, "1.写短信");

		ShowBmpFile(90, 95, "pic/button.bmp");
		TextOutByPixel(100, 100, "2.新短信");

		ShowBmpFile(90, 130, "pic/button.bmp");
		TextOutByPixel(100, 135, "3.收件箱");

		ShowBmpFile(90, 165, "pic/button.bmp");
		TextOutByPixel(100, 170, "4.设置");

		ShowBmpFile(90, 200, "pic/button.bmp");
		TextOutByPixel(100, 205, "5.短信管理");

		ucKey = WaitLimitKey("\x01\x02\x12\x03\x04\x05",6, 0);

		switch(ucKey)
		{
			case KEY_CANCEL:
			case 0:
				return OK;
				break;
			case 1:
				demo_SMS_send();
				break;
			case 2:
				demo_SMS_recv();
				break;
			case 3:
				for(i=1;i<=gSMS.uiMax;i++)
				{
					demo_SMS_read(i);
					ClearLine(1,9);
					TextOut(0,9,ALIGN_CENTER,"↓查看下一条");
					ucKey =WaitLimitKey("\x12\x0c",2,0);
					if(ucKey==KEY_CANCEL)
						break;
					
				}
				break;
			case 4:
				SMS_Setting();
				break;
			case 5:
				//SMS_Setting();
				break;
		}
	}
	return OK;
}




