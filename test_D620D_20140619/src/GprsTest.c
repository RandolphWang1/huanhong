#include "Main.h"

int iGprsHandle;    //无线句柄

/*
 * GPRS无线通讯函数
 */

void Gprs_Quectel_CommTest()
{
	int  i, nRet;
	char szBuff[30] ;
	char szDataSnd[1024], szDataRcv[1024];
	char szIP[20], szPORT[20];
	int  nSndLen, nRcvLen = 1024;
	char buff[20], buff1[20];
	unsigned long ulTrace;
	char szTmpBuff[100];
	UINT tempNum = gTerm.uiGprsDataNum; 
	char *pNum = (char *)&tempNum;
	int CommCount;
	
	Clear();
	SetScrFont(FONT20, WHITE);
	
	TextOut(0, 2, ALIGN_CENTER, "GPRS测试");
	TextOut(0, 3, ALIGN_CENTER, "移远模块版");
		
	memset(szDataSnd, 0, sizeof(szDataSnd));
	tempNum += 5;
	pNum = (char *)&tempNum;
	szDataSnd[0] = pNum[1];
	szDataSnd[1] = pNum[0];
	memcpy(szDataSnd+2, "\x60\x01\x80\x00\x00\x47\x50\x52\x53\x20\x00\x00\xc0\x00\x10\x91\x00\x00\x00\x00\x00\x31\x32\x33\x34\x35\x36\x37\x39\x31\x32\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32\x33\x34\x35\x01\x45\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x44\x2d\x30\x38\x30\x34\x31\x36\x2d\x46\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x31",
			gTerm.uiGprsDataNum+5);
	nSndLen = gTerm.uiGprsDataNum+7;
	sprintf(szIP, gTerm.szGprsServerIP);
	sprintf(szPORT, gTerm.szGprsServerPort);
	
	TextOut(2, 4, ALIGN_LEFT, "请输入通讯次数:");
	memset(buff, 0, sizeof(buff));
	SetScrFont(FONT20, RED);
	nRet = Input(18, 4, buff, 4, IME_NUMBER, BLACK, GREEN, FALSE, FALSE, FALSE);
	if(nRet == CANCEL)
		return;
	else if(nRet != OK)
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
	
	SetScrFont(FONT20, WHITE);
	
	for(i = 0; i < CommCount; i ++)
	{ 
		printf("\n\n第[%d]次通讯\n\n", i+1);
		ClearLine(1, 9);  
		TextOut(0, 3, ALIGN_CENTER, "连接主机...");  
		nRet = GPRS_Quectel_ConnectNet1(szIP, szPORT);
		if(nRet != OK)
		{
			GPRS_Quectel_Close();
			Beep();
			ClearLine(1,9); 
			TextOut(0, 4, ALIGN_CENTER, "连接主机失败!");		
			sprintf(szTmpBuff, "--GPRS测试-%d--", i+1); 
			PrintLog(szTmpBuff, "连接主机失败!!!!!!!!");			
			if(WaitLimitKey("\x0F\x12", 2, 1000) == KEY_CANCEL)
				return;
			else
				continue;  
		}
			 
		Clear();
		TextOut(0, 3, ALIGN_CENTER, "发送数据...");
		sprintf(szBuff, "第[%d]次通讯", i+1);
		TextOut(0, 5, ALIGN_CENTER, szBuff);  
	
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
			
		nRet = GPRS_Quectel_SendEx(szDataSnd, nSndLen); 
		if(nRet != 0)
		{
			Beep();
			Clear(); 
			TextOut(0, 3, ALIGN_CENTER, "发送数据失败!");	   
			sprintf(szTmpBuff, "--GPRS测试-%d--", i+1); 
			PrintLog(szTmpBuff, "发送数据失败!!!!!!!!");   
			GPRS_Quectel_Close();					 
			if(WaitLimitKey("\x0F\x12", 2, 1000) == KEY_CANCEL)
				return;
			else
				continue;  
		}
	
		Clear();
			
		TextOut(0, 3, ALIGN_CENTER, "接收数据...");
		sprintf(szBuff, "第[%d]次通讯", i+1);
		TextOut(0, 5, ALIGN_CENTER, szBuff);  
		memset(szDataRcv, 0, sizeof(szDataRcv));
		nRet = GPRS_Quectel_Recv(szDataRcv, &nRcvLen, 20, 1);
		if(nRet < 0)
		{
			Beep();
			Clear(); 
			TextOut(0, 3, ALIGN_CENTER, "接收数据失败!");		
			sprintf(szTmpBuff, "--GPRS测试-%d--", i+1); 
			PrintLog(szTmpBuff, "接收数据失败!!!!!!!!");				
//			ScrPrint(0, 9, 1, "取消键退出  其他重试");
			GPRS_Quectel_Close();
			if(WaitLimitKey("\x0F\x12", 2, 1000) == KEY_CANCEL)
				return;
			else
				continue;	
		}
	
			 
		memset(buff, 0, sizeof(buff)); 
		memcpy(buff, szDataRcv+7, 5); 
		printf("**buff**= [%s]\n", buff);
		if(memcmp(buff, "GPRS ", 5) != 0)
		{
			FailBeep();
			ClearLine(1, 9);
			TextOut(0, 3, ALIGN_CENTER, "返回数据包错误!");
			sprintf(szTmpBuff, "--GPRS测试-%d--", i+1); 
			PrintLog(szTmpBuff, "返回数据包错误!"); 	
			if(WaitLimitKey("\x0F\x12", 2, 1000) == KEY_CANCEL)
				return;
			else
				continue; 
		}
	
		Clear();
		Beep();
		TextOut(0, 3, ALIGN_CENTER, "GPRS通讯成功!");
		sprintf(szTmpBuff, "--GPRS测试-%d--", i+1); 
		PrintLog(szTmpBuff, "GPRS通讯成功");   
		if(WaitLimitKey("\x0F\x12", 2, 1000) == KEY_CANCEL)
			return;
		else
			continue; 
	}
}


