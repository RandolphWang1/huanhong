#include "Main.h"

 
static void PrintLogMagic(char *Msg1, char *Msg2, char *Msg3)
{ 
	int ret;
	char PrintBuff[1024];

	if(CheckPrinter() != TRUE)
		goto end;

	//清空打印缓冲区
	ClearPrintBuff();
	//设置打印字体大小
    SetPrintFont(24);

    SetPrintIndent(0); 

    ret = FillPrintBuff("PL");
    if(ret < 0)
		goto end;

	if (Msg1[0] != 0x00)    
	{        
		memset(PrintBuff, 0x00, sizeof(PrintBuff));
		strcpy(PrintBuff, "一磁道数据:");
		ret = FillPrintBuff(PrintBuff);
		if(ret<0)
			goto end;             
		ret = FillPrintBuff("PL");
		if(ret<0)
			goto end;
		SetPrintIndent(0); 

		memset(PrintBuff, 0x00, sizeof(PrintBuff));
		strcpy(PrintBuff, Msg1);
		ret = FillPrintBuff(PrintBuff);
		if (ret<0)
			goto end;            
		ret = FillPrintBuff("PL");
		if(ret<0)
			goto end;
		SetPrintIndent(0); 
	}

	if (Msg2[0] != 0x00)    
	{        
		memset(PrintBuff, 0x00, sizeof(PrintBuff));
		strcpy(PrintBuff, "二磁道数据:");
		ret = FillPrintBuff(PrintBuff);
		if(ret<0)
			goto end;             
		ret = FillPrintBuff("PL");
		if(ret<0)
			goto end;
		SetPrintIndent(0); 

		memset(PrintBuff, 0x00, sizeof(PrintBuff));
		strcpy(PrintBuff, Msg2);
		ret = FillPrintBuff(PrintBuff);
		if (ret<0)
			goto end;            
		ret = FillPrintBuff("PL");
		if(ret<0)
			goto end;
		SetPrintIndent(0); 
	}

	if (Msg3[0] != 0x00)    
	{        
		memset(PrintBuff, 0x00, sizeof(PrintBuff));
		strcpy(PrintBuff, "三磁道数据:");
		ret = FillPrintBuff(PrintBuff);
		if(ret<0)
			goto end;             
		ret = FillPrintBuff("PL");
		if(ret<0)
			goto end;
		SetPrintIndent(0); 

		memset(PrintBuff, 0x00, sizeof(PrintBuff));
		strcpy(PrintBuff, Msg3);
		ret = FillPrintBuff(PrintBuff);
		if (ret<0)
			goto end;            
		ret = FillPrintBuff("PL");
		if(ret<0)
			goto end;
		SetPrintIndent(0); 
	}

	ret = FillPrintBuff("         刷卡成功");
	if(ret<0)
		goto end;             
	ret = FillPrintBuff("PL");
	if(ret<0)
		goto end;
	SetPrintIndent(0); 
	ret = FillPrintBuff("PL");
	if(ret<0)
		goto end;
	ret = FillPrintBuff("PL");
	if(ret<0)
		goto end;
	ret = FillPrintBuff("PL");
	if(ret<0)
		goto end;

	SetPrintIndent(0);  

	//开始打印    
	ret =StartPrint();
	if(ret != 0)
		goto end;

	return;

end:  
	OkBeep(); 
	Clear();
	TextOut(0, 3, ALIGN_CENTER, "请检查打印机");
	TextOut(0, 4, ALIGN_CENTER, "打印失败");
	WaitKey(3000);

	return;
}

