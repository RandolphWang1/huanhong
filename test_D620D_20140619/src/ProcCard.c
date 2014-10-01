#include "Main.h" 
#include <sys/wait.h>

#define SIGUSER1 10
#define SIGUSER2 12

extern int iM1Handle;  //射频卡模块句柄

extern int debugSignal;


//USER1响应函数
void user1_exit(int num)
{
    printf("Normal exiting!\n");
//    alarm(0); 
    MagClose();
   if (gTerm.bIsDC == TRUE)   
    	dc_exit(iM1Handle);
   else if(gTerm.bIsDC == FALSE)
	FM17dcExit(iM1Handle);
    OkBeep();
    exit(0);
}

//USER2响应函数
void user2_exit(int num)
{
    printf("ESC exiting!\n");
    
//    alarm(0); 
    MagClose();
   if (gTerm.bIsDC == TRUE)   
    	dc_exit(iM1Handle);
   else if(gTerm.bIsDC == FALSE)
	FM17dcExit(iM1Handle);
    FailBeep();
    exit(-1);
}
 
void timeout_exit(int num)
{
    printf("Timeout exiting!\n");
//    alarm(0);
    MagClose();
   if (gTerm.bIsDC == TRUE)   
    	dc_exit(iM1Handle);
   else if(gTerm.bIsDC == FALSE)
	FM17dcExit(iM1Handle);
    FailBeep();
    exit(-2);
}

int Magic_proc()
{
    int  fp;
    char szBuff[256], szTmp[10];
    int  nSize, i;
    int  spid, pid, nRet;  
    char Track1[200], Track2[200], Track3[200]; 
    int  ErrorFlag;
    
    for (i = 5; i <= 29; i ++)
        signal(i, SIG_IGN); 
                
    signal(SIGUSER1, user1_exit);
    signal(SIGUSER2, user2_exit);
    signal(SIGALRM, timeout_exit);
    
    Wait(800);
    alarm(44);
    
    fp = open("Pid.dat",O_RDWR); 
    memset(szBuff, 0, sizeof(szBuff));   
    nSize = read(fp, szBuff, 12); 
    close(fp); 
    if (nSize != 12)  
    {
        printf("read pid file error!\n");
        kill(0, SIGUSER2);
        return(ERROR);
    } 
    
    memcpy(szTmp, szBuff+6, 6);
    szTmp[6] = 0x00;
    pid = atoi(szTmp);      //父进程号
    
    szBuff[6] = 0x00;
    spid = atoi(szBuff);    //子进程号
         
    if (pid != getpid())
    {
        printf("[%d]I'm not your father [%d]!\n", getpid(), pid);
        printf("I'll kill you !\n");
        kill(spid, SIGUSER2);
        Wait(20);
        kill(pid, SIGUSER2);
        Wait(20);
         
        printf("I'm exiting !\n");
        exit(-1);
    }
    
    /* 磁卡读卡 */
    if (MagOpen() != 0)
	{
		printf("open mgc error\n");
		Clear();
		TextOut(6, 4, ALIGN_CENTER, "打开磁头失败!");
		kill(spid, SIGUSER2);
        exit(-1);  
	} 

    printf("Mag Processing ... [%d]\n", getpid());
    while (1)
    { 
    	memset(Track1,0x00,sizeof(Track1));
    	memset(Track2,0x00,sizeof(Track2));
    	memset(Track3,0x00,sizeof(Track3));
	    ErrorFlag = 0;
	    
    	//nRet = MagRead(Track1, Track2, Track3);
        if(nRet != -1)
        { 
        	Beep();
    	    if (nRet & TRACK1_OK)   //有第一磁道
    	    { 
    	        ErrorFlag = 1;
    	        PrintLog("一磁道数据:", Track1);
    	    }
        	if (nRet & TRACK2_OK)
        	{ 
        	    ErrorFlag = 2;
    	        PrintLog("二磁道数据:", Track2);
        	}
        	if (nRet & TRACK3_OK)
        	{ 
        	    ErrorFlag = 3;
    	        PrintLog("三磁道数据:", Track3);
        	}  
        	break;
        }
    }
    ////////////////////////////////////////////// 
    fp = open("Pan.dat",O_WRONLY|O_CREAT);
    if (fp < 0)
    {
        printf("son failed to open file\n");
        kill(spid, SIGUSER1);
        MagClose();
        exit(-1);
    }
    
    //文件中存储结构：卡类型（10Byte）
    memset(szBuff, 0, sizeof(szBuff)); 
    memcpy(szBuff, "MagicCard ", 10); 
    if (ErrorFlag == 0)
    { 
        memcpy(szBuff, "MagicError", 10); 
    }
    nSize = write(fp, szBuff, 10);
    close(fp);
    Wait(100);
      
    kill(spid, SIGUSER1);
    
    printf("magic proc end!\n");
    exit(0);
}

