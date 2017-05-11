#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "test_utils.h"




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

    return 0;
}

void *pvSerialReaderThread(void *p)	//-一个全新的线程处理函数
{

	return NULL;
}