void Gprs_Siemens_CommTest()
{
    int  i, nRet;
    char szBuff[30] ;
    char szDataSnd[1024], szDataRcv[1024];
    int  nSndLen, nRcvLen = 1024;
    char buff[20], buff1[20];
    unsigned long ulTrace;
    char szTmpBuff[100];
	UINT tempNum = gTerm.uiGprsDataNum; 
	char *pNum = (char *)&tempNum;
	int CommCount;

    Clear();
	SetScrFont(FONT20, WHITE);

    TextOut(0, 2, ALIGN_CENTER, "GPRS测试");
    TextOut(0, 3, ALIGN_CENTER, "西门子模块版");
    
    memset(szDataSnd, 0, sizeof(szDataSnd));
	tempNum += 5;
	pNum = (char *)&tempNum;
	szDataSnd[0] = pNum[1];
	szDataSnd[1] = pNum[0];
    memcpy(szDataSnd+2, "\x60\x01\x80\x00\x00\x47\x50\x52\x53\x20\x00\x00\xc0\x00\x10\x91\x00\x00\x00\x00\x00\x31\x32\x33\x34\x35\x36\x37\x39\x31\x32\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32\x33\x34\x35\x01\x45\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x44\x2d\x30\x38\x30\x34\x31\x36\x2d\x46\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x31",
		gTerm.uiGprsDataNum+5);
    nSndLen = gTerm.uiGprsDataNum+7;

	TextOut(2, 4, ALIGN_LEFT, "请输入通讯次数:");
	memset(buff, 0, sizeof(buff));
	SetScrFont(FONT20, RED);
	nRet = Input(18, 4, buff, 4, IME_NUMBER, BLACK, GREEN, FALSE, FALSE, FALSE);
	if(nRet == CANCEL)
		return;
	else if(nRet != OK)
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

	SetScrFont(FONT20, WHITE);

    for(i = 0; i < CommCount; i ++)
    { 
        printf("\n\n第[%d]次通讯\n\n", i+1);
        ClearLine(1, 9);  
        TextOut(0, 3, ALIGN_CENTER, "连接主机...");  
        nRet = GPRS_ConnectNet1();
        if(nRet != OK)
        {
            GPRS_Close();
		    Beep();
    		ClearLine(1,9); 
	        TextOut(0, 4, ALIGN_CENTER, "连接主机失败!");       
            sprintf(szTmpBuff, "--GPRS测试-%d--", i+1); 
            PrintLog(szTmpBuff, "连接主机失败!!!!!!!!");            
			if(WaitLimitKey("\x0F\x12", 2, 1000) == KEY_CANCEL)
                return;
            else
                continue;  
        }
         
        Clear();
        TextOut(0, 3, ALIGN_CENTER, "发送数据...");
        sprintf(szBuff, "第[%d]次通讯", i+1);
        TextOut(0, 5, ALIGN_CENTER, szBuff);  

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
        
        nRet = GPRS_SendEx(szDataSnd, nSndLen); 
        if(nRet != 0)
        {
	    Beep();
    	    Clear(); 
	    TextOut(0, 3, ALIGN_CENTER, "发送数据失败!");      
            sprintf(szTmpBuff, "--GPRS测试-%d--", i+1); 
            PrintLog(szTmpBuff, "发送数据失败!!!!!!!!");   
            GPRS_Close();                    
			if(WaitLimitKey("\x0F\x12", 2, 1000) == KEY_CANCEL)
                return;
            else
                continue;  
        }

		Clear();
        
        TextOut(0, 3, ALIGN_CENTER, "接收数据...");
        sprintf(szBuff, "第[%d]次通讯", i+1);
        TextOut(0, 5, ALIGN_CENTER, szBuff);  
        memset(szDataRcv, 0, sizeof(szDataRcv));
        nRet = GPRS_Recv(szDataRcv, &nRcvLen, 20, 1);
        if(nRet < 0)
        {
		    Beep();
    		Clear(); 
	        TextOut(0, 3, ALIGN_CENTER, "接收数据失败!");       
            sprintf(szTmpBuff, "--GPRS测试-%d--", i+1); 
            PrintLog(szTmpBuff, "接收数据失败!!!!!!!!");                
//	        ScrPrint(0, 9, 1, "取消键退出  其他重试");
            GPRS_Close();
			if(WaitLimitKey("\x0F\x12", 2, 1000) == KEY_CANCEL)
                return;
            else
                continue;   
        }

        GPRS_Close();
         
        memset(buff, 0, sizeof(buff)); 
        memcpy(buff, szDataRcv+7, 5); 
        if(memcmp(buff, "GPRS ", 5) != 0)
        {
            FailBeep();
            ClearLine(1, 9);
            TextOut(0, 3, ALIGN_CENTER, "返回数据包错误!");
            sprintf(szTmpBuff, "--GPRS测试-%d--", i+1); 
            PrintLog(szTmpBuff, "返回数据包错误!");     
			if(WaitLimitKey("\x0F\x12", 2, 1000) == KEY_CANCEL)
                return;
            else
                continue; 
        }

		Clear();
		Beep();
        TextOut(0, 3, ALIGN_CENTER, "GPRS通讯成功!");
        sprintf(szTmpBuff, "--GPRS测试-%d--", i+1); 
        PrintLog(szTmpBuff, "GPRS通讯成功");   
		if(WaitLimitKey("\x0F\x12", 2, 1000) == KEY_CANCEL)
            return;
        else
            continue; 
    }
}

