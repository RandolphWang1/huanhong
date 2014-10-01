//#include  "Sms.h"
//#include  "unicode.h"
//#include  "stdio.h"
//#include  "string.h"
//#include  "stdlib.h"
// 可打印字符串转换为字节数据
// 如："C8329BFD0E01" --> {0xC8, 0x32, 0x9B, 0xFD, 0x0E, 0x01}
// 输入: pSrc - 源字符串指针
//       nSrcLength - 源字符串长度
// 输出: pDst - 目标数据指针
// 返回: 目标数据长度
#include "Main.h"

extern int AT_GetOK();
extern int ClearGprsBuffer();

int gsmString2Bytes(const char* pSrc, unsigned char* pDst, int nSrcLength)
{
	int i;
	
	for (i = 0; i < nSrcLength; i += 2)
	{
		// 输出高4位
		if ((*pSrc >= '0') && (*pSrc <= '9'))
		{
			*pDst = (*pSrc - '0') << 4;
		}
		else
		{
			*pDst = (*pSrc - 'A' + 10) << 4;
		}

		pSrc++;

		// 输出低4位
		if ((*pSrc>='0') && (*pSrc<='9'))
		{
			*pDst |= *pSrc - '0';
		}
		else
		{
			*pDst |= *pSrc - 'A' + 10;
		}

		pSrc++;
		pDst++;
	}

	// 返回目标数据长度
	return (nSrcLength / 2);
}

// 字节数据转换为可打印字符串
// 如：{0xC8, 0x32, 0x9B, 0xFD, 0x0E, 0x01} --> "C8329BFD0E01" 
// 输入: pSrc - 源数据指针
//       nSrcLength - 源数据长度
// 输出: pDst - 目标字符串指针
// 返回: 目标字符串长度
int gsmBytes2String(const unsigned char* pSrc, char* pDst, int nSrcLength)
{
	const char tab[]="0123456789ABCDEF";	// 0x0-0xf的字符查找表
	int i;
	
	for (i =0; i < nSrcLength; i++)
	{
		*pDst++ = tab[*pSrc >> 4];		// 输出高4位
		*pDst++ = tab[*pSrc & 0x0f];	// 输出低4位
		pSrc++;
	}

	// 输出字符串加个结束符
	*pDst = '\0';

	// 返回目标字符串长度
	return (nSrcLength * 2);
}

// 7bit编码
// 输入: pSrc - 源字符串指针
//       nSrcLength - 源字符串长度
// 输出: pDst - 目标编码串指针
// 返回: 目标编码串长度
int gsmEncode7bit(const char* pSrc, unsigned char* pDst, int nSrcLength)
{
	int nSrc;		// 源字符串的计数值
	int nDst;		// 目标编码串的计数值
	int nChar;		// 当前正在处理的组内字符字节的序号，范围是0-7
	unsigned char nLeft;	// 上一字节残余的数据

	// 计数值初始化
	nSrc = 0;
	nDst = 0;

	// 将源串每8个字节分为一组，压缩成7个字节
	// 循环该处理过程，直至源串被处理完
	// 如果分组不到8字节，也能正确处理
	while (nSrc < nSrcLength)
	{
		// 取源字符串的计数值的最低3位
		nChar = nSrc & 7;

		// 处理源串的每个字节
		if(nChar == 0)
		{
			// 组内第一个字节，只是保存起来，待处理下一个字节时使用
			nLeft = *pSrc;
		}
		else
		{
			// 组内其它字节，将其右边部分与残余数据相加，得到一个目标编码字节
			*pDst = (*pSrc << (8-nChar)) | nLeft;

			// 将该字节剩下的左边部分，作为残余数据保存起来
			nLeft = *pSrc >> nChar;

			// 修改目标串的指针和计数值
			pDst++;
			nDst++;
		}

		// 修改源串的指针和计数值
		pSrc++;
		nSrc++;
	}

	// 返回目标串长度
	return nDst;
}

// 7bit解码
// 输入: pSrc - 源编码串指针
//       nSrcLength - 源编码串长度
// 输出: pDst - 目标字符串指针
// 返回: 目标字符串长度
int gsmDecode7bit(const unsigned char* pSrc, char* pDst, int nSrcLength)
{
	int nSrc;		// 源字符串的计数值
	int nDst;		// 目标解码串的计数值
	int nByte;		// 当前正在处理的组内字节的序号，范围是0-6
	unsigned char nLeft;	// 上一字节残余的数据

	// 计数值初始化
	nSrc = 0;
	nDst = 0;
	
	// 组内字节序号和残余数据初始化
	nByte = 0;
	nLeft = 0;

	// 将源数据每7个字节分为一组，解压缩成8个字节
	// 循环该处理过程，直至源数据被处理完
	// 如果分组不到7字节，也能正确处理
	while(nSrc<nSrcLength)
	{
		// 将源字节右边部分与残余数据相加，去掉最高位，得到一个目标解码字节
		*pDst = ((*pSrc << nByte) | nLeft) & 0x7f;

		// 将该字节剩下的左边部分，作为残余数据保存起来
		nLeft = *pSrc >> (7-nByte);

		// 修改目标串的指针和计数值
		pDst++;
		nDst++;

		// 修改字节计数值
		nByte++;

		// 到了一组的最后一个字节
		if(nByte == 7)
		{
			// 额外得到一个目标解码字节
			*pDst = nLeft;

			// 修改目标串的指针和计数值
			pDst++;
			nDst++;

			// 组内字节序号和残余数据初始化
			nByte = 0;
			nLeft = 0;
		}

		// 修改源串的指针和计数值
		pSrc++;
		nSrc++;
	}

	// 输出字符串加个结束符
	*pDst = '\0';

	// 返回目标串长度
	return nDst;
}

