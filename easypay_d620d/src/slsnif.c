
/*  slsnif.c
 *  Copyright (C) 2001 Yan "Warrior" Gurtovoy (ymg@dakotacom.net)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "ascii.h"
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "slsnif.h"
#include "qrgenerator.h"
#include <stdbool.h>
#if 0
#include <pthread.h>
#include "msglib.h"
#endif
#define VERSION  "0.0.1"
#define DEBUG 1
#define ALIPAY_QUERY 1
#ifdef ALIPAY_QUERY
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include "aliqr.h"
#endif
unsigned int possetting[3] = {0x1b,0x3d,0x1};
unsigned char posdle[6] = {0x1b,0x3d,0x1,0x10,0x4,0x01};
unsigned char pos_command_gs_v[3] = {0x1d,0x56,0x31};
unsigned char pos_command_gs_r[3] = {0x1d,0x72,0x1};
pos_gs_struct  pos_gs_command1;//temp 
pos_gs_struct  pos_gs_command2;//temp 
static bool found2 = FALSE;
unsigned char companyname[]={0xd3,0xaf,0xc8,0xf3,0xbd,0xdd,0xcd,0xa8,0x0a};
unsigned char alipay_logo[]={0xd6,0xa7,0xb8,0xb6,0xb1,0xa6,0xc7,0xae,0xb0,0xfc,0xd6,0xa7,0xb8,0xb6,0x0a};
unsigned char alipay_receipt[]={0xd6,0xa7,0xb8,0xb6,0xb1,0xa6,0xbd,0xbb,0xd2,0xd7,0xc6,0xbe,0xcc,0xf5,0x0a};

void copyright() {
    printf("\n\nSerial Line Sniffer. Version %s\n", VERSION);
    printf("\tCopyright (C) 2001 Yan \"Warrior\" Gurtovoy (ymg@azstarnet.com)\n\n");
}

void usage() {
    copyright();
    printf("Usage: slsnif [options] <port>\n\n");
    printf("REQUIRED PARAMETERS:\n");
    printf("  <port>     - serial port to use (i.e /dev/ttyS0, /dev/ttyS1, etc.)\n\n");
    printf("OPTIONS:\n");
    printf("  -h (--help)             - displays this help.\n");
    printf("  -b (--bytes)            - print number of bytes transmitted on every read.\n");
    printf("  -n (--nolock)           - don't try to lock the port.\n");
    printf("  -t (--timestamp)        - print timestamp for every transmission.\n");
    printf("  -l (--log) <logfile>    - file to store output in, defaults to stdout.\n");
    printf("  -i (--in-tee)  <file>   - write raw data from device to this file(s).\n");
    printf("  -o (--out-tee) <file>   - write raw data from host to this file(s).\n");
    printf("  -s (--speed) <speed>    - baudrate to use, defaults to 9600 baud.\n");
    printf("  -p (--port2) <port2>    - serial port to use instead of pty.\n");
    printf("  --color      <color>    - color to use for normal output.\n");
    printf("  --timecolor  <color>    - color to use for timestamp.\n");
    printf("  --bytescolor <color>    - color to use for number of bytes transmitted.\n\n");
    printf("Following names are valid colors:\n");
    printf("?\tblack, red, green, yellow, blue, magenta, cyan, white,\n");
    printf("\tbrightblack,brightred, brightgreen, brightyellow,\n");
    printf("\tbrightblue, brightmagenta, brightcyan, brightwhite\n\n");
    printf("Example: slsnif -l log.txt -s 2400 /dev/ttyS1\n\n");
}

void fatalError(char *msg) {
    perror(msg);
    _exit(-1);
}

int setRaw(int fd, struct termios *ttystate_orig) {
/* set tty into raw mode */

    struct termios    tty_state;

    if (tcgetattr(fd, &tty_state) < 0) return 0;
    /* save off original settings */
    *ttystate_orig = tty_state;
    /* set raw mode */
    //tty_state.c_lflag =0;
    tty_state.c_lflag &= ~(ICANON | IEXTEN | ISIG | ECHO);
    tty_state.c_iflag &= ~(ICRNL | INPCK | ISTRIP | IXON | BRKINT);
    //tty_state.c_oflag =0;
    tty_state.c_oflag &= ~OPOST;
    tty_state.c_cflag |= CS8;
    //tty_state.c_cflag = CS8 | CLOCAL | CREAD | HUPCL;
//======================
    tty_state.c_iflag |= (IXON | IXOFF);
//======================
    tty_state.c_cc[VMIN]  = 1;
    tty_state.c_cc[VTIME] = 0;
    cfsetispeed(&tty_state, tty_data.baudrate);
    cfsetospeed(&tty_state, tty_data.baudrate);
    if (tcsetattr(fd, TCSAFLUSH, &tty_state) < 0) return 0;

    return 1;
}

