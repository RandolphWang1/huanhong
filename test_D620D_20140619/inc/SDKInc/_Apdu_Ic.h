#include <math.h>

//////////////////////////////////////////////////////////////////////////////

#define MAC_LEN		4
#define PASSWD		8
#define PRIINFO		8
#define FILENAME_LEN	5
#define MAC_DATA_LEN	8

//////////////////////////////////////////////////////////////////////////////
// 
struct Status_map 
{
	char SWA[2+1];			// SWA
	char SWB[2+1];			// SWB
	char Err_Desc[30+1];		// 错误描述
};

BYTE str_To_Dec(BYTE_PTR pStr);

//////////////////////////////////////////////////////////////////////////////
//MAC注若使用到此函数在应用部分先分配MAC空间初始化为0x000x000x00x00
int APDU_S_Mac(APDU_SEND* pApduSend, BYTE_PTR pMak, INT Des);
// 释放发送数据空间
int APDU_S_Free(APDU_SEND *pApduSend);
// 这个期望得到的数据, 可以自己控制
int APDU_R_Malloc(APDU_RESP * ApduResp, UINT  le);
// 释放发送数据空间
int APDU_R_Free(APDU_RESP *pApduResp);
int APDU_R_Length(APDU_RESP * pApduResp, UINT * pLen);
// 长度为3,SW  字符串
int APDU_R_SW1(APDU_RESP * pApduResp, CHAR * pSW1);
// 长度为3,SW  字符串
int APDU_R_SW2(APDU_RESP * pApduResp, CHAR * pSW2);
// 空间可根据APDU_R_Length的长度分配
int APDU_R_Data(APDU_RESP * pApduResp, CHAR * data);
//============================================================================
// 应用锁定
int AppBlock(APDU_SEND * pApduSend, BYTE p1, BYTE p2, UINT lc, BYTE_PTR data);
// 应用解锁
int AppUnBlock(APDU_SEND * pApduSend, BYTE p1, BYTE p2, UINT lc, BYTE_PTR data);
// 卡锁定
int CardBlock(APDU_SEND * pApduSend, BYTE p1, BYTE p2, UINT lc, BYTE_PTR data);
// 外部验证
int ExAuth(APDU_SEND * pApduSend, UINT lc, BYTE_PTR data);
// 请求随机数
int GetChallenge(APDU_SEND * pApduSend, UINT le);
// 请求响应
int GetResponse(APDU_SEND * pApduSend, UINT le);
// 内部验证
int InAuth(APDU_SEND * pApduSend, UINT lc, BYTE_PTR data);
// PIN解锁
int PinUnBlock(APDU_SEND * pApduSend, UINT lc, BYTE_PTR data);
// 读二进制数据00
int ReadBinary(APDU_SEND * pApduSend, BYTE p1, BYTE p2);
// 读二进制数据04
int ReadBinaryMac(APDU_SEND * pApduSend, BYTE p1, BYTE p2, UINT lc, BYTE_PTR data);
// 读记录00
int ReadRecord(APDU_SEND * pApduSend, BYTE p1, BYTE p2);
// 读记录04
int ReadRecordMac(APDU_SEND * pApduSend, BYTE p1, BYTE p2, UINT lc, BYTE_PTR data);
// SELECT控制参数04
int Select(APDU_SEND * pApduSend, BYTE p1, BYTE p2, UINT lc, BYTE_PTR data);
// 更新二进制数据00
int UpdateBinary(APDU_SEND * pApduSend, BYTE p1, BYTE p2, UINT lc, BYTE_PTR data);
// 更新二进制数据04
int UpdateBinaryMac(APDU_SEND * pApduSend, BYTE p1, BYTE p2, UINT lc, BYTE_PTR data);
// 更新记录00
int UpdateRecord(APDU_SEND * pApduSend, BYTE p1, BYTE p2, UINT lc, BYTE_PTR data);
// 更新记录04
int UpdateRecordMac(APDU_SEND * pApduSend, BYTE p1, BYTE p2, UINT lc, BYTE_PTR data);
// 校验PIN
int Verify(APDU_SEND * pApduSend, UINT lc, BYTE_PTR data);
//============================================================================
// 修改个人识别码
int ChangePin(APDU_SEND * pApduSend, UINT lc, BYTE_PTR data);
// 圈存
int CreditForLoad(APDU_SEND * pApduSend, BYTE_PTR data);
// 消费
int DebitForPur(APDU_SEND * pApduSend, BYTE_PTR data);
// 取现
int CashWithdraw(APDU_SEND * pApduSend, BYTE_PTR data);
// 圈提
int DebitForUnload(APDU_SEND * pApduSend, BYTE_PTR data);
// 查询余额
int GetBalance(APDU_SEND * pApduSend, BYTE p2);
// 取交易认证
int GetTransProve(APDU_SEND * pApduSend, BYTE p2, BYTE_PTR data);
// 初始化取现
int InitForCashWD(APDU_SEND * pApduSend, BYTE p2, BYTE_PTR data);
// 初始化圈存
int InitForLoad(APDU_SEND * pApduSend, BYTE p2, BYTE_PTR data);
// 初始化消费
int InitForPur(APDU_SEND * pApduSend, BYTE p2, BYTE_PTR data);
// 初始化圈提
int InitForUnload(APDU_SEND * pApduSend, BYTE p2, BYTE_PTR data);
// 修改初始化
int InitForUpdate(APDU_SEND * pApduSend, BYTE_PTR data);
// 重装个人识别码
int ReloadPin(APDU_SEND * pApduSend, BYTE lc, BYTE_PTR data);
// 修改透支限额
int UpdateLimit(APDU_SEND * pApduSend, BYTE_PTR data);

//////////////////////////////////////////////////////////////////////////////

