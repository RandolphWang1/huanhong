#include "Main.h"


static void GetID(handle)
{
	char buff[100] = "";
	int ret;

	ret = PinpadGetID(handle, buff);
	if(ret != SUCCESS_PINPAD)
	{
		DebugOut("ret=[%d]\n", ret);
		ClearClient();
		strcpy(buff, "获取ID失败");
		TextOut(0, 4, ALIGN_CENTER, buff);
		DebugOut("%s\n", buff);
		ReStoreScrFont();
		WaitKey(2000);
		return;
	}

	ClearClient();
	SetScrFont(FONT20, GREEN);
	TextOut(0, 3, ALIGN_CENTER, "该密码键盘ID为");
	TextOut(0, 5, ALIGN_CENTER, buff);
	ReStoreScrFont();
	WaitKey(0);
}

static void ShowText(handle)
{
	char buff[100] = "D620D测试版本";
	int ret;

	ClearClient();
	SetScrFont(FONT20, GREEN);
	TextOut(0, 4, ALIGN_CENTER, "请输入显示文字:");
	ret = Input(4, 5, buff, 15, IME_ENGLISH, BLACK, GREEN, FALSE, FALSE, TRUE);
	if(ret != OK)
	{
		ReStoreScrFont();
		return;
	}
	if(strlen(buff) <= 0)
	{
		ClearClient();
		TextOut(0, 4, ALIGN_CENTER, "输入错误");
		WaitKey(3000);
		ReStoreScrFont();
		return;
	}
	ClearClient();
	TextOut(0, 4, ALIGN_CENTER, buff);
	WaitKey(0);

	//display text at row 0
	ret = PinpadShowText(handle, 0, buff);
	if(ret != SUCCESS_PINPAD)
	{
		DebugOut("ret=[%d]\n", ret);
		ClearClient();
		strcpy(buff, "显示失败");
		TextOut(0, 4, ALIGN_CENTER, buff);
		DebugOut("%s\n", buff);
		WaitKey(3000);
		ReStoreScrFont();
		return;
	}

	//display text at row 1
	ret = PinpadShowText(handle, 1, buff);
	if(ret != SUCCESS_PINPAD)
	{
		DebugOut("ret=[%d]\n", ret);
		ClearClient();
		strcpy(buff, "显示失败");
		TextOut(0, 4, ALIGN_CENTER, buff);
		DebugOut("%s\n", buff);
		WaitKey(3000);
		ReStoreScrFont();
		return;
	}

	OkBeep();
	ClearClient();
	TextOut(0, 4, ALIGN_CENTER, "显示成功");
	WaitKey(2000);

	//clear LCD
	ret = PinpadClearLCD(handle);
	if(ret != SUCCESS_PINPAD)
	{
		DebugOut("ret=[%d]\n", ret);
		ClearClient();
		strcpy(buff, "清除LCD失败");
		TextOut(0, 4, ALIGN_CENTER, buff);
		DebugOut("%s\n", buff);
		WaitKey(3000);
		ReStoreScrFont();
		return;
	}

	//display default text at row 0
	ret = PinpadShowText(handle, 0, "  欢迎您使用");
	if(ret != SUCCESS_PINPAD)
	{
		DebugOut("ret=[%d]\n", ret);
		ClearClient();
		strcpy(buff, "显示失败");
		TextOut(0, 4, ALIGN_CENTER, buff);
		DebugOut("%s\n", buff);
		WaitKey(3000);
		ReStoreScrFont();
		return;
	}

	ReStoreScrFont();
}