// UCS2编码
// 输入: pSrc - 源字符串指针
//       nSrcLength - 源字符串长度
// 输出: pDst - 目标编码串指针
// 返回: 目标编码串长度
int gsmEncodeUcs2(const char* pSrc, unsigned char* pDst, int nSrcLength)
{
	int nDstLength;		// UNICODE宽字符数目
	unsigned short wchar[128];	// UNICODE串缓冲区
	int i;
	
	// 字符串-->UNICODE串
	nDstLength = MultiByteToWideChar(pSrc, nSrcLength, wchar);
	
	// 高低字节对调，输出
	for(i=0; i<nDstLength; i++)
	{
		*pDst++ = wchar[i] >> 8;		// 先输出高位字节
		*pDst++ = wchar[i] & 0xFF;		// 后输出低位字节
	}
	
	// 返回目标编码串长度
	return nDstLength * 2;
}

// UCS2解码
// 输入: pSrc - 源编码串指针
//       nSrcLength -  源编码串长度
// 输出: pDst -  目标字符串指针
// 返回: 目标字符串长度
int gsmDecodeUcs2(unsigned char* pSrc, char* pDst, int nSrcLength)
{
	int nDstLength;		// UNICODE宽字符数目
	
	// UNICODE串-->字符串
	nDstLength = WideCharToMultiByte(pSrc, nSrcLength, pDst);

	// 输出字符串加个结束符
	pDst[nDstLength] = '\0';

	// 返回目标字符串长度
	return nDstLength;
}

// 正常顺序的字符串转换为两两颠倒的字符串，若长度为奇数，补'F'凑成偶数
// 如："8613851872468" --> "683158812764F8"
// 输入: pSrc - 源字符串指针
//       nSrcLength - 源字符串长度
// 输出: pDst - 目标字符串指针
// 返回: 目标字符串长度
int gsmInvertNumbers(const char* pSrc, char* pDst, int nSrcLength)
{
	int nDstLength;		// 目标字符串长度
	char ch;			// 用于保存一个字符
	int i;
	// 复制串长度
	nDstLength = nSrcLength;

	// 两两颠倒
	for(i=0; i<nSrcLength;i+=2)
	{
		ch = *pSrc++;		// 保存先出现的字符
		*pDst++ = *pSrc++;	// 复制后出现的字符
		*pDst++ = ch;		// 复制先出现的字符
	}

	// 源串长度是奇数吗？
	if(nSrcLength & 1)
	{
		*(pDst-2) = 'F';	// 补'F'
		nDstLength++;		// 目标串长度加1
	}

	// 输出字符串加个结束符
	*pDst = '\0';

	// 返回目标字符串长度
	return nDstLength;
}

// 两两颠倒的字符串转换为正常顺序的字符串
// 如："683158812764F8" --> "8613851872468"
// 输入: pSrc - 源字符串指针
//       nSrcLength - 源字符串长度
// 输出: pDst - 目标字符串指针
// 返回: 目标字符串长度
int gsmSerializeNumbers(const char* pSrc, char* pDst, int nSrcLength)
{
	int nDstLength;		// 目标字符串长度
	char ch;			// 用于保存一个字符
	int i;
	// 复制串长度
	nDstLength = nSrcLength;

	// 两两颠倒
	for(i=0; i<nSrcLength;i+=2)
	{
		ch = *pSrc++;		// 保存先出现的字符
		*pDst++ = *pSrc++;	// 复制后出现的字符
		*pDst++ = ch;		// 复制先出现的字符
	}

	// 最后的字符是'F'吗？
	if(*(pDst-1) == 'F')
	{
		pDst--;
		nDstLength--;		// 目标字符串长度减1
	}

	// 输出字符串加个结束符
	*pDst = '\0';

	// 返回目标字符串长度
	return nDstLength;
}

// PDU编码，用于编制、发送短消息
// 输入: pSrc - 源PDU参数指针
// 输出: pDst - 目标PDU串指针
// 返回: 目标PDU串长度
int gsmEncodePdu(const SM_PARAM* pSrc, char* pDst)
{
	int nLength;			// 内部用的串长度
	int nDstLength;			// 目标PDU串长度
	unsigned char buf[256];	// 内部用的缓冲区
	//unsigned char tmp[256]={0};	// 内部用的缓冲区
	//int i;

	// SMSC地址信息段
	nLength = strlen(pSrc->SCA);	// SMSC地址字符串的长度	
	buf[0] = (char)((nLength & 1) == 0 ? nLength : nLength + 1) / 2 + 1;	// SMSC地址信息长度
	buf[1] = 0x91;		// 固定: 用国际格式号码
	nDstLength = gsmBytes2String(buf, pDst, 2);		// 转换2个字节到目标PDU串
	nDstLength += gsmInvertNumbers(pSrc->SCA, &pDst[nDstLength], nLength);	// 转换SMSC号码到目标PDU串

	// TPDU段基本参数、目标地址等
	nLength = strlen(pSrc->TPA);	// TP-DA地址字符串的长度
	buf[0] = 0x11;					// 是发送短信(TP-MTI=01)，TP-VP用相对格式(TP-VPF=10)
	buf[1] = 0;						// TP-MR=0
	buf[2] = (char)nLength;			// 目标地址数字个数(TP-DA地址字符串真实长度)
	buf[3] = 0x91;					// 固定: 用国际格式号码
	nDstLength += gsmBytes2String(buf, &pDst[nDstLength], 4);		// 转换4个字节到目标PDU串
	nDstLength += gsmInvertNumbers(pSrc->TPA, &pDst[nDstLength], nLength);	// 转换TP-DA到目标PDU串

	// TPDU段协议标识、编码方式、用户信息等
	nLength = strlen(pSrc->TP_UD);	// 用户信息字符串的长度
	buf[0] = pSrc->TP_PID;			// 协议标识(TP-PID)
	buf[1] = pSrc->TP_DCS;			// 用户信息编码方式(TP-DCS)
	buf[2] = 0;						// 有效期(TP-VP)为5分钟
	if(pSrc->TP_DCS == GSM_7BIT)	
	{
		// 7-bit编码方式
		buf[3] = nLength;			// 编码前长度
		nLength = gsmEncode7bit(pSrc->TP_UD, &buf[4], nLength+1) + 4;	// 转换TP-DA到目标PDU串
		nDstLength += gsmBytes2String(buf, &pDst[nDstLength], nLength);		// 转换该段数据到目标PDU串
	}
	else if(pSrc->TP_DCS == GSM_UCS2)
	{
		// UCS2编码方式
		buf[3] = gsmEncodeUcs2(pSrc->TP_UD, &buf[4], nLength);	// 转换TP-DA到目标PDU串
		nLength = buf[3] + 4;		// nLength等于该段数据长度
		Hex2Asc(&pDst[nDstLength],buf,nLength);
		nDstLength +=(nLength*2);
	}

	// 返回目标字符串长度
	return nDstLength;
}