void MagCardTest()
{
	int  nRet;
	char Track1[200], Track2[200], Track3[200];
	int track1_len, track2_len, track3_len;
	int  printflg;

	#define	TRACK1_OK		0x0001
	#define	TRACK2_OK		0x0002
	#define	TRACK3_OK		0x0004

	Clear();

	if(MagOpen() != 0)
	{
		DebugOut("open mgc error\n");
		Clear();
		TextOut(0, 4, ALIGN_CENTER, "打开磁头失败!");
		WaitKey(2000);
		return;
	} 

	TextOut(0, 2, ALIGN_CENTER, "磁条读卡器测试");
	TextOut(0, 4, ALIGN_CENTER, "请刷磁卡 ---->");
	TextOut(0, 7, ALIGN_CENTER, "取消键退出");

	while(1)
	{ 
		memset(Track1, 0x00, sizeof(Track1)); 
		memset(Track2, 0x00, sizeof(Track2)); 
		memset(Track3, 0x00, sizeof(Track3)); 
		printflg = 0;
		nRet = MagRead(&track1_len, Track1, &track2_len, Track2, &track3_len, Track3);
		if(nRet > 0)
		{
			OkBeep();
			Clear();
			if(nRet & TRACK1_OK)   //有第一磁道
			{
				TextOut(0, 2, ALIGN_CENTER, "一磁道数据:");
				if (track1_len > 32)
				{
					TextOut(0, 3, ALIGN_CENTER, "数据过长,部分显示");
					Track1[32] = '\x00';
					TextOut(0, 4, ALIGN_CENTER, Track1);
				}
				else
				{
					TextOut(0, 3, ALIGN_CENTER, Track1);
				}
				printf("track1 data:\n%s\n", Track1);
				printflg = 1;
			}
			if(nRet & TRACK2_OK)
			{
				TextOut(0, 5, ALIGN_CENTER, "二磁道数据:");
				if (track2_len > 32)
				{
					TextOut(0, 6, ALIGN_CENTER, "数据过长,部分显示");
					Track2[32] = '\x00';
					TextOut(0, 7, ALIGN_CENTER, Track2);
				}
				else
				{
					TextOut(0, 6, ALIGN_CENTER, Track2);
				}
				printf("track2 data:\n%s\n", Track2);
				printflg = 1;
			}
			if(nRet & TRACK3_OK)
			{
				TextOut(0, 8, ALIGN_CENTER, "三磁道数据:");
				if (track3_len > 32)
				{
					TextOut(0, 9, ALIGN_CENTER, "数据过长,部分显示");
					Track3[32] = '\x00';
					TextOut(0, 10, ALIGN_CENTER, Track3);
				}
				else
				{
					TextOut(0, 9, ALIGN_CENTER, Track3);
				}
				printf("track3 data:\n%s\n", Track3);
				printflg = 1;
			} 

			Wait(500);

			if(printflg != 0)
				PrintLogMagic(Track1, Track2, Track3);
			else
			{
				Clear();
				TextOut(0, 4, ALIGN_CENTER, "刷卡失败!");
				WaitKey(0);
			}

			Clear();
			TextOut(0, 5, ALIGN_CENTER, "读卡成功");
			WaitKey(1000);

			Clear();
			TextOut(0, 2, ALIGN_CENTER, "磁条读卡器测试");
			TextOut(0, 4, ALIGN_CENTER, "请刷磁卡 ---->");
			TextOut(0, 9, ALIGN_CENTER, "取消键退出");
		} 

		if(KbdHit())
		{
			if(GetKey() == KEY_CANCEL)
			{
				MagClose();
				return;
			}
		}
	}

	MagClose();
}

void MEMRead()
{
    UCHAR  atr[4];
	UCHAR  szBuff[500]; 
	char   szTmp[1024];
	UCHAR  ucKey;
	int    i, j, nRet = 0; 
	static int slot = CARD_USER;
	
	while(1)
	{  
    	//清空键盘缓存
    	ClearKbd();
    	
    	Clear();
		SetScrFont(FONT20, WHITE);
    	TextOut(5, 3, ALIGN_LEFT, "请插入IC卡");
    	TextOut(0, 9, ALIGN_LEFT, "按 [取消] 键退出");
    	
    	nRet = IccOpen(slot);
        if(nRet != 0)
        {
            FailBeep();
        	Clear();
        	TextOut(2, 3, ALIGN_LEFT, "读卡设备打开失败");
        	IccClose(slot);
    		PrintLog("接触MEM卡测试:", "ERROR打开设备错误");
        	return;
        }
        printf("IC Card Open OK!\n");
        
    	while(1)
    	{
    	    if(KbdHit())
    	    {
    	        ucKey = GetKey();
    	        if(ucKey == KEY_CANCEL)
    	            return ;
    	    }
    	    
    	    //检测IC卡是否到位
    	    nRet = IccDetect(slot);
    		if(nRet != 0)
    		{
    	        printf("Detected No Card! \n"); 
    	        
    	        //关闭IC卡座
    			IccClose(slot);
    			Wait(100);

    			//打开IC卡座
    			IccOpen(slot);
    			continue;
    		}
    		Wait(100);
    		break;
    	}
    	printf("Card Detected OK! \n");
    	
    	//初始化Memory IC卡座
        nRet = IccMemCardInit(slot);
        if(nRet != 0)
        {
        	FailBeep();
        	Clear();
        	TextOut(0, 3, ALIGN_LEFT, "初始化卡片失败");   	
        	IccClose(slot);
    		PrintLog("接触MEM卡测试:", "ERROR初始化错误");
        	return ;
        }
        printf("Init Memory Card OK! \n"); 
    					
        //Memory IC卡复位
    	nRet = sle4442_reset(atr);
    	if(nRet == 1)
    	{
        	IccClose(slot);
    		FailBeep();
        	Clear();
    		TextOut(6, 3, ALIGN_LEFT, "复位错误");
    		PrintLog("接触MEM卡测试:", "ERROR复位错误");
    		return ;
    	}	
    	printf("Reset OK!\n");
       memset(szBuff, 0, sizeof(szBuff));
        //读取IC卡数据：sle_read_mm()
        //              参数1-卡片内存起始位移；
        //              参数2-数据缓存；
        //              参数3-欲读取的数据长度；
        
//        void sle_read_mm(unsigned char addr, unsigned char *buf, int length)
//        原型修订为:
//        int sle_read_mm(unsigned char addr,unsigned char *buf,int length)
//        增加报错返回。如读取过程发生异常，将返回负值报错。

    	sle_read_mm(0, szBuff, 256);
    	
    	//
    	for(i = 0, j = 0; i < 256; i ++)
    	{
    	    if(szBuff[i] == 0xff)
    	        j ++;
    	}
    	if(j == 256)
    	{
    	    Clear();
    	    TextOut(0, 3, ALIGN_LEFT, "   数据全是FF!"); 
    	    printf("j = [%d]\n", j);
    	    FailBeep();
    	    WaitKey(1000);
    	    return;
    	}
    	
    	for(i = 0, j = 0; i < 256; i ++)
    	{
    	    if(szBuff[i] == 0x00)
    	        j ++;
    	}
    	if(j == 256)
    	{
    	    Clear();
    	    TextOut(0, 3, ALIGN_LEFT, "   数据全是00!"); 
    	    printf("j = [%d]\n", j);
    	    FailBeep();
    	    WaitKey(1000);
			return;
    	}
    	
        //======================================================//
    	printf("\nIC Info:\n");	
    	for(i=0; i<256; i++)
        { 
    		printf("%02x ", szBuff[i]);
    		if( !((1+i) % 8) )
    		{
    			printf("\n");
    		}
        }
    	printf("\n");
        //======================================================//
        IccClose(slot);
    	Clear();
    	TextOut(0, 3, ALIGN_LEFT, "      读卡成功!");
		memset(szTmp, 0, sizeof(szTmp));
    	BcdToAsc(szTmp, szBuff+64, 12, 0);
    	//memcpy(szTmp,szBuff+64,12);
		printf("%s",szTmp);
    	TextOut(0, 4, ALIGN_LEFT, szTmp); 
		WaitKey(0);
        PrintLog("接触MEM卡测试:", szTmp);
		return;
    }
	
    return ;
}

