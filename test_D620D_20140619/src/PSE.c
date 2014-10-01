//////////////////////////////////////////////////////////////////////////////
//20120328
//ymm
// 添加上了_Convert.h头文件
//////////////////////////////////////////////////////////////////////////////

#include <Main.h>

//////////////////////////////////////////////////////////////////////////////

PRECORD g_pRecord = NULL;
BOOL g_bPSE = TRUE;
int g_nAsi = 0;

//////////////////////////////////////////////////////////////////////////////
//CPU卡函数传输数据结构：
//typedef struct {
//        unsigned char Command[4]; 	// 命令头
//        unsigned int Lc;		// 要发送的数据长度(0表示无该字节)
//        unsigned char * DataIn;	// 要发送的数据缓冲区(没有数据赋NULL)
//        int Le;			// 要接收的数据长度(-1表示无该字节,
					// 0表示根据卡回送的字节)
//}APDU_SEND;
//
//typedef struct {
//        unsigned int LenOut;		// 实际接收到的数据长度
//        unsigned char *DataOut;	// 接收的数据缓冲区
//        unsigned char SWA;		// 响应码SW1
//        unsigned char SWB;		// 响应码SW2
//}APDU_RESP;
//
//typedef struct{
// 	UCHAR icc_style;		// 卡类型
//	UCHAR wk_vol;			// 工作电压
//	UCHAR show_info;		// 打印卡信息
//	UCHAR reserved;			// PPS。0X02=自动设置PPS
//	Unsigned long wait_time;	// 超时时间。0=默认
//}WORK_MODE;

// 初始化CPU卡
int OpenCpuCard(int nSlot)
{
	char szATR[ATR_TEMP_LEN] = {0};
        int nAtrLen = 0;
        int nRet = 0;
	WORK_MODE WorkMode;
	int i = 0;

	DebugOut("slot = %d.\n", nSlot);
	WorkMode.icc_style = ST_EMV;
	WorkMode.wk_vol	= VOLTAGE_5V;
	WorkMode.show_info = 1;
	WorkMode.reserved = MANUAL_RESP;
	WorkMode.wait_time = 0;

	// 打开设备文件
	nRet = IccOpen(nSlot);
	if (0 != nRet)
	{
		DebugOut("Open Icc ErrCode: %d\n", nRet);
		return OPEN_ERR;		
	}
	DebugOut("Open Icc ErrCode: OK \n");

	// 检测是否有卡插入 对SAM卡不检测
	nRet = IccDetect(nSlot);
	if (0 != nRet)
	{
		DebugOut("Detect Icc ErrCode: %d\n", nRet);
		return DETECT_ERR;		
	}
	DebugOut("Detect Icc ErrCode: OK \n");

	// 初始化卡片 

	nRet = IccInitEx(nSlot, szATR, &nAtrLen, WorkMode);
	if (0 != nRet)
	{
		DebugOut("Init Icc Ex ErrCode: %d\n", nRet);
		return DETECT_ERR;		
	}

	for ( i = 0; i < nAtrLen; i++)
	{
		DebugOut("%02x ", szATR[i]);
		DebugOut("\n");
	}
	
	DebugOut("Init Icc Ex ErrCode: OK \n");

	return OK;	
}
// 关闭CPU卡
int CloseCpuCard(int nSlot)
{
	if (0 != IccClose(nSlot))
	{
		return ERROR;
	}
	return OK;
}
// 
int GetMFData(BYTE nSlot, BYTE_PTR SWA, BYTE_PTR SWB, INT * nOutLen, BYTE_PTR data)
{
	int  nRet = 0;
	APDU_SEND ApduSend;
	APDU_RESP ApduRecv;

	if (SWA == NULL || SWB == NULL || data == NULL || nOutLen == NULL)
	{
		return ERROR;
	}

	if (ERROR == Select(&ApduSend, '\x00', '\x00', 2, "\x3F\x00"))
	{
		return ERROR;
	}
	
	if (ERROR == APDU_R_Malloc(&ApduRecv, 255))
	{
		return ERROR;
	}

	nRet = IccIsoCommand(nSlot, &ApduSend, &ApduRecv);
	if (0 != nRet)
	{
		APDU_S_Free(&ApduSend);
		APDU_R_Free(&ApduRecv);
		return nRet;
	}
        APDU_R_Length(&ApduRecv, nOutLen);
        APDU_R_SW1(&ApduRecv, SWA);
        APDU_R_SW2(&ApduRecv, SWB);
        APDU_R_Data(&ApduRecv, data);
        APDU_S_Free(&ApduSend);
        APDU_R_Free(&ApduRecv);

	return OK;	
}
// 
int GetDDFData(BYTE nSlot, BYTE_PTR SFI, BYTE_PTR SWA, BYTE_PTR SWB, INT * nOutLen, BYTE_PTR data)
{
	int  nRet = 0;
	APDU_SEND ApduSend;
	APDU_RESP ApduRecv;

	if (SWA == NULL || SWB == NULL || data == NULL || nOutLen == NULL)
	{
		return ERROR;
	}

	if (ERROR == Select(&ApduSend, '\x00', '\x00', 2, SFI))
	{
		return ERROR;
	}
	
	if (ERROR == APDU_R_Malloc(&ApduRecv, 255))
	{
		return ERROR;
	}

	nRet = IccIsoCommand(nSlot, &ApduSend, &ApduRecv);
	if (0 != nRet)
	{
		APDU_S_Free(&ApduSend);
		APDU_R_Free(&ApduRecv);
		return nRet;
	}
        APDU_R_Length(&ApduRecv, nOutLen);
        APDU_R_SW1(&ApduRecv, SWA);
        APDU_R_SW2(&ApduRecv, SWB);
        APDU_R_Data(&ApduRecv, data);
        APDU_S_Free(&ApduSend);
        APDU_R_Free(&ApduRecv);

	return OK;	
}

