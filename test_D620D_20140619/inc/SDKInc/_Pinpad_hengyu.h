#ifndef __PINPAD_H_
#define __PINPAD_H_


extern int iPinpadHandle;

/*
 * 函数功能：激活用户密钥:激活主密钥号为MKIndex用户密钥号为WKIndex的用户密钥
 * 入口：        
 *              MKIndex - 主密钥索引（0~9）；
 *              WKIndex - 用户密钥索引（0~1）；
 *              DesMode - DES加密方式：1-单DES，2-3DES; 
 * 出口：       无；
 * 函数返回值： -1 - 失败；
 *              0  - 成功； 
 */
int PINPAD_ActiateKey(int MKIndex, int WKIndex, int DesMode);

/*
 * 函数功能：   检测密码键盘连接状态
 * 入口：       无；
 * 出口：       无；
 * 函数返回值： -1 - 失败；
 *              0  - 成功； 
 */
int PINPAD_ChkStatus(void);

/*
 * 函数功能：   通过密码键盘显示交易金额；
 * 入口：       
 *              pSrcBuf - 要显示的内容；
 *              nSrcSz - 要显示内容的长度；
 *              nSecond - 显示时间(1~15秒)；
 * 出口：       
 *              无；
 * 函数返回值： 
 *              -1 - 失败；
 *              0 - 成功； 
 */
void PINPAD_ConfirmAmount(char *pSrcBuf, int nSrcSz, int nSecond);

/*
 * 函数功能：   通过密码键盘显示余额；
 * 入口：       
 *              pSrcBuf - 要显示的内容；
 *              nSrcSz - 要显示内容的长度;
 *              nSecond - 显示时间(1~15秒)；
 * 出口：       
 *              无；
 * 函数返回值： 
 *              -1 - 失败；
 *              0 - 成功； 
 */
void PINPAD_DisplayBalance(char *pSrcBuf, int nSrcSz, int nSecond);

/*
 * 函数功能：使用当前激活的工作密钥对用户数据进行加密；
 * 入口：         
 *              InData - 需要加密的用户数据； 
 *              InDataLen - 用户数据的长度；
 * 出口：       
 *              OutData - 加密后的数据；
 *              OutDataLen - 出口数据长度的地址；
 * 函数返回值： 
 *              -1 - 失败；
 *              0  - 成功； 
 */
int PINPAD_Encrypt(unsigned char *InData, int InDataLen, unsigned char *OutData, int *OutDataLen);

/*
 * 函数功能：读取软件版本号 
 * 入口：无；
 * 出口：无；
 *返回值：无；
 */
void PINPAD_GetVersion(void);

/*
 * 函数功能：读取加密模式
 * 入口：无；
 * 出口：cDesMode:当前密键的加密模式：
 *          1 - 单DES；
 *          2 - 3DES；
 *返回值： 
 *        0 - 成功;
 *        -1 - 失败；
 */
int PINPAD_GetDesMode(char *cDesMode);
/*
 * 函数功能：   根据密钥索引号设置主密钥；
 * 入口：       DesMode - 单/双DES模式；
 *              Index - 主密钥索引；
 *              Mkey - 主密钥；
 *              MkeyLen - 主密钥长度；
 * 出口：       无；
 * 函数返回值： -1 - 失败；
 *              0  - 成功； 
 */
int PINPAD_SetMasterKey(int DesMode, int Index, unsigned char *Mkey, int MkeyLen);

/*
 * 函数功能：设置最大、最小密码长度;
 * 入口：Mode - 设置pin的最大或最小长度选项：
 *           0：最小密码长度；
 *           1：最大密码长度；
 *       Pinlen - 密码长度值；
 * 出口：无；
 * 返回值：0 - 成功；
 *         -1 - 失败；
 */
int PINPAD_SetPinLen(int Mode, int PinLen);

/*
 * 函数功能：   设置用户密钥；
 * 入口：        
 *              MKIndex - 主密钥索引（0~9）；
 *              WKIndex - 用户密钥索引（0~1）；
 *              WKey - 用户密钥内容；
 *              WKeyLen - 用户密钥长度；
 * 出口：       无；
 * 函数返回值： -1 - 失败；
 *              0  - 成功； 
 */
int PINPAD_SetWorkKey(int MKIndex, int WKIndex, unsigned char *WKey, int WKeyLen);

/*
 * 函数功能：   通过密码键盘计算MAC；
 * 入口：        
 *              PanBuf - 卡帐号(12位长度)；  
 * 出口：       
 *              PinBlock - 加密后的Pin Block；
 *              PinLen - 客户输入的密码长度地址；
 *              lWaitTime - 超时时间 以毫秒为单位；
 * 函数返回值： 
 *              -1 - 失败；
 *              0  - 成功；  
 */
int PINPAD_CalcPinBlock(char *PanBuf, char *PinBlock,  int *PinLen, ULONG lWaitTime);

/*
 * 函数功能：   通过密码键盘计算MAC；
 * 入口：        
 *              MacMode - MAC加密算法：
 *                      0：ANSI X9.9算法；
 *                      1：ECB算法；  
 *              InData - 要加密数据；
 *              InDataLen - 要加密数据的长度；
 * 出口：       
 *              MacData - 计算出来的MAC值；
 *              MacLen - 客户输入的密码长度地址；
 * 函数返回值： 
 *              -1 - 失败；
 *              0  - 成功；  
 */
int PINPAD_CalcMAC(int MacMode, char *InData, int InDataLen, char *MacData,  int *MacLen);

/*
 * 函数功能：   将PINPAD复位到出厂状态
 * 入口：       无；
 * 出口：       无；
 * 函数返回值： 无； 
 */
void PINPAD_ResetPinpad(void);

/*
 * 函数功能：   清除PINPAD屏幕显示
 * 入口：       无；
 * 出口：       无；
 * 函数返回值： 无； 
 */
void PINPAD_ResetLCD(void);

#endif


