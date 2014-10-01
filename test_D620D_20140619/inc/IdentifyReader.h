#ifndef _IDENTIFY_READER_H_
#define _IDENTIFY_READER_H_

#include "Main.h"


#define READ_MAX_LENGTH					2048


typedef struct
{

	char name[50];
	char sex[10];
	char minzu[20];
	char birthday[30];
	char address[100];
	char id[50];
	char department[100];
	char date_start[30];
	char date_end[30];
	char photo_data[READ_MAX_LENGTH];

}T_IDENTIFY_INFO;


int ReadIdentify(T_IDENTIFY_INFO *pInfo);


#endif