int GetResp(INT nSlot, BYTE_PTR SWA, BYTE_PTR SWB, INT nOutLen, BYTE_PTR data)
{
	int  nRet = 0;
	APDU_SEND ApduSend;
	APDU_RESP ApduRecv;

	if (SWB == NULL || SWB[0] == '\x00' || SWA == NULL || data == NULL)
	{
		return ERROR;
	}
	
	if (ERROR == GetResponse(&ApduSend, str_To_Dec(SWB)))
	{
		return ERROR;
	}
	
	if (ERROR == APDU_R_Malloc(&ApduRecv, 255))
	{
		return ERROR;
	}

	nRet = IccIsoCommand(nSlot, &ApduSend, &ApduRecv);
	if (0 != nRet)
	{
		APDU_S_Free(&ApduSend);
		APDU_R_Free(&ApduRecv);
		return nRet;
	}
        APDU_R_Length(&ApduRecv, &nOutLen);
        APDU_R_SW1(&ApduRecv, SWA);
        APDU_R_SW2(&ApduRecv, SWB);
        APDU_R_Data(&ApduRecv, data);
        APDU_S_Free(&ApduSend);
        APDU_R_Free(&ApduRecv);

	return OK;	
}
// 分析返回的数据MF
int AnalyMFData(BYTE nSlot, BYTE nOutLen, BYTE_PTR data, BYTE_PTR SFI, BYTE_PTR nRecCount, BYTE_PTR bPSE)
{
	BYTE nFCILen = 0;
	BYTE nFCIPriLen = 0;
	BYTE szFCIPri[255] = {0};
	BYTE nTmpLen = 0;
	BYTE nFCINext = 0;

	//9000
	// 返回数据分三种情况
	if (data == NULL || SFI == NULL)
	{
		return ERROR;
	}
	
	// 第一个字节'6F'0
	// 6F
	if ('\x6f' != data[nTmpLen])
	{
		return ERROR;
	}

	// FCI模板长度1
	//15
	nTmpLen += 1;
	nFCILen = data[1];

	// 一个字节'6F'2
	//84
	nTmpLen += 1;
	if ('\x84' != data[nTmpLen])
	{
		return ERROR;
	}

	// DF名长度3
	//0E
	nTmpLen += 1;
	nFCIPriLen = data[nTmpLen];

	// DF名4
	//315041592E5359532E4444463031
	nTmpLen += 1;
	memset(szFCIPri, 0, sizeof(szFCIPri));
	memcpy(szFCIPri, data + nTmpLen, nFCIPriLen);
	if (0 == memcmp(szFCIPri, "\x31\x50\x41\x59\x2e\x53\x59\x53\x2e\x44\x44\x46\x30\x31", 14))
	{
		*bPSE = 0;
		// 一个字节 '\xA5'4+nFCIPriLen
		//A5
		nTmpLen += nFCIPriLen;
		if ('\xA5' != data[nTmpLen])
		{
			return ERROR;
		}

		// FCI数据专用模板长度5+nFCIPriLen
		//03
		nTmpLen += 1;
		nFCINext = data[nTmpLen];

		// 一个字节'\x88'6+nFCIPriLen
		//88
		nTmpLen += 1;
		if ('\x88' != data[nTmpLen])
		{
			return ERROR;
		}
	 
		// SFI数据长度
		//01
		nTmpLen += 1;
		*nRecCount =  *(data + nTmpLen);
		// SF1
		//01
		nTmpLen += 1;
		*SFI = *(data + nTmpLen);
	
		nTmpLen += 1;

		DebugOut("%d == %d\n", nOutLen, nTmpLen);
		if (nTmpLen != nOutLen)
		{
			return ERROR;
		}
	}
	else
	{
		*bPSE = 1;
	}

	return OK;
}
// 分析返回的数据DDF
int AnalyDDFData(BYTE nSlot, BYTE nOutLen, BYTE_PTR data, BYTE_PTR SFI, BYTE_PTR nRecCount)
{
	BYTE nFCILen = 0;
	BYTE nFCIPriLen = 0;
	BYTE szFCIPri[255] = {0};
	BYTE nTmpLen = 0;
	BYTE nFCINext = 0;

	//9000
	// 返回数据分三种情况
	if (data == NULL || SFI == NULL)
	{
		return ERROR;
	}
	
	// 第一个字节'6F'0
	// 6F
	if ('\x6f' != data[nTmpLen])
	{
		return ERROR;
	}

	// FCI模板长度1
	//15
	nTmpLen += 1;
	nFCILen = data[1];

	// 一个字节'6F'2
	//84
	nTmpLen += 1;
	if ('\x84' != data[nTmpLen])
	{
		return ERROR;
	}

	// DF名长度3
	//0E
	nTmpLen += 1;
	nFCIPriLen = data[nTmpLen];

	// DF名4
	//315041592E5359532E4444463031
	nTmpLen += 1;
	memset(szFCIPri, 0, sizeof(szFCIPri));
	memcpy(szFCIPri, data + nTmpLen, nFCIPriLen);
	
	// 一个字节 '\xA5'4+nFCIPriLen
	//A5
	nTmpLen += nFCIPriLen;
	if ('\xA5' != data[nTmpLen])
	{
		return ERROR;
	}

	// FCI数据专用模板长度5+nFCIPriLen
	//03
	nTmpLen += 1;
	nFCINext = data[nTmpLen];

	// 一个字节'\x88'6+nFCIPriLen
	//88
	nTmpLen += 1;
	if ('\x88' != data[nTmpLen])
	{
		return ERROR;
	}
 
	// SFI数据长度
	//01
	nTmpLen += 1;
	*nRecCount =  *(data + nTmpLen);
	// SF1
	//01
	nTmpLen += 1;
	*SFI = *(data + nTmpLen);

	nTmpLen += 1;

	DebugOut("%d == %d\n", nOutLen, nTmpLen);
	if (nTmpLen != nOutLen)
	{
		return ERROR;
	}

	if (ERROR == GetSFIRecord(nSlot, *nRecCount, *SFI))
	{
		return ERROR;
	}

	return OK;
}
//============================================================================
// 获取SFI
int GetSFIRec(INT nSlot, INT RecNum, BYTE SFI, BYTE_PTR SWA, BYTE_PTR SWB,
	 INT *  nOutLen, BYTE_PTR data)
{
	int  nRet = 0;
	APDU_SEND ApduSend;
	APDU_RESP ApduRecv;

	DebugOut("ReadRecord %d\n", RecNum);	
	if (ERROR == ReadRecord(&ApduSend, RecNum, SFI))
	{
		return ERROR;
	}
	
	if (ERROR == APDU_R_Malloc(&ApduRecv, 255))
	{
		return ERROR;
	}

	nRet = IccIsoCommand(nSlot, &ApduSend, &ApduRecv);
	if (0 != nRet)
	{
		APDU_S_Free(&ApduSend);
		APDU_R_Free(&ApduRecv);
		return nRet;
	}
        APDU_R_Length(&ApduRecv, nOutLen);
        APDU_R_SW1(&ApduRecv, SWA);
        APDU_R_SW2(&ApduRecv, SWB);
        APDU_R_Data(&ApduRecv, data);
        APDU_S_Free(&ApduSend);
        APDU_R_Free(&ApduRecv);

	return OK;	
}
// 分析返回的数据SFI是DDF或是ADF
int AnalyRecData(BYTE nSlot, BYTE nOutLen, BYTE_PTR data, BYTE_PTR SFI)
{
	BYTE nFCILen = 0;
	BYTE nFCIPriLen = 0;
	BYTE szFCIPri[255] = {0};
	BYTE nTmpLen = 0;
	BYTE nFCINext = 0;
	BYTE nRecCount;
	
	// 返回数据分2种情况
	if (data == NULL || SFI == NULL)
	{
		return ERROR;
	}
	
	// 第一个字节'6F'0
	// 6F
	if ('\x6f' != data[nTmpLen])
	{
		return ERROR;
	}

	// FCI模板长度1
	//15
	nTmpLen += 1;
	nFCILen = data[1];

	// 一个字节'6F'2
	//84
	nTmpLen += 1;
	if ('\x84' != data[nTmpLen])
	{
		return ERROR;
	}

	// DF名长度3
	//0E
	nTmpLen += 1;
	nFCIPriLen = data[nTmpLen];

	// DF名4
	//315041592E5359532E4444463031
	nTmpLen += 1;
	memset(szFCIPri, 0, sizeof(szFCIPri));
	memcpy(szFCIPri, data + nTmpLen, nFCIPriLen);

	// 一个字节 '\xA5'4+nFCIPriLen
	//A5
	nTmpLen += nFCIPriLen;
	if ('\xA5' != data[nTmpLen])
	{
		return ERROR;
	}

	// FCI数据专用模板长度5+nFCIPriLen
	//03
	nTmpLen += 1;
	nFCINext = data[nTmpLen];

	// 一个字节'\x88'6+nFCIPriLen
	//88
	nTmpLen += 1;
	if ('\x88' == data[nTmpLen])
	{
		//DDF
		// SFI数据长度
		//01
		nTmpLen += 1;
		nRecCount =  *(data + nTmpLen);
		// SF1
		//01
		nTmpLen += 1;
		*SFI = *(data + nTmpLen);
		/*SFI = *(data + nTmpLen) >> 3;
		*SFI &= 0x03;*/
		
		nTmpLen += 1;
		DebugOut("%d == %d\n", nOutLen, nTmpLen);
		if (nTmpLen != nOutLen)
		{
			return ERROR;
		}

		if (ERROR == GetDDFSFI(nSlot, nRecCount, SFI))
		{
			return ERROR;
		}
	}
	if ('\x50' == data[nTmpLen])
	{
		// ADF
	}

	return OK;
}
//////////////////////////////////////////////////////////////////////////////
// 获取记录集信息
int GetSFIRecord(BYTE nSlot, BYTE RecNum, BYTE SFI)
{
	BYTE	SWA[3] = {0};
	BYTE	SWB[3] = {0};
	INT 	nOutLen = 0 ;
	BYTE	data[255] = {0};
	int i = 0;

	for (i = 0; i < RecNum; i++)
	{
		// 获取单条记录
		if (OK != GetSFIRec(nSlot, i + 1, SFI, SWA, SWB, &nOutLen, data))
		{
			CloseCpuCard(nSlot);
			return ERROR;
		}

		if (0 != strcmp(SWA, "90") || 0 != strcmp(SWB, "00"))
		{
			if (0 == strcmp(SWA, "61"))
			{

				memset(SWA, 0, sizeof(SWA));
				memset(data, 0, sizeof(data));
				nOutLen = 0;
				if (OK == GetResp(nSlot, SWA, SWB, nOutLen, data))
				{
					if (0 == strcmp(SWA, "90") || 0 == strcmp(SWB, "00"))
					{
							goto LABLE_SFI_DATA;
					}
				}
			
			}
			else if (0 != strcmp(SWA, "6A") && 0 != strcmp(SWB, "83"))
			{
				CloseCpuCard(nSlot);
				return -2;
			}
			CloseCpuCard(nSlot);
			return ERROR;
		}
	
		LABLE_SFI_DATA:

		// 循环获取记录数据添加到全局变量里
		if (OK != AnalyRecData(nSlot, nOutLen, data, &SFI))
		{
			CloseCpuCard(nSlot);
			return ERROR;
		}
	}
	return OK;
}