void fmtData(unsigned char *in, char *out, int in_size) {
/* format data */

    char    charbuf[15];
    int     i;

    /* flush output buffer */
    out[0] = 0;
    for (i = 0; i < in_size; i++) {
        if (in[i] == 127) {
            /* it's a DEL character */
            sprintf(charbuf, "%s (%03i) ", DEL, 127);
        } else {
            if (in[i] < 33)
                /* it's a control character or space */
                sprintf(charbuf, "%s (%03i) ", ascii_chars[(int) in[i]], in[i]);
            else
                /* it's a printable character */
                sprintf(charbuf, "%c (%03i) ", in[i], in[i]);
        }
        /* put formatted data into output buffer */
        strcat(out, charbuf);
    }
}
void setColor(int out, char *color) {
/* changes color on the terminal */

    if (color[0]) {
        write(out, color, 7);
    } else {
        write(out, colors[WHITE].color, 7);
    }
}
unsigned char menuarray[2048];//save a menu's data from pc
int menu_bytes = 0;//to record the count of menu's data

void receiveData(int in, int out) {
    unsigned char buffer[256];
    int n;
    int i;
    unsigned char* realdata= NULL;
    bool found=false;
    if ((n = read(in, buffer, 256)) < 0) {
            if (errno == EIO)
                sleep(1);
    } else {
        if (n > 0) {
            for(i=0;i<n;i++)
                printf("> %x\n",buffer[i]);
            for(i=0;i<n;i++)
            {
                if(buffer[i] == 0x1d && (buffer[i+1] ==0x56||buffer[i+1] ==0x72 ) )
                {
                   printf("gs command find\n");
                   found = true;
                   break;
                }
            }
            if(found){
              if(i!=0){
                  printf("i=%d, menu_bytes=%d\n",i,menu_bytes);
                  memcpy(menuarray+menu_bytes,buffer,i);
                  menu_bytes +=i;
                  if(realdata == NULL)
                     realdata = malloc(i);
                  memcpy(realdata,buffer, i);
#if 0
				  if(!tty_data.using_lp)
                  		write(out,realdata,i);
                  printf("write useful data\n");
#endif
				  free(realdata);

              }
              found = false;
              found2 = true;
              printf("menu_bytes = %d\n",menu_bytes);
           }
           else
           {
                memcpy(menuarray+menu_bytes,buffer,n);
                menu_bytes +=n;
#if 0
                printf("write data to pos\n");
				if(!tty_data.using_lp)
                	write(out, buffer, n);
#endif
          }
      }
    }
}
void writeData(int in, int out, int aux, int mode) {
/* reads data from `in`, formats it, writes it to `out` and `aux`.
 * mode 0 - read from pipe
 * mode 1 - read from port
 * mode 2 - read from pty
 */

    unsigned char   buffer[BUFFSIZE];
    unsigned char   out_buffer[BUFFSIZE];
    char            outbuf[BUFFSIZE * 16 + 1];
    int             n;
    int i;int count;
#ifdef HAVE_SYS_TIMEB_H
    struct timeb    tstamp;
    char            tbuf[29];
    char            tmp[25];
    char            tmp1[4];
#else
#ifdef HAVE_TIME_H
    time_t          tstamp;
#endif
#endif
    unsigned char* realdata=NULL;
    bool found = FALSE;
    //static bool found2 = FALSE;
    int QR_width;
    int version = 0;
    char *barcode = "this is barcode";
    int bit_count=0;int b;
    int size;
    if ((n = read(in, buffer, 256)) < 0) {
        if (mode)
            if (errno == EIO)
                sleep(1);
            else
                perror(mode == 1 ? RPORTFAIL : RPTYFAIL);
        else
            perror(RPIPEFAIL);
    } else {
        if (n > 0) {
            for(i=0;i<n;i++)
                printf("> %x\n",buffer[i]);
            if (mode) {
                //check GS command
                for(i=0;i<n;i++)
                {
                       //if(buffer[i] == 0x1d && (buffer[i+1] ==0x56 || buffer[i+1]==0x72) )
                       if(buffer[i] == 0x1d && (buffer[i+1] ==0x56 ) )
                       {
                          printf("gs command find\n");
                          found = true;
                          break;
                       }
                }
                if(found){ 
                   if(i!=0){
                     if(realdata == NULL)
                         realdata = malloc(i);
                     memcpy(realdata,buffer, i);
                     write(out,realdata,i); 
                     printf("write useful data\n");
                     free(realdata);
                   }
                   found = false;
#if 0
                   pos_gs_command1.data = malloc(n-i);
                   pos_gs_command1.length = n-i;
                   memcpy(pos_gs_command1.data,buffer+i,n-i);
#endif //actually, I don't save any cut command.
                   found2 = true;
#if 0
                   if(found2)
                   {
                       printf("found2 == true, n=%d\n",n);
                       pos_gs_command2.data = malloc(n);
                       pos_gs_command2.length = n;
                       memcpy(pos_gs_command2.data,buffer,n);
                       found2 = false;                
                       printf("do what you want in here\n");
                       generator_qrcode_to_bmp(tty_data.posfd);
#if DEBUG
                       for(i=0;i<pos_gs_command1.length;i++)
                           printf("* %x\n",pos_gs_command1.data[i]);
                       for(i=0;i<pos_gs_command2.length;i++)
                           printf("* %x\n",pos_gs_command2.data[i]);
#endif
                       free(pos_gs_command1.data);
                       free(pos_gs_command2.data);
                  
                       return;
                    }
#endif
                   }
                   else{
                       printf("write data to pos\n");
                       write(out, buffer, n);
                   }
            } else {
                /* print timestamp if necessary */
                if (tty_data.tstamp) {
                    if (out == STDOUT_FILENO) setColor(out, tty_data.tclr);
                    write(out, "\n\n", 2);
#ifdef HAVE_SYS_TIMEB_H
                    ftime(&tstamp);
                    tmp[0] = tmp1[0] = tbuf[0] = 0;
                    strncat(tmp, ctime(&(tstamp.time)), 24);
                    strncat(tbuf, tmp, 19);
                    sprintf(tmp1, ".%2ui", tstamp.millitm);
                    strncat(tbuf, tmp1, 3);
                    strcat(tbuf, tmp + 19);
                    write(out, tbuf, 28);
#else
#ifdef HAVE_TIME_H
                    time(&tstamp);
                    write(out, ctime(&tstamp), 24);
#endif
#endif
                } else {
                    write(out, "\n", 1);
                }
                if (out == STDOUT_FILENO) setColor(out, tty_data.clr);
                /* print prefix */
                write(out, aux ? PORT_IN : PORT_OUT, PRFXSIZE);
                /* format data */
                fmtData(buffer, outbuf, n);
                if (aux && reseek) {
                    /* rotate log file */
                    lseek(tty_data.logfd, 0, SEEK_SET);
		    for (entry= tee_files[0]; entry; entry = entry->next) lseek(entry->fd, 0, SEEK_SET);
                    /* clear the flag */
                    reseek = FALSE;
                }
                /* print data */
                write(out, outbuf, strlen(outbuf));
                /* print total number of bytes if necessary */
                if (tty_data.dspbytes) {
                    buffer[0] = 0;
                    sprintf(buffer, "\n%s %i", TOTALBYTES, n);
                    if (out == STDOUT_FILENO) setColor(out, tty_data.bclr);
                    write (out, buffer, strlen(buffer));
                }
                for (entry = (aux ? tee_files[0] : tee_files[1]); entry; entry = entry->next) {
                    if (n != write(entry->fd, buffer, n)) fatalError(TEEWRTFAIL);
                }
            }
        }
    }
}

