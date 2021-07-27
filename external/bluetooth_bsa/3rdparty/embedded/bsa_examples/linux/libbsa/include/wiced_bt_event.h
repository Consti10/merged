/*
 * $ Copyright Cypress Semiconductor $
 */
 /** @file
 *
 *  This implements the application-thread level event handling for WICED Apps
 */
#ifndef __WICED_BT_EVENT_H__
#define __WICED_BT_EVENT_H__
#include "wiced.h"

 typedef enum
 {
     WICED_SERIALIZATION_EVENT = 1,
 }wiced_bt_internal_events_t;

 /* The serialization queue will have these callbacks */
typedef struct
{
    // The function to invoke in application thread context
    int (*fn)(void*);

    // Any arbitrary data to be given to the callback. wiced_app_event_serialize Caller has to allocate and free once serialized event handled
    void* data;
} wiced_app_event_srzn_cb_t;


/*
 *This function lets you serialize a call onto the application thread.
 *
 *@param[in]    fn          : serialized call back on serialization
 *
 *@param[in]    data        : data to be handled in serialized call

 * @return      wiced_bool_t

 * Note: It is application's responsibility freeing data pointer
 */
wiced_bool_t wiced_app_event_serialize( int (*fn)(void*), void* data);

#endif //__WICED_BT_EVENT_H__