// PDU解码，用于接收、阅读短消息
// 输入: pSrc - 源PDU串指针
// 输出: pDst - 目标PDU参数指针
// 返回: 用户信息串长度
int gsmDecodePdu(const char* pSrc, SM_PARAM* pDst)
{
	int nDstLength;			// 目标PDU串长度
	unsigned char tmp;		// 内部用的临时字节变量
	unsigned char buf[256];	// 内部用的缓冲区
	char buff[20]="";
	int i;
	
	// SMSC地址信息段
	gsmString2Bytes(pSrc, &tmp, 2);	// 取长度
	tmp = (tmp - 1) * 2;	// SMSC号码串长度
	pSrc += 4;			// 指针后移，忽略了SMSC地址格式
	gsmSerializeNumbers(pSrc, pDst->SCA, tmp);	// 转换SMSC号码到目标PDU串
	pSrc += tmp;		// 指针后移
	printf("中心号码:%s\n",pDst->SCA);
	// TPDU段基本参数
	gsmString2Bytes(pSrc, &tmp, 2);	// 取基本参数
	pSrc += 2;		// 指针后移

	// 取回复号码
	gsmString2Bytes(pSrc, &tmp, 2);	// 取长度
	if(tmp & 1) tmp += 1;	// 调整奇偶性
	pSrc += 4;			// 指针后移，忽略了回复地址(TP-RA)格式
	gsmSerializeNumbers(pSrc, pDst->TPA, tmp);	// 取TP-RA号码
	pSrc += tmp;		// 指针后移
	printf("回复号码:%s\n",pDst->TPA);
	
	// TPDU段协议标识、编码方式、用户信息等
	gsmString2Bytes(pSrc, (unsigned char*)&pDst->TP_PID, 2);	// 取协议标识(TP-PID)
	printf("协议标识:%d\n",pDst->TP_PID);
	pSrc += 2;		// 指针后移
	gsmString2Bytes(pSrc, (unsigned char*)&pDst->TP_DCS, 2);	// 取编码方式(TP-DCS)
	printf("编码方式:%d\n",pDst->TP_DCS);

	pSrc += 2;		// 指针后移
	printf("剩余信息:%s\n",pSrc);
	gsmSerializeNumbers(pSrc, buff, 14);		// 服务时间戳字符串(TP_SCTS) 
	sprintf(pDst->TP_SCTS,"20");
	memcpy(pDst->TP_SCTS+2,buff,2);
	strcat(pDst->TP_SCTS+4,"-");
	memcpy(pDst->TP_SCTS+5,buff+2,2);
	strcat(pDst->TP_SCTS+7,"-");
	memcpy(pDst->TP_SCTS+8,buff+4,2);
	strcat(pDst->TP_SCTS+10," ");
	memcpy(pDst->TP_SCTS+11,buff+6,2);
	strcat(pDst->TP_SCTS+13,":");
	memcpy(pDst->TP_SCTS+14,buff+8,2);
	strcat(pDst->TP_SCTS+16,":");
	memcpy(pDst->TP_SCTS+17,buff+10,2);
	
	printf("服务器时间戳:%s\n",pDst->TP_SCTS);
	
	pSrc += 14;		// 指针后移
	gsmString2Bytes(pSrc, &tmp, 2);	// 用户信息长度(TP-UDL)
	pSrc += 2;		// 指针后移
	if(pDst->TP_DCS == GSM_7BIT)	
	{
		// 7-bit解码
		printf("GSM_7Bit编码:");
		for(i=0;i<nDstLength;i++)
		{
			printf("%.2X ",buf[i]);
		}
		printf("\n");
		nDstLength = gsmString2Bytes(pSrc, buf, tmp & 7 ? (int)tmp * 7 / 4 + 2 : (int)tmp * 7 / 4);	// 格式转换
		gsmDecode7bit(buf, pDst->TP_UD, nDstLength);	// 转换到TP-DU
		printf("7-bit解码:%s\n",pDst->TP_UD);
		nDstLength = tmp;
	}
	else if(pDst->TP_DCS == GSM_UCS2)
	{
		// UCS2解码
		nDstLength = gsmString2Bytes(pSrc, buf, tmp * 2);			// 格式转换
		printf("unicode编码:");
		for(i=0;i<nDstLength;i++)
		{
			printf("%.2X ",buf[i]);
		}
		printf("\n");
		nDstLength = gsmDecodeUcs2(buf, pDst->TP_UD, nDstLength);	// 转换到TP-DU
		printf("ucs2解码:%s\n",pDst->TP_UD);
	}
	else
	{
		nDstLength = gsmString2Bytes(pSrc, buf, tmp * 2);
		printf("GSM_8Bit编码:");
		for(i=0;i<nDstLength;i++)
		{
			printf("%.2X ",buf[i]);
		}
		printf("\n");
		gsmDecode8bit(buf, pDst->TP_UD, nDstLength);
		printf("8-bit解码:%s\n",pDst->TP_UD);
	}
	// 返回目标字符串长度
	return nDstLength;
}

