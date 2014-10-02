#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "statement.h"
#include "aliqr.h"

unsigned char ToHex(char text)
{
	unsigned char var;
	//printf("text=%x\n",text);
	if (text>='A' && text <='F')
			var = text-55;//a-f之间的ascii与对应数值相差55如'A'为65,65-55即为A
	else
			var = text-48;
	//printf("var=%x\n",var);
	return var;
}


int parse_file(char** gbk_name,int *gbk_size,int position, int offset) {
	  
	  int length = 0;
	  FILE *config = fopen("config.txt", "r");
	  int i;
	  char buffer[100];
	  if (config == NULL) {
			perror ("Error opening config file");
			exit(1);
	  }
	  for(i=0;i<position;i++){
	     //continue use fgets to read.
	  	//memset(buffer,100,'\0');
		//printf("read i=%d\n",i);
	  	if (fgets(buffer, 100, config) == NULL) {
			perror ("Error reading config");
			exit(1);
	   	}
	  }
	  // and replace the newline with a nullbyte
	   for (i = 0; i < 100; i++)
	   if (buffer[i] == '\n') {
		  buffer[i] = '\0';
		  break;
	   }
	   
	   length = strlen(buffer);
	   //printf("length is %d\n",length);
	   //printf("offset is %d\n",offset);
           *gbk_name = malloc(length - offset + 1);// not include offset(for example 'title: '), add 1 for \0
	   *gbk_size = length - offset;
           //printf("gbksize is %d\n",*gbk_size);
	   memcpy(*gbk_name,buffer+offset,*gbk_size + 1);
	   //printf("... %s\n",buffer);
	   //printf(".... %s\n",*gbk_name);
	   fclose(config);
	   return 0;
}
void WritePayment(int posfd, struct receipt_info *rt_detail)
{
	char* title = NULL;
	unsigned char* commpany_title = NULL;
	int title_size ;

        char* serial_number = NULL;
        unsigned char* serial_title = NULL;
        int serial_size;

	char* ticket_number = NULL;
	unsigned char* ticket_title = NULL;
	int ticket_size ;

	char* date = NULL;
	unsigned char* date_title = NULL;
	int date_size ;

	char* config_time = NULL;
	unsigned char* time_title = NULL;
	int time_size ;

	char* line = NULL;
	unsigned char* line_title= NULL;
	int line_size;

	char* transaction = NULL;
	unsigned char* transaction_title= NULL;
	int transaction_size;

	char* account = NULL;
	unsigned char* account_title= NULL;
	int account_size;

	char* transaction_number = NULL;
	unsigned char* transaction_number_title= NULL;
	int transaction_number_size;

	char* money = NULL;
	unsigned char* money_title= NULL;
	int money_size;

	char* sign = NULL;
	unsigned char* sign_title= NULL;
	int sign_size;

	char* agree = NULL;
	unsigned char* agree_title= NULL;
	int agree_size;

	int i,j = 0;
	unsigned long high, low;
	unsigned char chinesecmd[]={0x1c,0x26};
	unsigned char cutCommand[]= {0x1d,0x56,0x42,0x80,0x1d,0x72,0x01};
	unsigned char cmd1[]={0xa3,0xb1,0x0a};
        struct pos_statement_of_account testdata;

        struct tm *ptr;
        time_t td;
        char pos_time[12];
        
	memset(testdata.ticket_number,0,32);
	strcpy(testdata.ticket_number,"123456789012345\n");
       
	write(posfd, chinesecmd,sizeof(chinesecmd));
	//parse file, get title
        
        parse_file(&title,&title_size,1, 7);//'title: ' 
	//get_title(&title,&title_size);
	//printf("title_size=%d\n",title_size);
	//printf("title is %s\n",title);
	commpany_title = malloc(title_size/2+1);//add '\n' at the last
 	i=j=0;
	for(i=0;i< title_size;)
	{
		commpany_title[j] = ToHex(title[i]) << 4 | ToHex(title[i+1]);
						//printf("%02x\n",commpany_title[j]);
		j++;
		i = i+2;

	}
	//printf("j=%d\n",j);
	commpany_title[j] = '\n';

	parse_file(&serial_number,&serial_size,2, 15);//'serial_number: ' 
	serial_title= malloc(serial_size/2 + 1);
 	i=j=0;
	for(i=0;i< serial_size;)
	{
		serial_title[j] = ToHex(serial_number[i]) << 4 | ToHex(serial_number[i+1]);
		//printf("%02x\n",serial_title[j]);
		j++;
		i = i+2;

	}
	serial_title[j] = '\n';

	parse_file(&ticket_number,&ticket_size,3, 15);//'ticket_number: ' 
	//get_ticket_number(&ticket_number,&ticket_size);
	ticket_title= malloc(ticket_size/2 + 1);
 	i=j=0;
	for(i=0;i< ticket_size;)
	{
		ticket_title[j] = ToHex(ticket_number[i]) << 4 | ToHex(ticket_number[i+1]);
		//printf("%02x\n",commpany_title[j]);
		j++;
		i = i+2;

	}
	ticket_title[j] = '\n';


	parse_file(&date,&date_size,4, 6);//'date: ' 
	date_title= malloc(date_size/2 + 1);
 	i=j=0;
	for(i=0;i< date_size;)
	{
		date_title[j] = ToHex(date[i]) << 4 | ToHex(date[i+1]);
		j++;
		i = i+2;

	}
	date_title[j] = '\n';

	parse_file(&config_time,&time_size,5, 6);//'time: ' 

	time_title= malloc(time_size/2 + 1);
 	i=j=0;
	for(i=0;i< time_size;)
	{
		time_title[j] = ToHex(config_time[i]) << 4 | ToHex(config_time[i+1]);
		j++;
		i = i+2;

	}
	time_title[j] = '\n';

	parse_file(&line,&line_size,6, 6);//'line: ' 

	line_title= malloc(line_size/2 + 1);
 	i=j=0;
	for(i=0;i< line_size;)
	{
		line_title[j] = ToHex(line[i]) << 4 | ToHex(line[i+1]);
		j++;
		i = i+2;

	}
	line_title[j] = '\n';

	parse_file(&transaction,&transaction_size,7, 13);//'transaction: ' 

	transaction_title= malloc(transaction_size/2 + 1);
 	i=j=0;
	for(i=0;i< transaction_size;)
	{
		transaction_title[j] = ToHex(transaction[i]) << 4 | ToHex(transaction[i+1]);
		j++;
		i = i+2;

	}
	transaction_title[j] = '\n';

	parse_file(&account,&account_size,8, 9);//'account: ' 

	account_title= malloc(account_size/2 + 1);
 	i=j=0;
	for(i=0;i< account_size;)
	{
		account_title[j] = ToHex(account[i]) << 4 | ToHex(account[i+1]);
		j++;
		i = i+2;

	}
	account_title[j] = '\n';

	parse_file(&transaction_number,&transaction_number_size,9, 20);//'transaction_number: ' 

	transaction_number_title= malloc(transaction_number_size/2 + 1);
 	i=j=0;
	for(i=0;i< transaction_number_size;)
	{
		transaction_number_title[j] = ToHex(transaction_number[i]) << 4 | ToHex(transaction_number[i+1]);
		j++;
		i = i+2;

	}
	transaction_number_title[j] = '\n';

	parse_file(&money,&money_size,10, 7);//'money: ' 

	money_title= malloc(money_size/2 + 1);
 	i=j=0;
	for(i=0;i< money_size;)
	{
		money_title[j] = ToHex(money[i]) << 4 | ToHex(money[i+1]);
		j++;
		i = i+2;

	}
	money_title[j] = '\n';

	parse_file(&sign,&sign_size,11, 6);//'sign: ' 

	sign_title= malloc(sign_size/2 + 1);
 	i=j=0;
	for(i=0;i< sign_size;)
	{
		sign_title[j] = ToHex(sign[i]) << 4 | ToHex(sign[i+1]);
		j++;
		i = i+2;

	}
	sign_title[j] = '\n';

	parse_file(&agree,&agree_size,12, 7);//'agree: ' 

	agree_title= malloc(agree_size/2 + 1);
 	i=j=0;
	for(i=0;i< agree_size;)
	{
		agree_title[j] = ToHex(agree[i]) << 4 | ToHex(agree[i+1]);
		j++;
		i = i+2;

	}
	agree_title[j] = '\n';

        /* get system time */
        time(&td);
        ptr = localtime(&td);
        
	write(posfd,commpany_title,title_size/2+1);
        write(posfd,"\n",1);
        write(posfd,"\n",1);
	write(posfd,serial_title,serial_size/2+1);
	write(posfd,rt_detail->serial_number,strlen(rt_detail->serial_number)+1);
        write(posfd,"\n",1);
	write(posfd,ticket_title,ticket_size/2+1);
	write(posfd,rt_detail->out_trade_no,strlen(rt_detail->out_trade_no)+1);
        write(posfd,"\n",1);

        /* compose date */
	write(posfd,date_title,date_size/2+1);
        strftime(pos_time,sizeof(pos_time),"%Y-%m-%d",ptr);
        write(posfd,pos_time,strlen(pos_time));
        write(posfd,"\n",1);

        /* compose time */
	write(posfd,time_title,time_size/2+1);
        strftime(pos_time,sizeof(pos_time),"%H:%M:%S",ptr);
        write(posfd,pos_time,strlen(pos_time));
        write(posfd,"\n",1);

write(posfd,line_title,line_size/2+1);

write(posfd,transaction_title,transaction_size/2+1);
write(posfd,account_title,account_size/2+1);
write(posfd,transaction_number_title,transaction_number_size/2+1);
write(posfd,rt_detail->trade_no,strlen(rt_detail->trade_no)+1);
write(posfd,"\n",1);
write(posfd,money_title,money_size/2+1);
write(posfd,rt_detail->total_fee,strlen(rt_detail->total_fee)+1);
write(posfd,"\n",1);
write(posfd,sign_title,sign_size/2+1);
write(posfd,agree_title,agree_size/2+1);
	write(posfd,'\n',1);

	write(posfd, cutCommand,sizeof(cutCommand));

	free(agree_title);
	free(agree);

	free(sign_title);
	free(sign);

	free(money_title);
	free(money);

	free(transaction_number_title);
	free(transaction_number);

	free(account_title);
	free(account);

	free(transaction_title);
	free(transaction);

	free(line_title);
	free(line);

	free(date_title);
	free(date);

	free(time_title);
	free(config_time);

	free(ticket_title);
	free(ticket_number);

        free(serial_title);
        free(serial_number);

	free(commpany_title);
	free(title);
}
