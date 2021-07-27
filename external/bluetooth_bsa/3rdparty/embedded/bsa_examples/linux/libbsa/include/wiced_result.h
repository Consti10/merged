/*
 * $ Copyright Cypress Semiconductor $
 */

/** @file
 *
 * Generic types
 *
 */
#ifndef __WICED_RESULT_H
#define __WICED_RESULT_H
#pragma once
#include "wiced_bt_constants.h"

#ifndef RESULT_ENUM
#define RESULT_ENUM( prefix, name, value )  prefix ## name = (value)
#endif /* ifndef RESULT_ENUM */

/*************************************************************************/
/**
 *  @addtogroup  Result         WICED Result Codes
 *  @ingroup     gentypes
 *
 *  <b> WICED Result list </b> for Bluetooth BR/EDR and LE @b .
 *
 *  @{
 */
/*************************************************************************/

/** WICED result list */
#define WICED_RESULT_LIST( prefix ) \
    RESULT_ENUM( prefix, SUCCESS,                       0x00 ),   /**< Success */                        \
    RESULT_ENUM( prefix, DELETED                       ,0x01 ),   \
    RESULT_ENUM( prefix, NO_MEMORY                     ,0x10 ),   \
    RESULT_ENUM( prefix, POOL_ERROR                    ,0x02 ),   \
    RESULT_ENUM( prefix, PTR_ERROR                     ,0x03 ),   \
    RESULT_ENUM( prefix, WAIT_ERROR                    ,0x04 ),   \
    RESULT_ENUM( prefix, SIZE_ERROR                    ,0x05 ),   \
    RESULT_ENUM( prefix, GROUP_ERROR                   ,0x06 ),   \
    RESULT_ENUM( prefix, NO_EVENTS                     ,0x07 ),   \
    RESULT_ENUM( prefix, OPTION_ERROR                  ,0x08 ),   \
    RESULT_ENUM( prefix, QUEUE_ERROR                   ,0x09 ),   \
    RESULT_ENUM( prefix, QUEUE_EMPTY                   ,0x0A ),   \
    RESULT_ENUM( prefix, QUEUE_FULL                    ,0x0B ),   \
    RESULT_ENUM( prefix, SEMAPHORE_ERROR               ,0x0C ),   \
    RESULT_ENUM( prefix, NO_INSTANCE                   ,0x0D ),   \
    RESULT_ENUM( prefix, THREAD_ERROR                  ,0x0E ),   \
    RESULT_ENUM( prefix, PRIORITY_ERROR                ,0x0F ),   \
    RESULT_ENUM( prefix, START_ERROR                   ,0x10 ),   \
    RESULT_ENUM( prefix, DELETE_ERROR                  ,0x11 ),   \
    RESULT_ENUM( prefix, RESUME_ERROR                  ,0x12 ),   \
    RESULT_ENUM( prefix, CALLER_ERROR                  ,0x13 ),   \
    RESULT_ENUM( prefix, SUSPEND_ERROR                 ,0x14 ),   \
    RESULT_ENUM( prefix, TIMER_ERROR                   ,0x15 ),   \
    RESULT_ENUM( prefix, TICK_ERROR                    ,0x16 ),   \
    RESULT_ENUM( prefix, ACTIVATE_ERROR                ,0x17 ),   \
    RESULT_ENUM( prefix, THRESH_ERROR                  ,0x18 ),   \
    RESULT_ENUM( prefix, SUSPEND_LIFTED                ,0x19 ),   \
    RESULT_ENUM( prefix, WAIT_ABORTED                  ,0x1A ),   \
    RESULT_ENUM( prefix, WAIT_ABORT_ERROR              ,0x1B ),   \
    RESULT_ENUM( prefix, MUTEX_ERROR                   ,0x1C ),   \
    RESULT_ENUM( prefix, NOT_AVAILABLE                 ,0x1D ),   \
    RESULT_ENUM( prefix, NOT_OWNED                     ,0x1E ),   \
    RESULT_ENUM( prefix, INHERIT_ERROR                 ,0x1F ),   \
    RESULT_ENUM( prefix, NOT_DONE                      ,0x20 ),   \
    RESULT_ENUM( prefix, CEILING_EXCEEDED              ,0x21 ),   \
    RESULT_ENUM( prefix, INVALID_CEILING               ,0x22 ),   \
    RESULT_ENUM( prefix, STA_JOIN_FAILED               ,0x23),   /**< Join failed */\
    RESULT_ENUM( prefix, SLEEP_ERROR                   ,0x24),   \
    RESULT_ENUM( prefix, PENDING,                       0x25),   /**< Pending */                        \
    RESULT_ENUM( prefix, TIMEOUT,                       0x26),   /**< Timeout */                        \
    RESULT_ENUM( prefix, PARTIAL_RESULTS,               0x27),   /**< Partial results */                \
    RESULT_ENUM( prefix, ERROR,                         0x28),   /**< Error */                          \
    RESULT_ENUM( prefix, BADARG,                        0x29),   /**< Bad Arguments */                  \
    RESULT_ENUM( prefix, BADOPTION,                     0x2A),   /**< Mode not supported */             \
    RESULT_ENUM( prefix, UNSUPPORTED,                   0x2B),   /**< Unsupported function */           \
    RESULT_ENUM( prefix, OUT_OF_HEAP_SPACE,             0x2C),   /**< Dynamic memory space exhausted */ \
    RESULT_ENUM( prefix, NOTUP,                         0x2D),   /**< Interface is not currently Up */  \
    RESULT_ENUM( prefix, UNFINISHED,                    0x2E),   /**< Operation not finished yet */     \
    RESULT_ENUM( prefix, CONNECTION_LOST,               0x2F),   /**< Connection to server lost */      \
    RESULT_ENUM( prefix, NOT_FOUND,                     0x30),   /**< Item not found */                 \
    RESULT_ENUM( prefix, PACKET_BUFFER_CORRUPT,         0x31),   /**< Packet buffer corrupted */        \
    RESULT_ENUM( prefix, ROUTING_ERROR,                 0x32),   /**< Routing error */                  \
    RESULT_ENUM( prefix, BADVALUE,                      0x33),   /**< Bad value */                      \
    RESULT_ENUM( prefix, WOULD_BLOCK,                   0x34),   /**< Function would block */           \
    RESULT_ENUM( prefix, ABORTED,                       0x35),   /**< Operation aborted */              \
    RESULT_ENUM( prefix, CONNECTION_RESET,              0x36),   /**< Connection has been reset */      \
    RESULT_ENUM( prefix, CONNECTION_CLOSED,             0x37),   /**< Connection is closed */           \
    RESULT_ENUM( prefix, NOT_CONNECTED,                 0x38),   /**< Connection is not connected */    \
    RESULT_ENUM( prefix, ADDRESS_IN_USE,                0x39),   /**< Address is in use */              \
    RESULT_ENUM( prefix, NETWORK_INTERFACE_ERROR,       0x3A),   /**< Network interface error */        \
    RESULT_ENUM( prefix, ALREADY_CONNECTED,             0x3B),   /**< Socket is already connected */    \
    RESULT_ENUM( prefix, INVALID_INTERFACE,             0x3C),   /**< Interface specified in invalid */ \
    RESULT_ENUM( prefix, SOCKET_CREATE_FAIL,            0x3D),   /**< Socket creation failed */         \
    RESULT_ENUM( prefix, INVALID_SOCKET,                0x3E),   /**< Socket is invalid */              \
    RESULT_ENUM( prefix, CORRUPT_PACKET_BUFFER,         0x3F),   /**< Packet buffer is corrupted */     \
    RESULT_ENUM( prefix, UNKNOWN_NETWORK_STACK_ERROR,   0x40),   /**< Unknown network stack error */    \
    RESULT_ENUM( prefix, NO_STORED_AP_IN_DCT,           0x41),   /**< DCT contains no AP credentials */ \
    RESULT_ENUM( prefix, ALREADY_INITIALIZED,           0x42),   /**< Already initialized*/ \
    RESULT_ENUM( prefix, FEATURE_NOT_ENABLED           ,0xFF ),   \

/**@}  WICED result list */


/*************************************************************************/
/**
 *  @addtogroup  Result       WICED Result Codes
 *  @ingroup     gentypes
 *
 *  <b> Result types </b> @b .
 *
 *  @{
 */
/*************************************************************************/
/** WICED result */
typedef enum
{
    WICED_RESULT_LIST(WICED_)
    BT_RESULT_LIST      (  WICED_BT_       )  /**< 8000 - 8999 */
} wiced_result_t;

/**@}  WICED Result */

/******************************************************
 *            Structures
 ******************************************************/

/******************************************************
 *            Function Declarations
 ******************************************************/

#endif