int GetMFSFI(BYTE nSlot, BYTE_PTR nRecCount, BYTE_PTR SFI, BYTE_PTR bPSE)
{
	BYTE	SWA[3] = {0};
	BYTE	SWB[3] = {0};
	INT 	nOutLen = 0 ;
	BYTE	data[255] = {0};

	if (OK != GetMFData(nSlot, SWA, SWB, &nOutLen, data))
	{
		CloseCpuCard(nSlot);
		return ERROR;
	}
	DebugOut("GetMFData()\n");

	if (0 != strcmp(SWA, "90") || 0 != strcmp(SWB, "00"))
	{
		if (0 == strcmp(SWA, "61"))
		{
			memset(SWA, 0, sizeof(SWA));
			memset(data, 0, sizeof(data));
			nOutLen = 0;
			if (OK == GetResp(nSlot, SWA, SWB, nOutLen, data))
			{
				if (0 == strcmp(SWA, "90") || 0 == strcmp(SWB, "00"))
				{
					goto LABLE_SFI_DATA;
				}
			}
			
		}
		CloseCpuCard(nSlot);
		return ERROR;
	}
	
LABLE_SFI_DATA:
	// 获取数据
	if (OK != AnalyMFData(nSlot, nOutLen, data, SFI, nRecCount, bPSE))
	{
		CloseCpuCard(nSlot);
		return ERROR;
	}

	return OK;
}