int Mifare_proc()
{
	int     i,nRet; 
    	UCHAR   ucSector;   //扇区号
    	UCHAR   ucModeA;    //KeyA方式  
    	int     nCom;       //串口号
    	long    lBaud;      //波特率
    	int     tagtype; 
    	UCHAR   strSize[5];
    	unsigned long   cardsnr = 0;  
    	int     fp;  
    	char szBuff[256] ;
    	int  nSize;
    	int  pid, ppid;
    	char cKey;
    	UCHAR   szKeyA[10]= {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00}; 
      
    //记录进程号
    	pid = getpid();
    	ppid = getppid(); 
    	fp = open("Pid.dat",O_WRONLY|O_CREAT); 
    	memset(szBuff, 0, sizeof(szBuff));
    	sprintf(szBuff, "%06d%06d", pid, ppid);     
    	write(fp, szBuff, 12);
    	close(fp); 
            
    	for (i = 5; i <= 29; i ++)
        	signal(i, SIG_IGN);
     
    	signal(SIGUSER1, user1_exit);
    	signal(SIGUSER2, user2_exit);
    	signal(SIGALRM, timeout_exit);
    
    	alarm(45);
	nCom = 2;         
    	lBaud = 115200;   
	if(OK != FM17dcInit(nCom, 19200))
     	{
                gTerm.bIsDC = FALSE;
     	}
	else
	{
     		FM17dcExit(iM1Handle);
                gTerm.bIsDC = TRUE;
	}
    	//初始化射频模块串口
	if (gTerm.bIsDC == TRUE)
	{
    		iM1Handle = dc_init(nCom, lBaud);
    		if (iM1Handle < 0)
    		{
        		Clear();
        		FailBeep();
        		PutsXY(0, 3, "初始化D8失败!");
        		WaitKey(2000);

        		kill(ppid, SIGUSER2);
        		exit(-1);
		}
    	}
    	printf("init OK [%d]\n", pid);
 
	/* M1 IC卡读卡 */ 
	ClearKbd();
	while (1)
	{
        	if(KbdHit() == TRUE)  //判断是否有按键 
	    	{			        
	        	cKey = GetKey();							 
            		if( cKey == KEY_CANCEL )
            		{ 
				if (gTerm.bIsDC == TRUE)
				{
        				dc_reset(iM1Handle, 0); 
                			printf("mifare proc ESC termed!\n");
                			dc_exit(iM1Handle);
				}
				else
				{
					FM17dcReset(iM1Handle, 0);
                			printf("mifare proc ESC termed!\n");
					FM17dcExit(iM1Handle);
				}
                		kill(ppid, SIGUSER2);
                		exit(-1);
            		} 
	    	}  
	    
                if (gTerm.bIsDC == FALSE)
                {
                        if (OK != FM17dcReset(iM1Handle, 0))
                        {
                                printf("FM17Reset -1!\n");
                                continue;
                        }
    
        		if (FM17dcRequest(iM1Handle, 0, &tagtype) != 0) 
        		{
            			printf("dc_request error!\n"); 
    	    			Wait(300);
            			continue;
        		}
    			//        printf("dc_request OK!\n"); 
          
        		if (FM17dcAnticoll(iM1Handle, 0, &cardsnr) != 0)
        		{
            			printf("dc_anticoll error!\n"); 
    	    			Wait(200);  
            			dc_exit(iM1Handle);
    	    			WaitKey(2000);  
            			FM17dcInit(nCom, 19200);
            			continue;
        		}
    			//        printf("dc_anticoll OK!\n"); 
          
        		if (FM17dcSelect(iM1Handle, cardsnr, strSize) != 0)
        		{
            			printf("dc_select error!\n"); 
            			continue;
        		}
            		ucModeA = 0; 
            		ucSector = 1; 
            		if (FM17Loadkey(iM1Handle, ucModeA, ucSector, szKeyA) != 0)
            		{
                		Clear();
                		FailBeep();
                		PutsXY(0, 6, "装载密钥失败!");
               
               			kill(ppid, SIGUSER2); 
               			WaitKey(2000);
               			exit(-1);
            		} 
            		printf("Load_key  OK\n");
        
            		if (FM17dcAuth(iM1Handle, ucModeA, ucSector) != 0) 
            		{
               			Clear();
               			FailBeep();
               			PutsXY(0, 6, "卡片认证失败!");
               			printf("dc_authentication error!\n");
               			kill(ppid, SIGUSER2);
               			WaitKey(2000);
               			exit(-1);
            		}    
           
          		memset(szBuff, 0, sizeof(szBuff));
            		nRet = FM17dcRead(iM1Handle, 4, szBuff);
            		if (nRet != 0)
            		{
               			FailBeep();
                		PutsXY(0, 6, "ReadCard 4失败!");
                		kill(ppid, SIGUSER2);
                		WaitKey(2000);
                		exit(-1);
            		}    
            		PrintLog("非接m1卡数据:", szBuff); 
            
            		FM17dcHalt(iM1Handle);
            		FM17dcReset(iM1Handle, 0); 
                }else
		{

        		if (dc_reset(iM1Handle, 0) != 0) 
        		{
            			printf("dc_reset error [ %d]!\n", getpid()); 
    	    			Wait(200);
            			continue;
        		}
    			//        printf("dc_reset OK!\n"); 
    
        		if (dc_request(iM1Handle, 0, &tagtype) != 0) 
        		{
            			printf("dc_request error!\n"); 
    	    			Wait(300);
            			continue;
        		}
    			//        printf("dc_request OK!\n"); 
          
        		if (dc_anticoll(iM1Handle, 0, &cardsnr) != 0)
        		{
            			printf("dc_anticoll error!\n"); 
    	    			Wait(200);  
            			dc_exit(iM1Handle);
    	    			WaitKey(2000);  
            			dc_init(nCom, lBaud);
            			continue;
        		}
    			//        printf("dc_anticoll OK!\n"); 
          
        		if (dc_select(iM1Handle, cardsnr, strSize) != 0)
        		{
            			printf("dc_select error!\n"); 
            			continue;
        		}
            		ucModeA = 0; 
            		ucSector = 1; 
            		if (dc_load_key(iM1Handle, ucModeA, ucSector, szKeyA) != 0)
            		{
                		Clear();
                		FailBeep();
                		PutsXY(0, 6, "装载密钥失败!");
               
               			kill(ppid, SIGUSER2); 
               			WaitKey(2000);
               			exit(-1);
            		} 
            		printf("Load_key  OK\n");
        
            		if (dc_authentication(iM1Handle, ucModeA, ucSector) != 0) 
            		{
               			Clear();
               			FailBeep();
               			PutsXY(0, 6, "卡片认证失败!");
               			printf("dc_authentication error!\n");
               			kill(ppid, SIGUSER2);
               			WaitKey(2000);
               			exit(-1);
            		}    
           
          		memset(szBuff, 0, sizeof(szBuff));
            		nRet = dc_read(iM1Handle, 4, szBuff);
            		if (nRet != 0)
            		{
               			FailBeep();
                		PutsXY(0, 6, "ReadCard 4失败!");
                		kill(ppid, SIGUSER2);
                		WaitKey(2000);
                		exit(-1);
            		}    
            		PrintLog("非接m1卡数据:", szBuff); 
            
            		dc_halt(iM1Handle);
            		dc_reset(iM1Handle, 0); 
            		dc_exit(iM1Handle);
           
          		OkBeep();
           		break;
        	}  
    	}
    
    ////////////////////////////////////////////////// 
    fp = open("Pan.dat",O_WRONLY|O_CREAT);
    if (fp < 0)
    {
        printf("son failed to open file\n");
        kill(ppid, SIGUSER2);
        exit(-1);
    }  
    
    //文件中存储结构：卡类型（10Byte）
    memset(szBuff, 0x00, sizeof(szBuff)); 
    memcpy(szBuff, "RFDCard   ", 10);  
    nSize = write(fp, szBuff, 10); 
    close(fp); 

    Wait(100);
    
    kill(ppid, SIGUSER1);
        
    printf("Mifare proc end!\n");
    exit(0);
}