static void SetKey(int handle)
{
	char key_des[16+1] = "1234567890123456";
	char key_3des[32+1] = "12345678901234567890123456789012";
	char buff[100];
	int ret;
	int i, j;
	char *p = NULL;
	int print_flag = 1;

	ClearClient();
	SetScrFont(FONT20, GREEN);

	TextOut(0, 4, ALIGN_CENTER, "是否打印测试结果");
	TextOut(0, 5, ALIGN_CENTER, "确认/取消");
	if(WaitLimitKey("\x0F\x12", 2, 0) == KEY_CANCEL)
		print_flag = 0;

	ClearClient();

	//set primary key
	p = key_des;
	for(i = 0; i < 16; i ++)
	{
		if(i == 10)
			p = key_3des;
		//set primary key
		//ret = PinpadSetPriKey(handle, i, p);
		ret = PINPAD_SetMasterKey(1, 0, p, 16);

		if(ret != SUCCESS_PINPAD)
		{
			DebugOut("ret=[%d]\n", ret);
			ClearClient();
			memset(buff, 0, sizeof(buff));
			sprintf(buff, "设置[%d]号主密钥失败", i);
			DebugOut("%s\n", buff);
			TextOut(0, 4, ALIGN_CENTER, buff);
			if(print_flag)
				PrintMsg(buff);
			WaitKey(1000);
			continue;
		}
		//display result
		ClearClient();
		memset(buff, 0, sizeof(buff));
		sprintf(buff, "设置[%d]号主密钥成功", i);
		DebugOut("%s\n", buff);
		TextOut(0, 4, ALIGN_CENTER, buff);
		if(print_flag)
			PrintMsg(buff);
		WaitKey(400);
	}

	OkBeep();
	ClearClient();
	TextOut(0, 4, ALIGN_CENTER, "设置主密钥完毕");
	WaitKey(1000);

	//set work key
	p = key_des;
	for(i = 0; i < 16; i ++)
	{
		if(i == 10)
			p = key_3des;
		for(j = 0; j < 4; j ++)
		{
			//set work key
			//ret = PinpadSetWorkKey(handle, i, j, p);
			ret = PINPAD_SetWorkKey(i, j, p, 32);

			if(ret != SUCCESS_PINPAD)
			{
				DebugOut("ret=[%d]\n", ret);
				ClearClient();
				memset(buff, 0, sizeof(buff));
				sprintf(buff, "设置[%d][%d]号工作密钥失败", i, j);
				DebugOut("%s\n", buff);
				TextOut(0, 4, ALIGN_CENTER, buff);
				if(print_flag)
					PrintMsg(buff);
				WaitKey(1000);
				continue;
			}
			//display result
			ClearClient();
			memset(buff, 0, sizeof(buff));
			sprintf(buff, "设置[%d][%d]号工作密钥成功", i, j);
			DebugOut("%s\n", buff);
			TextOut(0, 4, ALIGN_CENTER, buff);
			if(print_flag)
				PrintMsg(buff);
			WaitKey(200);
		}
	}

	OkBeep();
	ClearClient();
	TextOut(0, 4, ALIGN_CENTER, "设置工作密钥完毕");
	WaitKey(1000);

	ReStoreScrFont();
}