// 发送短消息，仅发送命令，不读取应答
// 输入: pSrc - 源PDU参数指针
int gsmSendMessage(SM_PARAM* pSrc)
{
	int nPduLength;		// PDU串长度
	unsigned char nSmscLength;	// SMSC串长度
	int nLength;		// 串口收到的数据长度
	char cmd[16];		// 命令串
	char pdu[512];		// PDU串
	char ans[128];		// 应答串
	
	nPduLength = gsmEncodePdu(pSrc, pdu);	// 根据PDU参数，编码PDU串
	strcat(pdu, "\x01a");		// 以Ctrl-Z结束
	
	gsmString2Bytes(pdu, &nSmscLength, 2);	// 取PDU串中的SMSC信息长度
	nSmscLength++;		// 加上长度字节本身
	// 命令中的长度，不包括SMSC信息长度，以数据字节计
	sprintf(cmd, "AT+CMGS=%d\r", nPduLength / 2 - nSmscLength);	// 生成命令
	printf("cmd=%s\n",cmd);

	WriteComm(cmd, strlen(cmd),0);	// 先输出命令串

	nLength = ReadComm(ans, 128);	// 读应答数据

	// 根据能否找到"\r\n> "决定成功与否
	if(nLength == 4 && strncmp(ans, "\r\n> ", 4) == 0)
	{
		printf("%s\n",pdu);
		return WriteComm(pdu, strlen(pdu),1);		// 得到肯定回答，继续输出PDU串
	}

	return 0;
}

// 读取短消息，仅发送命令，不读取应答
// 用+CMGL代替+CMGR，可一次性读出全部短消息
int gsmReadMessageList()
{
	return WriteComm("AT+CMGL\r", 8,0);
}

int gsmReadMessage(int index)
{
	char  cmd[12+1]="";
	int   msize;
	
	sprintf(cmd,"AT+CMGR=%d\r",index);
	msize=strlen(cmd);
	return WriteComm(cmd, msize,0);
}

// 删除短消息，仅发送命令，不读取应答
// 输入: index - 短消息序号，1-255
int gsmDeleteMessage(int index)
{
	char cmd[16];		// 命令串

	sprintf(cmd, "AT+CMGD=%d\r", index);	// 生成命令

	// 输出命令串
	return WriteComm(cmd, strlen(cmd),0);
}

// 读取GSM MODEM的应答，可能是一部分
// 输出: pBuff - 接收应答缓冲区
// 返回: GSM MODEM的应答状态, GSM_WAIT/GSM_OK/GSM_ERR
// 备注: 可能需要多次调用才能完成读取一次应答，首次调用时应将pBuff初始化
int gsmGetResponse(SM_BUFF* pBuff)
{
	int nLength;		// 串口收到的数据长度
	int nState;

	// 从串口读数据，追加到缓冲区尾部
	nLength = ReadComm(&pBuff->data[pBuff->len], 128);	
	pBuff->len += nLength;

	// 确定GSM MODEM的应答状态
	nState = GSM_WAIT;
	if ((nLength > 0) && (pBuff->len >= 4))
	{
		if (strncmp(&pBuff->data[pBuff->len - 4], "OK\r\n", 4) == 0)  
			nState = GSM_OK;
		else if (strstr(pBuff->data, "+CMS ERROR") != NULL) 
			nState = GSM_ERR;
	}

	return nState;
}

// 从列表中解析出全部短消息
// 输入: pBuff - 短消息列表缓冲区
// 输出: pMsg - 短消息缓冲区
// 返回: 短消息条数
int gsmParseMessageList(SM_PARAM* pMsg, SM_BUFF* pBuff)
{
	int nMsg;			// 短消息计数值
	char* ptr;			// 内部用的数据指针

	nMsg = 0;
	ptr = pBuff->data;

	// 循环读取每一条短消息, 以"+CMGL:"开头
	while((ptr = strstr(ptr, "+CMGL:")) != NULL)
	{
		ptr += 6;		// 跳过"+CMGL:", 定位到序号
		sscanf(ptr, "%d", &pMsg->index);	// 读取序号

		ptr = strstr(ptr, "\r\n");	// 找下一行
		if (ptr != NULL)
		{
			ptr += 2;		// 跳过"\r\n", 定位到PDU
			printf("Pdu:=[%s]\n",ptr);
			gsmDecodePdu(ptr, pMsg);	// PDU串解码

			pMsg++;		// 准备读下一条短消息
			nMsg++;		// 短消息计数加1
		}
	}

	return nMsg;
}

//-------------------------------------------------------------------------
// 初始化GPRS短信功能             
//0:PDU模式
//1:text模式
//-------------------------------------------------------------------------

