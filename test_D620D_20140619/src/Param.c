/***************************************************************************
    Li,Hengbo
    2009-10-14
***************************************************************************/
#include "Main.h"


T_TERM gTerm;

T_SMS  gSMS;

void InitTerm()
{
	//初始化各变量
	memset(&gTerm, 0, sizeof(T_TERM));

	//通讯信息
	strcpy(gTerm.szTPDU, "0180");
	strcpy(gTerm.szTelNo, "812");
	gTerm.uiModemDataNum = 193;
	gTerm.uiGprsDataNum = 193;
	gTerm.uiEthDataNum = 193;
	gTerm.uiWifiDataNum = 193;

	strcpy(gTerm.tEthernet.szServerIP, "192.168.8.180");
	strcpy(gTerm.tEthernet.szServerPort, "8181");
	strcpy(gTerm.tEthernet.szLocalIP, "192.168.1.168");
	strcpy(gTerm.tEthernet.szMask, "255.255.255.0");
	strcpy(gTerm.tEthernet.szGateway, "192.168.1.1");

	strcpy(gTerm.szGprsServerIP, "124.205.159.130");
	strcpy(gTerm.szGprsServerPort, "7979");

	strcpy(gTerm.szMemKey, "FFFFFF");
	strcpy(gTerm.szM1KeyA, "FFFFFFFFFFFF");
	strcpy(gTerm.szM1KeyB, "FFFFFFFFFFFF");
       
        gTerm.t_Wifi.nChannelNum = 1;
        memset(gTerm.t_Wifi.szBssid, 0, sizeof(gTerm.t_Wifi.szBssid));
        memcpy(gTerm.t_Wifi.szBssid, "d85d4c3c3a88", 12);
        memset(gTerm.t_Wifi.szSsid, 0, sizeof(gTerm.t_Wifi.szSsid));
        memcpy(gTerm.t_Wifi.szSsid, "huanhong", 8);
        gTerm.t_Wifi.nDatalink = 0x00;
        memset(gTerm.t_Wifi.szEquipmentIP, 0, sizeof(gTerm.t_Wifi.szEquipmentIP));
        memcpy(gTerm.t_Wifi.szEquipmentIP, "192.168.1.116", 13);
        memset(gTerm.t_Wifi.szServerMask, 0, sizeof(gTerm.t_Wifi.szServerMask));
        memcpy(gTerm.t_Wifi.szServerMask, "255.255.255.0", 13);
        memset(gTerm.t_Wifi.szServerGateway, 0, sizeof(gTerm.t_Wifi.szServerGateway));
        memcpy(gTerm.t_Wifi.szServerGateway, "192.168.1.1", 11);
        memset(gTerm.t_Wifi.szServerIP, 0, sizeof(gTerm.t_Wifi.szServerIP));
        memcpy(gTerm.t_Wifi.szServerIP, "192.168.1.100", 13);
        memset(gTerm.t_Wifi.szEquipmentPort, 0, sizeof(gTerm.t_Wifi.szEquipmentPort));
        memcpy(gTerm.t_Wifi.szEquipmentPort, "9191", 4);
        gTerm.t_Wifi.nEncrypType = 1;
        memset(&gTerm.t_Wifi.key_wep, 0, sizeof(KEY_WEP));
        gTerm.t_Wifi.key_wep.KeyLength = 0;
        gTerm.t_Wifi.key_wep.KeyIndex = 0;
        memcpy(gTerm.t_Wifi.key_wep.Group, "\0x1\0x1\0x1\0x1\0x1", 5);
	gTerm.bIsDC = TRUE;

	//保存
	if(!WriteData("test-term.dat", (char *)(&gTerm), sizeof(T_TERM), 0))
	{
		Clear();                      
		SetScrFont(FONT24, RED);
		TextOut(0, 2, ALIGN_CENTER, "机器硬件出错"); 
		TextOut(0, 3, ALIGN_CENTER, "请更换设备");
		FailBeep();					  
		WaitKey(2000);
		return;
	}
}

void InitSMS()
{
	//初始化各变量
	memset(&gSMS, 0, sizeof(T_SMS));

	memcpy(gSMS.SCA,"13800100500",11);
	gSMS.TP_PID=0;  				// 0:PDU   1:text
	gSMS.TP_DCS=GSM_UCS2;
	gSMS.uiMax=40;
	//保存
	if(!WriteData("test-sms.dat", (char *)(&gSMS), sizeof(T_SMS), 0))
	{
		Clear();                      
		SetScrFont(FONT24, RED);
		TextOut(0, 2, ALIGN_CENTER, "机器硬件出错"); 
		TextOut(0, 3, ALIGN_CENTER, "请更换设备");
		FailBeep();					  
		WaitKey(2000);
		return;
	}
}



