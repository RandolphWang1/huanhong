//////////////////////////////////////////////////////////////////////////////
// 本公司的读卡器
#include"Main.h"
//////////////////////////////////////////////////////////////////////////////
// 读块
void FM17TestRead()
{
	char szBuff[1024] = {0};
	char szSAK[10] = {0};
	char szUID[10] = {0};
	char szATQA[10] = {0};
	int nSector = 0;
	int nBlock = 0;	
	int nRet = 0;

	strcpy(gTerm.szM1KeyA, "FFFFFFFFFFFF");
	while(1)
	{
		//扇区
		Clear();
		SetScrFont(FONT20, WHITE);
		TextOut(0, 4, ALIGN_LEFT, "扇区:");
		memset(szBuff, 0, sizeof(szBuff));
		SetScrFont(FONT20, BLUE);
		nRet = Input(6, 4, szBuff, 2, IME_NUMBER, BLACK, GREEN, FALSE, FALSE, FALSE);
		if(nRet != OK)
		{
			continue;
		}
		nSector = atoi(szBuff);
		if (nSector < 0 || nSector > 15)
		{
			continue;
		}
		//块
	LABLE_B:
		SetScrFont(FONT20, WHITE);
		TextOut(0, 5, ALIGN_LEFT, "块: ");
		memset(szBuff, 0, sizeof(szBuff));
		SetScrFont(FONT20, BLUE);
		nRet = Input(6, 5, szBuff, 1, IME_NUMBER, BLACK, GREEN, FALSE, FALSE, FALSE);
		if(nRet != OK)
		{
			continue;
		}
		nBlock = atoi(szBuff);
		if (nBlock < 0 || nBlock > 3)
		{
			goto LABLE_B;
		}
		break;
	}
	SwitchCom2_HH();
	printf("============ymm init================\n");
	while (OK != FM17Init(NULL, 2, 19200, NULL))
	{
		break;
	}
	while(1)
	{
		printf("============ymm connect================\n");
		memset(szBuff, 0, sizeof(szBuff));
		if (OK == FM17Connect(NULL, szBuff, NULL))
		{
			break;	
		}
	}
	printf("============%s================\n", szBuff);
	while(1)
	{
		Clear();
		TextOut(0, 4, ALIGN_CENTER, "请把卡片放在射频区");
		if (TRUE == KbdHit())
		{
			if (OK == WaitLimitKey("\x12", 1, 0))
			{
				SerialClose();
				return;
			}
			ClearKbd();
		}
		if (OK != FM17Reset(NULL, NULL))
		{
			continue ;
		}
		printf("============ymm cardactive================\n");
		if (OK != FM17CardActive(NULL, 1, szSAK, szUID, szATQA, NULL))
		{
			continue ;
		}
		printf("SAK: %02x\n", szSAK[0]);
		printf("UID: %02x%02x%02x%02x\n", szUID[0], szUID[1], szUID[2], szUID[3]);
		printf("ATQA: %02x%02x\n", szATQA[0], szATQA[1]);
		printf("============ymm verify============%s====\n", gTerm.szM1KeyA);
		if (OK != FM17Verify(NULL, nSector, gTerm.szM1KeyA, NULL))
		{
			continue ;
		}
		printf("============ymm readblock================\n");
		memset(szBuff, 0, sizeof(szBuff));
		if (OK != FM17ReadBlock(NULL, nSector, nBlock, szBuff, NULL))
		{
			continue;
		}

		/*if (szBuff[0] == 0x00)
		{
			continue;	
		}*/

		break;
	}
	printf("============ymm close================\n");
	SerialClose();
	Clear();
	TextOut(3, 5, ALIGN_LEFT, szBuff);

	WaitKey(0);
	return ;	
}
// 写块
void FM17TestWrite()
{
	char szBuff[1024];
	char szName[1024];
	char szBuffT[1024];
	int nBlock = 0;
	int nSector = 0;
	int nRet = 0;
	char szSAK[10] = {0};
	char szUID[10] = {0};
	char szATQA[10] = {0};

	strcpy(gTerm.szM1KeyA, "FFFFFFFFFFFF");
	while(1)
	{
		//扇区
		Clear();
		SetScrFont(FONT20, WHITE);
		TextOut(0, 4, ALIGN_LEFT, "扇区:");
		memset(szBuff, 0, sizeof(szBuff));
		SetScrFont(FONT20, BLUE);
		nRet = Input(6, 4, szBuff, 2, IME_NUMBER, BLACK, GREEN, FALSE, FALSE, FALSE);
		if(nRet != OK)
		{
			continue;
		}
		nSector = atoi(szBuff);
                if (nSector < 0 || nSector > 15)
                {
                        continue;
                }
                //块
        LABLE_B:

		SetScrFont(FONT20, WHITE);
		TextOut(0, 5, ALIGN_LEFT, "块: ");
		memset(szBuff, 0, sizeof(szBuff));
		SetScrFont(FONT20, BLUE);
		nRet = Input(6, 5, szBuff, 1, IME_NUMBER, BLACK, GREEN, FALSE, FALSE, FALSE);
		if(nRet != OK)
		{
			continue;
		}
		nBlock = atoi(szBuff);
		if (nBlock < 0 || nBlock > 2)
		{
			goto LABLE_B;
		}
		//写入的数据
		SetScrFont(FONT20, WHITE);
		TextOut(0, 6, ALIGN_LEFT, "写入数据:");
		memset(szBuff, 0, sizeof(szBuff));
		SetScrFont(FONT20, BLUE);
		nRet = Input(1, 7, szBuff, 16, IME_NUMBER, BLACK, GREEN, FALSE, TRUE, FALSE);
		if(nRet != OK)
			continue;
		break;
	}
	SwitchCom2_HH();
	printf("============%s================\n", szBuff);
	printf("============ymm init================\n");
	while (OK != FM17Init(NULL, 2, 19200, NULL))
	{
		break;
	}
	while(1)
	{
		printf("============ymm connect================\n");
		memset(szName, 0, sizeof(szName));
		if (OK == FM17Connect(NULL, szName, NULL))
		{
			break;	
		}
	}
	printf("============%s================\n", szName);
	while(1)
	{
	
		ClearKbd();
		while(1)
		{
			if (TRUE == KbdHit())
			{
				if (OK == WaitLimitKey("\x12", 1, 0))
				{
					SerialClose();
					return ;
				}
			}
			break;
		}
		if (OK != FM17Reset(NULL, NULL))
		{
			continue ;
		}
		printf("============ymm cardactive================\n");
		if (OK != FM17CardActive(NULL, 1, szSAK, szUID, szATQA, NULL))
		{
			continue ;
		}
		printf("SAK: %02x\n", szSAK[0]);
		printf("UID: %02x%02x%02x%02x\n", szUID[0], szUID[1], szUID[2], szUID[3]);
		printf("ATQA: %02x%02x\n", szATQA[0], szATQA[1]);
		printf("============ymm verify============%s====\n", gTerm.szM1KeyA);
		if (OK != FM17Verify(NULL, nSector, gTerm.szM1KeyA, NULL))
		{
			continue ;
		}
		printf("============ymm writeblock================\n");
		if (OK != FM17WriteBlock(NULL, nSector, nBlock, szBuff, NULL))
		{
			continue;
		}
		printf("============ymm readblock================\n");
		memset(szBuffT, 0, sizeof(szBuffT));
		if (OK != FM17ReadBlock(NULL, nSector, nBlock, szBuffT, NULL))
		{
			continue;
		}

		/*if (szBuffT[0] == 0x00)
		{
			continue;	
		}*/

		break;
	}
	printf("============ymm close================\n");
	SerialClose();

	Clear();
	TextOut(3, 5, ALIGN_LEFT, szBuffT);

	WaitKey(0);
	return ;	
}
// 增值
void FM17TestAddValue()
{
	char szBuff[1024];
	char szName[1024];
	char szBuffT[1024];
	int nBlock = 0;
	int nSector = 0;
	int nRet = 0;
	char szSAK[10] = {0};
	char szUID[10] = {0};
	char szATQA[10] = {0};

	strcpy(gTerm.szM1KeyA, "FFFFFFFFFFFF");
	while(1)
	{
		//扇区
		Clear();
		SetScrFont(FONT20, WHITE);
		TextOut(0, 4, ALIGN_LEFT, "扇区:");
		memset(szBuff, 0, sizeof(szBuff));
		SetScrFont(FONT20, BLUE);
		nRet = Input(6, 4, szBuff, 2, IME_NUMBER, BLACK, GREEN, FALSE, FALSE, FALSE);
		if(nRet != OK)
		{
			continue;
		}
		nSector = atoi(szBuff);
                if (nSector < 0 || nSector > 15)
                {
                        continue;
                }
                //块
        LABLE_B:
		SetScrFont(FONT20, WHITE);
		TextOut(0, 5, ALIGN_LEFT, "块: ");
		memset(szBuff, 0, sizeof(szBuff));
		SetScrFont(FONT20, BLUE);
		nRet = Input(6, 5, szBuff, 1, IME_NUMBER, BLACK, GREEN, FALSE, FALSE, FALSE);
		if(nRet != OK)
		{
			continue;
		}
		nBlock = atoi(szBuff);
		if (nBlock < 0 || nBlock > 2)
		{
			goto LABLE_B;
		}
		//写入的数据
		SetScrFont(FONT20, WHITE);
		TextOut(0, 6, ALIGN_LEFT, "增值数据:");
		memset(szBuff, 0, sizeof(szBuff));
		SetScrFont(FONT20, BLUE);
		nRet = Input(6, 7, szBuff, 8, IME_NUMBER, BLACK, GREEN, FALSE, TRUE, FALSE);
		if(nRet != OK)
			continue;
		break;
	}
	printf("============%s================\n", szBuff);
	printf("============ymm init================\n");
	SwitchCom2_HH();
	while (OK != FM17Init(NULL, 2, 19200, NULL))
	{
		break;
	}
	while(1)
	{
		printf("============ymm connect================\n");
		memset(szName, 0, sizeof(szName));
		if (OK == FM17Connect(NULL, szName, NULL))
		{
			break;	
		}
	}
	printf("============%s================\n", szName);
	while(1)
	{
	
		ClearKbd();
		while(1)
		{
			if (TRUE == KbdHit())
			{
				if (OK == WaitLimitKey("\x12", 1, 0))
				{
					SerialClose();
					return ;
				}
			}
			break;
		}
		if (OK != FM17Reset(NULL, NULL))
		{
			continue ;
		}
		printf("============ymm cardactive================\n");
		if (OK != FM17CardActive(NULL, 1, szSAK, szUID, szATQA, NULL))
		{
			continue ;
		}
		printf("SAK: %02x\n", szSAK[0]);
		printf("UID: %02x%02x%02x%02x\n", szUID[0], szUID[1], szUID[2], szUID[3]);
		printf("ATQA: %02x%02x\n", szATQA[0], szATQA[1]);
		printf("============ymm verify================\n");
		if (OK != FM17Verify(NULL, nSector, gTerm.szM1KeyA, NULL))
		{
			continue ;
		}
		printf("============ymm addvalue================\n");
		if (OK != FM17AddValue(NULL, nSector, nBlock, szBuff, NULL))
		{
			continue;
		}
		printf("============ymm transfer================\n");
		if (OK != FM17Transfer(NULL, nSector, nBlock, NULL))
		{
			continue;
		}
		printf("============ymm readblock================\n");
		memset(szBuffT, 0, sizeof(szBuffT));
		if (OK != FM17ReadBlockValue(NULL, nSector, nBlock, szBuffT, NULL))
		{
			continue;
		}
		/*if (szBuffT[0] == 0x00)
		{
			continue;	
		}*/

		break;
	}
	printf("============ymm close================\n");
	SerialClose();

	Clear();
	TextOut(3, 5, ALIGN_LEFT, szBuffT);

	WaitKey(0);
	return ;	
}

