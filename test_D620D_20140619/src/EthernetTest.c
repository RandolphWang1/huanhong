#include "Main.h"


void EthernetTest()
{ 
	char    szBuff[1024];
	char    szDataSnd[1024];
	int     ret, nSndLen;  
	char    buff[30], buff1[30];
	unsigned long ulTrace;
	int     i, num;
	char    szTmpBuff[100];
	UINT tempNum = gTerm.uiEthDataNum; 
	char *pNum = (char *)&tempNum;

	Clear();
	SetScrFont(FONT20, WHITE);
	TextOut(0, 2, ALIGN_CENTER, "以太网测试");

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
	num = atoi(buff);
	DebugOut("通讯次数:[%d]\n", num);

	Clear();
	SetScrFont(FONT20, WHITE);
	TextOut(0, 4, ALIGN_CENTER, "配置以太网...");

	memset(szBuff, 0, sizeof(szBuff));	
	strcat(szBuff, "ifconfig eth0 ");
	strcat(szBuff, gTerm.tEthernet.szLocalIP);
	strcat(szBuff, " netmask ");
	strcat(szBuff, gTerm.tEthernet.szMask);
	strcat(szBuff, " up");	
	DebugOut("\n%s\n", szBuff);
	system(szBuff);	

	memset(szBuff, 0, sizeof(szBuff));	
	strcat(szBuff, "route del default gw ");
	strcat(szBuff, gTerm.tEthernet.szGateway);
	DebugOut("\n%s\n", szBuff);
	system(szBuff);  

	memset(szBuff, 0, sizeof(szBuff));	
	strcat(szBuff, "route add default gw ");
	strcat(szBuff, gTerm.tEthernet.szGateway);
	DebugOut("\n%s\n", szBuff);
	system(szBuff);  

	//ETHER
	memset(szDataSnd, 0, sizeof(szDataSnd));  
	tempNum += 5;
	szDataSnd[0] = pNum[1];
	szDataSnd[1] = pNum[0];
	memcpy(szDataSnd+2, "\x60\x01\x80\x00\x00\x45\x54\x48\x45\x52\x00\x00\xc0\x00\x10\x91\x00\x00\x00\x00\x00\x31\x32\x33\x34\x35\x36\x37\x39\x31\x32\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32\x33\x34\x35\x01\x45\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x44\x2d\x30\x38\x30\x34\x31\x36\x2d\x46\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x31",
	gTerm.uiEthDataNum+5);
	nSndLen = gTerm.uiEthDataNum+7;

	for (i = 0; i < num; i ++)
	{   
		Clear();
		TextOut(0, 4, ALIGN_CENTER, "连接主机....");
		ret = TcpConnect(gTerm.tEthernet.szServerIP, gTerm.tEthernet.szServerPort, 5);
		if(ret != 0)
		{ 
			Clear();
			TextOut(0, 4, ALIGN_CENTER, "连接主机失败!");
			FailBeep(); 
			sprintf(szTmpBuff, "--以太网测试-%d--", i+1);
			PrintLog(szTmpBuff, "连接主机失败!!!!!!!!");         
			if(WaitKey(100) == KEY_CANCEL)
				return;
			else
				continue;  
		}

		Clear();
		TextOut(0, 4, ALIGN_CENTER, "发送数据....");
		memset(buff, 0, sizeof(buff));
		sprintf(buff, "第[%d]次通讯", i+1);
		TextOut(0, 5, ALIGN_CENTER, buff);

		memset(buff, 0, sizeof(buff));
		BcdToAsc(buff, szDataSnd+20, 6, 0);    
		ulTrace = atol(buff);
		ulTrace ++;
		if(ulTrace >= 999999L)
			ulTrace = 1;
		memset(buff, 0, sizeof(buff));
		sprintf(buff, "%06ld", ulTrace); 
		memset(buff1, 0, sizeof(buff1));
		AscToBcd(buff1, buff, 6, 0);
		memcpy(szDataSnd+20, buff1, 3);

		DebugOut("发送数据[%d]:\n", nSndLen);
		printf_x("", szDataSnd, nSndLen);

		ret = TcpSend(szDataSnd, nSndLen, 5);
		if(ret != 0)
		{
			TcpClose();
			Clear();
			TextOut(0, 4, ALIGN_CENTER, "发送数据失败!");
			FailBeep();
			sprintf(szTmpBuff, "--以太网测试-%d--", i+1);
			PrintLog(szTmpBuff, "发送数据失败!!!!!!!!");                     
			if(WaitKey(100) == KEY_CANCEL)
				return;
			else
				continue;
		}

		Clear();
		TextOut(0, 4, ALIGN_CENTER, "接收数据....");
		memset(buff, 0, sizeof(buff));
		sprintf(buff, "第[%d]次通讯", i+1);
		TextOut(0, 5, ALIGN_CENTER, buff);
		memset(szBuff, 0, sizeof(szBuff)); 
		ret = TcpRecv(szBuff, &nSndLen, 5);
		if(ret != 0) 
		{
			TcpClose();
			Clear();
			TextOut(0, 4, ALIGN_CENTER, "接收数据失败!");
			FailBeep(); 
			sprintf(szTmpBuff, "--以太网测试-%d--", i+1);
			PrintLog(szTmpBuff, "接收数据失败!!!!!!!!");                       
			if(WaitKey(100) == KEY_CANCEL)
				return;
			else
				continue;  
		}
		TcpClose();

		DebugOut("接收数据[%d]:\n", nSndLen);
		printf_x("", szBuff, nSndLen);

		memset(buff, 0, sizeof(buff)); 
		memcpy(buff, szBuff+7, 5); 
		if(memcmp(buff, "ETHER", 5) != 0)
		{
			DebugOut("get=[%s]\n", buff);
			FailBeep();
			Clear();
			TextOut(0, 4, ALIGN_CENTER, "返回数据包错误!");
			sprintf(szTmpBuff, "--以太网测试-%d--", i+1);
			PrintLog(szTmpBuff, "返回数据包错误!");     
			if(WaitKey(100) == KEY_CANCEL)
				return;
			else
				continue; 
		}

		OkBeep();
		Clear();
		TextOut(0, 4, ALIGN_CENTER, "以太网通讯成功!");
		sprintf(szTmpBuff, "--以太网测试-%d--", i+1);
		PrintLog(szTmpBuff, "以太网通讯成功");     
		if(WaitKey(1000) == KEY_CANCEL)
			return;
		else
			continue; 
	}

	return;
}






