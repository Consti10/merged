/*****************************************************************************
**
**  Name:           app_ble_test_main.c
**
**  Description:    Bluetooth BLE test app main application
**
**  Copyright (c) 2016, Cypress Semiconductor., All Rights Reserved.
**  Proprietary and confidential.
**
*****************************************************************************/

#include <stdlib.h>

#include "app_ble.h"
#include "app_utils.h"
#include "app_disc.h"
#include "app_mgt.h"
#include "app_dm.h"
#include "app_ble_client.h"
#include "app_ble_server.h"
#include "app_ble_test_server.h"
#include "app_ble_test_client.h"

/*
 * Defines
 */


/* BLE TEST menu items */
enum
{
    APP_BLE_TEST_MENU_START_SERVER = 1,
    APP_BLE_TEST_MENU_STOP_SERVER,

    APP_BLE_TEST_MENU_SEARCH_SERVER,
    APP_BLE_TEST_MENU_START_CLIENT,
    APP_BLE_TEST_MENU_REGISTER_NOTIFICATION,
    APP_BLE_TEST_MENU_REGISTER_INDICATION,
    APP_BLE_TEST_MENU_ENABLE_NOTIFICATION,
    APP_BLE_TEST_MENU_ENABLE_INDICATION,
    APP_BLE_TEST_MENU_DISABLE_NOTIFICATION,
    APP_BLE_TEST_MENU_DISABLE_INDICATION,
    APP_BLE_TEST_MENU_WRITE,
    APP_BLE_TEST_MENU_WRITE_PREPARE,
    APP_BLE_TEST_MENU_WRITE_EXECUTE,
    APP_BLE_TEST_MENU_STOP_CLIENT,
    APP_BLE_TEST_MENU_DISPLAY_CLIENT,
    APP_BLE_TEST_MENU_QUIT = 99
};
/*
 * Global Variables
 */


/*
 * Local functions
 */
static void app_ble_test_menu(void);



/*******************************************************************************
 **
 ** Function         app_ble_test_display_menu
 **
 ** Description      This is the TEST menu
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_ble_test_display_menu (void)
{
    APP_INFO0("\t**** BLE TEST Server menu ***");
    APP_INFO1("\t%d => Start TEST BLE Server", APP_BLE_TEST_MENU_START_SERVER);
    APP_INFO1("\t%d => Stop TEST BLE Server\n", APP_BLE_TEST_MENU_STOP_SERVER);
    APP_INFO0("\t**** BLE TEST Client menu ***");
    APP_INFO1("\t%d => Search TEST BLE Server", APP_BLE_TEST_MENU_SEARCH_SERVER);
    APP_INFO1("\t%d => Start TEST BLE Client", APP_BLE_TEST_MENU_START_CLIENT);
    APP_INFO1("\t%d => Register Notification", APP_BLE_TEST_MENU_REGISTER_NOTIFICATION);
    APP_INFO1("\t%d => Register Indication", APP_BLE_TEST_MENU_REGISTER_INDICATION);
    APP_INFO1("\t%d => Enable notification", APP_BLE_TEST_MENU_ENABLE_NOTIFICATION);
    APP_INFO1("\t%d => Enable indication", APP_BLE_TEST_MENU_ENABLE_INDICATION);
    APP_INFO1("\t%d => Disable notification", APP_BLE_TEST_MENU_DISABLE_NOTIFICATION);
    APP_INFO1("\t%d => Disable indication", APP_BLE_TEST_MENU_DISABLE_INDICATION);
    APP_INFO1("\t%d => Write data", APP_BLE_TEST_MENU_WRITE);
    APP_INFO1("\t%d => Prepare Write", APP_BLE_TEST_MENU_WRITE_PREPARE);
    APP_INFO1("\t%d => Execute Write", APP_BLE_TEST_MENU_WRITE_EXECUTE);
    APP_INFO1("\t%d => Stop TEST BLE Client", APP_BLE_TEST_MENU_STOP_CLIENT);
    APP_INFO1("\t%d => Display Clients", APP_BLE_TEST_MENU_DISPLAY_CLIENT);
    APP_INFO1("\t%d => Exit", APP_BLE_TEST_MENU_QUIT);
}


/*******************************************************************************
 **
 ** Function         app_ble_test_menu
 **
 ** Description      ble test menu
 **
 ** Returns          void
 **
 *******************************************************************************/