void MEMWrite()
{
    UCHAR  atr[4];
	char  szBuff[100]; 
	UCHAR  ucKey;
	int nRet = 0;
	static int slot = CARD_USER;
	char key_bcd[3+1] = "";

	//转换密钥
	AscToBcd(key_bcd, gTerm.szMemKey, 6, 0);

	//写入的数据
	Clear();
	SetScrFont(FONT20, WHITE);
	TextOut(0, 4, ALIGN_LEFT, "写入数据:");
	memset(szBuff, 0, sizeof(szBuff));
	SetScrFont(FONT20, BLUE);
	nRet = Input(1, 5, szBuff, 15, IME_NUMBER, BLACK, GREEN, FALSE, FALSE, FALSE);
	if(nRet != OK)
    	return;

	SetScrFont(FONT20, WHITE);
	while(1)
	{  
    	//清空键盘缓存
    	ClearKbd();
    	Clear();
    	TextOut(5, 3, ALIGN_LEFT, "请插入IC卡");
    	TextOut(0, 9, ALIGN_LEFT, "按 [取消] 键退出");
    	nRet = IccOpen(slot);
        if(nRet != 0)
        {
            FailBeep();
        	Clear();
        	TextOut(2, 3, ALIGN_LEFT, "读卡设备打开失败");
        	IccClose(slot);
    		PrintLog("接触MEM卡测试:", "ERROR打开设备错误");
        	return;
        }
        printf("IC Card Open OK!\n");

    	while(1)
    	{
    	    if(KbdHit())
    	    {
    	        ucKey = GetKey();
    	        if(ucKey == KEY_CANCEL)
    	            return ;
    	    }

    	    //检测IC卡是否到位
    	    nRet = IccDetect(slot);
    		if(nRet != 0)
    		{
    	        printf("Detected No Card! \n"); 

    	        //关闭IC卡座
    			IccClose(slot);
    			Wait(100);

    			//打开IC卡座
    			IccOpen(slot);
    			continue;
    		}
    		Wait(100);
    		break;
    	}
    	printf("Card Detected OK! \n");
    	
    	//初始化Memory IC卡座
        nRet = IccMemCardInit(slot);
        if(nRet != 0)
        {
        	FailBeep();
        	Clear();
        	TextOut(0, 3, ALIGN_LEFT, "初始化卡片失败");   	
        	IccClose(slot);
    		PrintLog("接触MEM卡测试:", "ERROR初始化错误");
        	return ;
        }
        printf("Init Memory Card OK! \n"); 
    					
        //Memory IC卡复位
    	nRet = sle4442_reset(atr);
    	if(nRet == 1)
    	{
        	IccClose(slot);
    		FailBeep();
        	Clear();
    		TextOut(6, 3, ALIGN_LEFT, "复位错误");
    		PrintLog("接触MEM卡测试:", "ERROR复位错误");
    		return ;
    	}	
    		printf("Reset OK!\n");		 
		//比较密码
		printf("[%02x %02x %02x]\n", key_bcd[0], key_bcd[1], key_bcd[2]);
		nRet=sle_verify_EC(key_bcd,0);
		if(nRet!=0)
		{
			Clear();
			IccClose(slot);
			TextOut(6, 3, ALIGN_LEFT, "密钥错误");
			printf("verify fail  ret=%d \n",nRet);
			WaitKey(1000);
			return;
		}
		else
			printf("verify ok \n");

		//写数据
		if(sle_update_mm(64,szBuff,strlen(szBuff))!=0)
			printf("write card error\n");

        //======================================================//
       IccClose(slot);
    	Clear();
    	TextOut(0, 3, ALIGN_CENTER, "写卡成功!"); 
		WaitKey(1000);
		PrintLog("接触MEM卡测试:", szBuff);
		return;
    }
	
    return ;
}


