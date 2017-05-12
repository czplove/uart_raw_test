#ifndef __TEST_UTILS_H__
#define __TEST_UTILS_H__

#if defined __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <arpa/inet.h>

#include "Serial.h"
#include "SerialLink.h"




extern int input_cmd_handler(void);
extern void *pvSerialReaderThread(void *p);
extern void *pvATAutoSendThread(void *p);

#if defined __cplusplus
}
#endif

#endif


