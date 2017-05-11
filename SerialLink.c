/****************************************************************************
*
* MODULE:             SerialLink
*
* COMPONENT:          $RCSfile: SerialLink.c,v $
*
* REVISION:           $Revision: 43420 $
*
* DATED:              $Date: 2012-06-18 15:13:17 +0100 (Mon, 18 Jun 2012) $
*
* AUTHOR:             Lee Mitchell
*
****************************************************************************
*
* This software is owned by NXP B.V. and/or its supplier and is protected
* under applicable copyright laws. All rights are reserved. We grant You,
* and any third parties, a license to use this software solely and
* exclusively on NXP products [NXP Microcontrollers such as JN5148, JN5142, JN5139]. 
* You, and any third parties must reproduce the copyright and warranty notice
* and any other legend of ownership on each copy or partial copy of the 
* software.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.

* Copyright NXP B.V. 2012. All rights reserved
*
***************************************************************************/

/** \addtogroup zb
 * \file
 * \brief Serial Link layer
 */

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

#include "SerialLink.h"
#include "Serial.h"

/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#if DEBUG_SERIALLINK
#define DEBUG_PRINTF(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINTF(...)
#endif /* DEBUG_SERIALLINK */

#define DBG_vPrintf(a,b,ARGS...) do {  if (a) printf("%s: " b, __FUNCTION__, ## ARGS); } while(0)
#define DBG_vAssert(a,b) do {  if (a && !(b)) printf(__FILE__ " %d : Asset Failed\n", __LINE__ ); } while(0)

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

typedef enum
{
    FALSE,
    TRUE,    
} bool;

typedef enum
{
    E_STATE_RX_WAIT_START,
    E_STATE_RX_WAIT_TYPEMSB,
    E_STATE_RX_WAIT_TYPELSB,
    E_STATE_RX_WAIT_LENMSB,
    E_STATE_RX_WAIT_LENLSB,
    E_STATE_RX_WAIT_CRC,
    E_STATE_RX_WAIT_DATA,
} teSL_RxState;

/** Linked list structure for a callback function entry */
typedef struct _tsSL_CallbackEntry
{
    uint16_t                u16Type;        /**< Message type for this callback */
    tprSL_MessageCallback   prCallback;     /**< User supplied callback function for this message type */
    void                    *pvUser;        /**< User supplied data for the callback function */
    struct _tsSL_CallbackEntry *psNext;     /**< Pointer to next in linked list */
} tsSL_CallbackEntry;

/** Structure of data for the serial link */
typedef struct
{
    int     iSerialFd;

#ifndef WIN32
    pthread_mutex_t         mutex;
#endif /* WIN32 */
    
    struct
    {
#ifndef WIN32
        pthread_mutex_t         mutex;
#endif /* WIN32 */
        tsSL_CallbackEntry      *psListHead;
    } sCallbacks;
    
    //-tsUtilsQueue sCallbackQueue;
    //-tsUtilsThread sCallbackThread;
    
    // Array of listeners for messages
    // eSL_MessageWait uses this array to wait on incoming messages.
    struct 
    {
        uint16_t u16Type;
        uint16_t u16Length;
        uint8_t *pu8Message;
#ifndef WIN32
        pthread_mutex_t mutex;
        pthread_cond_t cond_data_available;
#else
    
#endif /* WIN32 */
    } asReaderMessageQueue[SL_MAX_MESSAGE_QUEUES];

    
    //-tsUtilsThread sSerialReader;
} tsSerialLink;


/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

static uint8_t u8SL_CalculateCRC(uint16_t u16Type, uint16_t u16Length, uint8_t *pu8Data);

static int iSL_TxByte(bool bSpecialCharacter, uint8_t u8Data);

static bool bSL_RxByte(uint8_t *pu8Data);


/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

extern int verbosity;

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

//-读串口数据,如果没有立即返回,如果有那么进行最底层的转译,校验等,把最终报文进行返回
teSL_Status eSL_ReadMessage(uint16_t *pu16Type, uint16_t *pu16Length, uint16_t u16MaxLength, uint8_t *pu8Message)
{

    static teSL_RxState eRxState = E_STATE_RX_WAIT_START;
    static uint8_t u8CRC;
    uint8_t u8Data;
    static uint16_t u16Bytes;
    static bool bInEsc = FALSE;

    while(bSL_RxByte(&u8Data))	//-这里是目前整个程序唯一读一个字节的地方
    {
        DBG_vPrintf(DBG_SERIALLINK_COMMS, "0x%02x\n", u8Data);
        
    }
    
    return E_SL_NOMESSAGE;
}







/****************************************************************************
*
* NAME: vSL_TxByte
*
* DESCRIPTION:
*
* PARAMETERS:  Name                RW  Usage
*								bSpecialCharacter		表示是否是特殊符号,是就不需要转化
* RETURNS:
* void
****************************************************************************/
static int iSL_TxByte(bool bSpecialCharacter, uint8_t u8Data)	//-原始数据一个字节一个字节的发送,如果需要的话会自动转译,所以上层不用考虑转译
{
    if(!bSpecialCharacter && (u8Data < 0x10))
    {
        u8Data ^= 0x10;

        if (eSerial_Write(SL_ESC_CHAR) != E_SERIAL_OK) return -1;
        //DBG_vPrintf(DBG_SERIALLINK_COMMS, " 0x%02x", SL_ESC_CHAR);
    }
    //DBG_vPrintf(DBG_SERIALLINK_COMMS, " 0x%02x", u8Data);

    return eSerial_Write(u8Data);
}


/****************************************************************************
*
* NAME: bSL_RxByte
*
* DESCRIPTION:
*
* PARAMETERS:  Name                RW  Usage
*
* RETURNS:
* void
****************************************************************************/
static bool bSL_RxByte(uint8_t *pu8Data)
{
    if (eSerial_Read(pu8Data) == E_SERIAL_OK)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