int ReadCard()
{
    int  pid, nPid, spid;
    int  fp;
    char szBuff[256], szTmp[50];
    int  ret, nSize;
    int  i, stat; 
       
	ClearKbd();
	
	setpgrp();
    nPid = fork();
    switch (nPid)
    {
    case -1:    /* fork失败 */
        printf("fork error!\n");
        return(ERROR);
        
    case 0:     /* 子进程 */  
        pid = fork();
        switch (pid)
        {
            case -1:
                printf("Son fork error!\n");
                return(ERROR);
                
            case 0: /* 孙进程 */ 
                Mifare_proc();  
                break;
                
            default: /* 子进程 */ 
                Magic_proc();  
                break;
        }
        break;
        
    default:    /* 父进程 */   
        for (i = 5; i <= 16; i ++)
            signal(i, SIG_IGN);
        
        for (i = 18; i <= 29; i ++)
            signal(i, SIG_IGN); 
        
        //多进程处理中不能使用fopen、fread、fwrite等函数
        fp = open("Pan.dat",O_WRONLY|O_CREAT); 
        
        memset(szBuff, 0, sizeof(szBuff));
        write(fp, szBuff, 188);
        close(fp); 
        
        ret = waitpid(0, &stat, 0);
        printf("stat=[%d]; pid=[%d]\n", WEXITSTATUS(stat), ret);
         
//            printf("%s\n", system("ps"));   //debugshang
        
        //确保结束全部子进程
        fp = open("Pid.dat",O_RDWR); 
        memset(szBuff, 0, sizeof(szBuff));   
        nSize = read(fp, szBuff, 12); 
        close(fp);
        if (nSize != 12)  
        {
            printf("read pid file error!\n"); 
            return(ERROR);
        } 
        
        memset(szTmp, 0, sizeof(szTmp));
        memcpy(szTmp, szBuff+6, 6); 
        pid = atoi(szTmp);      //子进程号
        
        memset(szTmp, 0, sizeof(szTmp));
        memcpy(szTmp, szBuff, 6); 
        spid = atoi(szTmp);    //孙进程号
        
        if (pid != 0)
        {
            printf("kill pid ![%d]\n", pid); 
            kill(pid, SIGKILL);
        }
        if (spid != 0)
        {
            printf("kill spid ![%d]\n", spid); 
            kill(pid, SIGKILL);
        }
        
        fp = open("Pid.dat",O_RDWR); 
        memset(szBuff, 0, sizeof(szBuff));
        write(fp, szBuff, 30);
        close(fp);  
        //End
        
        if (ret > 0)
        {
            //WEXITSTATUS(stat) :取子进程传送给exit、_exit参数的低8位
            if (WEXITSTATUS(stat) != 0) 
            {
                printf("PROCSTATUS Error!\n");
                kill(0, SIGUSER2);
                return(ERROR); 
            }
        }
        else
        {
            printf("get pan err!\n");
            
            kill(0, SIGUSER2);
            return(ERROR); 
        }
        
        ////////////////////////////////////////////////////////////
        fp = open("Pan.dat",O_RDWR);
        if (fp < 0)
        {
            printf("Open file Failed!\n");
            kill(0, SIGUSER2);
            return(ERROR);
        }  
        
        //文件中存储结构：卡类型（10Byte）
        memset(szBuff, 0, sizeof(szBuff)); 
        nSize = read(fp, szBuff, 10); 
        close(fp);
        if ((nSize != 10) || 
            (memcmp(szBuff, "\x00\x00\x00\x00\x00\x00\x00\x00", 8) == 0))
        {
            printf("read file error!\n");
            printf("nSize:[%d]\n", nSize);
            kill(0, SIGUSER2);
            return(ERROR);
        }
          
        fp = open("Pan.dat",O_RDWR);
        memset(szTmp, 0, sizeof(szTmp));
        write(fp, szTmp, 188);
        close(fp); 
         
        kill(0, SIGUSER1);
        
        if (memcmp(szBuff, "MagicCard", 9) == 0)
        {
            ClearLine(1, 9);
            PutsXY(0, 3, "   读到磁条卡数据!");
            WaitKey(2000); 
            return(OK); 
        } 
        else if (memcmp(szBuff, "RFDCard", 7) == 0)
        {
            ClearLine(1, 9);
            PutsXY(0, 3, "   读到非接卡数据!");
            WaitKey(2000); 
            return(OK); 
        }
        else 
        {
            printf("tttt:%s\n", szBuff);
            ClearLine(1, 9);
            PutsXY(0, 3, "     读卡失败!");
            WaitKey(2000);  
        }
    } 
    printf("ReadCard Return ERROR!\n");
    return(ERROR);
} 
/****************************************************************/
/* End of this file */
 
