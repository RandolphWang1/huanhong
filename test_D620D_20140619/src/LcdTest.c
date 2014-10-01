#include "Main.h"


static int ColorTest(int color, char *text)
{
	Clear();

	if(FillRect(0, 319, 0, 239, color) != OK)
		return ERROR;

	SetScrFont(0, BLACK);
	if(TextOut(0, 2, ALIGN_CENTER, text) != OK)
		return ERROR;

	SetScrFont(0, BLUE);
	if(TextOut(0, 3, ALIGN_CENTER, text) != OK)
		return ERROR;

	SetScrFont(0, RED);
	if(TextOut(0, 4, ALIGN_CENTER, text) != OK)
		return ERROR;

	SetScrFont(0, GREEN);
	if(TextOut(0, 5, ALIGN_CENTER, text) != OK)
		return ERROR;

	SetScrFont(0, CYAN);
	if(TextOut(0, 6, ALIGN_CENTER, text) != OK)
		return ERROR;

	SetScrFont(0, MAGENTA);
	if(TextOut(0, 7, ALIGN_CENTER, text) != OK)
		return ERROR;

	SetScrFont(0, YELLOW);
	if(TextOut(0, 8, ALIGN_CENTER, text) != OK)
		return ERROR;

	SetScrFont(0, WHITE);
	if(TextOut(0, 9, ALIGN_CENTER, text) != OK)
		return ERROR;

	WaitKey(0);

	return OK;
}

void LcdTest()
{
	//黑色测试
	if(ColorTest(BLACK, "黑色") != OK)
		goto TEST_FAIL;
	//蓝色测试
	if(ColorTest(BLUE, "蓝色") != OK)
		goto TEST_FAIL;
	//红色测试
	if(ColorTest(RED, "红色") != OK)
		goto TEST_FAIL;
	//绿色测试
	if(ColorTest(GREEN, "绿色") != OK)
		goto TEST_FAIL;
	//浅蓝色测试
	if(ColorTest(CYAN, "浅蓝色") != OK)
		goto TEST_FAIL;
	//粉色测试
	if(ColorTest(MAGENTA, "粉色") != OK)
		goto TEST_FAIL;
	//黄色测试
	if(ColorTest(YELLOW, "黄色") != OK)
		goto TEST_FAIL;
	//白色测试
	if(ColorTest(WHITE, "白色") != OK)
		goto TEST_FAIL;

	//字体大小及图片测试
	Clear();

	if(FillRect(0, 319, 0, 239, BLACK) != OK)
		goto TEST_FAIL;

	SetScrFont(FONT12, WHITE);
	if(TextOutByPixel(20, 20, "12号字体 12345 abcde") != OK)
		goto TEST_FAIL;

	SetScrFont(FONT16, WHITE);
	if(TextOutByPixel(20, 40, "16号字体 12345 abcde") != OK)
		goto TEST_FAIL;

	SetScrFont(FONT20, WHITE);
	if(TextOutByPixel(20, 70, "20号字体 12345 abcde") != OK)
		goto TEST_FAIL;

	SetScrFont(FONT24, WHITE);
	if(TextOutByPixel(20, 100, "24号字体 12345 abcde")!= OK)
		goto TEST_FAIL;

	//显示测试图片
	ShowBmpFile(80, 140, "pic/test.bmp");

	WaitKey(0);
	return;

TEST_FAIL:
	Clear();
	TextOut(5, 5, ALIGN_CENTER, "显示屏测试失败");
	GetKey(0);
}