void pipeReader() {
/* get data drom pipes */

    int             maxfd;
    fd_set          read_set;

    maxfd = max(tty_data.ptypipefd[0], tty_data.portpipefd[0]);
    while (TRUE) {
        FD_ZERO(&read_set);
        //FD_SET(tty_data.ptypipefd[0], &read_set);
        FD_SET(tty_data.portpipefd[0], &read_set);
        if (select(maxfd + 1, &read_set, NULL, NULL, NULL) < 0) {
	    /* don't bail out if error was caused by interrupt */
	    if (errno != EINTR) {
                perror(SELFAIL);
                return;
            } else {
	        continue;
            }
        }
        if (FD_ISSET(tty_data.ptypipefd[0], &read_set))
            writeData(tty_data.ptypipefd[0], tty_data.logfd, 0, 0);
        else
            if (FD_ISSET(tty_data.portpipefd[0], &read_set))
                writeData(tty_data.portpipefd[0], tty_data.logfd, 1, 0);
    }
}

void closeAll() {
    int i;
/* close all opened file descriptors */
    /* unlock the port(s) if necessary */
    if (!tty_data.nolock) {
        if (tty_data.portName && tty_data.portName[0])
            dev_unlock(tty_data.portName);
        /* this pointer should be NULL if pty is used */
        if (tty_data.ptyName) dev_unlock(tty_data.ptyName);
    }
    /* restore color */
    if (tty_data.logfd == STDOUT_FILENO) 
	   setColor(tty_data.logfd, colors[WHITE].color);
    /* restore settings on pty */
    if (tty_data.ptyraw)
        tcsetattr(tty_data.ptyfd, TCSAFLUSH, &tty_data.ptystate_orig);
    /* close pty */
    if (tty_data.ptyfd >= 0) close(tty_data.ptyfd);
    /* restore settings on port */
    if (tty_data.portraw)
        tcsetattr(tty_data.portfd, TCSAFLUSH, &tty_data.portstate_orig);
    /* close port */
    if (tty_data.portfd >= 0) close(tty_data.portfd);
    /* close log file */
    write(tty_data.logfd, "\n", 1);
    if (tty_data.logfd != STDOUT_FILENO && tty_data.logfd >= 0)
        if ((close(tty_data.logfd)) < 0) perror(CLOSEFAIL);
    /* close write pipes */
    if (tty_data.ptypipefd[1] >= 0) close(tty_data.ptypipefd[1]);
    if (tty_data.portpipefd[1] >= 0) close(tty_data.portpipefd[1]);
    /* close tee files and free allocated memory for in/out tees */
    for (i = 0; i < 2; i++) {
        entry = tee_files[i];
        while (entry) {
            close(entry->fd);
            free(entry->name);
            tmp_entry = entry;
            entry = entry->next;
            free(tmp_entry);
        }
    }
    /* free allocated memory for portName */
    if (tty_data.portName) free(tty_data.portName);
    if (pid >= 0) kill(pid, SIGINT);
}