// 减值
void FM17TestDecValue()
{
	char szBuff[1024];
	char szName[1024];
	char szBuffT[1024];
	int nSector = 0;
	int nBlock = 0;
	int nRet = 0;
	char szSAK[10] = {0};
	char szUID[10] = {0};
	char szATQA[10] = {0};

	strcpy(gTerm.szM1KeyA, "FFFFFFFFFFFF");
	while(1)
	{
		//扇区
		Clear();
		SetScrFont(FONT20, WHITE);
		TextOut(0, 4, ALIGN_LEFT, "扇区:");
		memset(szBuff, 0, sizeof(szBuff));
		SetScrFont(FONT20, BLUE);
		nRet = Input(6, 4, szBuff, 2, IME_NUMBER, BLACK, GREEN, FALSE, FALSE, FALSE);
		if(nRet != OK)
		{
			continue;
		}
		nSector = atoi(szBuff);
                if (nSector < 0 || nSector > 15)
                {
                        continue;
                }
                //块
        LABLE_B:
		SetScrFont(FONT20, WHITE);
		TextOut(0, 5, ALIGN_LEFT, "块: ");
		memset(szBuff, 0, sizeof(szBuff));
		SetScrFont(FONT20, BLUE);
		nRet = Input(6, 5, szBuff, 1, IME_NUMBER, BLACK, GREEN, FALSE, FALSE, FALSE);
		if(nRet != OK)
		{
			continue;
		}
		nBlock = atoi(szBuff);
		if (nBlock < 0 || nBlock > 2)
		{
			goto LABLE_B;
		}
		//写入的数据
		SetScrFont(FONT20, WHITE);
		TextOut(0, 6, ALIGN_LEFT, "减值数据:");
		memset(szBuff, 0, sizeof(szBuff));
		SetScrFont(FONT20, BLUE);
		nRet = Input(6, 7, szBuff, 8, IME_NUMBER, BLACK, GREEN, FALSE, TRUE, FALSE);
		if(nRet != OK)
			continue;
		break;
	}
	printf("============%s================\n", szBuff);
	printf("============ymm init================\n");
	SwitchCom2_HH();
	while (OK != FM17Init(NULL, 2, 19200, NULL))
	{
		break;
	}
	while(1)
	{
		printf("============ymm connect================\n");
		memset(szName, 0, sizeof(szName));
		if (OK == FM17Connect(NULL, szName, NULL))
		{
			break;	
		}
	}
	printf("============%s================\n", szName);
	while(1)
	{
	
		ClearKbd();
		while(1)
		{
			if (TRUE == KbdHit())
			{
				if (OK == WaitLimitKey("\x12", 1, 0))
				{
					SerialClose();
					return ;
				}
			}
			break;
		}
		if (OK != FM17Reset(NULL, NULL))
		{
			continue ;
		}
		printf("============ymm cardactive================\n");
		if (OK != FM17CardActive(NULL, 1, szSAK, szUID, szATQA, NULL))
		{
			continue ;
		}
		printf("SAK: %02x\n", szSAK[0]);
		printf("UID: %02x%02x%02x%02x\n", szUID[0], szUID[1], szUID[2], szUID[3]);
		printf("ATQA: %02x%02x\n", szATQA[0], szATQA[1]);
		printf("============ymm verify================\n");
		if (OK != FM17Verify(NULL, nSector, gTerm.szM1KeyA, NULL))
		{
			continue ;
		}
		printf("============ymm writeblock================\n");
		if (OK != FM17DecValue(NULL, nSector, nBlock, szBuff, NULL))
		{
			continue;
		}
		printf("============ymm transfer================\n");
		if (OK != FM17Transfer(NULL, nSector, nBlock, NULL))
		{
			continue;
		}
		printf("============ymm readblock================\n");
		memset(szBuffT, 0, sizeof(szBuffT));
		if (OK != FM17ReadBlockValue(NULL, nSector, nBlock, szBuffT, NULL))
		{
			continue;
		}

	/*	if (szBuffT[0] == 0x00)
		{
			continue;	
		}*/

		break;
	}
	printf("============ymm close================\n");
	SerialClose();

	Clear();
	TextOut(3, 5, ALIGN_LEFT, szBuffT);

	WaitKey(0);
	return ;	
}
// 初始化块数据
void FM17TestInit()
{
	char szBuff[1024];
	char szName[1024];
	char szBuffT[1024];
	int nSector = 0;
	int nBlock = 0;
	int nRet = 0;
	char szSAK[10] = {0};
	char szUID[10] = {0};
	char szATQA[10] = {0};

	strcpy(gTerm.szM1KeyA, "FFFFFFFFFFFF");
	while(1)
	{
		Clear();
		//扇区
		SetScrFont(FONT20, WHITE);
		TextOut(0, 4, ALIGN_LEFT, "扇区:");
		memset(szBuff, 0, sizeof(szBuff));
		SetScrFont(FONT20, BLUE);
		nRet = Input(6, 4, szBuff, 2, IME_NUMBER, BLACK, GREEN, FALSE, FALSE, FALSE);
		if(nRet != OK)
		{
			continue;
		}
		nSector = atoi(szBuff);
                if (nSector < 0 || nSector > 15)
                {
                        continue;
                }
                //块
        LABLE_B:
		SetScrFont(FONT20, WHITE);
		TextOut(0, 5, ALIGN_LEFT, "块 :");
		memset(szBuff, 0, sizeof(szBuff));
		SetScrFont(FONT20, BLUE);
		nRet = Input(6, 5, szBuff, 1, IME_NUMBER, BLACK, GREEN, FALSE, FALSE, FALSE);
		if(nRet != OK)
		{
			continue;
		}
		nBlock = atoi(szBuff);
		if (nBlock < 0 || nBlock > 2)
		{
			goto LABLE_B;
		}
		//写入的数据
		SetScrFont(FONT20, WHITE);
		TextOut(0, 6, ALIGN_LEFT, "初始化数据: ");
		memset(szBuff, 0, sizeof(szBuff));
		SetScrFont(FONT20, BLUE);
		nRet = Input(6, 7, szBuff, 8, IME_NUMBER, BLACK, GREEN, FALSE, TRUE, FALSE);
		if(nRet != OK)
			continue;
		break;
	}
	printf("============%s================\n", szBuff);
	printf("============ymm init================\n");
	SwitchCom2_HH();
	while (OK != FM17Init(NULL, 2, 19200, NULL))
	{
		break;
	}
	while(1)
	{
		printf("============ymm connect================\n");
		memset(szName, 0, sizeof(szName));
		if (OK == FM17Connect(NULL, szName, NULL))
		{
			break;	
		}
	}
	printf("============%s================\n", szName);
	while(1)
	{
	
		ClearKbd();
		while(1)
		{
			if (TRUE == KbdHit())
			{
				if (OK == WaitLimitKey("\x12", 1, 0))
				{
					SerialClose();
					return ;
				}
			}
			break;
		}
		if (OK != FM17Reset(NULL, NULL))
		{
			continue ;
		}
		printf("============ymm cardactive================\n");
		if (OK != FM17CardActive(NULL, 1, szSAK, szUID, szATQA, NULL))
		{
			continue ;
		}
		printf("SAK: %02x\n", szSAK[0]);
		printf("UID: %02x%02x%02x%02x\n", szUID[0], szUID[1], szUID[2], szUID[3]);
		printf("ATQA: %02x%02x\n", szATQA[0], szATQA[1]);
		printf("============ymm verify================\n");
		if (OK != FM17Verify(NULL, nSector, gTerm.szM1KeyA, NULL))
		{
			continue ;
		}
		printf("============ymm initblock================\n");
		if (OK != FM17InitBlock(NULL, nSector, nBlock, szBuff, NULL))
		{
			continue;
		}
		printf("============ymm readblock================\n");
		memset(szBuffT, 0, sizeof(szBuffT));
		if (OK != FM17ReadBlockValue(NULL, nSector, nBlock, szBuffT, NULL))
		{
			continue;
		}

	/*	if (szBuffT[0] == 0x00)
		{
			continue;	
		}*/

		break;
	}
	printf("============ymm close================\n");
	SerialClose();

	Clear();
	TextOut(3, 5, ALIGN_LEFT, szBuffT);

	WaitKey(0);
	return ;	
}

