/*
 * $ Copyright Cypress Semiconductor $
 */

/****************************************************************************
**
** Name:         wiced.h
**
** Description:  wiced header file for VS2010 projects
**
**
******************************************************************************/
#ifndef WICED_H
#define WICED_H

//#include "bt_target.h"
#include "wiced_result.h"

/* Suppress unused variable warning */
#ifndef UNUSED_VARIABLE
#define UNUSED_VARIABLE(x) /*@-noeffect@*/ ( (void)(x) ) /*@+noeffect@*/
#endif

#define WICED_SUPPRESS_WARNINGS(m) if((m)){;}

//typedef int wiced_thread_t;

#include "stdint.h"

#if 0
typedef unsigned int    uint32_t;
typedef unsigned int    uint32_tt;
typedef signed char     int8_t;
typedef short           int16_t;
typedef int             int32_t;
typedef unsigned char   uint8_t;
typedef unsigned short  uint16_t;
#endif

typedef unsigned int   wiced_bool_t;

#define WICED_FALSE 0
#define WICED_TRUE  1

#define WPRINT_BT_APP_INFO(info)   {printf info;}

#endif //WICED_H