void AT24Read()
{

	UCHAR  szBuff[500]; 

	UCHAR  ucKey;

	static int slot = CARD_USER;
	int nRet;
	int err;
	char test[3];
	
	while(1)
	{  
    	//清空键盘缓存
    	ClearKbd();
    	Clear();
		SetScrFont(FONT20, WHITE);
    	TextOut(5, 3, ALIGN_LEFT, "请插入IC卡");
    	TextOut(0, 9, ALIGN_LEFT, "按 [取消] 键退出");
    	
    	nRet = IccOpen(slot);
        if(nRet != 0)
        {
            FailBeep();
        	Clear();
        	TextOut(2, 3, ALIGN_LEFT, "读卡设备打开失败");
        	IccClose(slot);
    		PrintLog("接触MEM卡测试:", "ERROR打开设备错误");
        	return;
        }
        printf("IC Card Open OK!\n");
        
    	while(1)
    	{
    	    if(KbdHit())
    	    {
    	        ucKey = GetKey();
    	        if(ucKey == KEY_CANCEL)
    	            return ;
    	    }
    	    
    	    //检测IC卡是否到位
    	    nRet = IccDetect(slot);
    		if(nRet != 0)
    		{
    	        printf("Detected No Card! \n"); 
    	        
    	        //关闭IC卡座
    			IccClose(slot);
    			Wait(100);

    			//打开IC卡座
    			IccOpen(slot);
    			continue;
    		}
    		Wait(100);
    		break;
    	}
    	printf("Card Detected OK! \n");
    	
    	//初始化Memory IC卡座
        nRet =at24c_init();
		printf("AT24=%d\n",nRet);
        if(nRet != 0)
        {
        	FailBeep();
        	Clear();
        	TextOut(0, 3, ALIGN_LEFT, "初始化卡片失败");   	
        	IccClose(slot);
    		PrintLog("接触MEM卡测试:", "ERROR初始化错误");
        	return ;
        }
        printf("Init Memory Card OK! \n"); 
			
  
        memset(szBuff, 0, sizeof(szBuff));
		err=0;
	    test[0]=0XAA;
	    if(at24c_write_sub(AT24C04_PAGE0,0xfe,1,test))
	        printf("at24c write fail 1.\n");
	    test[0]=0x00;
	    if(at24c_read_sub(AT24C04_PAGE0,0xfe,1,test))
	        printf("at24c read fail 1:%#02x.\n",test[0]);
	    if(test[0]!=0XAA)
	    {
	        printf("read byte:%#02x.\n",test[0]);
	        err--;
	    }
       
        IccClose(slot);
    	Clear();
    	TextOut(0, 3, ALIGN_LEFT, "      读卡成功!");
		OkBeep();
		PrintLog("AT24卡测试:", "读写卡成功!!!");
		Wait(1000);
		return;
    }
	
    return ;
}

void MEMCardSet()
{
	int nRet = 0; 

	Clear();
	SetScrFont(FONT20, WHITE);
	TextOut(0, 3, ALIGN_LEFT, "  写卡密钥:");
	SetScrFont(FONT20, BLUE);
	nRet = Input(11, 4, gTerm.szMemKey, 6, IME_NUMBER, BLACK, GREEN, FALSE, FALSE, TRUE);
	if(nRet != OK)
	if(nRet != OK)
    	return;

	Clear();
	SetScrFont(FONT20, WHITE);
	TextOut(0, 3, ALIGN_CENTER, "密钥设置成功");
	WaitKey(2000);
}

/*
 * Memory IC存储卡测试函数
 */
 void MEMTest()
{
	UCHAR ucKey;
	char sKeyName[20];

	ClearKbd();
	while(1)
	{
		Clear();

		SetScrFont(FONT20, WHITE);

		//菜单
		ShowBmpFile(90, 60, "pic/button.bmp");
		TextOutByPixel(100, 65, "1.读卡");

		ShowBmpFile(90, 95, "pic/button.bmp");
		TextOutByPixel(100, 100, "2.写卡");

		ShowBmpFile(90, 130, "pic/button.bmp");
		TextOutByPixel(100, 135, "3.设置");

		ShowBmpFile(90, 130, "pic/button.bmp");
		TextOutByPixel(100, 135, "0.退出");

		ucKey = WaitLimitKey("\x00\x01\x02\x03\x12", 5, 0);
		memset(sKeyName, 0, sizeof(sKeyName));
		GetKeyName(ucKey, sKeyName);
		DebugOut("press key: %s\n", sKeyName);

		switch(ucKey)
		{
			case KEY_CANCEL:
			case 0:
				return;
				break;
			case 1:
				MEMRead();
				break;
			case 2:
				MEMWrite();
			case 3:
				MEMCardSet();
				break;
		}
	}
	return;
}