// 读
void FM17TestReadAllValue()
{
	char szBuff[1024];
	char szName[1024];
	char szBuffT[1024];
	int nSector = 0;
	int nBlock = 0;
	char szSAK[10] = {0};
	char szUID[10] = {0};
	char szATQA[10] = {0};
	int nRet = 0;

	strcpy(gTerm.szM1KeyA, "FFFFFFFFFFFF");
	printf("============ymm init================\n");
	SwitchCom2_HH();
	while (OK != FM17Init(NULL, 2, 19200, NULL))
	{
		break;
	}
	while(1)
	{
		printf("============ymm connect================\n");
		memset(szName, 0, sizeof(szName));
		if (OK == FM17Connect(NULL, szName, NULL))
		{
			break;	
		}
	}
	printf("============%s================\n", szName);
	while(1)
	{
		while(1)
		{
			if (nSector == 16)
			{
				Clear();
				TextOut(0, 4, ALIGN_CENTER, "是否继续读取");
				nRet = WaitLimitKey("\x12\x0f", 2, 0);
				if (nRet == KEY_CANCEL)
				{
					SerialClose();
					return ;
				}
			}
			break;
		}
		nBlock = 0;
		nSector = 0;
		printf("============ymm Reset================\n");
		if (OK != FM17Reset(NULL, NULL))
		{
			continue ;
		}
		printf("============ymm cardactive================\n");
		if (OK != FM17CardActive(NULL, 1, szSAK, szUID, szATQA, NULL))
		{
			continue ;
		}
		printf("SAK: %02x\n", szSAK[0]);
		printf("UID: %02x%02x%02x%02x\n", szUID[0], szUID[1], szUID[2], szUID[3]);
		printf("ATQA: %02x%02x\n", szATQA[0], szATQA[1]);
		while(1)
		{
			if (nBlock % 4 == 0)
			{
				printf("============ymm verify================\n");
				if (OK != FM17Verify(NULL, nSector, gTerm.szM1KeyA, NULL))
				{
					continue ;
				}
			}
			printf("============ymm readblock================\n");
			memset(szBuffT, 0, sizeof(szBuffT));
			if (OK != FM17ReadBlockValue(NULL, nSector, nBlock, szBuffT, NULL))
			{
				continue;
			}

			printf("============%d===========%d=====\n", nSector, nBlock);
			Clear();
			memset(szBuff, 0, sizeof(szBuff));
			sprintf(szBuff, "扇区 : %d", nSector);
			TextOut(3, 3, ALIGN_LEFT, szBuff);
			memset(szBuff, 0, sizeof(szBuff));
			sprintf(szBuff, "块 : %d", nBlock);
			TextOut(3, 4, ALIGN_LEFT, szBuff);
			TextOut(3, 5, ALIGN_LEFT, szBuffT);
			nBlock++;
			if (nBlock % 3 == 0 && nBlock != 0)
			{
				nSector++;
				nBlock = 0;
				if (nSector > 15)
				{
					break;	
				}
			}
		}
	}
	printf("============ymm close================\n");
	SerialClose();

	WaitKey(0);
	return ;	
}
//////////////////////////////////////////////////////////////////////////////
void FM17CPU()
{
	char szName[1024];
	char szBuff[1024];
	char szBuffT[1024];
	int nRet = 0;
	int i = 0;
	char szSAK[10] = {0};
	char szUID[10] = {0};
	char szATQA[10] = {0};
        APDU_SEND ApduSend;
        APDU_RESP ApduRecv;
	int nCpu = 0;
	char ucKeyT = 0;
	int bFlag = 0;

	SwitchCom2_HH();
	printf("============ymm init================\n");
	while (OK != FM17Init(NULL, 2, 19200, NULL))
	{
		break;
	}
	printf("============ymm connect================\n");
	memset(szName, 0, sizeof(szName));
	while(1)
	{
		if (OK == FM17Connect(NULL, szName, NULL))
		{
			break;
		}
	}
	printf("============%s================\n", szName);
	Clear();
	TextOut(0, 2, ALIGN_LEFT, "  1.CPU卡 主目录");
        TextOut(0, 3, ALIGN_LEFT, "  2.CPU卡 随机数");
        ucKeyT = WaitLimitKey("\x01\x02\x12", 3, 0);
        if ('\x01' == ucKeyT)
               nCpu = 1;
        if ('\x02' == ucKeyT)
               nCpu = 2;
	while(1)
	{
		Clear();
        	TextOut(0, 3, ALIGN_CENTER, "按CANCEL键退出");
		if (KEY_CANCEL == WaitLimitKey("\x12", 1, 1000))
		{
			SerialClose();
			return;
		}
		Clear();
		if (0 == bFlag)
		{
			printf("============ymm Reset================\n");
			if (OK != FM17Reset(NULL, NULL))
			{
				bFlag = 0;
				continue ;
			}
			/*printf("============ymm TYPEA================\n");
			if (OK != FM17TYPEA())
			{
				bFlag = 0;
				continue ;
			}*/
			printf("============ymm cardactive================\n");
			if (OK != FM17CardActive(NULL, 1, szSAK, szUID, szATQA, NULL))
			{
				bFlag = 0;
				continue ;
			}
			printf("SAK: %02x\n", szSAK[0]);
			printf("UID: %02x%02x%02x%02x\n", szUID[0], szUID[1], szUID[2], szUID[3]);
			printf("ATQA: %02x%02x\n", szATQA[0], szATQA[1]);
			printf("============ymm CardReset================\n");
			if (OK != FM17CardReset(NULL, szBuffT, NULL))
			{
				bFlag = 0;
				continue ;
			}
			bFlag = 1;
		}

		switch(nCpu)
		{
			case 1 :
				printf("============ymm Select================\n");
				nRet = Select(&ApduSend, 0x00, 0x00, 2, "\x02\x3F");
				break;
			case 2 :
				printf("============ymm GetChallenge================\n");
				nRet = GetChallenge(&ApduSend, 8);
				break;
		}
        	if (nRet == ERROR)
        	{
			bFlag = 0;
                	continue;
        	}
        	nRet = APDU_R_Malloc(&ApduRecv, 8);
		nRet = OperCpuAPDU(NULL, &ApduSend, &ApduRecv, FALSE, NULL);
        	APDU_S_Free(&ApduSend);
        	printf("ret=[%d]\n", nRet);
        	printf("\nApduRecv.SWA=%02X\n", ApduRecv.SWA );
        	printf("ApduRecv.SWB=%02X\n", ApduRecv.SWB );
        	if(nRet == 0)
        	{
                	printf("ApduRecv.LenOut=%d\n", ApduRecv.LenOut );
                	for(i=0; i<ApduRecv.LenOut; i++)
                        	printf("%02X ", ApduRecv.DataOut[i] );
			
			memset(szBuff, 0, sizeof(szBuff));
			switch(nCpu)
			{
				case 1:
					Clear();
					memset(szBuff, 0, sizeof(szBuff));
					memcpy(szBuff, ApduRecv.DataOut+ 5, ApduRecv.LenOut - 5);
        				TextOut(0, 4, ALIGN_CENTER, szBuff);
					break;
				case 2:
					Clear();
					Bcd_To_Ascii(ApduRecv.DataOut, szBuff, ApduRecv.LenOut);
        				TextOut(0, 4, ALIGN_CENTER, szBuff);
					break;
			}
        		APDU_R_Free(&ApduRecv);
        	}
		else
		{
				bFlag = 0;
                        	printf("Select MF Failed!\n");
                		TextOut(0, 4, ALIGN_LEFT, "    IC卡测试失败!");
						FailBeep();
        			APDU_R_Free(&ApduRecv);
                        	WaitKey(3000);
				continue ;
		}
		break;	
	}
	printf("\n============ymm close================\n");
	SerialClose();
	ClearLine(5, 5);
   	TextOut(3, 5, ALIGN_LEFT, "    IC卡测试成功!");
	OkBeep();
	WaitKey(0);

	return ;	
}

//////////////////////////////////////////////////////////////////////////////

