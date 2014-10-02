#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <resolv.h>
#include <signal.h>
#include <stdarg.h>
#include <termio.h>

#include "SDKInc/_Type.h"
#include "SDKInc/_Convert.h"
#include "SDKInc/_Lcd.h"
#include "SDKInc/_Kbd.h"
#include "SDKInc/_Printer.h"
#include "SDKInc/app_icc.h"
#include "SDKInc/_Control.h"
#include "SDKInc/_Timer.h"
#include "SDKInc/_Func.h"
#include "SDKInc/_File.h"
#include "SDKInc/_Input.h"
#include "SDKInc/_InputMoney.h"
#include "SDKInc/_InputDate.h"
#include "SDKInc/_Proc8583.h"
#include "SDKInc/_Modem.h"
#include "SDKInc/_Socket.h"
#include "SDKInc/_Magcard.h"
#include "SDKInc/_Gprs_siemens.h"
#include "SDKInc/_Gprs_Quectel.h"
#include "SDKInc/_Pinpad_rongci.h"
#include "SDKInc/_RfCard.h"
#include "SDKInc/_Mac.h"
#include "SDKInc/app_control.h"
#include "SDKInc/_Cdma_huawei.h"
#include "SDKInc/_TfCard.h"
#include "SDKInc/librf.h"
//#include "SDKInc/_RficProc.h"
#include "SDKInc/_Apdu_Ic.h"
#include "SDKInc/_FM17XX.h"
#include "SDKInc/_Wifi.h"



#include "InitPos.h"
#include "Param.h"
#include "IdentifyReader.h"
#include "RficProc.h"
#include "FM17XX.h"

#include "Sms.h"
#include "unicode.h"
#include "Epp.h"

extern int iGprsHandle;
extern int iPinpadHandle;
extern BOOL iScanHandle;
int iM1Handle;  //ÉäÆµ¿¨Ä£¿é¾ä±ú



typedef const CHAR *LPCSTR;
typedef unsigned short *LPWSTR;
typedef const unsigned short *LPCWSTR;


int KbdTest();
void RficTest();
int CardReaderTest();
void LcdTest();
int PrinterTest();
int PrinterQRTest();
int PinpadTest();
int PinpadN20Test();
int JingZhenTest();
int StartStatusBar();
void ShutDown();
int OtherFunc();
void UpdateAppFromU();
void OtherTest();
void UpdateTms();

void ModemTest();

void Gprs_Quectel_CommTest();
void Gprs_Siemens_CommTest();
void GprsTest();
void EthernetTest();
int Set_Wifi();
int Set_Wifi_Auto();
void WifiCommTest();
void PrintLog(char *Title, char *Msg);
void PrintMsg(char *Msg);

//clear client region
void ClearClient();
void ScanBarCode();
int CommTest();
void SetCommParam();
void CDMACommTest();
int SMS_Demo();
int SNSet();


void RficWirte(void);
void SetKeyM1();
int ReadCardNO();
int ReadCardNo();



#endif