int Init_SMS(int model,char *CSCA)
{
	char cmd[18];
	int nret;
	
	memset(cmd,0x00,sizeof(cmd));
	sprintf(cmd, "AT+CMGF=%d\x0d\x0a",model);	// 
	printf("cmd=%s\n",cmd);
	WriteComm(cmd, strlen(cmd),0);
	nret=AT_GetOK(iGprsHandle,2000);
	if(nret==ERROR)
	{
		return ERROR;
	}
	memset(cmd,0x00,sizeof(cmd));
	sprintf(cmd, "AT+CSCA=\"+86%s\"\x0d\x0a",CSCA);	// 
	printf("cmd=%s\n",cmd);
	WriteComm(cmd, strlen(cmd),0);
	nret=AT_GetOK(iGprsHandle,2000);
	if(nret==ERROR)
	{
		return ERROR;
	}
	return OK;
 }

//---------------------------------------------------------------------------------
// 开启GPRS模式函数
// 该函数在GSM模式下开启GPRS功能模式
//---------------------------------------------------------------------------------
int Open_GPGS_Mode()
{
    return(1); 
}  

//----------------------------------------------------------------------------------
//  关闭GPRS功能函数
// 改函数关闭GPES功能，返回到短信模式下 
//----------------------------------------------------------------------------------
int Close_GPGS_Mode()
{
    return(1); 
} 


//---------------------------------------------------------------------------------- 
// GPRS模式下读取数据命令
// 待修改
//----------------------------------------------------------------------------------
int Read_GPRS_Data()
{
	return 1;
}

//-----------------------------------------------------------------------------------
//  GPRS模式下写数据命令
// 写数据到控制中心 参数a为数据地址，length为数据长度，并且长度必须大于2
// 如果数据长度为1，2，返回信息无法区分是URC还是回复玛
//-----------------------------------------------------------------------------------
int Write_GPRS_Data(char a[],int length)
{
    return(0);    
}

//----------------------------------------------------------------------------------
// 接收到GSM回复信息区分判断函数
// 该函数判串口接收的GSM回复信息，并将他们用不同的代码标记，以便以后的程序利用
// 如果在回复信息中有两个不同的信息，那么他们的代码标记相加为最后的代码标记
// 该函数代码标记方便AT_Command()函数的回复比较操作。
// 同时在GSM_REP_Buffer缓冲区中很可能混有URC代码，故在本程序中同时进行了URC判断，防止
// URC信息丢失，检测到URC信息后将相应标志位置位，最后将信息缓冲区清0
//----------------------------------------------------------------------------------
char GSM_RecString_Comp(char ans[], int length)
{
    char code = 0;                                        // GSM回复信息代码标记变量      
    return(code);
}

//------------------------------------------------------------------------------------
// AT命令发送函数
// 该函数向GSM模块发送AT命令，并等待恢复结果，如果恢复结果不正确，重发，连续三次错误，返回发送失败
// 入口参数：cmd要发送的命令指针，str与接收到的结果字符串进行比较的字符串，length为字符串的比较长度
// 出口：发送成功返回TRUE，发送失败返回FALSE
//------------------------------------------------------------------------------------
int AT_Command(char *cmd, char length, char code, int time)
{
    return(0);
}

// 写串口
// 输入: pData - 待写的数据缓冲区指针
//       nLength - 待写的数据长度
// 返回: 实际写入的数据长度
int WriteComm(void* pData, int nLength,int flg)
{
	unsigned long dwNumWrite;	// 串口发出的数据长度
	if(flg==0)
	{
		ClearGprsBuffer(iGprsHandle);
	}
	printf("write cmd=[%s]\n",(unsigned char *)pData);
	dwNumWrite=WriteUart(iGprsHandle, pData, (DWORD)nLength);
	return (int)dwNumWrite;
}

// 读串口
// 输入: pData - 待读的数据缓冲区指针
//       nLength - 待读的最大数据长度
// 返回: 实际读出的数据长度
int ReadComm(void* pData, int nLength)
{
	unsigned long dwNumRead;	// 串口收到的数据长度

	dwNumRead=ReadUart(iGprsHandle, pData, (DWORD)nLength,5000);
	printf("######################\n");
	printf("recv:[%s]\n",(unsigned char *)pData);
	printf("######################\n");
	return (int)dwNumRead;
}

// 字符串-->UNICODE串
int MultiByteToWideChar(const char* lpMultiByteStr,int cchMultiByte,unsigned short * lpWideCharStr)