static void InputPwd(int handle)
{
	char key_des[16+1] = "1234567890123456";
	char valid_pwd[8+1] = "\x41\x89\x69\xc6\x7b\xce\x2d\xa8";
	char buff[100];
	char cKey;
	UINT mode1 = 0, mode2 = 0;
	int ret;

	//select mode1
	while(1)
	{
		ClearClient();
		SetScrFont(FONT20, GREEN);
		TextOut(0, 3, ALIGN_CENTER, "选择提示方式");
		TextOut(0, 5, ALIGN_LEFT, "      1.方式1(默认)");
		TextOut(0, 6, ALIGN_LEFT, "      2.方式2");
		TextOut(0, 7, ALIGN_LEFT, "      3.方式3");
		TextOut(0, 8, ALIGN_LEFT, "      4.方式4");
		ReStoreScrFont();

		cKey = WaitKey(0);

		switch(cKey)
		{
			case KEY_CANCEL:
				return;
				break;
			case KEY_1:
			case KEY_ENTER:
				ClearClient();
				SetScrFont(FONT20, GREEN);
				TextOut(0, 3, ALIGN_CENTER, "方式1(默认)");
				TextOut(0, 5, ALIGN_LEFT, "   选择此种方式,密码键盘上会");
				TextOut(0, 6, ALIGN_LEFT, "   显示'请输入密码',并有语音");
				TextOut(0, 7, ALIGN_LEFT, "   提示,输入的内容以*号显示");
				ReStoreScrFont();
				mode1 = 0;
				break;
			case KEY_2:
				ClearClient();
				SetScrFont(FONT20, GREEN);
				TextOut(0, 3, ALIGN_CENTER, "方式2");
				TextOut(0, 5, ALIGN_LEFT, "   选择此种方式,密码键盘上会");
				TextOut(0, 6, ALIGN_LEFT, "   显示'请再输入一次',并有语音");
				TextOut(0, 7, ALIGN_LEFT, "   提示,输入的内容以*号显示");
				ReStoreScrFont();
				mode1 = 1;
				break;
			case KEY_3:
				ClearClient();
				SetScrFont(FONT20, GREEN);
				TextOut(0, 3, ALIGN_CENTER, "方式3");
				TextOut(0, 5, ALIGN_LEFT, "   选择此种方式,密码键盘上会");
				TextOut(0, 6, ALIGN_LEFT, "   显示'请输入密码',无语音");
				TextOut(0, 7, ALIGN_LEFT, "   提示,输入的内容以*号显示");
				ReStoreScrFont();
				mode1 = 2;
				break;
			case KEY_4:
				ClearClient();
				SetScrFont(FONT20, GREEN);
				TextOut(0, 3, ALIGN_CENTER, "方式4");
				TextOut(0, 5, ALIGN_LEFT, "   选择此种方式,密码键盘上会");
				TextOut(0, 6, ALIGN_LEFT, "   显示'请再输入一次',无语音");
				TextOut(0, 7, ALIGN_LEFT, "   提示,输入的内容以*号显示");
				ReStoreScrFont();
				mode1 = 3;
				break;
		}

		if(WaitKey(0) == KEY_ENTER)
			break;
	}

	DebugOut("get pwd mode1=[%d]\n", mode1);

	//select mode2
	while(1)
	{
		ClearClient();
		SetScrFont(FONT20, GREEN);
		TextOut(0, 3, ALIGN_CENTER, "选择结束方式");
		TextOut(0, 5, ALIGN_LEFT, "      1.方式1(默认)");
		TextOut(0, 6, ALIGN_LEFT, "      2.方式2");
		TextOut(0, 7, ALIGN_LEFT, "      3.方式3");
		TextOut(0, 8, ALIGN_LEFT, "      4.方式4");
		ReStoreScrFont();

		cKey = WaitKey(0);

		switch(cKey)
		{
			case KEY_CANCEL:
				return;
				break;
			case KEY_1:
			case KEY_ENTER:
				ClearClient();
				SetScrFont(FONT20, GREEN);
				TextOut(0, 3, ALIGN_CENTER, "方式1(默认)");
				TextOut(0, 5, ALIGN_LEFT, "   选择此种方式,到达指定密码");
				TextOut(0, 6, ALIGN_LEFT, "   长度并按确认键会终止输入");
				ReStoreScrFont();
				mode2 = 3;
				break;
			case KEY_2:
				ClearClient();
				SetScrFont(FONT20, GREEN);
				TextOut(0, 3, ALIGN_CENTER, "方式2");
				TextOut(0, 5, ALIGN_LEFT, "   选择此种方式,按确认键或到达");
				TextOut(0, 6, ALIGN_LEFT, "   指定密码长度后会终止输入");
				ReStoreScrFont();
				mode2 = 0;
				break;
			case KEY_3:
				ClearClient();
				SetScrFont(FONT20, GREEN);
				TextOut(0, 3, ALIGN_CENTER, "方式3");
				TextOut(0, 5, ALIGN_LEFT, "   选择此种方式,按确认键会终止");
				TextOut(0, 6, ALIGN_LEFT, "   输入");
				ReStoreScrFont();
				mode2 = 1;
				break;
			case KEY_4:
				ClearClient();
				SetScrFont(FONT20, GREEN);
				TextOut(0, 3, ALIGN_CENTER, "方式4");
				TextOut(0, 5, ALIGN_LEFT, "   选择此种方式,到达指定密码");
				TextOut(0, 6, ALIGN_LEFT, "   长度后会终止输入");
				ReStoreScrFont();
				mode2 = 2;
				break;
		}

		if(WaitKey(0) == KEY_ENTER)
			break;
	}

	//set DES mode
	ret = PinpadSetMode(handle, PINPAD_MODE_DES);
	if(ret != SUCCESS_PINPAD)
	{
		DebugOut("ret=[%d]\n", ret);
		ClearClient();
		SetScrFont(FONT20, GREEN);
		strcpy(buff, "设置加密方式失败");
		TextOut(0, 4, ALIGN_CENTER, buff);
		DebugOut("%s\n", buff);
		ReStoreScrFont();
		WaitKey(2000);
		return;
	}

	//set primary key and work key
	ret = PinpadSetPriKey(handle, 0, key_des);
	if(ret != SUCCESS_PINPAD)
	{
		DebugOut("ret=[%d]\n", ret);
		ClearClient();
		SetScrFont(FONT20, GREEN);
		strcpy(buff, "设置[0]号主密钥失败");
		DebugOut("%s\n", buff);
		TextOut(0, 4, ALIGN_CENTER, buff);
		WaitKey(1000);
		ReStoreScrFont();
		return;
	}

	ret = PinpadSetWorkKey(handle, 0, 0, key_des);
	if(ret != SUCCESS_PINPAD)
	{
		DebugOut("ret=[%d]\n", ret);
		ClearClient();
		strcpy(buff, "设置[0][0]号工作密钥失败");
		DebugOut("%s\n", buff);
		SetScrFont(FONT20, GREEN);
		TextOut(0, 4, ALIGN_CENTER, buff);
		WaitKey(1000);
		ReStoreScrFont();
		return;
	}

	//active key
	ret = PinpadActiveWorkKey(handle, 0, 0);
	if(ret != SUCCESS_PINPAD)
	{
		DebugOut("ret=[%d]\n", ret);
		ClearClient();
		strcpy(buff, "激活[0][0]号工作密钥失败");
		DebugOut("%s\n", buff);
		SetScrFont(FONT20, GREEN);
		TextOut(0, 4, ALIGN_CENTER, buff);
		WaitKey(1000);
		ReStoreScrFont();
		return;
	}

	ClearClient();
	SetScrFont(FONT20, GREEN);
	TextOut(0, 4, ALIGN_CENTER, "   请在密码键盘上输入123456");
	TextOut(0, 5, ALIGN_CENTER, "   并按确认键");
	ReStoreScrFont();

	//begin to get pwd
	memset(buff, 0, sizeof( buff));
	ret = PinpadInputPwd(handle, mode1, mode2, "123456789012", buff);
	if(ret != SUCCESS_PINPAD)
	{
		DebugOut("ret=[%d]\n", ret);
		ClearClient();
		SetScrFont(FONT20, GREEN);
		strcpy(buff, "输入密码失败");
		TextOut(0, 4, ALIGN_CENTER, buff);
		DebugOut("%s\n", buff);
		ReStoreScrFont();
		WaitKey(2000);
		return;
	}

	DebugOut("\n");
	printf_x("valid pwd:", valid_pwd, 8);
	if(memcmp(buff, valid_pwd, 8))
	{
		FailBeep();
		printf_x("invalid pwd:", buff, 8);
		ClearClient();
		SetScrFont(FONT20, GREEN);
		TextOut(0, 4, ALIGN_CENTER, "测试失败");
		ReStoreScrFont();
		WaitKey(2000);
		return;
	}

	OkBeep();
	ClearClient();
	SetScrFont(FONT20, GREEN);
	TextOut(0, 4, ALIGN_CENTER, "测试成功");
	ReStoreScrFont();
	WaitKey(1000);
}