/*
 * 非接触IC卡处理函数
 */
void RficTest(void)
{
 	int     nRet;
 	UCHAR   ucKey;
 	UCHAR   ucSector;   //扇区号
 	UCHAR   ucModeA;    //KeyA方式
 	UCHAR   szBuff[128];
 	UCHAR   buff[1024];
 	int     nCom;  
 	int     j;     //串口号
 	long    lBaud;      //波特率
 	int     tagtype;
 	UCHAR   strSize[5];
 	ULONG   cardsnr = 0;
  	//  UCHAR   szKeyA[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	 char szKeyA[12+1];
	//密钥转化
	 memset(szKeyA,0,sizeof(szKeyA));
 	printf("szKeyA[%s]\n",gTerm.szM1KeyA);
	AscToBcd(szKeyA,gTerm.szM1KeyA, 12, 0);
	printf_x("M1:",szKeyA, strlen(szKeyA));

	SwitchCom2_Rf(1);
    	nCom = 2;
    	lBaud = 115200;
    	ucModeA = 0;
    	ucSector = 1;

    	printf("use com2b - com%d\n", nCom);

	if(-1 == FM17dcInit(nCom, 19200))
	{
     		FM17dcExit(iM1Handle);
                gTerm.bIsDC = TRUE;
	}else if(OK != FM17dcInit(nCom, 19200))
     {
        gTerm.bIsDC = FALSE;
    	printf("FM17 init OK\n");
    	printf("FM17Loadkey---------------%s\n", gTerm.szM1KeyA);
	    //装载密钥A
	    if(FM17Loadkey(iM1Handle, ucModeA, ucSector, gTerm.szM1KeyA) != 0)
	    {
	      Clear();
	      FailBeep();
	      PutsXY(0, 3, "装载密钥失败!");
	      PrintLog("非接触M1卡测试:", "ERROR装载密钥错误");
	      return;
	    }
	    printf("FM17 Load_key OK\n");
     }
	
    	//初始化射频模块串口
	if (gTerm.bIsDC == TRUE)
	{
			
    		iM1Handle = dc_init(nCom, lBaud);
    		if(iM1Handle < 0)
    		{
        		Clear();
        		FailBeep();
        		printf("iM1Handle = [%d] \n", iM1Handle);
        		PutsXY(0, 3, "初始化模块失败!");
        		PrintLog("非接触M1卡测试:", "ERROR打开设备错误");
        		WaitKey(2000);
        		return;
		}
    		printf("init OK\n");

	    	//装载密钥A
	    	if(dc_load_key(iM1Handle, ucModeA, ucSector, szKeyA) != 0)
	    	{
	       	 	Clear();
	        	FailBeep();
	        	PutsXY(0, 3, "装载密钥失败!");
	                PrintLog("非接触M1卡测试:", "ERROR装载密钥错误");
	        	return;
	    	}
	    	printf("Load_key OK\n");

    	}

    	ClearLine(1, 9);
    	PutsXY(0, 1, "      非接测试      ");
    	PutsXY(0, 3, "      请靠近IC卡");
    	PutsXY(0, 9, "     按取消键退出");
//	ClearKbd();
    	while (TRUE)
    	{
        	PutsXY(0, 5, "                    ");

		if(TRUE == KbdHit())
    	    	{
    	        	ucKey = GetKey();
    	        	if(ucKey == KEY_CANCEL)
			{
				if (gTerm.bIsDC == TRUE)
        				dc_reset(iM1Handle, 0);
				else if (gTerm.bIsDC == FALSE)
					FM17dcReset(iM1Handle, 0);
    	            		return ;
			}
    	    	}
    	    
		
        	Wait(300);
		if (gTerm.bIsDC == TRUE)
		{
        		//复位射频卡模块
        		if(dc_reset(iM1Handle, 0) != 0)
        		{
            			printf("dc_reset error!\n");
            			continue;
        		}

	        	//寻卡
        		if(dc_request(iM1Handle, 0, &tagtype) != 0)
        		{
            			printf("dc_request error!\n");
            			continue;
        		}
			printf("tagtype:%d\n", tagtype);
        		//防冲突
        		if(dc_anticoll(iM1Handle, 0, &cardsnr) != 0)
        		{
            			printf("dc_anticoll error!\n");
            			continue;
        		}
			//printf("cardsnr:%d\n", cardsnr);

        		//选取一张卡
	        	if(dc_select(iM1Handle, cardsnr, strSize) != 0)
       		 	{
            			printf("dc_select error!\n");
            			continue;
        		}
			printf("strSize:%02x\n", strSize[0]);
			printf("strSize:%02x\n", strSize[1]);
			printf("strSize:%02x\n", strSize[2]);
			printf("strSize:%02x\n", strSize[3]);
			printf("strSize:%02x\n", strSize[4]);
	
       		 	//认证密钥A
       		 	if(dc_authentication(iM1Handle, ucModeA, ucSector) != 0)
	        	{
        	    		Clear();
            			FailBeep();
            			PutsXY(0, 3, "认证卡片密钥失败!");
            			printf("dc_authentication error!\n");
               		     	PrintLog("非接触M1卡测试:", "ERROR认证错误");
            			return ;
        		}
	        	printf("dc_authentication Ok!\n");

		}else if (gTerm.bIsDC == FALSE)
		{

      		  	//复位射频卡模块
        		if(OK != FM17dcReset(iM1Handle, 0))
        		{
            			printf("FM17dcReset error!\n");
            			continue;
        		}
            		printf("FM17dcReset OK\n");

        		//寻卡
	        	if(OK != FM17dcRequest(iM1Handle, 0, &tagtype))
       		 	{
            			printf("FM17dcRequest error!\n");
            			continue;
        		}
            		printf("FM17dcRequest OK!\n");
			printf("tagtype:%d\n", tagtype);
        		//防冲突
	        	if(OK != FM17dcAnticoll(iM1Handle, 0, &cardsnr))
       		 	{
            			printf("FM17dcAnticoll error!\n");
            			continue;
        		}
            		printf("FM17dcAnticoll OK!\n");
			//printf("cardsnr:%d\n", cardsnr);

        		//选取一张卡
	        	if(OK != FM17dcSelect(iM1Handle, cardsnr, strSize))
       		 	{
            			printf("FM17dcSelect error!\n");
            			continue;
        		}
            		printf("FM17dcSelect OK!\n");
			printf("strSize:%02x\n", strSize[0]);
			printf("strSize:%02x\n", strSize[1]);
			printf("strSize:%02x\n", strSize[2]);
			printf("strSize:%02x\n", strSize[3]);
			printf("strSize:%02x\n", strSize[4]);

			printf("ucSector = %d\n", ucSector);
        		//认证密钥A
        		if(OK != FM17dcAuth(iM1Handle, ucModeA, ucSector))
        		{
            			Clear();
            			FailBeep();
            			PutsXY(0, 3, "认证卡片密钥失败!");
            			printf("dc_authentication error!\n");
                    		PrintLog("非接触M1卡测试:", "ERROR认证错误");
            			return ;
        		}
	        	printf("FM17dcAuth Ok!\n");
		}
        	break;
    	}

	if (gTerm.bIsDC == TRUE)
	{
    		//读M1卡第1号扇区第一块数据（数据块绝对值为4）
    		memset(szBuff, 0, sizeof(szBuff));
    		nRet = dc_read(iM1Handle, 4, szBuff);
    		if(nRet != 0)
    		{
        		FailBeep();
        		PutsXY(0, 3, "ReadCard 4失败!");
         		PrintLog("非接触M1卡测试:", "ERROR读卡失败");
        		return ;
    		}

    		//中止对该卡操作
    		if(dc_halt(iM1Handle) != 0)
    		{
        		FailBeep();
        		printf("Halt Card Failed!\n");
    		}

    		if(dc_reset(iM1Handle, 0) != 0)
    		{
        		printf("dc_reset error!\n");
    		}
    			printf("Halt Card OK!\n");
    		dc_exit(iM1Handle);
	}else if (gTerm.bIsDC == FALSE)
	{
		//读M1卡第1号扇区第一块数据（数据块绝对值为4）
		j=4;
		memset(szBuff, 0, sizeof(szBuff));
		memset(buff, 0, sizeof(buff));

    	
    		nRet = FM17dcRead(iM1Handle, j, szBuff);
    		if(nRet != 0)
    		{
        		FailBeep();
        		PutsXY(0, 3, "ReadCard 4失败!");
         		PrintLog("非接触M1卡测试:", "ERROR读卡失败");
	        	printf("FM17dcRead error!\n");
        		return ;
    		}
  
			printf("Data:%s\n", szBuff);
        		
        		
	        printf("FM17dcRead Ok!\n");

    		//中止对该卡操作
    		if(OK != FM17dcHalt(iM1Handle))
    		{
        		FailBeep();
        		printf("FM17dcHalt Failed!\n");
    		}
	        printf("FM17dcHalt Ok!\n");

    		if(OK != FM17dcReset(iM1Handle, 0))
    		{
        		printf("FM17dcReset error!\n");
    		}
	        printf("FM17dcReset Ok!\n");
    		FM17dcExit(iM1Handle);

	}
    	OkBeep();
    	ClearLine(1, 9);
    	PutsXY(0, 3, "    读卡成功");
    	PutsXY(0, 4, buff);
    	PutsXY(0, 9, "按取消键退出");
    	printf("Data:%s\n", buff);
    	PrintLog("非接触M1卡测试:", szBuff);

    	return;
}