/*
 * AT24存储卡测试函数
 */
 void AT24Test()
{
	UCHAR ucKey;
	char sKeyName[20];

	ClearKbd();
	while(1)
	{
		Clear();

		SetScrFont(FONT20, WHITE);

		//菜单
		ShowBmpFile(90, 60, "pic/button.bmp");
		TextOutByPixel(100, 65, "1.读卡");

		ShowBmpFile(90, 95, "pic/button.bmp");
		TextOutByPixel(100, 100, "2.写卡");

		ShowBmpFile(90, 130, "pic/button.bmp");
		TextOutByPixel(100, 135, "0.退出");

		ucKey = WaitLimitKey("\x00\x01\x02\x12", 4, 0);
		memset(sKeyName, 0, sizeof(sKeyName));
		GetKeyName(ucKey, sKeyName);
		DebugOut("press key: %s\n", sKeyName);

		switch(ucKey)
		{
			case KEY_CANCEL:
			case 0:
				return;
				break;
			case 1:
				AT24Read();
				break;
			case 2:
				break;
		}
	}
	return;
}


/*************************************************************************/ 
///////////////////////// 张一元卡流程处理操作 /////////////////////////
/*
 * 读取射频IC卡的相关信息
 */
int TestRfic(void)
{
    	int nRet;
    	int iM1Handle;
	APDU_SEND ApduSend;
	APDU_RESP ApduRecv;
	int  i = 0;
	char szBuff[128] = {0};

    	//初始化射频模块
    	nRet=RficInit(&iM1Handle);
    	if(nRet!=0)
    	{
        	ClearLine(1,9);
        	PutsXY(0,3,"初始化m1失败");
        	WaitKey(2000);
        	return(-1);
    	}

   	//检测射频卡是否到位
    	nRet=RficDetect(iM1Handle);
    	if(nRet!=0)
        	return(-1);
	
    	nRet=RficResetCpu(iM1Handle);
    	if(nRet!=0)
        	return(-1);

	nRet = GetChallenge(&ApduSend, 8);
	if (nRet == ERROR)
	{
		return ERROR;
	}
	nRet = APDU_R_Malloc(&ApduRecv, 8);	
    	nRet = RficOperateCpu(iM1Handle, &ApduSend, &ApduRecv);
	APDU_S_Free(&ApduSend);
	printf("ret=[%d]\n", nRet);
	printf("\nApduRecv.SWA=%02X\n", ApduRecv.SWA );
	printf("ApduRecv.SWB=%02X\n", ApduRecv.SWB );
	if(nRet == 0)
	{ 
		printf("ApduRecv.LenOut=%d\n", ApduRecv.LenOut );
		
		for(i=0; i<ApduRecv.LenOut; i++)
			printf("%02X ", ApduRecv.DataOut[i] );

		if(ApduRecv.SWA != 0x90 || ApduRecv.SWB != 0x00)
		{
	        	printf("Select MF Failed!\n");
	        	TextOut(0, 4, ALIGN_LEFT, "    IC卡测试失败!"); 
	    		RficClose(iM1Handle); 
			WaitKey(1000);
	        	return(-1);
	    	} 
	}
	else 
	{
	    	TextOut(0, 4, ALIGN_LEFT, "    IC卡测试失败!"); 
	    	printf("选择主文件失败!\n"); 
	    	RficClose(iM1Handle); 
		FailBeep(); 
		return(-1);
	}
	
	Clear();
	Bcd_To_Ascii(ApduRecv.DataOut, szBuff, ApduRecv.LenOut);	
	TextOut(0, 3, ALIGN_CENTER, szBuff); 
	APDU_R_Free(&ApduRecv);
	printf( "Select MF OK!\n\n\n"); 
	TextOut(0, 4, ALIGN_CENTER, "非接触式IC卡测试成功!"); 

	OkBeep();
	
	//关闭设备, 断电
	RficClose(iM1Handle); 
	WaitKey(1000);


    return(0);
}
/*************************************************************************/ 
//CPU卡函数传输数据结构：	
//typedef struct {
//        unsigned char Command[4];
//        unsigned int Lc;
//        unsigned char * DataIn;
//        int Le;
//}APDU_SEND;
//
//typedef struct {
//        unsigned int LenOut;
//        unsigned char *DataOut;
//        unsigned char SWA;
//        unsigned char SWB;
//}APDU_RESP;
/*
 * 接触式CPU IC卡测试函数
 */
