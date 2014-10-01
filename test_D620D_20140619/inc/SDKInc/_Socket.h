#ifndef __SOCKET_H__
#define __SOCKET_H__
 
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <resolv.h>
#include <sys/mman.h>
#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include "sys/types.h"
#include "sys/socket.h"
#include "unistd.h"
#include "arpa/inet.h"

#define SOCKET int

//超时时间以秒为单位
int TcpConnect(char *ip, char *port, int nTimeout);
int TcpSend(char *send, int len, int nTimeout);
//超时时间以秒为单位
int TcpRecv(char *recv, int *len, int timeout);
int TcpClose();

#endif /* #ifndef __SOCKET_H__ */
