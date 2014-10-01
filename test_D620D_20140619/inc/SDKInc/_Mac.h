#ifndef __MAC_H_
#define __MAC_H_


void ExclusiveOR(char *pResult, char *pBuf1, char *pBuf2, int nLen);
int MakeUserKey(char *pDecryptUserKeyHex, char *pDecryptUserKeyAscii);
void EncryptUserKey(char *pEncryptUserKeyHex, char *pDecryptUserKeyHex);
void CreateMAC(char *pMac);
void function_des(char cr_decr, char *Data, char *Key, char *Res);
void soft_des(unsigned char input[],unsigned char output[],unsigned char deskey[],int flg);
void Mac_ANSIX99 (
	unsigned char *mac_key,
	unsigned char *buf,
	int len,
	unsigned char *mac );
void Mac_Normal (
	unsigned char *mac_key,
	unsigned char *buf,
	int len,
	unsigned char *mac );
void Mac_ECB(
	unsigned char *mac_key,
	unsigned char *buff,
	int len,
	unsigned char *mac);



#endif  //#ifndef _MAC_H_