void GprsTest()
{
	char cKey;
	char szSig[10];
	char szBuff[30];
	int i;
	int iSig = 0;

	while(1)
	{
		Clear();
		TextOut(0, 2, ALIGN_CENTER, "G网测试");
		TextOut(0, 4, ALIGN_CENTER, "1. 网络信号侦测  ");
		TextOut(0, 5, ALIGN_CENTER, "2. 西门子通信测试");
		TextOut(0, 6, ALIGN_CENTER, "3. 移远通信测试  ");
		cKey = WaitKey(0);
		switch(cKey)
		{
			case KEY_1:
			Clear();
			TextOut(0, 3, ALIGN_CENTER, "网络信号侦测");
			TextOut(0, 9, ALIGN_CENTER, "按取消键退出");

			i = 1;
			while(1)
			{
				DebugOut("gprs_handle2:%d\n", iGprsHandle);
				GetSignal(&iSig);
				memset(szSig, 0, sizeof(szSig));
				sprintf(szSig, "%d", iSig);

				sprintf(szBuff, "  第%d次侦测信号值:", i);
				ClearLine(4, 3);
				TextOut(0, 5, ALIGN_CENTER, szBuff);
				sprintf(szBuff, "< %s >", szSig);
				TextOut(0, 6, ALIGN_CENTER, szSig);

				i ++;

				if(WaitKey(2000) == KEY_CANCEL)
					break;
			}
			break;
			case KEY_2:
				Gprs_Siemens_CommTest();
				break;
			case KEY_3:
				Gprs_Quectel_CommTest();
				break;
			case KEY_CANCEL:
				return;
		}
	}
}