{
	int i,j;
	unsigned char tmp[128];
	unsigned char pdu[128];
	char  Asc='\x80';
	int  size;
	int  len;
	unsigned char unistr[2+1]="";

	memset(tmp,0x00,sizeof(tmp));
	size=0;
	len=0;
	for(i=0;i<cchMultiByte;i++)
	{
		if((lpMultiByteStr[i]&0xFF)<=Asc)
		{
			tmp[size++]='\x00'&0xFF;
			tmp[size++]=lpMultiByteStr[i]&0xFF;
			len++;
		}
		else
		{
			for(j=0;j<Unicode_Max;j++)
			{
				memset(unistr,0x00,sizeof(unistr));
				gsmString2Bytes(uni_table[j].gbk,unistr,4);
				if((lpMultiByteStr[i]&0xFF)==(unistr[0]&0xFF) &&(lpMultiByteStr[i+1]&0xFF)==(unistr[1]&0xFF))
					break;
			}
			if(j>=Unicode_Max)
			{
				tmp[size++]='\x00'&0xFF;
				tmp[size++]=lpMultiByteStr[i]&0xFF;
			}
			else
			{
				memset(unistr,0x00,sizeof(unistr));
				gsmString2Bytes(uni_table[j].unicode,unistr,4);
				tmp[size++]=unistr[0]&0xFF;
				tmp[size++]=unistr[1]&0xFF;
				i++;
			}
			len++;
		}
	}
	memset(pdu,0x00,sizeof(pdu));
	printf("=====================\n");
	for(i=0;i<size;i++)
	{
		pdu[i]=tmp[i];
		printf("%02X ",tmp[i]);
	}
	printf("\n=====================\n");
	for(i=0,j=0;i<size;i+=2,j++)
	{
		lpWideCharStr[j]=(((short)tmp[i])<<8)|((short)tmp[i+1]&0x00FF);
	}

	return (len);
}
// UNICODE串-->字符串
int WideCharToMultiByte(unsigned char*  lpWideCharStr,int  cchWideChar,char*  lpMultiByteStr)
{
	
	int i,j;
	char  tmp[128];
	int   size;
	int   len;
	unsigned char unistr[2+1]="";

	memset(tmp,0x00,sizeof(tmp));
	size=0;
	len=0;
	for(i=0;i<cchWideChar;i+=1)
	{
		if(lpWideCharStr[i]=='\x00')
		{
			tmp[size]=lpWideCharStr[i+1];
			size+=1;
			i++;
			len++;
		}
		else
		{
			for(j=0;j<Unicode_Max;j++)
			{
				memset(unistr,0x00,sizeof(unistr));
				gsmString2Bytes(uni_table[j].unicode,unistr,4);
				if((lpWideCharStr[i]==unistr[0]) &&(lpWideCharStr[i+1]==unistr[1]))
					break;
			}
			if(j>=Unicode_Max)
			{
				tmp[size]=lpWideCharStr[i+1];
				size+=1;
				len++;
				i++;
			}
			else
			{
				memset(unistr,0x00,sizeof(unistr));
				gsmString2Bytes(uni_table[j].gbk,unistr,4);
				memcpy(tmp+size,unistr,2);
				size+=2;
				i++;
				len+=2;
			}
		}
	}
	memcpy(lpMultiByteStr,tmp,size);
	return (len);
}

//高低转换
void Hex2Asc(char *dest_asc, unsigned char*src_bcd, char src_len)
{
	int 	i, j;
	char 	val;
	
	for( i=0, j=0; i < src_len; i++, j++ )
	{
		val = (src_bcd[i] >> 4) & 0x0F;
		
		if( val <= 9 )	
			dest_asc[j++] = val + 0x30;
		else			
			dest_asc[j++] = val + 0x37;
		
		val = src_bcd[i] & 0x0F;
		
		if( val <= 9 )	
			dest_asc[j] = val + 0x30;
		else			
			dest_asc[j] = val + 0x37;
	}
	dest_asc[j]='\0';
}

int demo_SMS_send()
{
	SM_PARAM  info;
	char tmp[128]="";
	int ret;

	memset(&info,0x00,sizeof(SM_PARAM));
	sprintf(info.SCA,"86%s",gSMS.SCA);
	info.TP_PID = gSMS.TP_PID;
	info.TP_DCS = gSMS.TP_DCS;
	
	Clear();
	TextOut(0, 0, ALIGN_CENTER, "写短信");

	ClearLine(1,9);
	ret = Input(1,1, tmp,50, IME_CHINESE, BLACK, GREEN, FALSE, FALSE, TRUE);
	if(ret==CANCEL)
		return CANCEL;
	else if(ret != OK)
	{
		FailBeep(); 
		ClearLine(1, 9);
		SetScrFont(FONT20, WHITE);
		TextOut(0, 4, ALIGN_CENTER, "失败");
		WaitKey(2000);
		return ERROR;
	}
	memcpy(info.TP_UD,tmp,strlen(tmp));

	ClearLine(1,9);
	TextOut(1,1, ALIGN_LEFT, "收件人号码:");
	memset(tmp,0x00,sizeof(tmp));
	ret = Input(1,3, tmp,11, IME_NUMBER, BLACK, GREEN, FALSE, FALSE, FALSE);
	if(ret==CANCEL)
		return CANCEL;
	if(ret != OK)
	{
		FailBeep(); 
		ClearLine(1, 9);
		SetScrFont(FONT20, WHITE);
		TextOut(0, 4, ALIGN_CENTER, "失败");
		WaitKey(2000);
		return ERROR;
	}
	sprintf(info.TPA,"86%s",tmp);
	ClearLine(1,9);
	TextOut(0,4, ALIGN_CENTER, "发送中...");
	while(1)
	{
		ret=gsmSendMessage(&info);
		if(ret>0)
		{
			ret=AT_GetOK(iGprsHandle,2000);
			if(ret==ERROR)
			{
				ClearLine(1,9);
				TextOut(0,3, ALIGN_CENTER, "发送失败!!");
				TextOut(0,4, ALIGN_CENTER, "是否重发?");
				if(WaitLimitKey("\x0F\x12", 2, 0) == KEY_CANCEL)
					return CANCEL;
				else
					continue;
			}
			ClearLine(1,9);
			TextOut(0,4, ALIGN_CENTER, "发送成功!!");
			WaitKey(5000);
			break;
		}
		else
		{
			ClearLine(1,9);
			TextOut(0,3, ALIGN_CENTER, "发送失败!!");
			TextOut(0,4, ALIGN_CENTER, "是否重发?");
			if(WaitLimitKey("\x0F\x12", 2, 0) == KEY_CANCEL)
				return CANCEL;
		}
	}
	
	return ret;
}
int demo_SMS_recv()
{
	int nMsg;				// 收到短消息条数
	int nDelete;			// 目前正在删除的短消息编号
	SM_BUFF buff;			// 接收短消息列表的缓冲区
	SM_PARAM param[256];	//接收短消息缓冲区
	int i;

	for(i=0;i<256;i++)
	{
		memset(&param[i],0x00,sizeof(SM_PARAM));
	}
	Clear();
	TextOut(0, 0, ALIGN_CENTER, "收件箱");
	TextOut(0, 4, ALIGN_CENTER, "正在查收新短信");
	TextOut(0, 5, ALIGN_CENTER, "请稍后...");
	memset(&buff, 0, sizeof(buff));
	while(1)
	{
		if(WaitLimitKey("\x0F\x12", 2, 1000) == KEY_CANCEL)
				return CANCEL;
		
		memset(&buff, 0, sizeof(buff));
		gsmReadMessageList();

	doing:	
		Wait(500);
		switch (gsmGetResponse(&buff))
		{
			case GSM_OK: 
				nMsg = gsmParseMessageList(param, &buff);
				if (nMsg > 0)
				{
					OkBeep();
					PutRecvMessage(param,nMsg);
					nDelete = 0;
				}
				break;
			case GSM_ERR:
				Wait(500);
				break;
			case GSM_WAIT:
				printf("recv char doing...\n");
				Wait(500);
				goto doing;
				break;
			default:
				break;
		}
	}
	
	return OK;
}