void app_ble_test_menu(void)
{
    int choice;

    do
    {
        app_ble_test_display_menu();

        choice = app_get_choice("Menu");

        switch(choice)
        {
        case APP_BLE_TEST_MENU_START_SERVER:
            app_ble_test_server_start();
            break;

        case APP_BLE_TEST_MENU_STOP_SERVER:
            app_ble_test_server_stop();
            break;

        case APP_BLE_TEST_MENU_START_CLIENT:
            app_ble_test_client_start();
            break;

        case APP_BLE_TEST_MENU_REGISTER_NOTIFICATION:
            app_ble_test_client_register_notification(APP_BLE_TS_CHAR_NOTI_UUID);
            break;

        case APP_BLE_TEST_MENU_REGISTER_INDICATION:
            app_ble_test_client_register_notification(APP_BLE_TS_CHAR_INDI_UUID);
            break;

        case APP_BLE_TEST_MENU_ENABLE_NOTIFICATION:
            app_ble_test_client_write_enable(APP_BLE_TS_CHAR_NOTI_UUID);
            break;

        case APP_BLE_TEST_MENU_ENABLE_INDICATION:
            app_ble_test_client_write_enable(APP_BLE_TS_CHAR_INDI_UUID);
            break;

        case APP_BLE_TEST_MENU_DISABLE_NOTIFICATION:
            app_ble_test_client_write_disable(APP_BLE_TS_CHAR_NOTI_UUID);
            break;

        case APP_BLE_TEST_MENU_DISABLE_INDICATION:
            app_ble_test_client_write_disable(APP_BLE_TS_CHAR_INDI_UUID);
            break;

        case APP_BLE_TEST_MENU_SEARCH_SERVER:
            app_disc_start_ble_regular(app_ble_test_client_disc_cback, 0);
            break;

        case APP_BLE_TEST_MENU_WRITE:
            app_ble_test_client_write(APP_BLE_TS_CHAR_UUID);
            break;

        case APP_BLE_TEST_MENU_WRITE_PREPARE:
            app_ble_test_client_write_prepare(APP_BLE_TS_CHAR_UUID);
            break;

        case APP_BLE_TEST_MENU_WRITE_EXECUTE:
            app_ble_test_client_write_execute(APP_BLE_TS_CHAR_UUID);
            break;

        case APP_BLE_TEST_MENU_STOP_CLIENT:
            app_ble_test_client_stop();
            break;

        case APP_BLE_TEST_MENU_DISPLAY_CLIENT:
            app_ble_client_display(1);
            break;

        case APP_BLE_TEST_MENU_QUIT:
            APP_INFO0("Quit");
            break;

        default:
            APP_ERROR1("Unknown choice:%d", choice);
            break;
        }
    } while (choice != APP_BLE_TEST_MENU_QUIT); /* While user don't exit sub-menu */
}

/*******************************************************************************
 **
 ** Function         app_ble_test_mgt_callback
 **
 ** Description      This callback function is called in case of server
 **                  disconnection (e.g. server crashes)
 **
 ** Parameters
 **
 ** Returns          BOOLEAN
 **
 *******************************************************************************/
BOOLEAN app_ble_test_mgt_callback(tBSA_MGT_EVT event, tBSA_MGT_MSG *p_data)
{
    switch(event)
    {
    case BSA_MGT_STATUS_EVT:
        APP_DEBUG0("BSA_MGT_STATUS_EVT");
        if (p_data->status.enable)
        {
            APP_DEBUG0("Bluetooth restarted => re-initialize the application");
            app_ble_start();
        }
        break;

    case BSA_MGT_DISCONNECT_EVT:
        APP_DEBUG1("BSA_MGT_DISCONNECT_EVT reason:%d", p_data->disconnect.reason);
        /* Connection with the Server lost => Just exit the application */
        exit(-1);
        break;

    default:
        break;
    }
    return FALSE;
}

/*******************************************************************************
 **
 ** Function        main
 **
 ** Description     This is the main function
 **
 ** Parameters      Program's arguments
 **
 ** Returns         status
 **
 *******************************************************************************/
int main(int argc, char **argv)
{
    int status;

    /* Initialize BLE application */
    status = app_ble_init();
    if (status < 0)
    {
        APP_ERROR0("Couldn't Initialize BLE app, exiting");
        exit(-1);
    }

    /* Open connection to BSA Server */
    app_mgt_init();
    if (app_mgt_open(NULL, app_ble_test_mgt_callback) < 0)
    {
        APP_ERROR0("Unable to connect to server");
        return -1;
    }

    /* Start BLE application */
    status = app_ble_start();
    if (status < 0)
    {
        APP_ERROR0("Couldn't Start BLE app, exiting");
        return -1;
    }

    /* The main BLE loop */
    app_ble_test_menu();

    /* Exit BLE mode */
    app_ble_exit();

    /* Close BSA Connection before exiting (to release resources) */
    app_mgt_close();

    exit(0);
}