static void DataEncrypt(int handle)
{
	char key_des[16+1] = "1234567890123456";
	char key_3des[32+1] = "12345678901234567890123456789012";
	char in_data[100] = "\x12\x34\x56\x78\x90\x12\x34\x56";
	char valid_encrypt_des[50+1] = "\x0F\xDC\x09\xB9\x00\xE6\x61\x5C";
	char valid_encrypt_3des[50+1] = "\x2D\x8F\x0B\x22\xF7\x05\xA2\x13";
	char result[50+1] = "";
	char buff[100] = "";
	char cKey;
	UINT DesMode;
	int ret;
	char *p_key;
	char *p_result;

	//select DES mode
	while(1)
	{
		ClearClient();
		SetScrFont(FONT20, GREEN);
		TextOut(0, 3, ALIGN_CENTER, "选择加密方式");
		TextOut(0, 5, ALIGN_LEFT, "      1.DES(默认)");
		TextOut(0, 6, ALIGN_LEFT, "      2.3DES");
		ReStoreScrFont();

		cKey = WaitKey(0);

		switch(cKey)
		{
			case KEY_CANCEL:
				return;
				break;
			case KEY_1:
			case KEY_ENTER:
				DesMode = PINPAD_MODE_DES;
				p_key = key_des;
				p_result = valid_encrypt_des;
				break;
			case KEY_2:
				DesMode = PINPAD_MODE_3DES;
				p_key = key_3des;
				p_result = valid_encrypt_3des;
				break;
			default:
				continue;
				break;
		}
		break;
	}

	DebugOut("DES mode=[%d]\n", DesMode);

	//set DES mode
	//ret = PinpadSetMode(handle, DesMode);
	
	if(ret != SUCCESS_PINPAD)
	{
		DebugOut("ret=[%d]\n", ret);
		ClearClient();
		SetScrFont(FONT20, GREEN);
		strcpy(buff, "设置加密方式失败");
		TextOut(0, 4, ALIGN_CENTER, buff);
		DebugOut("%s\n", buff);
		ReStoreScrFont();
		WaitKey(2000);
		return;
	}

	//set primary key and work key
	ret = PinpadSetPriKey(handle, 0, p_key);
	if(ret != SUCCESS_PINPAD)
	{
		DebugOut("ret=[%d]\n", ret);
		ClearClient();
		strcpy(buff, "设置[0]号主密钥失败");
		DebugOut("%s\n", buff);
		TextOut(0, 4, ALIGN_CENTER, buff);
		WaitKey(1000);
		return;
	}
	ret = PinpadSetWorkKey(handle, 0, 0, p_key);
	if(ret != SUCCESS_PINPAD)
	{
		DebugOut("ret=[%d]\n", ret);
		ClearClient();
		strcpy(buff, "设置[0][0]号工作密钥失败");
		DebugOut("%s\n", buff);
		TextOut(0, 4, ALIGN_CENTER, buff);
		WaitKey(1000);
		return;
	}

	//active key
	ret = PinpadActiveWorkKey(handle, 0, 0);
	if(ret != SUCCESS_PINPAD)
	{
		DebugOut("ret=[%d]\n", ret);
		ClearClient();
		strcpy(buff, "激活[0][0]号工作密钥失败");
		DebugOut("%s\n", buff);
		TextOut(0, 4, ALIGN_CENTER, buff);
		WaitKey(1000);
		return;
	}

	//encrypt data
	memset(result, 0, sizeof(result));
	ret = PinpadEncrypt(handle, in_data, result, 0);
	if(ret != SUCCESS_PINPAD)
	{
		DebugOut("ret=[%d]\n", ret);
		ClearClient();
		SetScrFont(FONT20, GREEN);
		strcpy(buff, "数据加密失败");
		TextOut(0, 4, ALIGN_CENTER, buff);
		DebugOut("%s\n", buff);
		ReStoreScrFont();
		WaitKey(2000);
		return;
	}

	DebugOut("\n");
	printf_x("valid encrypt data:", p_result, 8);
	if(memcmp(result, p_result, 8))
	{
		FailBeep();
		printf_x("invalid encrypt data:", result, 8);
		ClearClient();
		ReStoreScrFont();
		TextOut(0, 4, ALIGN_CENTER, "测试失败");
		ReStoreScrFont();
		WaitKey(2000);
		return;
	}

	OkBeep();
	ClearClient();
	SetScrFont(FONT20, GREEN);
	TextOut(0, 4, ALIGN_CENTER, "加密成功");
	ReStoreScrFont();
	WaitKey(1000);

	//decrypt data
	memset(result, 0, sizeof(result));
	ret = PinpadEncrypt(handle, p_result, result, 1);
	if(ret != SUCCESS_PINPAD)
	{
		DebugOut("ret=[%d]\n", ret);
		ClearClient();
		SetScrFont(FONT20, GREEN);
		strcpy(buff, "数据加密失败");
		TextOut(0, 4, ALIGN_CENTER, buff);
		DebugOut("%s\n", buff);
		ReStoreScrFont();
		WaitKey(2000);
		return;
	}

	DebugOut("\n");
	printf_x("valid decrypt data:", in_data, 8);
	if(memcmp(result, in_data, 8))
	{
		FailBeep();
		printf_x("invalid decrypt data:", result, 8);
		ClearClient();
		ReStoreScrFont();
		TextOut(0, 4, ALIGN_CENTER, "测试失败");
		ReStoreScrFont();
		WaitKey(2000);
		return;
	}

	OkBeep();
	ClearClient();
	SetScrFont(FONT20, GREEN);
	TextOut(0, 4, ALIGN_CENTER, "解密成功");
	ReStoreScrFont();
	WaitKey(1000);
}