//RETSIGTYPE sighupP(int sig) {
void sighupP(int sig) {
/*  parent signal handler for SIGHUP */
    if (pid >= 0) kill(pid, SIGHUP);
    return;
}

//RETSIGTYPE sighupC(int sig) {
void sighupC(int sig) {
/*  child signal handler for SIGHUP */
    reseek = 1;
    return;
}

//RETSIGTYPE sigintP(int sig) {
void sigintP(int sig) {
/*parent signal handler for SIGINT */
    closeAll();
    _exit(1);
}

//RETSIGTYPE sigintC(int sig) {
void sigintC(int sig) {
/* child signal handler for SIGINT */
    /* close read pipes */
    if (tty_data.ptypipefd[0] >= 0) close(tty_data.ptypipefd[0]);
    if (tty_data.portpipefd[0] >= 0) close(tty_data.portpipefd[0]);
    _exit(1);
}

//RETSIGTYPE sigchldP(int sig) {
void sigchldP(int sig) {
/* signal handler for SIGCHLD */

    int status;

    wait(&status);
}

char *getColor(char *name) {
/* returns escape sequence that corresponds to a given color name */

    int i = 0;

    while (colors[i].name && strcmp(name, colors[i].name)) i++;
    if (colors[i].name) return colors[i].color;
    return NULL;
}

int get_posfd(void)
{
    return tty_data.posfd;
}
/*static void add_qrcode_function(void)*/
static void add_qrcode_function(char* qr_price)
{
        printf("%s\n",__func__);
        generator_qrcode_to_bmp(tty_data.posfd, qr_price);
}

/*static bool get_price(int menubytes, double* price)*/
static bool get_price(int menubytes, char* price)
{
        int i = 0;
        int j = 0;
        int k = 0;
        bool price_found = false;
        unsigned char count[256];
        char* account = NULL;
		int index = 0; //save i
        for(i=0;i<menubytes;i++)
        {
			//??
           //if((menuarray[i] == 0xd3)&&(menuarray[i+1] == 0xa6) && (menuarray[i+2] ==0xca) &&(menuarray[i+3] == 0xd5))
           if((menuarray[i] == 0xd3)&&(menuarray[i+1] == 0xa6) && (menuarray[i+2] ==0xb8) &&(menuarray[i+3] == 0xb6))
           //if((menuarray[i] == 0xba)&&(menuarray[i+1] == 0xcf) && (menuarray[i+2] ==0xbc) &&(menuarray[i+3] == 0xc6))
           {
                printf("find the price\n");
                price_found = true;
				index = i;
                break;
           }
           /*if((menuarray[i] == 0xba)&&(menuarray[i+1] == 0xcf) && (menuarray[i+2] ==0xbc) &&(menuarray[i+3] == 0xc6))
           {
                printf("find the heji\n");
                price_found = true;
		index = i;
                break;
           }*/
		   //??
           if((menuarray[i] == 0xbd)&&(menuarray[i+1] == 0xf0) && (menuarray[i+2] ==0xb6) &&(menuarray[i+3] == 0xee))
		   {
			    printf("find the jin'e\n");
                price_found = true;
				index = i;
				continue;
		   }

        }
        if(price_found)
        {
               //there are 5 bytes to ignore, they are:d3a6cad5:
               //so copy 256 bytes to count from the fifth byte.
               memcpy(count,menuarray+index+5,256);
               //memcpy(count,menuarray+i+5,256);
               for(i=0;i<256;i++)
               {
                    //find the .
                    if(count[i]==0x2e) //0x2e = . 
                        break;
               }
               //i respresent the ., so before the dot, they are the entire price
               printf("you should pay: ");
               for(j=0;j<i;j++)
                   printf("%c",count[j]);
               printf(".");
               printf("%c%c\n",count[i+1],count[j+2]);
               if(i>0)
               {
                  account = malloc(i+3+1);
                  for(j=0,k=0;j<i+3;j++){
                      if(count[j] != 32){
                      sprintf( account+k, "%c", count[j] );
                      k++;
                      }
                  }
                  account[k] ='\0';
                  
                  printf("account=%s\n",account);
                  //atof cannot work normally, use atoi
                  #if 0
                  *price = atoi(account);
                  #endif
                  strcpy(price,account);
                  free(account);
               }
        }
        return price_found;
      
}
#if 0
char prompt[1024];
void* recvThread( void *param )
{
	    int n;
		MsgHandle* handle = (MsgHandle*)param;
		MSG msg;

		msgTrace( handle, "recvThread is running...\n");
    	while( (n = msgGet(handle, &msg)) != -1 )
		{   
				msgTrace(handle,"easypay receive message\n");
				switch( msg.type )
				{
						case MSG_APP_ACTIVE:
							strcpy( prompt, (char*)msg.data );
							msgTrace(  handle, "msg:%s\n", prompt );
							break;
						case MSG_GETLINE:
							break;
						default:
							msgTrace( handle, "recevied a message with wrong type!\n" );
							break;
																										}
		}
exit:
		msgTrace(handle, "recvThread exit.\n");
		return NULL;
}
#endif

