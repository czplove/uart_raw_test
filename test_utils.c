#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "test_utils.h"

#define HELP_CMD  "help"
#define AT_CMD  "AT"
#define ATAUTO_CMD  "ATAUTO"

static tsSL_Msg_Status gTxMsgStatus;
static pthread_mutex_t gSeialMsgSendMutex;
static pthread_mutex_t gTxAckListMutex;
static pthread_mutex_t gDevsListMutex;



int verbosity = 11; 
int AUTO_SEND_FLAG = 0;

static pthread_t gATAUTO_SendTh;

/***************************************************************************/
static void show_help(void)
{
    int i;

    printf("\t*** test cmd usage ***\n");
    //-for (i = 0; i < (sizeof(gaInputCmdList)/sizeof(tsInputCmd)); i ++)
    //-    printf("\t%s\n\t  %s\n\n", gaInputCmdList[i].pCmdStr, gaInputCmdList[i].pCmdTipStr);

    printf("\n");
}


teSL_Status eSL_SendMessage(uint16_t u16Type, uint16_t u16Length, void *pvMessage, uint8_t *pu8SequenceNo)
{
    teSL_Status eStatus;
    int ret;

    pthread_mutex_lock(&gSeialMsgSendMutex);	//-上锁,如果已锁将阻塞等待
    //-clear_txmsg_status();	//-清除发送标志位的状态
    eStatus = eSL_WriteMessage(u16Type, u16Length, (uint8_t *)pvMessage);
    if (eStatus == E_SL_OK)
    {
        //-ret = check_txmsg_status(u16Type, 100);	//-检查发送标志位的状态,确保对方已经收到,通过延时等待实现的,另一个线程在接收处理
        //-if (ret == 0)
        {//-等到了需要的状态,即正确的
            // get the zcl seq num
            //-if(pu8SequenceNo)
            //-    *pu8SequenceNo = gTxMsgStatus.u8SequenceNo;

            //-eStatus = E_SL_OK;
        }
        //-else
        {
            //-eStatus = E_SL_ERROR;
        }
    }
    else
    {
        printf("\n\t eSL_WriteMessage fail (%d)", eStatus);        
    } 

    pthread_mutex_unlock(&gSeialMsgSendMutex);
    return eStatus;
}



// return 1 if quit command is received, otherwise return 0
int input_cmd_handler(void)
{
    #define MAX_CMD_LEN 16 // max length of cmd string, not including the following para seperated by space
    int i, j, cmd_start_flag, cmd_end_flag, cmd_para_start_index;
    int ret;
    char inputBuf[256];
    char cmd_str[MAX_CMD_LEN];

    // read cmd from standard input
    memset(inputBuf, 0, sizeof(inputBuf));
    memset(cmd_str, 0, sizeof(cmd_str));
    i = 0;
    j = 0;
    cmd_start_flag = 0;
    cmd_end_flag = 0;
    cmd_para_start_index = 0;
    while(1)	//-阻塞等待命令,直到一个命令成功接收后返回
    {
    	//-当程序调用getchar时.程序就等着用户按键.用户输入的字符被存放在键盘缓冲
    	//-区中.直到用户按回车为止（回车字符也放在缓冲区中）.当用户键入回车之后，
    	//-getchar才开始从stdio流中每次读入一个字符.getchar函数的返回值是用户输
    	//-入的字符的ASCII码，如出错返回-1，且将用户输入的字符回显到屏幕.如用户在
    	//-按回车之前输入了不止一个字符，其他字符会保留在键盘缓存区中，等待后续
    	//-getchar调用读取.也就是说，后续的getchar调用不会等待用户按键，而直接读
    	//-取缓冲区中的字符，直到缓冲区中的字符读完为后，才等待用户按键.      
      inputBuf[i] = getchar();	//-从stdio流中读字符，相当于getc(stdin），它从标准输入里读取下一个字符。
	  if(inputBuf[i] != ' ')
          cmd_start_flag = 1;
      else
      {
          if(cmd_start_flag)
          {
              if(!cmd_end_flag)
                  cmd_para_start_index = i + 1;

              cmd_end_flag =1;               
          }
      }

      if((cmd_start_flag) && (! cmd_end_flag) && (j < MAX_CMD_LEN))
      {
          if ((inputBuf[i] != ' ') && (inputBuf[i] != '\n'))
              cmd_str[j++] = inputBuf[i];
      }
      
      if(inputBuf[i] == '\n')
      {
          inputBuf[i] = 0;
          break;
      }
      
      i++;
    }

    // check input command and do accordingly
    printf("\ncmd=%s, len=%d, para index=%d, \n", cmd_str, strlen(cmd_str), cmd_para_start_index);
    if (strcmp(cmd_str, HELP_CMD) == 0)
    {
        show_help();
    }
    else if (strcmp(cmd_str, AT_CMD) == 0)
    {
        // send AT cmd to L218
        //-eSL_SendMessage(E_SL_MSG_RESET, 2, "AT", NULL);
        eSL_SendMessage(E_SL_MSG_RESET, i - cmd_para_start_index, &inputBuf[cmd_para_start_index], NULL);
    }
    else if (strcmp(cmd_str, ATAUTO_CMD) == 0)
    {
        ret = pthread_create(&gATAUTO_SendTh, NULL, pvATAutoSendThread, NULL);	//-这里引入了一个线程,是一个重要的编程思想.
	    if (ret != 0)
	    {
	        printf("Create serial reader thread failed !\n");
	        return -2;
	    }
    }
    else if (strcmp(cmd_str, "") != 0)
    {
        printf("\tUnkown cmd.\n");
    }

    return 0;
}