void PutRecvMessage(SM_PARAM* pparam, int nCount)
{
	int i,j;
	char tmp[128];
	char ckey;
	BOOL Flg=TRUE;
	int nret;
	
	Clear();
	for (i = 0; i < nCount && Flg; i++)
	{
		ClearLine(1,9);
		memset(tmp,0x00,sizeof(tmp));
		sprintf(tmp,"发件人:%s",pparam[i].TPA);
		TextOut(0, 1, ALIGN_LEFT,tmp);
		memset(tmp,0x00,sizeof(tmp));
		sprintf(tmp,"接收时间:%s",pparam[i].TP_SCTS);
		TextOut(0, 2, ALIGN_LEFT,tmp);
		TextOut(0, 3, ALIGN_LEFT,"--------------------------------");
		TextOut(0, 4, ALIGN_LEFT,pparam[i].TP_UD);
		for(j=0;j<strlen(pparam[i].TP_UD);j++)
		{
			printf("%.2X ",pparam[i].TP_UD[j]&0xff);
		}
		printf("\n");
		memset(tmp,0x00,sizeof(tmp));
		sprintf(tmp,"%d/%d",i+1,nCount);
		TextOut(0, 9, ALIGN_CENTER,tmp);
		ckey=WaitLimitKey("\x0F\x12\x0E", 3, 0);
		switch(ckey)
		{
			case KEY_CANCEL:
				Flg=FALSE;
				break;
			case KEY_BACKSPACE:
				ClearLine(1,9);
				TextOut(0, 3, ALIGN_CENTER,"要删除这条短信?");
				ckey=WaitLimitKey("\x0F\x12",2, 0);
				if(ckey==KEY_ENTER)
				{
					ClearLine(1,9);
					TextOut(0, 3, ALIGN_CENTER,"正在删除..");
					nret=gsmDeleteMessage(pparam[i].index);
					if(nret>0)
					{
						nret=AT_GetOK(iGprsHandle,3000);
						if(nret==ERROR)
						{
							ClearLine(1,9);
							TextOut(0, 3, ALIGN_CENTER,"删除失败");
						}
						else
						{
							ClearLine(1,9);
							TextOut(0, 3, ALIGN_CENTER,"删除成功");
							OkBeep();
						}
					}
				}
				break;
			default:
				break;
		}
		
	}

}

