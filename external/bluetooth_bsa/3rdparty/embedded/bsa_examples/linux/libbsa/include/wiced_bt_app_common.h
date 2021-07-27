/*
 * $ Copyright Cypress Semiconductor $
 */
/** @file
 *
 * WICED BT App Common Utilities. This file provides the interfaces to the utilities that
 * can be used by the applications
 */
#include "wiced_gki.h"
#include "wiced_timer.h"

#ifndef _WICED_BT_APP_COMMON_H_
#define _WICED_BT_APP_COMMON_H_

/* BLE Advertisement Maximum Length*/
#define ADV_LEN_MAX        31

#ifndef WICED_SDK_MAJOR_VER
#define WICED_SDK_MAJOR_VER    5
#endif
#ifndef WICED_SDK_MINOR_VER
#define WICED_SDK_MINOR_VER    0
#endif
#ifndef WICED_SDK_REV_NUMBER
#define WICED_SDK_REV_NUMBER   0
#endif
#ifndef WICED_SDK_BUILD_NUMBER
#define WICED_SDK_BUILD_NUMBER 0
#endif
#define POWER_CLASS            1

/*****************************************************************************
**                                                  Function Declarations
*****************************************************************************/
/** Wiced Bt App Initialization
 *
 * Initializes Application events serialization. Performs the HAL initialization
 *
  * @return   wiced_result_t
 */
wiced_result_t wiced_bt_app_init( void );

/** Starts the application timers . One is app timer which is a seconds timer, and the other is
 * app fine timerwhich is a milli second timer.This function initializes the timers using the application
 *defined timeouts and also starts the timers. If application does not want to start the timer,
 * timer interval can be passed as 0
 *
 *@param[in]    app_timer_interval       :App timer interval in seconds
 *
 *@param[in]    fine_timer_interval      :App fine timer interval in milliseconds
 *
 *@param[in]    p_app_timer_cb           :Pointer to the application timer callback
 *
 *@param[in]    p_app_fine_timer_cb      :Pointer to the application fine timer callback
 *
 * @return      wiced_result_t
 */
wiced_result_t wiced_bt_app_start_timer ( uint16_t app_timer_interval,
    uint16_t fine_timer_interval, wiced_timer_callback_t p_app_timer_cb,
    wiced_timer_callback_t p_app_fine_timer_cb  );

/** Stops the application timers.
 *
 * @return   None
 */
void wiced_bt_app_stop_timer ( void );

/** Starts the application connection idle timer.This function initialize the timer using the application
 *defined connection idle timeout and also starts the timer
 *
  *@param[in]   con_idle_timeout         :Connection idle timeout in seconds
 *
 *@param[in]    p_app_idle_timer_cb      :Pointer to the application idle timer callback
 *
 * @return      wiced_result_t
 */
wiced_result_t wiced_bt_app_start_conn_idle_timer(
                        UINT8 con_idle_timeout,
                        wiced_timer_callback_t p_app_idle_timer_cb );

/** Stops the application connection idle timer.
 *
 * @return   None
 */
void wiced_bt_app_stop_conn_idle_timer( void );

#endif //_WICED_BT_APP_COMMON_H_