static void CalcMac(int handle)
{
	char key_des[16+1] = "1234567890123456";
	char buff[100] = "\x12\x34\x56\x78\x90\x12\x34\x56\x78\x90\x12\x34\x56\x78\x90\x12\x34\x56\x78\x90\x12\x34\x56\x78\x90\x12\x34\x56\x78\x90";
	char valid_mac[50] = "\x4B\xAA\xFB\x9A\x1C\x14\x49\x65";
	char mac[50];
	int ret;

	//set DES mode
	ret = PinpadSetMode(handle, PINPAD_MODE_DES);
	if(ret != SUCCESS_PINPAD)
	{
		DebugOut("ret=[%d]\n", ret);
		ClearClient();
		SetScrFont(FONT20, GREEN);
		strcpy(buff, "设置加密方式失败");
		TextOut(0, 4, ALIGN_CENTER, buff);
		DebugOut("%s\n", buff);
		ReStoreScrFont();
		WaitKey(2000);
		return;
	}

	//set primary key and work key
	ret = PinpadSetPriKey(handle, 0, key_des);
	if(ret != SUCCESS_PINPAD)
	{
		DebugOut("ret=[%d]\n", ret);
		ClearClient();
		SetScrFont(FONT20, GREEN);
		strcpy(buff, "设置[0]号主密钥失败");
		DebugOut("%s\n", buff);
		TextOut(0, 4, ALIGN_CENTER, buff);
		WaitKey(1000);
		ReStoreScrFont();
		return;
	}

	ret = PinpadSetWorkKey(handle, 0, 0, key_des);
	if(ret != SUCCESS_PINPAD)
	{
		DebugOut("ret=[%d]\n", ret);
		ClearClient();
		strcpy(buff, "设置[0][0]号工作密钥失败");
		DebugOut("%s\n", buff);
		SetScrFont(FONT20, GREEN);
		TextOut(0, 4, ALIGN_CENTER, buff);
		WaitKey(1000);
		ReStoreScrFont();
		return;
	}

	//active key
	ret = PinpadActiveWorkKey(handle, 0, 0);
	if(ret != SUCCESS_PINPAD)
	{
		DebugOut("ret=[%d]\n", ret);
		ClearClient();
		strcpy(buff, "激活[0][0]号工作密钥失败");
		DebugOut("%s\n", buff);
		SetScrFont(FONT20, GREEN);
		TextOut(0, 4, ALIGN_CENTER, buff);
		WaitKey(1000);
		ReStoreScrFont();
		return;
	}

	memset(mac, 0, sizeof(mac));
	ret = PinpadCalcMac(handle, buff, strlen(buff), mac);
	if(ret != SUCCESS_PINPAD)
	{
		DebugOut("ret=[%d]\n", ret);
		ClearClient();
		SetScrFont(FONT20, GREEN);
		strcpy(buff, "计算MAC失败");
		TextOut(0, 4, ALIGN_CENTER, buff);
		DebugOut("%s\n", buff);
		ReStoreScrFont();
		WaitKey(2000);
		return;
	}

	DebugOut("\n");
	printf_x("valid mac:", valid_mac, 8);
	if(memcmp(mac, valid_mac, 8))
	{
		FailBeep();
		printf_x("invalid mac:", mac, 8);
		ClearClient();
		SetScrFont(FONT20, GREEN);
		TextOut(0, 4, ALIGN_CENTER, "测试失败");
		ReStoreScrFont();
		WaitKey(2000);
		return;
	}

	OkBeep();
	ClearClient();
	SetScrFont(FONT20, GREEN);
	TextOut(0, 4, ALIGN_CENTER, "测试成功");
	ReStoreScrFont();
	WaitKey(1000);
}