int GetDDFSFI(BYTE nSlot, BYTE_PTR nRecCount, BYTE_PTR SFI)
{
	BYTE	SWA[3] = {0};
	BYTE	SWB[3] = {0};
	INT 	nOutLen = 0 ;
	BYTE	data[255] = {0};

	if (OK != GetDDFData(nSlot, SFI, SWA, SWB, &nOutLen, data))
	{
		CloseCpuCard(nSlot);
		return ERROR;
	}
	DebugOut("GetDDFData()\n");

	if (0 != strcmp(SWA, "90") || 0 != strcmp(SWB, "00"))
	{
		if (0 == strcmp(SWA, "61"))
		{
			memset(SWA, 0, sizeof(SWA));
			memset(data, 0, sizeof(data));
			nOutLen = 0;
			if (OK == GetResp(nSlot, SWA, SWB, nOutLen, data))
			{
				if (0 == strcmp(SWA, "90") || 0 == strcmp(SWB, "00"))
				{
				goto LABLE_SFI_DATA;
				}
			}
			
		}
	CloseCpuCard(nSlot);
	return ERROR;
	}
	
LABLE_SFI_DATA:
	// 获取数据
	if (OK != AnalyDDFData(nSlot, nOutLen, data, SFI, nRecCount))
	{
		CloseCpuCard(nSlot);
		return ERROR;
	}
return OK;
}

