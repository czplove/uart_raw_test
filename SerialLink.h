/****************************************************************************
 *
 * MODULE:             SerialLink
 *
 * COMPONENT:          $RCSfile: SerialLink.h,v $
 *
 * REVISION:           $Revision: 43420 $
 *
 * DATED:              $Date: 2012-06-18 15:13:17 +0100 (Mon, 18 Jun 2012) $
 *
 * AUTHOR:             Lee Mitchell
 *
 * DESCRIPTION:
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
 *
 * Copyright NXP B.V. 2012. All rights reserved
 *
 ***************************************************************************/

#ifndef  SERIALLINK_H_INCLUDED
#define  SERIALLINK_H_INCLUDED

#if defined __cplusplus
extern "C" {
#endif

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/

#include "Serial.h"


/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/

#define PACKED __attribute__((__packed__))

#define DBG_SERIALLINK 0
#define DBG_SERIALLINK_CB 0
#define DBG_SERIALLINK_COMMS 1
#define DBG_SERIALLINK_QUEUE 0

#define SL_START_CHAR   0x01
#define SL_ESC_CHAR     0x02
#define SL_END_CHAR     0x03

#define SL_MAX_MESSAGE_LENGTH 256

#define SL_MAX_MESSAGE_QUEUES 3

#define SL_MAX_CALLBACK_QUEUES 3

/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/

typedef enum
{
    E_SL_OK,
    E_SL_ERROR,
    E_SL_NOMESSAGE,
    E_SL_ERROR_SERIAL,
    E_SL_ERROR_NOMEM,
} teSL_Status;








/** Structure used to contain a message */
typedef struct
{
    uint16_t u16Type;
    uint16_t u16Length;
    uint8_t  au8Message[SL_MAX_MESSAGE_LENGTH];
} tsSL_Message;

/** Status message */
typedef struct
{
    enum
    {
        E_SL_MSG_STATUS_SUCCESS,
        E_SL_MSG_STATUS_INCORRECT_PARAMETERS,
        E_SL_MSG_STATUS_UNHANDLED_COMMAND,
        E_SL_MSG_STATUS_BUSY,
        E_SL_MSG_STATUS_STACK_ALREADY_STARTED,
    } PACKED eStatus;
    uint8_t             u8SequenceNo;           /**< Sequence number of outgoing message */
    uint16_t            u16MessageType;         /**< Type of message that this is status to */
    char                acMessage[];            /**< Optional message */
} PACKED tsSL_Msg_Status;



/** Callback function for a given message type 
 *  \param pvUser           User supplied pointer to be passed to the callback function
 *  \param u16Length        Length of the received message
 *  \param pvMessage        Pointer to the message data.
 *  \return Nothing
 */
typedef void (*tprSL_MessageCallback)(void *pvUser, uint16_t u16Length, void *pvMessage);

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

/****************************************************************************/
/***        Local Variables                                               ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Functions                                            ***/
/****************************************************************************/
extern teSL_Status eSL_WriteMessage(uint16_t u16Type, uint16_t u16Length, uint8_t *pu8Data);
extern teSL_Status eSL_ReadMessage(uint16_t *pu16Type, uint16_t *pu16Length, uint16_t u16MaxLength, uint8_t *pu8Message);

/****************************************************************************/
/***        Local Functions                                               ***/
/****************************************************************************/

#if defined __cplusplus
}
#endif

#endif  /* SERIALLINK_H_INCLUDED */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