/*int DoCpuTest(int nSlot)	
{ 
    int slot = CARD_USER;
	char ATR[33]={0};
	int atr_len=0; 
	int ret=0; 
    	int nRet;
	int i;
	APDU_SEND ApduSend;
	APDU_RESP ApduRecv;
	WORK_MODE workMode;		//设置模式
	char szBuff[128] = {0};
	
	slot = nSlot; 
	printf("slot=%d.\n",slot);
	workMode.icc_style = 0x01; 	//ST_EMV
	workMode.wk_vol = 2;
	workMode.show_info = 1;
	workMode.wait_time = 0;   	//0表示使用默认值
	workMode.reserved = 0x02;    	//自动设置PPS
	
	Clear();
	//IccUserPower(1);	
	//打开设备文件
	if( (ret=IccOpen( slot  )) !=0 ){
		TextOut(0, 1, ALIGN_LEFT, "打开IC卡座失败!" );
		FailBeep();
		WaitKey(1000);
		return -1;
	}
	printf( "Open Icc OK!\n");
	
	//检测是否有卡插入
	if( IccDetect(slot) != 0 ){
		TextOut(0, 2, ALIGN_LEFT, "没有检测到IC卡");
		FailBeep();
	    IccClose(slot); 
		WaitKey(1000);
		return -1;
	}
	printf("IccDetect Icc OK!\n"); 
	 
	//初始化卡片
	if( (ret=IccInitEx( slot, ATR, &atr_len, workMode)) !=0 )
	{
		TextOut(0, 3, ALIGN_LEFT, "卡片初始化失败!" );
		FailBeep();
		WaitKey(1000);
		WaitKey(1000);
		return(-1);
	}
	else 
	{
		for( i=0; i<atr_len; i++ )
			 printf("%02X ", ATR[i] );
		printf("\n");
	}
	printf( "Init Icc OK!\n"); 

        nRet = GetChallenge(&ApduSend, 8);
        if (nRet == ERROR)
        {
                return ERROR;
        }
        nRet = APDU_R_Malloc(&ApduRecv, 8);
	ret = IccIsoCommand(slot, &ApduSend, &ApduRecv);
	APDU_S_Free(&ApduSend);
	printf("ret=[%d]\n", nRet);
	printf("\nApduRecv.SWA=%02X\n", ApduRecv.SWA );
	printf("ApduRecv.SWB=%02X\n", ApduRecv.SWB );
	if(nRet == 0)
	{ 
		printf("ApduRecv.LenOut=%d\n", ApduRecv.LenOut );
		
		for(i=0; i<ApduRecv.LenOut; i++)
			printf("%02X ", ApduRecv.DataOut[i] );

		if(ApduRecv.SWA != 0x90 || ApduRecv.SWB != 0x00)
		{
	        	printf("Select MF Failed!\n");
	        	TextOut(0, 4, ALIGN_LEFT, "    IC卡测试失败!"); 
			IccClose(slot);
			WaitKey(1000);
	        	return(-1);
	    	} 
	}
	else 
	{
	    	TextOut(0, 4, ALIGN_LEFT, "    IC卡测试失败!"); 
	    	printf("选择主文件失败!\n"); 
		IccClose(slot);
		FailBeep(); 
		return(-1);
	}
	
	Clear();
	Bcd_To_Ascii(ApduRecv.DataOut, szBuff, ApduRecv.LenOut);	
	TextOut(0, 3, ALIGN_CENTER, szBuff); 
	APDU_R_Free(&ApduRecv);
	printf( "Select MF OK!\n\n\n"); 

*/
/* 清华同方用户卡测试 */	
	//选择主文件
	/*memcpy(ApduSend.Command, "\x00\xA4\x02\x00", 4); 	//要发送的命令:
	ApduSend.Lc = 2;					//告诉底层要发送的数据长度=2;
	ApduSend.Le = 0;					//告诉底层接收最大长度
	memcpy(sendbuf, "\xEF\x05", 2); 			//设定2个字节的发送缓冲
	ApduSend.DataIn = (char *)&sendbuf;			//指向要发送的数据缓冲

	
	if( IccIsoCommand(slot, &ApduSend, &ApduRecv) == 0)
	{ 
		printf("ApduRecv.LenOut=%d\n", ApduRecv.LenOut );
		
		for(i=0; i<ApduRecv.LenOut; i++)
			printf("%02X ", ApduRecv.DataOut[i] );
			
		printf("\nApduRecv.SWA=%02X\n", ApduRecv.SWA );
		printf("ApduRecv.SWB=%02X\n", ApduRecv.SWB );
		
		if(ApduRecv.SWA != 0x90 || ApduRecv.SWB != 0x00)
		{
	        printf("Select EF05 Failed!\n");
	    IccClose(slot); 
	        return(-1);
	    } 
	}
	else 
	{
	    TextOut(0, 4, ALIGN_LEFT, "    IC卡测试失败!"); 
	    printf("选择主文件失败!\n"); 
	    IccClose(slot); 
		FailBeep(); 
		return(-1);
	}
	printf( "Select EF05 OK!\n\n\n"); 

    ///////////////////////写命令 00 D6 00 00-- 05 01 02 03 04 05
    ///////////////////////读命令 00 B0 00 00-- 05(Le)
    //发送读命令,发送命令同时会自动接收。
	memcpy(ApduSend.Command, "\x00\xB0\x00\x00", 4);  	//要发送的命令:00 B0 00 00
	ApduSend.Lc = 0; 					//告诉底层要发送的数据长度=0,发送命令后自动开始接收
	ApduSend.Le = 5; 					//期望接收数据长度(0 = 最大; -1 = 不接收)

	if(IccIsoCommand(slot, &ApduSend, &ApduRecv) == 0)
	{ 
		printf("ApduRecv.LenOut=%d\n", ApduRecv.LenOut );
		
		for(i=0; i<ApduRecv.LenOut; i++)
		{
			printf("%02X ",recvbuf[i]);
		}
		
		printf("\nApduRecv.SWA=%02X\n", ApduRecv.SWA );
		printf("ApduRecv.SWB=%02X\n", ApduRecv.SWB );
        
        if(ApduRecv.SWA != 0x90 || ApduRecv.SWB != 0x00) 
		{
	        printf("Get Response failed!\n");
	        IccClose(slot); 
	        return(-1);
	    }  
	}
	else
	{
	    TextOut(0, 4, ALIGN_LEFT, "    IC卡测试失败!"); 
	    printf("选择主文件失败!\n"); 
	    IccClose(slot); 
		FailBeep(); 
		return(-1);
	}
	printf( "Read EF05 OK!\n\n\n"); */