void CDMATest()
{
	char cKey;
	char szSig[10];
	char szBuff[30];
	int i;
	int iSig = 0;

	while(1)
	{
		Clear();
		TextOut(0, 2, ALIGN_CENTER, "C网测试");
		TextOut(0, 3, ALIGN_CENTER, "华为模块版");
		TextOut(0, 5, ALIGN_CENTER, "1. 网络信号侦测");
		TextOut(0, 6, ALIGN_CENTER, "2. 数据通信测试");
		cKey = WaitKey(0);
		switch(cKey)
		{
			case KEY_1:
			Clear();
			TextOut(0, 3, ALIGN_CENTER, "网络信号侦测");
			TextOut(0, 9, ALIGN_CENTER, "按取消键退出");

			i = 1;
			while(1)
			{
				DebugOut("gprs_handle2:%d\n", iGprsHandle);
				GetSignal(&iSig);
				memset(szSig, 0, sizeof(szSig));
				sprintf(szSig, "%d", iSig);

				sprintf(szBuff, "  第%d次侦测信号值:", i);
				ClearLine(4, 3);
				TextOut(0, 5, ALIGN_CENTER, szBuff);
				sprintf(szBuff, "< %s >", szSig);
				TextOut(0, 6, ALIGN_CENTER, szSig);

				i ++;

				if(WaitKey(2000) == KEY_CANCEL)
					break;
			}
			break;
			case KEY_2:
				CDMACommTest();
				break;
			case KEY_CANCEL:
				return;
		}
	}
}
/*
 *CDMA无线通讯函数
 */