int PinpadTest()
{
	char cKey;
	int handle;

	SetDebug(TRUE);

	while(1)
	{
		ClearClient();
		SetScrFont(FONT20, GREEN);
		TextOut(0, 1, ALIGN_LEFT, "     1.获取键盘ID");
		TextOut(0, 2, ALIGN_LEFT, "     2.屏幕显示");
		TextOut(0, 3, ALIGN_LEFT, "     3.设置密钥");
		TextOut(0, 4, ALIGN_LEFT, "     4.数据加解密");
		TextOut(0, 5, ALIGN_LEFT, "     5.输入密码");
		TextOut(0, 6, ALIGN_LEFT, "     6.MAC加密");
		ReStoreScrFont();

		cKey = WaitKey(0);

		handle = PinpadOpen();
		if(handle <= 0)
		{
			ClearClient();
			SetScrFont(FONT20, GREEN);
			TextOut(0, 4, ALIGN_CENTER, "打开密码键盘失败");
			DebugOut("打开密码键盘失败\n");
			ReStoreScrFont();
			WaitKey(2000);
			continue;
		}

		printf("--------%d---------\n", handle);
		switch(cKey)
		{
			case KEY_CANCEL:
				return 0;
				break;
			case KEY_1:
				GetID(handle);
				break;
			case KEY_2:
				ShowText(handle);
				break;
			case KEY_3:
				SetKey(handle);
				break;
			case KEY_4:
				DataEncrypt(handle);
				break;
			case KEY_5:
				InputPwd(handle);
				break;
			case KEY_6:
				CalcMac(handle);
				break;
		}

		PinpadClose(handle);
	}

	return 0;
}

