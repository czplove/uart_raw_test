#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <limits.h>
#include <time.h>
#include <pthread.h>
#include <getopt.h>
#include <stdint.h>

#define VER_INFO "1.3.3"

static char *pcSerialDevice = "/dev/ttyUSB0";
static uint32_t u32BaudRate = 115200;

static struct option long_options[] =
{
    {"help",		no_argument,		NULL, 'h'},
    {"serial",		required_argument,	NULL, 's'},
    {"baud",		required_argument,  NULL, 'B'},
    
    {"reqpin",		required_argument,  NULL, 'R'},
    {"statpin",		required_argument,  NULL, 'S'},
    {"grantpin",	required_argument,  NULL, 'G'},
    {"pollperiod",	required_argument,  NULL, 'P'},

    { NULL, 0, NULL, 0}
};

uint32_t		gu32RfActiveOutDioMask = 0x1; // DIO0
uint32_t		gu32StatusOutDioMask = 0x2;   // DIO1
uint32_t		gu32TxConfInDioMask = 0x800;  // DIO11
uint16_t		gu16PollPeriod = 0x3E; // unit of 62500 HZ clock

/*********************************************************************************/
static void print_usage_exit(char *argv[])
{
    printf("ZCB test Version: %s\n", VER_INFO);
    printf("Usage: %s\n", argv[0]);
    printf("  Arguments:\n");
    printf("    -h --help                              Print this help.\n");    
    printf("    -s --serial        <serial device> Serial device for ZCB module, e.g. /dev/ttyS0\n");
    printf("    -B --baud          <baud rate>     Baud rate to communicate with ZCB at. Default %d\n", u32BaudRate);
    printf("    -R --reqpin        <DIO mask>      DIO pin msk for RF_request, hex fmt start with 0x. Default 0x%X\n", gu32RfActiveOutDioMask);
    printf("    -S --statpin       <DIO mask>      DIO pin msk for status/priority, hex fmt start with 0x. Default 0x%X\n", gu32StatusOutDioMask);
    printf("    -G --grantpin      <DIO mask>      DIO pin msk for grant, hex fmt start with 0x. Default 0x%X\n", gu32TxConfInDioMask);
    printf("    -P --pollperiod    <DIO mask>      Poll period of ZCB timer, in unit of 62500HZ clk, hex fmt start with 0x Default 0x%X\n", gu16PollPeriod);
    exit(-1);
}

int main(int argc, char *argv[])
{
	int ret, i32tmp;
	
	// get cmd line paras
	int8_t opt;
	int option_index;
	//-printf("HelloWorld!\n");

	//-和getopt相比即实现了功能,也增加了自己的功能,比如增加了长参数,可以更形象的命令含义
	while ((opt = getopt_long(argc, argv, "hs:B:R:S:G:P:", long_options, &option_index)) != -1) //-解析命令行参数,来运行程序
	{
		switch (opt) 
		{
			case 'h':
				print_usage_exit(argv);
				break;

			case 's':
				pcSerialDevice = optarg;
				break;

			case 'B':
				sscanf(optarg, "%d", &u32BaudRate);	//-sscanf会从buffer里读进数据，依照format的格式将数据写入到argument里。
				break;

			case 'R':
				sscanf(optarg, "0x%x", &gu32RfActiveOutDioMask);
				break;

			case 'S':
				sscanf(optarg, "0x%x", &gu32StatusOutDioMask);
				break;

			case 'G':
				sscanf(optarg, "0x%x", &gu32TxConfInDioMask);
				break;

			case 'P':
				sscanf(optarg, "0x%x", &i32tmp);
				gu16PollPeriod = (uint16_t)i32tmp;
				break;
                
			default: /* '?' */
				print_usage_exit(argv);
		}
	}

	return 0;
}