void CDMACommTest()
{
    int  i, nRet;
    char szBuff[30] ;
    char szDataSnd[1024], szDataRcv[1024];
    int  nSndLen, nRcvLen;
    char buff[20], buff1[20];
    unsigned long ulTrace;
    char szTmpBuff[100];
	char *pNum;
	int CommCount;

    Clear();
	SetScrFont(FONT20, WHITE);

    TextOut(0, 2, ALIGN_CENTER, "CDMA测试");
    TextOut(0, 3, ALIGN_CENTER, "新华为模块版");
    
    memset(szDataSnd, 0, sizeof(szDataSnd));
	pNum = (char *)&gTerm.uiGprsDataNum;
	szDataSnd[0] = pNum[1];
	szDataSnd[1] = pNum[0];
    memcpy(szDataSnd+2, "\x47\x50\x52\x53\x20\x00\x00\xc0\x00\x10\x91\x00\x00\x00\x00\x00\x31\x32\x33\x34\x35\x36\x37\x39\x31\x32\x33\x34\x35\x36\x37\x38\x39\x30\x31\x32\x33\x34\x35\x01\x45\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x44\x2d\x30\x38\x30\x34\x31\x36\x2d\x46\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x30\x31",
		gTerm.uiGprsDataNum);
    nSndLen = gTerm.uiGprsDataNum+2;

	TextOut(2, 4, ALIGN_LEFT, "请输入通讯次数:");
	memset(buff, 0, sizeof(buff));
	SetScrFont(FONT20, RED);
	nRet = Input(18, 4, buff, 4, IME_NUMBER, BLACK, GREEN, FALSE, FALSE, FALSE);
	if(nRet == CANCEL)
		return;
	else if(nRet != OK)
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

	SetScrFont(FONT20, WHITE);

    for(i = 0; i < CommCount; i ++)
    {
    	Wait(200);
        printf("\n\n第[%d]次通讯\n\n", i+1);
        ClearLine(1, 9);  
        TextOut(0, 3, ALIGN_CENTER, "连接主机...");
        nRet = CDMA_ConnectEx(iGprsHandle, gTerm.szGprsServerIP, gTerm.szGprsServerPort);
        if(nRet != OK)
        {
		Beep();
		ClearLine(1,9); 
		TextOut(0, 4, ALIGN_CENTER, "连接主机失败!");       
		sprintf(szTmpBuff, "--CDMA测试-%d--", i+1); 
		PrintLog(szTmpBuff, "连接主机失败!!!!!!!!");
		if(WaitLimitKey("\x0F\x12", 2, 1000) == KEY_CANCEL)
			return;
		else
			continue; 
        }
         
        Clear();
        TextOut(0, 3, ALIGN_CENTER, "发送数据...");
        sprintf(szBuff, "第[%d]次通讯", i+1);
        TextOut(0, 5, ALIGN_CENTER, szBuff);  

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
        
        nRet = CDMA_Send(iGprsHandle, szDataSnd, nSndLen); 
        if(nRet != 0)
        {
		Beep();
		Clear(); 
		TextOut(0, 3, ALIGN_CENTER, "发送数据失败!");      
		sprintf(szTmpBuff, "--CDMA测试-%d--", i+1); 
		PrintLog(szTmpBuff, "发送数据失败!!!!!!!!");   
		CDMA_Close(iGprsHandle);                    
		if(WaitLimitKey("\x0F\x12", 2, 1000) == KEY_CANCEL)
			return;
		else
			continue;  
        }

		Clear();

        TextOut(0, 3, ALIGN_CENTER, "接收数据...");
        sprintf(szBuff, "第[%d]次通讯", i+1);
        TextOut(0, 5, ALIGN_CENTER, szBuff);  
        memset(szDataRcv, 0, sizeof(szDataRcv));
        nRet = CDMA_Recv(iGprsHandle, szDataRcv, &nRcvLen, 30);
        if(nRet != 0 || nRcvLen < 1)
        {
		    Beep();
    		Clear(); 
	        TextOut(0, 3, ALIGN_CENTER, "接收数据失败!");       
            sprintf(szTmpBuff, "--CDMA测试-%d--", i+1); 
            PrintLog(szTmpBuff, "接收数据失败!!!!!!!!");                
//	        ScrPrint(0, 9, 1, "取消键退出  其他重试");
            CDMA_Close(iGprsHandle);
			if(WaitLimitKey("\x0F\x12", 2, 1000) == KEY_CANCEL)
                return;
            else
                continue;   
        }

		Clear();
		Beep();
        TextOut(0, 3, ALIGN_CENTER, "CDMA通讯成功!");

		Clear();
        TextOut(0, 3, ALIGN_CENTER, "关闭无线通讯...");
        CDMA_Close(iGprsHandle);

        sprintf(szTmpBuff, "--CDMA测试-%d--", i+1); 
        PrintLog(szTmpBuff, "CDMA通讯成功");   
		if(WaitLimitKey("\x0F\x12", 2, 1000) == KEY_CANCEL)
            return;
        else
            continue; 
    }
}