int SMS_Setting()
{
		int ret;
		char buff[128];
		int tempDataNum;
	
		while(1)
		{
			Clear();
			SetScrFont(FONT20, WHITE);
			TextOut(0, 0, ALIGN_CENTER, "短信模式");
			TextOut(2, 3, ALIGN_LEFT, "0:PDU模式");
			TextOut(2, 4, ALIGN_LEFT, "1:TEXT模式");
			TextOut(2, 5, ALIGN_LEFT, "模式:");
			TextOut(0, 8, ALIGN_CENTER, "保持不变请按确认键");
			memset(buff, 0, sizeof(buff));
			sprintf(buff, "%d", gSMS.TP_PID);
			SetScrFont(FONT20, RED);
			ret = Input(10, 5, buff, 1, IME_NUMBER, BLACK, GREEN, FALSE, TRUE, FALSE);
			if(ret != OK)
				goto FAILED;
			tempDataNum = atoi(buff);
			if(tempDataNum <0 || tempDataNum >1)
			{
				Clear();
				SetScrFont(FONT20, WHITE);
				TextOut(0, 4, ALIGN_CENTER, "输入内容非法");
				WaitKey(2000);
				continue;
			}
			gSMS.TP_PID= tempDataNum;
			DebugOut("sms module:%d\n", gSMS.TP_PID);
			break;
		}
		while(1)
		{
			Clear();
			SetScrFont(FONT20, WHITE);
			TextOut(0, 0, ALIGN_CENTER, "短信中心号码");
			TextOut(2, 5, ALIGN_LEFT, "中心号码:");
			TextOut(0, 8, ALIGN_CENTER, "保持不变请按确认键");
			memset(buff, 0, sizeof(buff));
			SetScrFont(FONT20, RED);
			ret = Input(14, 5, gSMS.SCA, 11, IME_NUMBER, BLACK, GREEN, FALSE, TRUE, FALSE);
			if(ret != OK)
				goto FAILED;
			DebugOut("sms sca:%d\n", gSMS.SCA);
			break;
		}
		while(1)
		{
			Clear();
			SetScrFont(FONT20, WHITE);
			TextOut(0, 0, ALIGN_CENTER, "短信编码方式");
			TextOut(2, 2, ALIGN_LEFT, "8:GSM_UCS2");
			TextOut(2, 3, ALIGN_LEFT, "4:GSM_8BIT");
			TextOut(2, 4, ALIGN_LEFT, "0:GSM_7BIT");
			
			TextOut(2, 5, ALIGN_LEFT, "编码:");
			TextOut(0, 8, ALIGN_CENTER, "保持不变请按确认键");
			memset(buff, 0, sizeof(buff));
			sprintf(buff, "%d", gSMS.TP_DCS);
			SetScrFont(FONT20, RED);
			ret = Input(10, 5, buff, 1, IME_NUMBER, BLACK, GREEN, FALSE, TRUE, FALSE);
			if(ret != OK)
				goto FAILED;
			tempDataNum = atoi(buff);
			if(tempDataNum%4!=0)
			{
				Clear();
				SetScrFont(FONT20, WHITE);
				TextOut(0, 4, ALIGN_CENTER, "输入内容非法");
				WaitKey(2000);
				continue;
			}
			gSMS.TP_DCS= tempDataNum;
			DebugOut("sms dcs:%d\n", gSMS.TP_DCS);
			break;
		}
		while(1)
		{
			Clear();
			SetScrFont(FONT20, WHITE);
			TextOut(0, 0, ALIGN_CENTER, "短信存储");
			TextOut(2, 5, ALIGN_LEFT, "当前:");
			TextOut(0, 8, ALIGN_CENTER, "保持不变请按确认键");
			memset(buff, 0, sizeof(buff));
			sprintf(buff, "%d", gSMS.uiMax);
			SetScrFont(FONT20, RED);
			ret = Input(10, 5, buff, 3, IME_NUMBER, BLACK, GREEN, FALSE, FALSE, FALSE);
			if(ret != OK)
				goto FAILED;
			tempDataNum = atoi(buff);
			if(tempDataNum <=0 || tempDataNum >250)
			{
				Clear();
				SetScrFont(FONT20, WHITE);
				TextOut(0, 4, ALIGN_CENTER, "输入内容非法");
				WaitKey(2000);
				continue;
			}
			gSMS.uiMax= tempDataNum;
			DebugOut("sms module:%d\n", gSMS.uiMax);
			break;
		}
		//保存文件
		if(WriteData("test-sms.dat", &gSMS, sizeof(T_SMS), 0) == FALSE)
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
		TextOut(2, 4, ALIGN_CENTER, "设置成功!");
		WaitKey(2000);
		return OK;
	
	FAILED:
		memset(&gTerm, 0, sizeof(T_TERM));
		ReadData("test-sms.dat", &gSMS,sizeof(T_SMS), 0);
	
		return ERROR;
}

int gsmDecode8bit(unsigned char* pSrc, char* pDst, int nSrcLength)
{
	// 简单复制
	memcpy(pDst, pSrc, nSrcLength);

	// 输出字符串加个结束符
	*pDst = '\0';

	return nSrcLength;
}

int demo_SMS_read(int index)
{
	int nMsg;				// 收到短消息条数
	SM_BUFF buff;			// 接收短消息列表的缓冲区
	SM_PARAM param[256];	//接收短消息缓冲区
	int i;
	BOOL nSend=TRUE;
	
	for(i=0;i<256;i++)
	{
		memset(&param[i],0x00,sizeof(SM_PARAM));
	}
	Clear();
	TextOut(0, 0, ALIGN_CENTER, "收件箱");
	TextOut(0, 4, ALIGN_CENTER, "正在查收短信");
	TextOut(0, 5, ALIGN_CENTER, "请稍后...");
	memset(&buff, 0, sizeof(buff));
	while(nSend)
	{
		if(WaitLimitKey("\x0F\x12", 2, 1000) == KEY_CANCEL)
				return CANCEL;
		
		memset(&buff, 0, sizeof(buff));
		gsmReadMessage(index);

	doing:	
		Wait(500);
		switch (gsmGetResponse(&buff))
		{
			case GSM_OK: 
				nMsg = gsmParseMessage(param, &buff);
				if (nMsg > 0)
				{
					OkBeep();
					PutRecvMessage(param,nMsg);
				}
				nSend=FALSE;
				break;
			case GSM_ERR:
				nSend=FALSE;
				break;
			case GSM_WAIT:
				printf("recv char doing...\n");
				Wait(500);
				goto doing;
				break;
			default:
				break;
		}
	}
	
	return OK;
}
int gsmParseMessage(SM_PARAM* pMsg, SM_BUFF* pBuff)
{
	int nMsg;			// 短消息计数值
	char* ptr;			// 内部用的数据指针

	nMsg = 0;
	ptr = pBuff->data;

	// 循环读取每一条短消息, 以"+CMGL:"开头
	while((ptr = strstr(ptr, "+CMGR:")) != NULL)
	{
		ptr += 6;		// 跳过"+CMGL:", 定位到序号
		sscanf(ptr, "%d", &pMsg->index);	// 读取序号

		ptr = strstr(ptr, "\r\n");	// 找下一行
		if (ptr != NULL)
		{
			ptr += 2;		// 跳过"\r\n", 定位到PDU
			printf("Pdu:=[%s]\n",ptr);
			gsmDecodePdu(ptr, pMsg);	// PDU串解码

			pMsg++;		// 准备读下一条短消息
			nMsg++;		// 短消息计数加1
		}
	}

	return nMsg;
}

