
#ifndef __MSG_LIB_H__
#define __MSG_LIB_H__ 

#include <sys/socket.h>
#include <netinet/in.h>  /* sockaddr_in{} and other Internet defns */
#include <sys/un.h>

//The length of appName can't more than APP_NAME_MAXLEN
#define APP_NAME_MAXLEN 20
#define MSG_MAXLEN 1024

//The message id define
#define MSG_PRINTF 		0
#define MSG_TRACE  		1
#define MSG_APP_ACTIVE 		2
#define MSG_APP_INACTIVE 	3
#define MSG_GETLINE		4

typedef struct
{
	int sockfd;
	int isRegistered;
	int myId;
	char myName[APP_NAME_MAXLEN];
	struct sockaddr_un servaddr;
}MsgHandle, *PMsgHandle;

typedef struct
{
	unsigned int senderId;	//This is a sender's ID
	unsigned int type;	//The msg type, defined by application.
	unsigned int len;	//The length of data
	char data[MSG_MAXLEN];  //The data of msg	
}MSG, *PMSG;


//before call other function, the msgRegsiter shouled be called first.
//ret val: 0 if success, not 0 otherwise.
int msgRegister( MsgHandle* handle, char *appName );

//send msg to other app.
//ret val: 0 if success, not 0 otherwise.
int msgPost( MsgHandle* handle, char *appName, MSG msg );

//recevie msg from other app, the msgGet will be blocking.
//ret val: number of bytes recv, -1 if error.
int msgGet( MsgHandle* handle, MSG *msg );

//if success return appid, otherwise return 0.
int msgGetAppId( char *appName, int *appId );

//return 0 if success, otherwise return -1
int msgGetAppName( int appId, char *appName );

//the msgUnRegister should be called, before app exit
//ret val: 0 if success, not 0 otherwise.
int msgUnRegister( MsgHandle* handle, char *appName );

//print to TRACE_SERVER
void msgTrace( MsgHandle* handle, char *tarce, ... );

//print to Controller
void msgPrintf( MsgHandle* handle, char *fmt, ... );

#endif /* __MSG_LIB_H__ */