void RficWirte(void)
{

	 int     nRet;
    UCHAR   ucKey;
    UCHAR   ucSector;   //扇区号
    UCHAR   ucModeA;    //KeyA方式
    UCHAR   buff[128];
    int     nCom;       //串口号
    long    lBaud; //波特率
	int     tagtype;
    UCHAR   strSize[5];
    ULONG   cardsnr = 0;
   UCHAR   szKeyA[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	
	//密钥转化
   	memset(szKeyA,0,sizeof(szKeyA));
	printf("szKeyA[%s]\n",gTerm.szM1KeyA);
	AscToBcd(szKeyA,gTerm.szM1KeyA, 12, 0);
	printf_x("M1:",szKeyA, strlen(szKeyA));
	
	
	memset(buff,0,sizeof(buff));
	memcpy(buff,"1234567890",10);

	SwitchCom2_Rf(1);
    nCom = 2;
    lBaud = 115200;
    ucModeA = 0;
    ucSector = 1;

    	printf("use com2b - com%d\n", nCom);
	if(-1 == FM17dcInit(nCom, 19200))
	{
     		FM17dcExit(iM1Handle);
                gTerm.bIsDC = TRUE;
	}else if(OK != FM17dcInit(nCom, 19200))
     	{
                gTerm.bIsDC = FALSE;
    		printf("FM17 init OK\n");
    		printf("FM17Loadkey---------------%s\n", gTerm.szM1KeyA);
	    	//装载密钥A
	    	if(FM17Loadkey(iM1Handle, ucModeA, ucSector, gTerm.szM1KeyA) != 0)
	    	{
	       	 	Clear();
	        	FailBeep();
	        	PutsXY(0, 3, "装载密钥失败!");
	                PrintLog("非接触M1卡测试:", "ERROR装载密钥错误");
	        	return;
	    	}
	    	printf("FM17 Load_key OK\n");
     	}
	
    	//初始化射频模块串口
	if (gTerm.bIsDC == TRUE)
	{
    		iM1Handle = dc_init(nCom, lBaud);
    		if(iM1Handle < 0)
    		{
        		Clear();
        		FailBeep();
        		printf("iM1Handle = [%d] \n", iM1Handle);
        		PutsXY(0, 3, "初始化模块失败!");
        		PrintLog("非接触M1卡测试:", "ERROR打开设备错误");
        		WaitKey(2000);
        		return;
		}
    		printf("init OK\n");

	    	//装载密钥A
	    	if(dc_load_key(iM1Handle, ucModeA, ucSector, szKeyA) != 0)
	    	{
	       	 	Clear();
	        	FailBeep();
	        	PutsXY(0, 3, "装载密钥失败!");
	                PrintLog("非接触M1卡测试:", "ERROR装载密钥错误");
	        	return;
	    	}
	    	printf("Load_key OK\n");

    	}
    	ClearLine(1, 9);
    	PutsXY(0, 1, "      非接测试      ");
    	PutsXY(0, 3, "      请靠近IC卡");
    	PutsXY(0, 9, "     按取消键退出");
		
    	while (TRUE)
    	{
        	PutsXY(0, 5, "                    ");

		if(TRUE == KbdHit())
    	 {
    	        	ucKey = GetKey();
    	        	if(ucKey == KEY_CANCEL)
			{
				if (gTerm.bIsDC == TRUE)
        				dc_reset(iM1Handle, 0);
				else if (gTerm.bIsDC == FALSE)
					FM17dcReset(iM1Handle, 0);
    	            		return ;
			}
    	  }
    	    
		
        	Wait(300);
		if (gTerm.bIsDC == TRUE)
		{
        		//复位射频卡模块
        		if(dc_reset(iM1Handle, 0) != 0)
        		{
            			printf("dc_reset error!\n");
            			continue;
        		}

	        	//寻卡
        		if(dc_request(iM1Handle, 0, &tagtype) != 0)
        		{
            			printf("dc_request error!\n");
            			continue;
        		}
			printf("tagtype:%d\n", tagtype);
        		//防冲突
        		if(dc_anticoll(iM1Handle, 0, &cardsnr) != 0)
        		{
            			printf("dc_anticoll error!\n");
            			continue;
        		}
        		//选取一张卡
	        	if(dc_select(iM1Handle, cardsnr, strSize) != 0)
       		 	{
            			printf("dc_select error!\n");
            			continue;
        		}
			printf("strSize:%02x\n", strSize[0]);
			printf("strSize:%02x\n", strSize[1]);
			printf("strSize:%02x\n", strSize[2]);
			printf("strSize:%02x\n", strSize[3]);
			printf("strSize:%02x\n", strSize[4]);
	
       		 	//认证密钥A
       		 	if(dc_authentication(iM1Handle, ucModeA, ucSector) != 0)
	        	{
        	    		Clear();
            			FailBeep();
            			PutsXY(0, 3, "认证卡片密钥失败!");
            			printf("dc_authentication error!\n");
               		     	PrintLog("非接触M1卡测试:", "ERROR认证错误");
            			return ;
        		}
	        	printf("dc_authentication Ok!\n");

		}else if (gTerm.bIsDC == FALSE)
		{

      		  	//复位射频卡模块
        		if(OK != FM17dcReset(iM1Handle, 0))
        		{
            			printf("FM17dcReset error!\n");
            			continue;
        		}
            		printf("FM17dcReset OK\n");

        		//寻卡
	        	if(OK != FM17dcRequest(iM1Handle, 0, &tagtype))
       		 	{
            			printf("FM17dcRequest error!\n");
            			continue;
        		}
            		printf("FM17dcRequest OK!\n");
			printf("tagtype:%d\n", tagtype);
        		//防冲突
	        	if(OK != FM17dcAnticoll(iM1Handle, 0, &cardsnr))
       		 	{
            			printf("FM17dcAnticoll error!\n");
            			continue;
        		}
            		printf("FM17dcAnticoll OK!\n");
			//printf("cardsnr:%d\n", cardsnr);

        		//选取一张卡
	        	if(OK != FM17dcSelect(iM1Handle, cardsnr, strSize))
       		 	{
            			printf("FM17dcSelect error!\n");
            			continue;
        		}
            		printf("FM17dcSelect OK!\n");

			printf("ucSector = %d\n", ucSector);
        		//认证密钥A
        		if(OK != FM17dcAuth(iM1Handle, ucModeA, ucSector))
        		{
            			Clear();
            			FailBeep();
            			PutsXY(0, 3, "认证卡片密钥失败!");
            			printf("dc_authentication error!\n");
                    		PrintLog("非接触M1卡测试:", "ERROR认证错误");
            			return ;
        		}
	        	printf("FM17dcAuth Ok!\n");
		}
        	break;
    	}

	if (gTerm.bIsDC == TRUE)
	{
    		//读M1卡第1号扇区第一块数据（数据块绝对值为4）
    		nRet = dc_write(iM1Handle, 4, buff);
    		if(nRet != 0)
    		{
        		FailBeep();
        		PutsXY(0, 3, "ReadCard 4失败!");
         		PrintLog("非接触M1卡测试:", "ERROR读卡失败");
        		return ;
    		}

    		//中止对该卡操作
    		if(dc_halt(iM1Handle) != 0)
    		{
        		FailBeep();
        		printf("Halt Card Failed!\n");
    		}

    		if(dc_reset(iM1Handle, 0) != 0)
    		{
        		printf("dc_reset error!\n");
    		}
    			printf("Halt Card OK!\n");
    		dc_exit(iM1Handle);
	}else if (gTerm.bIsDC == FALSE)
	{
		//读M1卡第1号扇区第一块数据（数据块绝对值为4）
		
		memset(buff, 0, sizeof(buff));
    		nRet = FM17dcWrite(iM1Handle, 4, buff);
    		if(nRet != 0)
    		{
        		FailBeep();
        		PutsXY(0, 3, "ReadCard 4失败!");
         		PrintLog("非接触M1卡测试:", "ERROR读卡失败");
	        	printf("FM17dcRead error!\n");
        		return ;
    		}
    		
	        printf("FM17dcRead Ok!\n");

    		//中止对该卡操作
    		if(OK != FM17dcHalt(iM1Handle))
    		{
        		FailBeep();
        		printf("FM17dcHalt Failed!\n");
    		}
	        printf("FM17dcHalt Ok!\n");

    		if(OK != FM17dcReset(iM1Handle, 0))
    		{
        		printf("FM17dcReset error!\n");
    		}
	        printf("FM17dcReset Ok!\n");
    		FM17dcExit(iM1Handle);

	}
    	OkBeep();
    	ClearLine(1, 9);
    	PutsXY(0, 3, "    写卡成功");
    	PutsXY(0, 4, buff);
    	PutsXY(0, 9, "按取消键退出");
    	printf("Data:%s\n", buff);
    	PrintLog("非接触M1卡测试:", buff);

		return ;
}
void SetKeyM1()
{
	int nRet = 0; 

	Clear();
	SetScrFont(FONT20, WHITE);
	TextOut(0, 3, ALIGN_LEFT, "  读写卡密钥:");
	SetScrFont(FONT20, BLUE);
	nRet = Input(8, 4, gTerm.szM1KeyA, 12, IME_NUMBER, BLACK, GREEN, FALSE, FALSE, TRUE);
	if(nRet != OK)
    	return;

	Clear();
	SetScrFont(FONT20, WHITE);
	TextOut(0, 3, ALIGN_CENTER, "密钥设置成功");
	WaitKey(2000);
}

int ReadCardNO()
{
    	int  nRet;//, nRet,num,block,m,n;
    	int  iM1Handle;
    	char szBuff[100];
    	unsigned char ucMode, ucSector;
    	char szPan[20];     //卡号 
    	char cKey;
	char tmp[20];
	int  MAX;
	char bcd[4+1]="";
	char buff[1024+1];
    	UCHAR   szKeyA[13]= "FFFFFFFFFFFF"; 
	
	ClearLine(1, 9);
	PutsXY(0, 2, "  按取消键返回");
	PutsXY(0, 3, "  确认键刷卡");
	
	cKey = WaitLimitKey("\x12\x0F",2,0);
	if(cKey == KEY_CANCEL)
		return -1;


	iM1Handle = 0;
	//if(cKey == KEY_1)
	//	return 1;

    //初始化射频模块
    	nRet = RficInit(&iM1Handle);
    	if (nRet != 0)
    	{
        	ClearLine(1, 9);
        	PutsXY(0, 3, "初始化m1失败");
        	//Inkey(2);
       		sleep(2);
        	return(-1);
    	}
    
    	//检测射频卡是否到位
    	nRet = RficDetect(iM1Handle);
    	if (nRet != 0)
        	return(-1);

    	//装载第1扇区KeyA密钥
    	ucMode = MODEKEYA;
    	ucSector = 2;  
	memset(buff,0,sizeof(buff));
	printf("buff[%s]\n",szKeyA);
	Ascii_To_Bcd(szKeyA,buff, 12);
	//AscToBcd(buff,szKeyA, 12, 0);
	//yd_printf_x("M1:",buff, strlen(buff));
	
    	if (dc_load_key(iM1Handle, ucMode, ucSector,buff) != 0)
    	{
		printf("装载[%d]扇区密钥失败\n", ucSector);
        	ClearLine(1, 9);
        	PutsXY(0, 3, "装载密钥失败!");
		Wait(1000);
        	return(-1);
    	}  

    	//读第2扇区第0块数据
    	memset(szBuff, 0, sizeof(szBuff));            
    	nRet = RficReadBlock(iM1Handle, 2, 8, szBuff);
    	if (nRet != 0)
    	{
        	ClearLine(1, 9);
        	PutsXY(0, 3, "读取08块数据失败!");
		Wait(2000);
        	return(-1);
    	}
    	memset(szPan, 0, sizeof(szPan));
    	memcpy(szPan, szBuff, 16);

    	memset(tmp,0x00,sizeof(tmp));
	switch(szPan[0])
	{
		case 0x30:
		case 0x31:
		case 0x32:
		case 0x33:
		case 0x34:
		case 0x35:
		case 0x36:
		case 0x37:
		case 0x38:
		case 0x39:
			MAX=8;
			break;
		default:
			MAX=4;
			break;
	}
	if(MAX==4)
	{
		printf("---1---\n");
		memcpy(bcd,szPan,16);
		Bcd_To_Ascii(bcd,tmp,16);
	}
	else
	{
		printf("---2---\n");
		memcpy(tmp,szPan,16);
	}
    	ClearLine(1,9);
    	TextOut(0, 3,ALIGN_CENTER,  "卡号:");
	TextOut(0, 4,ALIGN_CENTER,  tmp);
	TextOut(0, 9,ALIGN_CENTER,  " 【确定】/【取消】");
	cKey = WaitLimitKey("\x0F\x13\x14\x12",4,0);
    	if(cKey == KEY_CANCEL)
        	return -1;
	else
		return 0;
}