/* End */

/*	TextOut(0, 4, ALIGN_LEFT, "  接触IC卡测试成功!"); 

	OkBeep();
	
	//关闭设备, 断电
	IccClose(slot); 
	WaitKey(1000);

	return 0;
}*/
/*************************************************************************/ 
int DoCpuTest(int nSlot)	
{ 
    int slot = CARD_USER;
	char ATR[33]={0};
	int atr_len=0; 
	int ret=0; 
	int i;
	WORK_MODE workMode;		//设置模式
	
	slot = nSlot; 
	printf("slot=%d.\n",slot);
	workMode.icc_style = 0x01; 	//ST_EMV
	workMode.wk_vol = 2;
	workMode.show_info = 1;
	workMode.wait_time = 0;   	//0表示使用默认值
	workMode.reserved = 0x01;    	//自动设置PPS
	
	Clear();
	//IccUserPower(1);	
	//打开设备文件
	if( (ret=IccOpen( slot  )) !=0 ){
		TextOut(0, 1, ALIGN_LEFT, "打开IC卡座失败!" );
		FailBeep();
		WaitKey(1000);
		return -1;
	}
	printf( "Open Icc OK!\n");
	
	//检测是否有卡插入
	if( IccDetect(slot) != 0 ){
		TextOut(0, 2, ALIGN_LEFT, "没有检测到IC卡");
		FailBeep();
	    IccClose(slot); 
		WaitKey(1000);
		return -1;
	}
	printf("IccDetect Icc OK!\n"); 
	 
	//初始化卡片
	if( (ret=IccInitEx( slot, ATR, &atr_len, workMode)) !=0 )
	{
		TextOut(0, 3, ALIGN_LEFT, "卡片初始化失败!" );
		FailBeep();
		WaitKey(1000);
		WaitKey(1000);
		return(-1);
	}
	else 
	{
		for( i=0; i<atr_len; i++ )
			 printf("%02X ", ATR[i] );
		printf("\n");
	}
	printf( "Init Icc OK!\n"); 

	ReadCardNo(slot);
	TextOut(0, 4, ALIGN_LEFT, "  接触IC卡测试成功!"); 

	OkBeep();
	
	//关闭设备, 断电
	IccClose(slot); 
	WaitKey(1000);

	return 0;
}
/*************************************************************************/ 

void CPUTest()
{ 
	char ch; 
	int nSel;

	while(1)
	{
    	Clear(); 
    	TextOut(0, 2, ALIGN_LEFT, "      选择IC卡座     ");
    	TextOut(0, 3, ALIGN_LEFT, "先将卡插入相应的卡槽");
    	TextOut(0, 4, ALIGN_LEFT, "1. 用户卡");
    	TextOut(0, 5, ALIGN_LEFT, "2. SAM卡1");
    	TextOut(0, 6, ALIGN_LEFT, "3. SAM卡2");
    	TextOut(0, 7, ALIGN_LEFT, "4. SAM卡3");     //3
    	TextOut(0, 8, ALIGN_LEFT, "5. PCD-SAM");
    	TextOut(0, 9, ALIGN_LEFT, "0. 退出");
    	
    	while(1)
    	{
        	ch = WaitKey(0);   
    	    if(ch == KEY_0 || ch == KEY_CANCEL)
    	        return; 
    	        
    	    if(ch >= KEY_1 && ch <= KEY_4) 
    	    {
        	    nSel = ch-1;
        	    break;
       	    } 
    	    if(ch == KEY_5)
    	    {
    	        nSel = 9;
        	    Clear(); 
	            TextOut(0, 3, ALIGN_LEFT, "   暂不支持此卡槽!");
	            FailBeep();
        	    WaitKey(2000);
        	    break;
            }
        }
        if(nSel != 9)
		DoCpuTest(nSel);
    }
}

void M1Set()
{
	int nRet = 0; 

	Clear();
	SetScrFont(FONT20, WHITE);
	TextOut(0, 3, ALIGN_LEFT, "  读卡密钥:");
	SetScrFont(FONT20, BLUE);
	nRet = Input(8, 4, gTerm.szM1KeyA, 12, IME_NUMBER, BLACK, GREEN, FALSE, FALSE, TRUE);
	if(nRet != OK)
    	return;

	Clear();
	SetScrFont(FONT20, WHITE);
	TextOut(0, 3, ALIGN_LEFT, "  写卡密钥:");
	SetScrFont(FONT20, BLUE);
	nRet = Input(8, 4, gTerm.szM1KeyB, 12, IME_NUMBER, BLACK, GREEN, FALSE, FALSE, TRUE);
	if(nRet != OK)
    	return;

	Clear();
	SetScrFont(FONT20, WHITE);
	TextOut(0, 3, ALIGN_CENTER, "密钥设置成功");
	WaitKey(2000);
}