//////////////////////////////////////////////////////////////////////////////

int GetMF(int nSlot)
{
	BYTE	SFI = 0;
	BYTE	nRecCount = 0;
	BYTE	bPSE = 0;

	if (!g_bPSE)
	{
		// 调用AID列表方法
	}

	if ( 0 != OpenCpuCard(nSlot))
	{
		return ERROR;
	}
	DebugOut("OpenCpuCard()\n");
	if (OK != GetMFSFI(nSlot, &nRecCount, &SFI, &bPSE))
	{
		CloseCpuCard(nSlot);
		return ERROR;
	}
	DebugOut("GetMFSFI()\n");
	if (0 == bPSE)
	{
		// 调用AID列表方法
	}
	else if (1 == bPSE)
	{	
		SFI = 0x01;
	}
	// 读取SFI记录
	if (OK != GetSFIRecord(nSlot, nRecCount, SFI))
	{
		CloseCpuCard(nSlot);
		return ERROR;
	}
	DebugOut("GetSFIRecord()\n");

	CloseCpuCard(nSlot);
	return OK;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// 通过AID名子获取数据
int GetDFByName(BYTE_PTR pAid, BYTE nLen, BYTE_PTR SWA, BYTE_PTR SWB, BYTE_PTR data, BYTE_PTR nOutLen)
{
	int  nRet = 0;
	APDU_SEND ApduSend;
	APDU_RESP ApduRecv;

	if (pAid == NULL)
	{
		return ERROR;
	}

	if (SWA == NULL || SWB == NULL || data == NULL || nOutLen == NULL)
	{
		return ERROR;
	}

	if (ERROR == Select(&ApduSend, '\x04', '\x00', nLen, pAid))
	{
		return ERROR;
	}
	
	if (ERROR == APDU_R_Malloc(&ApduRecv, 255))
	{
		return ERROR;
	}

	nRet = IccIsoCommand(nSlot, &ApduSend, &ApduRecv);
	if (0 != nRet)
	{
		APDU_S_Free(&ApduSend);
		APDU_R_Free(&ApduRecv);
		return nRet;
	}
        APDU_R_Length(&ApduRecv, nOutLen);
        APDU_R_SW1(&ApduRecv, SWA);
        APDU_R_SW2(&ApduRecv, SWB);
        APDU_R_Data(&ApduRecv, data);
        APDU_S_Free(&ApduSend);
        APDU_R_Free(&ApduRecv);

	return OK;	
}
// 分析返回的数据AID
int AnalyAidData(BYTE nSlot, BYTE nOutLen, BYTE_PTR data, BYTE_PTR SFI)
{
	BYTE nFCILen = 0;
	BYTE nFCIPriLen = 0;
	BYTE szFCIPri[255] = {0};
	BYTE nTmpLen = 0;
	BYTE nFCINext = 0;
	BYTE nRecCount;
	
	// 返回数据分2种情况
	if (data == NULL || SFI == NULL)
	{
		return ERROR;
	}
	
	// 第一个字节'6F'0
	// 6F
	if ('\x6f' != data[nTmpLen])
	{
		return ERROR;
	}

	// FCI模板长度1
	//15
	nTmpLen += 1;
	nFCILen = data[1];

	// 一个字节'6F'2
	//84
	nTmpLen += 1;
	if ('\x84' != data[nTmpLen])
	{
		return ERROR;
	}

	// DF名长度3
	//0E
	nTmpLen += 1;
	nFCIPriLen = data[nTmpLen];

	// DF名4
	//315041592E5359532E4444463031
	nTmpLen += 1;
	memset(szFCIPri, 0, sizeof(szFCIPri));
	memcpy(szFCIPri, data + nTmpLen, nFCIPriLen);

	if (/* FCI中的DF名=AID?*/)
	{

	}
	else
	{

	}

	// 一个字节 '\xA5'4+nFCIPriLen
	//A5
	nTmpLen += nFCIPriLen;
	if ('\xA5' != data[nTmpLen])
	{
		return ERROR;
	}

	// FCI数据专用模板长度5+nFCIPriLen
	//03
	nTmpLen += 1;
	nFCINext = data[nTmpLen];

	// 一个字节'\x88'6+nFCIPriLen
	//88
	nTmpLen += 1;
	if ('\x88' == data[nTmpLen])
	{
		//DDF
		// SFI数据长度
		//01
		nTmpLen += 1;
		nRecCount =  *(data + nTmpLen);
		// SF1
		//01
		nTmpLen += 1;
		*SFI = *(data + nTmpLen);
		/*SFI = *(data + nTmpLen) >> 3;
		*SFI &= 0x03;*/
		
		nTmpLen += 1;
		DebugOut("%d == %d\n", nOutLen, nTmpLen);
		if (nTmpLen != nOutLen)
		{
			return ERROR;
		}

		if (ERROR == GetDDFSFI(nSlot, nRecCount, SFI))
		{
			return ERROR;
		}
	}
	if ('\x50' == data[nTmpLen])
	{
		// ADF
	}

	return OK;
}
//获取AID数据
int GetAidData(BYTE_PTR pAid, BYTE nLen)
{
	BYTE	SWA[3] = {0};
	BYTE	SWB[3] = {0};
	INT 	nOutLen = 0 ;
	BYTE	data[255] = {0};
	
	// 清空候选列表
	if (ERROR == GetDFByName(pAid, nLen, SWA, SWB, data, &nOutLen))
	{
		return ERROR;
	}
	DebugOut("GetDFByName()\n");

	if (0 != strcmp(SWA, "90") || 0 != strcmp(SWB, "00"))
	{
		if (0 == strcmp(SWA, "61"))
		{
			memset(SWA, 0, sizeof(SWA));
			memset(data, 0, sizeof(data));
			nOutLen = 0;
			if (OK == GetResp(nSlot, SWA, SWB, nOutLen, data))
			{
				if (0 == strcmp(SWA, "90") || 0 == strcmp(SWB, "00"))
				{
					goto LABLE_SFI_DATA;
				}
			}
			
		}
		CloseCpuCard(nSlot);
		return ERROR;
	}
	
	LABLE_SFI_DATA:

	// 循环获取记录数据添加到全局变量里
	if (OK != AnalyAidData(nSlot, nOutLen, data, &SFI))
	{
		CloseCpuCard(nSlot);
		return ERROR;
	}
	return OK;
}

int Aid()
{
	
}
