#include "Main.h"


static void DispKeyValue(UCHAR ucKey)
{
	char buff[100];

	memset(buff, 0, sizeof(buff));
	sprintf(buff, "键值: %d", ucKey);
	TextOut(6, 6, ALIGN_LEFT, buff);
}

int KbdTest()
{
	UCHAR ucLastKey = 0;
	UCHAR ucKey;
	char sKeyName[20];
	char buff[100];

	Clear();
/*	printf("Clear\n");
	ClearKbd();
	printf("ClearKbd\n");
	while(1)
	{
		printf("KbdHit...\n");
		if (TRUE == KbdHit())
		{
			ucKey = GetKey();
			DebugOut("press key: %d\n", ucKey);
			if (ucKey == 0x12)
				break;	
		}
	}
*/
	while(1)
	{
		TextOut(3, 3, ALIGN_LEFT, "请按键:");

		ucKey = WaitKey(0);

		memset(sKeyName, 0, sizeof(sKeyName));
		GetKeyName(ucKey, sKeyName);
		DebugOut("press key: %s\n", sKeyName);
		DebugOut("press key: %d\n", ucKey);

		ClearLine(3, 8);

		switch(ucKey)
		{
			case KEY_0:
			case KEY_1:
			case KEY_2:
			case KEY_3:
			case KEY_4:
			case KEY_5:
			case KEY_6:
			case KEY_7:
			case KEY_8:
			case KEY_9:
			case KEY_ASTERISK:	//*
			case KEY_POUND:		//#
			case KEY_F1:
			case KEY_F2:
			case KEY_BACKSPACE:
			case KEY_ENTER:
				TextOut(0, 8, ALIGN_CENTER, "按两次【取消键】退出");
				break;
			case KEY_CANCEL:
				if(ucLastKey == ucKey)
					return 0;
				TextOut(0, 8, ALIGN_CENTER, "再次按下【取消键】退出");
				break;
		}
		memset(buff, 0, sizeof(buff));
		sprintf(buff, "键名: %s", sKeyName);
		TextOut(6, 5, ALIGN_LEFT, buff);
		DispKeyValue(ucKey);
		ucLastKey = ucKey;
	}
	return 0;
}