int M1TestCard()
{
	UCHAR UCKey;
	Clear();
	SetScrFont(FONT20, WHITE);

	//菜单
	ShowBmpFile(90, 60, "pic/button.bmp");
	TextOutByPixel(100, 65, "1.读卡");

	ShowBmpFile(90, 95, "pic/button.bmp");
	TextOutByPixel(100, 100, "2.写卡");

	ShowBmpFile(90, 130, "pic/button.bmp");
	TextOutByPixel(100, 135, "3.设置");

	ShowBmpFile(90, 165, "pic/button.bmp");
	TextOutByPixel(100, 170, "0.退出");


	UCKey = WaitLimitKey("\x01\x02\x03\x00",4, 0);
	switch(UCKey)
	
	{
		case KEY_CANCEL:
		case 0:
				return OK;
				break;
		case 1:
		  		RficTest();
		   		break;
		case 2:
		  		RficWirte();
		   		break;
		case 3:
		  		SetKeyM1();
		   		break;
	}

	return 0;
}
int CardReaderTest()
{
	
	UCHAR ucKey;
 	int ucKeyT;
	char sKeyName[20];
	//int  iHandle;
	//int handle;
	
	ClearKbd();
	while(1)
	{
		Clear();

		SetScrFont(FONT20, WHITE);

		//菜单
		ShowBmpFile(30, 60, "pic/button.bmp");
		TextOutByPixel(40, 65, "1.磁条卡");

		ShowBmpFile(175, 60, "pic/button.bmp");
		TextOutByPixel(185, 65, "2.逻辑加密");

		ShowBmpFile(30, 95, "pic/button.bmp");
		TextOutByPixel(40, 100, "3.接触CPU");

		ShowBmpFile(175, 95, "pic/button.bmp");
		TextOutByPixel(185, 100, "4.非接触CPU");

		ShowBmpFile(30, 130, "pic/button.bmp");
		TextOutByPixel(40, 135, "5.M1卡");

		ShowBmpFile(175, 130, "pic/button.bmp");
		TextOutByPixel(185, 135, "6.AT存储卡");

		ShowBmpFile(30, 165, "pic/button.bmp");
		TextOutByPixel(40, 170, "7.其他M1卡");

		ShowBmpFile(175, 165, "pic/button.bmp");
		TextOutByPixel(185, 170, "0.退出");

		ucKey = WaitLimitKey("\x00\x01\x02\x03\x04\x05\x06\x07\x12\x0A\x0D",11, 0);
		memset(sKeyName, 0, sizeof(sKeyName));
		GetKeyName(ucKey, sKeyName);
		DebugOut("press key: %s\n", sKeyName);

		switch(ucKey)
		{
			case KEY_CANCEL:
			case 0:
				return OK;
				break;
			case 1:
				// 磁条卡 
				MagCardTest();
				break;
			case 2:
				// 逻辑加密卡
				MEMTest();
				break;
			case 3:
				// 接触CPU
				CPUTest();
				break;
			case 4:
				FM17CPU();
				// 德卡
				//TestRfic();
				//ReadCard();
				// 非接同方
				//RFCardCPUTest(1);
				break;
			case 5:
				//if(iScanHandle)
				{
				//	switch_COM2(1);
				}
				switch_COM2(1);
				// 德卡
				M1TestCard();
				//handle = SERIAL_Open("/dev/ttyS2", 115200);
				//printf("----------------------------%d--------\n", handle);
				
				break;
			case 6:
				AT24Test();
				break;	
			case 7:
                        	while(1)
                        	{
                                	Clear();
                                	TextOut(0, 3, ALIGN_LEFT, "  1.读一块");
                                	TextOut(0, 4, ALIGN_LEFT, "  2.写一块");
                                	TextOut(0, 5, ALIGN_LEFT, "  3.初始化数值型");
                                	TextOut(0, 6, ALIGN_LEFT, "  4.增值");
                                	TextOut(0, 7, ALIGN_LEFT, "  5.减值");
                                	TextOut(0, 8, ALIGN_LEFT, "  6.读所有块");
                                	TextOut(0, 9, ALIGN_LEFT, "  7.CPU卡");
                                	ucKeyT = WaitLimitKey("\x01\x02\x03\x04\x05\x06\x07\x12", 8, 0);
                                	if ('\x01' == ucKeyT)
                                        	FM17TestRead();
                                	if ('\x02' == ucKeyT)
                                        	FM17TestWrite();
                                	if ('\x03' == ucKeyT)
                                        	FM17TestInit();
                                	if ('\x04' == ucKeyT)
                                        	FM17TestAddValue();
                                	if ('\x05' == ucKeyT)
                                        	FM17TestDecValue();
                                	if ('\x06' == ucKeyT)
                                        	FM17TestReadAllValue();
                                	if ('\x07' == ucKeyT)
                                        	FM17CPU();
                                	if ('\x12' == ucKeyT)
                                        	break;
				}
				break;
			case KEY_F1:
				// 德卡
				//TestRfic();
				ReadCardNO();
				break;
			case KEY_F2:
				break;
		}
	}
	return OK;
}