void *pvSerialReaderThread(void *p)	//-一个全新的线程处理函数
{
    tsSL_Message  sMessage;
    tsSL_Msg_Status *psMsgStatus;

    while (1)	//-周期性的读数据,直到有为止
    {
        /* Initialise buffer */
        memset(&sMessage, 0, sizeof(tsSL_Message));
        /* Initialise length to large value so CRC is skipped if end received */
        sMessage.u16Length = 0xFFFF;

        //-printf("\n\tread date start.\n");

        if (eSL_ReadMessage(&sMessage.u16Type, &sMessage.u16Length, SL_MAX_MESSAGE_LENGTH, sMessage.au8Message) == E_SL_OK)
        {//-下面的处理是针对接收到的完好正确报文进行的
            
            if (verbosity >= 10)	//-调试信息的输出控制
            {
                char acBuffer[4096];
                int iPosition = 0, i;
                
                iPosition = sprintf(&acBuffer[iPosition], "L218->PC 0x%04X (Length % 4d)", sMessage.u16Type, sMessage.u16Length);
                for (i = 0; i < sMessage.u16Length; i++)
                {
                    //-iPosition += sprintf(&acBuffer[iPosition], " 0x%02X", sMessage.au8Message[i]);
                    iPosition += sprintf(&acBuffer[iPosition], "%c", sMessage.au8Message[i]);
                }
                printf( "\n\t%s", acBuffer);	//-\t 的意思是 横向跳到下一制表符位置
            }

            //-printf("\n\tread date.\n");
            
            fflush(stdout);
        }
        else
        {
            printf("\n\t***eSL_ReadMessage failed.\n");
        }


    }

    printf("Exit reader thread\n");

	return NULL;
}

void *pvATAutoSendThread(void *p)	//-一个全新的线程处理函数
{
    char inputBuf[1024];
    int i;

    for(i=0;i<1024;i++)
        inputBuf[i] = ' ';

    while (1)	//-周期性的读数据,直到有为止
    {
        AUTO_SEND_FLAG = 0;
        eSL_SendMessage(E_SL_MSG_RESET, 14, "AT+CIPSEND=900", NULL);
        
        i = 0;
        while(AUTO_SEND_FLAG == 0)
        {
            inputBuf[i] = i;
            i++;
            if(i > 1023)
                i = 0;
        }
        eSL_SendMessage(E_SL_MSG_RESET, 900, &inputBuf[0], NULL);


        //-printf("send auto thread\n");
        IOT_SLEEP(5);
    }

    printf("Exit reader thread\n");
    
    return NULL;
}






