#include "Main.h"


void ModemTest()
{ 
	char szTmp[50], szTpdu[12];
	char szTmpBuff[50];
	char szDataSnd[1024+1], szDataRcv[1024+1];
	int  nSndLen, nRcvLen;
	int  i, nRet, CommCount;
	char buff[50];
	int ret;

	Clear();
	SetScrFont(FONT20, WHITE);
	TextOut(0, 2, ALIGN_CENTER, "拨号测试");

	memcpy(szTmp, "6000080000", 10);
	memcpy(szTmp+2, gTerm.szTPDU, 4);
	memset(szTpdu,0,sizeof(szTpdu));  	
	AscToBcd(szTpdu, szTmp, 10, 0);

	TextOut(2, 4, ALIGN_LEFT, "请输入通讯次数:");
	memset(buff, 0, sizeof(buff));
	SetScrFont(FONT20, RED);
	ret = Input(18, 4, buff, 4, IME_NUMBER, BLACK, GREEN, FALSE, FALSE, FALSE);
	if(ret == CANCEL)
		return;
	else if(ret != OK)
	{
		Clear();
		SetScrFont(FONT20, WHITE);
		TextOut(0, 4, ALIGN_CENTER, "测试失败"); 
		FailBeep();
		WaitKey(2000);
		return;
	}
	CommCount = atoi(buff);
	DebugOut("通讯次数:[%d]\n", CommCount);

	//if(SwitchCom0_Modem()!=0)
	if(SwitchCom2_Modem()!=0)
		printf("switch com0 fail.\n");
    Wait(10);
	//打开modem
    PowerModem(1);
    Wait(10);
    ModemReset();

	for(i = 0; i < CommCount; i ++)
	{  
		//建立通讯线路
		ClearLine(1, 10);  
		SetScrFont(FONT20, WHITE);
		TextOut(0, 4, ALIGN_CENTER, "正在拨号...");
		nRet = Modem_Connect(gTerm.bOut, gTerm.szOutNo, gTerm.delay_times, gTerm.szTelNo, NULL, NULL);
		if(nRet != 0)
		{
			ClearLine(1, 10);  
			TextOut(0, 4, ALIGN_CENTER, "拨号失败!");
			PrintLog("MODEM测试:", "拨号失败");
			DialOff();
			if(WaitLimitKey("\x0F\x12", 2, 1000) == KEY_CANCEL)
				break;
			continue;
		}
		PrintLog("----MODEM测试:----", "拨号成功");

		memset(szDataSnd, 0, sizeof(szDataSnd));
		memcpy(szDataSnd, szTpdu, 5);
		nSndLen = 5;

		//MODEM
		memcpy(szDataSnd+5, "\x4D\x4F\x44\x45\x4D\x00\x00\xc0\x00\x10\x91\x00\x00\x00\x00\x00\x31\x32\x33\x34\x35\x36\x37\x39\x31\x32\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32\x33\x34\x35\x01\x45\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x44\x2d\x30\x38\x30\x34\x31\x36\x2d\x46\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x31",  186 );
		nSndLen += gTerm.uiModemDataNum;

		//发送数据
		ClearLine(1, 10);
		TextOut(0, 4, ALIGN_CENTER, "发送数据...");
		memset(buff, 0, sizeof(buff));
		sprintf(buff, "第[%d]次通讯", i+1);
		TextOut(0, 5, ALIGN_CENTER, buff);

		nRet = Modem_Send(szDataSnd, nSndLen);
		if(nRet != 0)
		{
			FailBeep();
			ClearLine(1, 10);  
			TextOut(0, 4, ALIGN_CENTER, "发送数据失败");
			sprintf(szTmpBuff, "--拨号测试-%d--", i+1);
			PrintLog(szTmpBuff, "发送数据失败!!!!!!!!!");

			if(WaitKey(100) == KEY_CANCEL)
				break;

			DialOff_Block();
			continue; 
		}

		ClearLine(1, 10);  

		//接收返回数据
		TextOut(0, 4, ALIGN_CENTER, "接收数据...");
		memset(buff, 0, sizeof(buff));
		sprintf(buff, "第[%d]次通讯", i+1);
		TextOut(0, 5, ALIGN_CENTER, buff);
		memset(szDataRcv,   0,   sizeof(szDataRcv));  
		nRcvLen = 0;  
		nRet = Modem_Recv(szDataRcv, &nRcvLen);  
		if(nRet != 0)
		{        
			FailBeep(); 
			ClearLine(1, 10);  
			TextOut(0, 4, ALIGN_CENTER, "接收数据失败");
			sprintf(szTmpBuff, "--拨号测试-%d--", i+1);
			PrintLog(szTmpBuff, "接收数据失败!!!!!!!!!");

			if(WaitKey(2000) == KEY_CANCEL)
				break; 

			DialOff_Block();
			continue; 
		} 

		if(nRcvLen < 5)
		{	      
			FailBeep(); 
			ClearLine(1, 10);  
			TextOut(0, 4, ALIGN_CENTER, "接收数据长度<5个");
			sprintf(szTmpBuff, "--拨号测试-%d--", i+1);
			PrintLog(szTmpBuff, "接收数据长度<5个");
			if(WaitKey(100) == KEY_CANCEL)
				break;
			DialOff_Block();
			continue; 
		}   

		memset(buff, 0, sizeof(buff)); 
		memcpy(buff, szDataRcv+5, 5); 
		if(memcmp(buff, "MODEM", 5) != 0)
		{ 
			FailBeep();
			ClearLine(1, 10);  
			TextOut(0, 4, ALIGN_CENTER, "返回数据包错误!");
			sprintf(szTmpBuff, "--拨号测试-%d--", i+1);
			PrintLog(szTmpBuff, "返回数据包错误!");     
			if(WaitKey(2000) == KEY_CANCEL)
				return;
			else
			{
				DialOff_Block();
				continue; 
			}
		}

		OkBeep();
		ClearLine(1, 10);  
		TextOut(0, 4, ALIGN_CENTER, "拨号通讯成功!");
		sprintf(szTmpBuff, "--拨号测试-%d--", i+1);
		PrintLog(szTmpBuff, "拨号通讯成功");

		DialOff_Block();

		if(WaitKey(1000) == KEY_CANCEL)
			break;

		continue;  
	}
	//关闭modem
    PowerModem(0);

	return;	
}