int PinpadN20Test()
{
	char cKey;
	int handle;

	SetDebug(TRUE);

	while(1)
	{
		ClearClient();
		SetScrFont(FONT20, GREEN);
		TextOut(0, 1, ALIGN_LEFT, "     1.屏幕显示");
		TextOut(0, 2, ALIGN_LEFT, "     2.获取随机数");
		TextOut(0, 3, ALIGN_LEFT, "     3.设置密钥");
		TextOut(0, 4, ALIGN_LEFT, "     4.数据加解密");
		TextOut(0, 5, ALIGN_LEFT, "     5.输入密码");
		TextOut(0, 6, ALIGN_LEFT, "     6.MAC加密");
		ReStoreScrFont();

		cKey = WaitKey(0);
		switch(cKey)
		{
			case KEY_CANCEL:
				return 0;
				break;
			case KEY_1:
				EppOpen();
				//EppGetRand(szBuff);
				EppClearScreen();
				EppDisplayString(0, 2, 16, "BeiJingHuanHong", 15);
				EppClose();
				break;
			case KEY_2:
				ShowText(handle);
				break;
			case KEY_3:
				SetKey(handle);
				break;
			case KEY_4:
				DataEncrypt(handle);
				break;
			case KEY_5:
				InputPwd(handle);
				break;
			case KEY_6:
				CalcMac(handle);
				break;
		}

		PinpadClose(handle);
	}

	return 0;
}