#ifdef ALIPAY_QUERY
int SplitStr(char *buff, char *parr[], char *token) 
{
        char *pc = strtok(buff, token); 
        int i;  

        for(i=0; pc != NULL; i++)
        {       
                parr[i] = pc;
                pc = strtok(NULL, token); 
        }       
        
        return i;
}

int connection_handler(int connection_fd)
{
    int nbytes;
    char buffer[1024];

    nbytes = read(connection_fd, buffer, 1024);
    buffer[nbytes] = 0;

    printf("MESSAGE FROM ALIPAY: %s\n", buffer);
    //nbytes = snprintf(buffer, 256, "hello from the server");
    //write(connection_fd, buffer, nbytes);

    close(connection_fd);
    return 0;
}
#endif
int main(int argc, char *argv[]) {

    int             i, j, maxfd, optret, needsync = 1;
    char            *logName = NULL, baudstr[7], *ptr1, *ptr2;
    struct termios  tty_state;
    fd_set          rset;
    unsigned char pos_buffer[256];
    int n;
    bool price_is_found = false;
    #if 0
    double price = 0;
    #endif
    char price[16]= {0};

    //struct lgsm_handle *gsm_handle =NULL;
    int gsm_fd =0;
    int rc;
    char gsm_buf[1024+1];
#ifdef ALIPAY_QUERY
    struct sockaddr_un address;
    int socket_fd, connection_fd;
    socklen_t address_length;
    pid_t child;
    int nbytes; 
    char buffer[1024];
    int trade_num;
    char *trade_ptr[100] = {NULL};
    char *trade_detail[5] = {NULL}; 
    struct receipt_info pos_receipt;
#endif
#ifdef HAVE_GETOPT_LONG
    struct option longopts[] = {
        {"help",       0, NULL, 'h'},
        {"log",        1, NULL, 'l'},
        {"nolock",     0, NULL, 'n'},
        {"port2",      1, NULL, 'p'},
        {"speed",      1, NULL, 's'},
        {"bytes",      0, NULL, 'b'},
        {"timestamp",  0, NULL, 't'},
        {"color",      1, NULL, 'x'},
        {"timecolor",  1, NULL, 'y'},
        {"bytescolor", 1, NULL, 'z'},
        {"in-tee",     1, NULL, 'i'},
        {"out-tee",    1, NULL, 'o'},
        {NULL,         0, NULL,   0}
    };
#endif
#if 0
	MsgHandle handle;
	pthread_t threadId;
	int retval;
#endif
    /* don't lose last chunk of data when output is non-interactive */
    setvbuf(stdout,NULL,_IONBF,0);
    setvbuf(stderr,NULL,_IONBF,0);

    /* initialize variables */
    baudstr[0] = 0;
    tee_files[0] = tee_files[1] = NULL;
    tty_data.portName = tty_data.ptyName = NULL;
    tty_data.posName=NULL;//test
    tty_data.ptyfd = tty_data.portfd = tty_data.logfd = -1;
    tty_data.ptyraw = tty_data.portraw = tty_data.nolock = FALSE;
    tty_data.dspbytes = tty_data.tstamp = FALSE;
    tty_data.ptypipefd[0] = tty_data.ptypipefd[1] = -1;
    tty_data.portpipefd[0] = tty_data.portpipefd[1] = -1;
    tty_data.baudrate = DEFBAUDRATE;
    tty_data.clr[0] = tty_data.bclr[0] = tty_data.tclr[0] = 0;
    /* parse rc-file */
    readRC(&tty_data);
    /* activate signal handlers */
    signal(SIGINT, sigintP);
    signal(SIGCHLD, sigchldP);
    signal(SIGHUP, sighupP);
    /* register closeAll() function to be called on normal termination */
    atexit(closeAll);
    /* process command line arguments */
#ifdef HAVE_GETOPT_LONG
    while ((optret = getopt_long(argc, argv, OPTSTR, longopts, NULL)) != -1)    {
#else
    while ((optret = getopt(argc, argv, OPTSTR)) != -1) {
#endif
        switch (optret) {
            case 'b':
                tty_data.dspbytes = TRUE;
                break;
            case 't':
                tty_data.tstamp = TRUE;
                break;
            case 'n':
                tty_data.nolock = TRUE;
                break;
            case 'l':
                logName = (optarg[0] == '=' ? optarg + 1 : optarg);
                break;
            case 's':
                i = 0;
                while (baudrates[i].spdstr &&
                        strcmp(optarg, baudrates[i].spdstr)) i++;
                if (baudrates[i].spdstr) {
                    tty_data.baudrate = baudrates[i].speed;
                    strcat(baudstr, baudrates[i].spdstr);
                }
                break;
            case 'p':
                tty_data.ptyName = (optarg[0] == '=' ? optarg + 1 : optarg);
                break;
            case 'x':
                ptr1 = getColor(optarg);
                if (ptr1) {
                    tty_data.clr[0] = 0;
                    strcat(tty_data.clr, ptr1);
                }
                break;
            case 'y':
                ptr1 = getColor(optarg);
                if (ptr1) {
                    tty_data.tclr[0] = 0;
                    strcat(tty_data.tclr, ptr1);
                }
                break;
            case 'z':
                ptr1 = getColor(optarg);
                if (ptr1) {
                    tty_data.bclr[0] = 0;
                    strcat(tty_data.bclr, ptr1);
                }
                break;
            case 'i':
            case 'o':
                    if (!(entry = malloc(sizeof(tee_entry)))) fatalError(MEMFAIL);
                    entryp = (optret == 'i' ? &tee_files[0] : &tee_files[1]);
                    if (!(entry->name = malloc((strlen(optarg) + 1) * sizeof(char))))
                        fatalError(MEMFAIL);
                    strcpy(entry->name, optarg);
                    entry->next = *entryp;
                    *entryp = entry;
                break;
            case 'h':
            case '?':
            default :
                usage();
                tty_data.ptyName = NULL;
                return -1;
        }
    }

#if 0
    if (optind < argc) {
	   if (!(tty_data.portName = malloc(PORTNAMELEN))) fatalError(MEMFAIL);
	   ptr1 = argv[optind];
	   ptr2 = tty_data.portName;
	   while((*ptr1 == '/' || isalnum(*ptr1))
                && ptr2 - tty_data.portName < PORTNAMELEN - 1) *ptr2++ = *ptr1++;
	   *ptr2 = 0;
    }
#endif
    if (!(tty_data.portName = malloc(PORTNAMELEN))) fatalError(MEMFAIL);
    tty_data.portName= argv[optind];
    printf(" portName=%s\n",tty_data.portName);
	if(access("/dev/lp0",0) == 0)
		tty_data.using_lp = true;
	else
		tty_data.using_lp = false;
	printf(" using lp =%d\n",tty_data.using_lp);
	if(tty_data.using_lp)
		tty_data.posName = "/dev/lp0";
	else
        tty_data.posName="/dev/s3c2410_serial2";
    if (!tty_data.portName || !tty_data.portName[0]) {
        usage();
        tty_data.ptyName = NULL;
        return -1;
    }
    if (tty_data.ptyName && !strncmp(tty_data.portName, tty_data.ptyName, strlen(tty_data.portName))) {
        /* first and second port point to the same device */
	errno = EINVAL;
        perror(DIFFFAIL);
        return -1;
    }

    //copyright();
    /* open logfile */
    if (!logName)
        tty_data.logfd = STDOUT_FILENO;
    else {
        if((tty_data.logfd = open(logName,
                    O_RDWR|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR)) < 0) {
            perror(LOGFAIL);
            tty_data.logfd = STDOUT_FILENO;
        } else {
            printf("Started logging data into file '%s'.\n", logName);
        }
    }
    /* open tee files */
    for (i = 0; i < 2; i++) {
        entry = tee_files[i];
        while (entry) {
            if ((entry->fd = open(entry->name, O_WRONLY|O_NOCTTY|O_CREAT|O_TRUNC, 0644)) < 0) {
                perror(TEEFAIL);
            } else {
                printf("Started logging raw data from %s into file '%s'.\n",
                                            i ? "host" : "device", entry->name);
            }
            entry = entry->next;
        }
    }
    /* create pipe */
    if (pipe(tty_data.ptypipefd) < 0 || pipe(tty_data.portpipefd) < 0) {
        perror(PIPEFAIL);
        return -1;
    }
    /* fork child process */
    switch (pid = fork()) {
    case 0:
        /* child process */
        /* close write pipe */
        close(tty_data.ptypipefd[1]);
        close(tty_data.portpipefd[1]);
        signal(SIGINT, sigintC);
        signal(SIGHUP, sighupC);
        pipeReader(&tty_data);
        break;
    case -1:
        /* fork() failed */
        perror(FORKFAIL);
        return -1;
    default:
        /* parent process */
        /* close read pipe */
        close(tty_data.ptypipefd[0]);
        close(tty_data.portpipefd[0]);
        break;
    }
    if (!tty_data.ptyName) {
        /* open master side of the pty */
        /* Search for a free pty */
        if (!(tty_data.ptyName = strdup(DEFPTRNAME))) fatalError(MEMFAIL);
        for(i = 0; i < 16 && tty_data.ptyfd < 0; i++) {
            tty_data.ptyName[8] = "pqrstuvwxyzPQRST"[i];
            for(j = 0; j < 16 && tty_data.ptyfd < 0; j++) {
                tty_data.ptyName[9] = "0123456789abcdef"[j];
                /* try to open master side */
                tty_data.ptyfd = open(tty_data.ptyName, O_RDWR|O_NONBLOCK|O_NOCTTY);
            } 
        }
        if (tty_data.ptyfd < 0) {
            /* failed to find an available pty */
            free(tty_data.ptyName);
            /*set pointer to NULL as it will be checked in closeAll() */
            tty_data.ptyName = NULL;
            perror(PTYFAIL);
            return -1;
        }
        /* create the name of the slave pty */
        tty_data.ptyName[5] = 't';
        free(tty_data.ptyName);
        /*set pointer to NULL as it will be checked in closeAll() */
        tty_data.ptyName = NULL;
    } else {
        /* open port2 instead of pty */
        /* lock port2 */
        if (!tty_data.nolock && dev_setlock(tty_data.ptyName) == -1) {
            /* couldn't lock the device */
            return -1;
        }
        /* try to open port2 */
        if ((tty_data.ptyfd = open(tty_data.ptyName, O_RDWR|O_NONBLOCK)) < 0) {
            perror(PORTFAIL);
            return -1;
        }
        printf("Opened port: %s\n", tty_data.ptyName);
    }
    /* set raw mode on pty */
    if(!setRaw(tty_data.ptyfd, &tty_data.ptystate_orig)) {
        perror(RAWFAIL);
        return -1;
    }
    tty_data.ptyraw = TRUE;
    /* lock port */
    if (!tty_data.nolock && dev_setlock(tty_data.portName) == -1) {
        /* couldn't lock the device */
        return -1;
    }
    /* try to open port */
    if ((tty_data.portfd = open(tty_data.portName, O_RDWR|O_NONBLOCK)) < 0) {
        perror(PORTFAIL);
        return -1;
    }
    printf("Opened port: %s\n", tty_data.portName);
	if(tty_data.using_lp){
    			if ((tty_data.posfd = open(tty_data.posName, O_RDWR)) < 0) {
				perror(PORTFAIL);
				return -1;
			    }
	}
	else{
    		if ((tty_data.posfd = open(tty_data.posName, O_RDWR|O_NONBLOCK)) < 0) {
        		perror(PORTFAIL);
        		return -1;
    		}
	}
    printf("Opened port:%s\n",tty_data.posName);

    /* set raw mode on port */
    if (!setRaw(tty_data.portfd, &tty_data.portstate_orig)) {
        perror(RAWFAIL);
        return -1;
    }
	if(!tty_data.using_lp){
    	if (!setRaw(tty_data.posfd, &tty_data.portstate_orig)) {
        	perror(RAWFAIL);
        	return -1;
    	}
    }
    tty_data.portraw = TRUE;
    printf("Baudrate is set to %s baud%s.\n",
                baudstr[0] ? baudstr : "9600",
                baudstr[0] ? "" : " (default)");

#ifdef ALIPAY_QUERY
    socket_fd = socket(PF_UNIX, SOCK_STREAM, 0);
    if(socket_fd < 0)
    {
        printf("socket() failed\n");
        return 1;
    } 

    unlink("/tmp/demo_socket");

    /* 从一个干净的地址结构开始 */ 
    memset(&address, 0, sizeof(struct sockaddr_un));

    address.sun_family = AF_UNIX;
    snprintf(address.sun_path, 20/*UNIX_PATH_MAX*/, "/tmp/demo_socket");

    if(bind(socket_fd, 
         (struct sockaddr *) &address, 
         sizeof(struct sockaddr_un)) != 0)
    {
        printf("bind() failed\n");
        return 1;
    }

    if(listen(socket_fd, 5) != 0)
    {
        printf("listen() failed\n");
        return 1;
    }
    address_length = sizeof(address);
#endif
/*
    gsm_handle  = (struct lgsm_handle *) libgmsd_tool_main();
	if(gsm_handle!=NULL){
    	shell_main(gsm_handle, 0);
    	if(!gsm_handle)
       		printf("error, connot get gsm handle\n");
    	else{
       		gsm_fd = lgsm_fd(gsm_handle);
       		fcntl(gsm_fd, F_SETFD, O_NONBLOCK);
       		printf("gsm_fd = %d\n",gsm_fd);
    	}
	}
*/
#ifdef ALIPAY_QUERY
//fcntl(socket_fd, F_SETFD, O_NONBLOCK);
#endif
    /* start listening to the slave and port */
	if(!tty_data.using_lp){
    	maxfd = max(tty_data.portfd, tty_data.posfd);
		printf("maxfd =%d\n",maxfd);
        /*
        if(gsm_fd !=0)
            maxfd = max(maxfd,gsm_fd);
        */
	}else{
    	maxfd = tty_data.portfd;
		printf("maxfd =%d\n",maxfd);
        /*
        if(gsm_fd !=0)
            maxfd = max(maxfd,gsm_fd);
        */
	}
#ifdef ALIPAY_QUERY
if(socket_fd != 0)
  maxfd = max(maxfd,socket_fd);
#endif
#if 0
	//pthread
	if( msgRegister( &handle, "easypay" ) )
	{
		printf("easypay Register failed!\n");
		return -1;
	}
	pthread_create( &threadId, NULL, recvThread, &handle );
#endif

	
    while (TRUE) {
        FD_ZERO(&rset);
	    if(tty_data.using_lp)
       	    FD_SET(tty_data.posfd, &rset);
        FD_SET(tty_data.portfd, &rset);
        //FD_SET(gsm_fd, &rset);
#ifdef ALIPAY_QUERY
        FD_SET(socket_fd, &rset);
#endif
        if(found2)
        {
            //fixme in the furture.
            found2 = false;
            price_is_found = get_price(menu_bytes,(char*)&price);
#if 0
			if(tty_data.using_lp){
				memcpy(menuarray+menu_bytes,companyname,sizeof(companyname));
				menu_bytes = menu_bytes+sizeof(companyname);
            	n = write(tty_data.posfd,menuarray,menu_bytes);
				printf("for LP, send data once, n=%d\n",n);
            	n = 0;
			}
			else
			{
				write(tty_data.posfd,companyname,sizeof(companyname));
			}
#else
		    //memcpy(menuarray+menu_bytes,companyname,sizeof(companyname));
		    //	menu_bytes = menu_bytes+sizeof(companyname);
		    memcpy(menuarray+menu_bytes,alipay_logo,sizeof(alipay_logo));
		    menu_bytes = menu_bytes+sizeof(alipay_logo);
		    n = write(tty_data.posfd,menuarray,menu_bytes);
		    printf("Now, we will start to send data to pos, n=%d\n",n);
			n = 0;
#endif
            menu_bytes = 0;
            if(price_is_found)
            {
                #if 0
                printf("price is %.2f\n",price);
                add_qrcode_function();
                #endif
                printf("price is %s\n",price);
                add_qrcode_function(price);
            }
        }
        if (select(maxfd + 1, &rset, NULL, NULL, NULL) < 0) {
	        if (errno != EINTR) {
                perror(SELFAIL);
                return -1;
            } else {
	        continue;
	    }
        }
        if (FD_ISSET(tty_data.portfd, &rset)) {
            /* data coming from device */
            receiveData(tty_data.portfd, tty_data.posfd);
        }
#if 0
        else if(FD_ISSET(gsm_fd, &rset)){
			/* we've received something on the gsmd socket, pass it
			 * on to the library */
			rc = read(gsm_fd, gsm_buf, sizeof(gsm_buf));
			if (rc <= 0) {
				printf("ERROR reading from gsm_fd\n");
				continue;
				//break;
			}
			rc = lgsm_handle_packet(gsm_handle, gsm_buf, rc);
			if (rc < 0)
				printf("ERROR processing packet: %d(%s)\n", rc, strerror(-rc));
        }
#endif
#ifdef ALIPAY_QUERY
        else if(FD_ISSET(socket_fd, &rset)) {
        if ((connection_fd = accept(socket_fd,
                               (struct sockaddr *) &address,
                               &address_length)) > -1)
            {
#if 0
            child = fork();
            printf("the fork child id is %d\n",child);
            if(child == 0)
            {
              /* 现在处于新建的连接处理进程中了 */
              printf("connection_handler is ongoing!\n");
              return connection_handler(connection_fd);
            }
#endif

    nbytes = read(connection_fd, buffer, 1024);
    buffer[nbytes] = 0;

    printf("MESSAGE FROM ALIPAY: %s\n", buffer);
    //nbytes = snprintf(buffer, 256, "hello from the server");
    //write(connection_fd, buffer, nbytes);
    /* start print out the payment query result */

    trade_num = SplitStr(buffer,trade_ptr,"|");

    //write(tty_data.posfd,alipay_receipt,sizeof(alipay_receipt));
    write(tty_data.posfd,"\n",1);
    printf("the pos fd is %d\n",tty_data.posfd);
    for (i=0; i<trade_num; i++){
        printf("number %d trade:%s\n",i,trade_ptr[i]);
        SplitStr(trade_ptr[i],trade_detail,",");
#if 0
        for(j=0; j<5; j++){
            printf("%d trade detail:%s\n",i,trade_detail[j]);
            write(tty_data.posfd,"trade detail:",14);
            write(tty_data.posfd,trade_detail[j],strlen(trade_detail[j])+1);
            write(tty_data.posfd,"\n",1);
        }
#endif
       memset(pos_receipt.serial_number,0,24);
       memset(pos_receipt.out_trade_no,0,16);
       memset(pos_receipt.trade_no,0,32);
       memset(pos_receipt.total_fee,0,16);

       strcpy(pos_receipt.serial_number,trade_detail[0]);
       strcpy(pos_receipt.out_trade_no,trade_detail[1]);
       strcpy(pos_receipt.trade_no,trade_detail[2]);
       strcpy(pos_receipt.total_fee,trade_detail[3]);
       WritePayment(tty_data.posfd, &pos_receipt);
       write(tty_data.posfd,"\n",1);
       write(tty_data.posfd,"\n",1);
    }



    close(connection_fd);
        }
    } 
#endif
    else {
			if(!tty_data.using_lp){
            	if(FD_ISSET(tty_data.posfd, &rset)){
                	printf("data coming from pos\n");
                	if ((n = read(tty_data.posfd, pos_buffer, 256)) < 0) {
                	printf("posfd n<0\n");
                	}
                	else
                	{
                   		printf("pos data n=%d\n",n);
                   		for(i=0;i<n;i++)
                     		printf("< %x\n",pos_buffer[i]);
                   		printf("send to pc\n");
                   		write(tty_data.portfd,pos_buffer,n);
                	}
        
          		}
		   }
        }
    }
#ifdef ALIPAY_QUERY
    close(socket_fd);
    unlink("/tmp/demo_socket");
#endif
#if 0
	pthread_join( threadId, (void**)&retval );
	msgUnRegister( &handle, "easypay" );
#endif
    return 1;
}
