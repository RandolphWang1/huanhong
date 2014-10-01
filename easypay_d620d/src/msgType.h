
#ifndef __MSG_TYPE_H__
#define __MSG_TYPE_H__

#include "msglib.h"

//The appId must be between APP_ID_MIN and APP_ID_MAX
#define APP_ID_MIN 10000
#define APP_ID_MAX 20000
#define APP_NUM 100

#define KNOWN_SHM_KEY 123456

typedef struct
{
        char appName[APP_NAME_MAXLEN];
		char isReg;
        int appId;
}REGITEM, *PREGITEM;

typedef struct
{
	REGITEM item;
	int 	isActive;
}APPITEM, *PAPPITEM;

#ifdef __DEBUG
void shm_print();
#endif

#endif /* __MSG_TYPE_H__ */