int PinpadHYTest()
{
	char cKey;
	int handle;
	int ret = 0;
	char buff[1024] = {0};
	char buff1[1024] = {0};
	int nlen = 0;
	

	SetDebug(TRUE);

	while(1)
	{
		ClearClient();
		SetScrFont(FONT20, GREEN);
		TextOut(0, 1, ALIGN_LEFT, "     1.屏幕显示");
		TextOut(0, 2, ALIGN_LEFT, "     2.读取加密模式");
		TextOut(0, 3, ALIGN_LEFT, "     3.设置密钥");
		TextOut(0, 4, ALIGN_LEFT, "     4.数据加解密");
		TextOut(0, 5, ALIGN_LEFT, "     5.输入密码");
		TextOut(0, 6, ALIGN_LEFT, "     6.MAC加密");
		ReStoreScrFont();

		cKey = WaitKey(0);
		switch(cKey)
		{
			case KEY_CANCEL:
				return 0;
				break;
			case KEY_1:
				ret = PINPAD_ChkStatus();
				if (ret == 0)
				{
					PINPAD_ResetLCD();
					printf("-------------PINPAD_ResetLCD----------\n");
					PINPAD_DisplayBalance("22.5", 4, 5);
					printf("----------PINPAD_DisplayBalance-------------\n");
					PINPAD_ConfirmAmount("44.4", 4, 14);
					printf("----------PINPAD_ConfirmAmount-------------\n", buff[0]);
				}
				break;
			case KEY_2:
				ret = PINPAD_ChkStatus();
				if (ret == 0)
				{
					memset(buff, 0, sizeof(buff));
					PINPAD_GetDesMode(buff);
					printf("----------PINPAD_GetDesMode:%d-------------\n", buff[0]);
				}
				break;
			case KEY_3:
				PINPAD_ResetPinpad();
				WaitKey(10000);
				ret = PINPAD_ChkStatus();
				if (ret == 0)
				{
					PINPAD_ResetLCD();
			/*		printf("----------PINPAD_ResetPinpad-------------\n");
					ret = PINPAD_SetMasterKey(1, 0, "12345678", 8);
					printf("----------PINPAD_SetMasterKey:%d-------------\n", ret);
					ret = PINPAD_SetWorkKey(0, 0, "98765432", 8);
					printf("----------PINPAD_SetWorkKey:%d-------------\n", ret);*/
					printf("----------PINPAD_ResetPinpad-------------\n");
					ret = PINPAD_SetMasterKey(2, 0, "1234567890123456", 16);
					printf("----------PINPAD_SetMasterKey:%d-------------\n", ret);
					ret = PINPAD_SetWorkKey(0, 0, "9876543210123456", 16);
				}
				break;
			case KEY_4:
				ret = PINPAD_ActiateKey(0, 0, 1);
				printf("----------PINPAD_ActiateKey:%d----------\n", ret);
				memset(buff, 0, sizeof(buff));
				memset(buff1, 0, sizeof(buff1));
				//memcpy(buff, "12345678", 8);
				memcpy(buff, "1234567890123456", 16);
				//PINPAD_Encrypt(buff,8, buff1, &nlen);
				PINPAD_Encrypt(buff,16, buff1, &nlen);
				printf("----------PINPAD_Encrypt:%d----------\n", nlen);
				printf_x("", buff1, nlen);
				break;
			case KEY_5:
				memset(buff, 0, sizeof(buff));
				ret = PINPAD_CalcPinBlock("123456789012", buff,  &nlen, 30000);
				printf("----------PINPAD_CalcPinBlock:%d----------\n", nlen);
				printf_x("", buff, nlen);
				break;
			case KEY_6:
				memset(buff, 0, sizeof(buff));
				memset(buff1, 0, sizeof(buff1));
				//memcpy(buff, "12345678", 8);
				memcpy(buff, "1234567890123456", 16);
				//ret = PINPAD_CalcMAC(1, buff, 8, buff1,  &nlen);
				ret = PINPAD_CalcMAC(1, buff, 16, buff1,  &nlen);
				printf("----------PINPAD_CalcMAC:%d nlen:%d----------\n",ret, nlen);
				printf_x("", buff1, nlen);
				break;
		}

		PinpadClose(handle);
	}

	return 0;
}





