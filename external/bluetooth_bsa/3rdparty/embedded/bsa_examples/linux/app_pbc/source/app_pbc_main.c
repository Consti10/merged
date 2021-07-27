/*****************************************************************************
**
**  Name:           app_pbc_main.c
**
**  Description:    Bluetooth Phonebook client sample application
**
**  Copyright (c) 2014, Broadcom Corp., All Rights Reserved.
**  Broadcom Bluetooth Core. Proprietary and confidential.
**
**  Copyright (C) 2018 Cypress Semiconductor Corporation
**
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "gki.h"
#include "uipc.h"
#include "bsa_api.h"
#include "app_xml_param.h"
#include "app_xml_utils.h"
#include "app_disc.h"
#include "app_mgt.h"
#include "app_utils.h"
#include "app_pbc.h"
#include "bd.h"

/* ui keypress definition */
enum
{
    APP_PBC_MENU_NULL,
    APP_PBC_MENU_OPEN,
    APP_PBC_MENU_GET_VCARD,
    APP_PBC_MENU_GET_LISTVCARDS,
    APP_PBC_MENU_GET_PHONEBOOK,
    APP_PBC_MENU_SET_CHDIR,
    APP_PBC_MENU_CLOSE,
    APP_PBC_MENU_ABORT,
    APP_PBC_MENU_DISC,
    APP_PBC_MENU_QUIT
};

/*
 * Local Variables
 */
static tBSA_PBC_FEA_MASK peer_features;

/*******************************************************************************
**
** Function         app_pbc_display_main_menu
**
** Description      This is the main menu
**
** Parameters
**
** Returns          void
**
*******************************************************************************/
static void app_pbc_display_main_menu(void)
{
    APP_INFO0("Bluetooth Phone Book Client Test Application");
    APP_INFO1("    %d => Open Connection", APP_PBC_MENU_OPEN);
    APP_INFO1("    %d => Get VCard", APP_PBC_MENU_GET_VCARD);
    APP_INFO1("    %d => Get VCard Listing", APP_PBC_MENU_GET_LISTVCARDS);
    APP_INFO1("    %d => Get Phonebook", APP_PBC_MENU_GET_PHONEBOOK);
    APP_INFO1("    %d => Set Change Dir", APP_PBC_MENU_SET_CHDIR);
    APP_INFO1("    %d => Close Connection", APP_PBC_MENU_CLOSE);
    APP_INFO1("    %d => Abort", APP_PBC_MENU_ABORT);
    APP_INFO1("    %d => Discovery", APP_PBC_MENU_DISC);
    APP_INFO1("    %d => Quit", APP_PBC_MENU_QUIT);
}

/*******************************************************************************
**
** Function         app_pbc_display_vcard_properties
**
** Description      Display vCard property masks
**
** Parameters
**
** Returns          void
**
*******************************************************************************/
static void app_pbc_display_vcard_properties(void)
{
    APP_INFO1("    VERSION (vCard Version)                     0x%08lx", BSA_PBC_FILTER_VERSION);
    APP_INFO1("    FN (Formatted Name)                         0x%08lx", BSA_PBC_FILTER_FN);
    APP_INFO1("    N (Structured Presentation of Name)         0x%08lx", BSA_PBC_FILTER_N);
    APP_INFO1("    PHOTO (Associated Image or Photo)           0x%08lx", BSA_PBC_FILTER_PHOTO);
    APP_INFO1("    BDAY (Birthday)                             0x%08lx", BSA_PBC_FILTER_BDAY);
    APP_INFO1("    ADR (Delivery Address)                      0x%08lx", BSA_PBC_FILTER_ADR);
    APP_INFO1("    LABEL (Delivery)                            0x%08lx", BSA_PBC_FILTER_LABEL);
    APP_INFO1("    TEL (Telephone Number)                      0x%08lx", BSA_PBC_FILTER_TEL);
    APP_INFO1("    EMAIL (Electronic Mail Address)             0x%08lx", BSA_PBC_FILTER_EMAIL);
    APP_INFO1("    MAILER (Electronic Mail)                    0x%08lx", BSA_PBC_FILTER_MAILER);
    APP_INFO1("    TZ (Time Zone)                              0x%08lx", BSA_PBC_FILTER_TZ);
    APP_INFO1("    GEO (Geographic Position)                   0x%08lx", BSA_PBC_FILTER_GEO);
    APP_INFO1("    TITLE (Job)                                 0x%08lx", BSA_PBC_FILTER_TITLE);
    APP_INFO1("    ROLE (Role within the Organization)         0x%08lx", BSA_PBC_FILTER_ROLE);
    APP_INFO1("    LOGO (Organization Logo)                    0x%08lx", BSA_PBC_FILTER_LOGO);
    APP_INFO1("    AGENT (vCard of Person Representing)        0x%08lx", BSA_PBC_FILTER_AGENT);
    APP_INFO1("    ORG (Name of Organization)                  0x%08lx", BSA_PBC_FILTER_ORG);
    APP_INFO1("    NOTE (Comments)                             0x%08lx", BSA_PBC_FILTER_NOTE);
    APP_INFO1("    REV (Revision)                              0x%08lx", BSA_PBC_FILTER_REV);
    APP_INFO1("    SOUND (Pronunciation of Name)               0x%08lx", BSA_PBC_FILTER_SOUND);
    APP_INFO1("    URL (Uniform Resource Locator)              0x%08lx", BSA_PBC_FILTER_URL);
    APP_INFO1("    UID (Unique ID)                             0x%08lx", BSA_PBC_FILTER_UID);
    APP_INFO1("    KEY (Public Encryption Key)                 0x%08lx", BSA_PBC_FILTER_KEY);
    APP_INFO1("    NICKNAME (Nickname)                         0x%08lx", BSA_PBC_FILTER_NICKNAME);
    APP_INFO1("    CATEGORIES (Categories)                     0x%08lx", BSA_PBC_FILTER_CATEGORIES);
    APP_INFO1("    PROID (Product ID)                          0x%08lx", BSA_PBC_FILTER_PROID);
    APP_INFO1("    CLASS (Class information)                   0x%08lx", BSA_PBC_FILTER_CLASS);
    APP_INFO1("    SORT-STRING (String used for sorting operation) 0x%08lx", BSA_PBC_FILTER_SORT_STRING);
    APP_INFO1("    X-IRMC-CALL-DATETIME (Time stamp)           0x%08lx", BSA_PBC_FILTER_CALL_DATETIME);
    APP_INFO1("    X-BT-SPEEDDIALKEY (Speed-dial shortcut)     0x%08lx", BSA_PBC_FILTER_X_BT_SPEEDDIALKEY);
    APP_INFO1("    X-BT-UCI (Uniform Caller Identifier)        0x%08lx", BSA_PBC_FILTER_X_BT_UCI);
    APP_INFO1("    X-BT-UID (Bluetooth Contact Unique Identifier)  0x%08lx", BSA_PBC_FILTER_X_BT_UID);
}

/*******************************************************************************
**
** Function         app_pbc_mgt_callback
**
** Description      This callback function is called in case of server
**                  disconnection (e.g. server crashes)
**
** Parameters
**
** Returns          void
**
*******************************************************************************/
static BOOLEAN app_pbc_mgt_callback(tBSA_MGT_EVT event, tBSA_MGT_MSG *p_data)
{
    switch(event)
    {
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
** Function         app_pbc_evt_cback
**
** Description      Example of PBC callback function to handle events related to PBC
**
** Parameters
**
** Returns          void
**
*******************************************************************************/
static void app_pbc_evt_cback(tBSA_PBC_EVT event, tBSA_PBC_MSG *p_data)
{
    switch (event)
    {
    case BSA_PBC_OPEN_EVT: /* Connection Open */
        if(p_data->open.status == BSA_SUCCESS)
        {
            peer_features = p_data->open.peer_features;
        }
        break;

    default:
        break;
    }
}

/*******************************************************************************
**
** Function         app_pbc_menu_open
**
** Description      Example of function to open up a connection to peer
**
** Parameters
**
** Returns          void
**
*******************************************************************************/
static tBSA_STATUS app_pbc_menu_open(void)
{
    int     device_index;
    BD_ADDR bd_addr;

    APP_INFO0("Bluetooth PBC menu:");
    APP_INFO0("    0 Device from XML database (already paired)");
    APP_INFO0("    1 Device found in last discovery");
    device_index = app_get_choice("Select source");
    switch (device_index)
    {
    case 0 :
        /* Devices from XML database */
        /* Read the Remote device xml file to have a fresh view */
        app_read_xml_remote_devices();
        app_xml_display_devices(app_xml_remote_devices_db,
            APP_NUM_ELEMENTS(app_xml_remote_devices_db));
        device_index = app_get_choice("Select device");
        if ((device_index >= APP_NUM_ELEMENTS(app_xml_remote_devices_db)) || (device_index < 0))
        {
            APP_ERROR0("Wrong Remote device index");
            return BSA_SUCCESS;
        }

        if (app_xml_remote_devices_db[device_index].in_use == FALSE)
        {
            APP_ERROR0("Wrong Remote device index");
            return BSA_SUCCESS;
        }
        bdcpy(bd_addr, app_xml_remote_devices_db[device_index].bd_addr);
        break;
    case 1 :
        /* Devices from Discovery */
        app_disc_display_devices();
        device_index = app_get_choice("Select device");
        if ((device_index >= APP_DISC_NB_DEVICES) || (device_index < 0))
        {
            APP_ERROR0("Wrong Remote device index");
            return BSA_SUCCESS;
        }
        if (app_discovery_cb.devs[device_index].in_use == FALSE)
        {
            APP_ERROR0("Wrong Remote device index");
            return BSA_SUCCESS;
        }
        bdcpy(bd_addr, app_discovery_cb.devs[device_index].device.bd_addr);
        break;
    default:
        APP_ERROR0("Wrong selection !!");
        return BSA_SUCCESS;
    }

    return app_pbc_open(bd_addr);
}

/*******************************************************************************
**
** Function         app_pbc_menu_close
**
** Description      Example of function to disconnect current connection
**
** Parameters
**
** Returns          void
**
*******************************************************************************/
static tBSA_STATUS app_pbc_menu_close(void)
{
    BD_ADDR bd_addr;

    if (app_pbc_choose_connected_devices(&bd_addr) == FALSE)
        return BSA_ERROR_CLI_BAD_PARAM;

    return app_pbc_close(bd_addr);
}

/*******************************************************************************
**
** Function         app_pbc_menu_abort
**
** Description      Example of function to abort current operation
**
** Parameters
**
** Returns          void
**
*******************************************************************************/
static tBSA_STATUS app_pbc_menu_abort(void)
{
    BD_ADDR bd_addr;

    if (app_pbc_choose_connected_devices(&bd_addr) == FALSE)
        return BSA_ERROR_CLI_BAD_PARAM;

    return app_pbc_abort(bd_addr);
}

/*******************************************************************************
**
** Function         app_pbc_menu_get_vcard
**
** Description      Example of function to perform a get VCard operation
**
** Parameters
**
** Returns          void
**
*******************************************************************************/
static tBSA_STATUS app_pbc_menu_get_vcard()
{
    char    file_name[50] = {0};
    int     file_len = 0;
    BD_ADDR bd_addr;

    if (app_pbc_choose_connected_devices(&bd_addr) == FALSE)
    {
        APP_ERROR0("app_pbc_menu_get_vcard cannot find connected device");
        return BSA_ERROR_CLI_BAD_PARAM;
    }

    file_len = app_get_string("Enter filename ", file_name, sizeof(file_name));
    if (file_len < 0)
    {
        APP_ERROR0("app_get_string failed");
        return BSA_ERROR_CLI_BAD_PARAM;
    }

    return app_pbc_get_vcard(bd_addr, file_name);
}

/*******************************************************************************
**
** Function         app_pbc_menu_get_list_vcards
**
** Description      Example of function to perform a GET - List VCards operation
**
** Parameters
**
** Returns          void
**
*******************************************************************************/
static tBSA_STATUS app_pbc_menu_get_list_vcards()
{
    UINT16                  max_list_count = 65535;
    tBSA_PBC_ORDER          order =  BSA_PBC_ORDER_INDEXED;
    tBSA_PBC_ATTR           attr = BSA_PBC_ATTR_NAME;
    BOOLEAN                 is_reset_missed_calls = FALSE;
    tBSA_PBC_FILTER_MASK    selector = BSA_PBC_FILTER_ALL;
    tBSA_PBC_OP             selector_op = BSA_PBC_OP_OR;
    int                     dir_len, search_len;
    char                    string_dir[20] = "telecom";
    char                    searchvalue[20] = "";
    BD_ADDR                 bd_addr;

    if (app_pbc_choose_connected_devices(&bd_addr) == FALSE)
    {
        APP_ERROR0("app_pbc_menu_get_list_vcards cannot find connected device");
        return BSA_ERROR_CLI_BAD_PARAM;
    }

    dir_len = app_get_string("Enter directory name", string_dir, sizeof(string_dir));
    if (dir_len < 0)
    {
        APP_ERROR0("app_get_string failed");
        return BSA_ERROR_CLI_BAD_PARAM;
    }

    search_len = app_get_string("Enter search value", searchvalue, sizeof(searchvalue));
    if (search_len < 0)
    {
        APP_ERROR0("app_get_string failed");
        return BSA_ERROR_CLI_BAD_PARAM;
    }

    if (peer_features & BSA_PBC_FEA_ENH_MISSED_CALLS)
    {
        APP_INFO0("Reset New Missed Calls?");
        APP_INFO0("    0 No");
        APP_INFO0("    1 Yes");
        is_reset_missed_calls = app_get_choice("Please select") == 0 ? FALSE : TRUE;
    }

    if (peer_features & BSA_PBC_FEA_VCARD_SELECTING)
    {
        APP_INFO0("List vCards with selected fields:");
        app_pbc_display_vcard_properties();
        selector = app_get_choice("Please enter selected fields (0 for all fields)");
        APP_INFO0("List vCards contain");
        APP_INFO0("    0 Any of selected fields");
        APP_INFO0("    1 All of selected fields");
        selector_op = app_get_choice("Please select");
    }

    return app_pbc_get_list_vcards(bd_addr, string_dir, order, attr, searchvalue, max_list_count,
        is_reset_missed_calls, selector, selector_op);
}

/*******************************************************************************
**
** Function         app_pbc_menu_get_phonebook
**
** Description      Example of function to perform a get phonebook operation
**
** Parameters
**
** Returns          void
**
*******************************************************************************/
static tBSA_STATUS app_pbc_menu_get_phonebook()
{
    BOOLEAN                 is_reset_missed_calls = FALSE;
    tBSA_PBC_FILTER_MASK    selector = BSA_PBC_FILTER_ALL;
    tBSA_PBC_OP             selector_op = BSA_PBC_OP_OR;

    char                    file_name[50] = {0};
    int                     file_len = 0;
    BD_ADDR                 bd_addr;

    if (app_pbc_choose_connected_devices(&bd_addr) == FALSE)
    {
        APP_ERROR0("app_pbc_menu_get_phonebook cannot find connected device");
        return BSA_ERROR_CLI_BAD_PARAM;
    }

    file_len = app_get_string("Enter filename like (telecom/pb.vcf)", file_name, sizeof(file_name));
    if (file_len < 0)
    {
        APP_ERROR0("app_get_string failed");
        return BSA_ERROR_CLI_BAD_PARAM;
    }

    if (peer_features & BSA_PBC_FEA_ENH_MISSED_CALLS)
    {
        APP_INFO0("Reset New Missed Calls?");
        APP_INFO0("    0 No");
        APP_INFO0("    1 Yes");
        is_reset_missed_calls = app_get_choice("Please select") == 0 ? FALSE : TRUE;
    }

    if (peer_features & BSA_PBC_FEA_VCARD_SELECTING)
    {
        APP_INFO0("Get vCards with selected fields:");
        app_pbc_display_vcard_properties();
        selector = app_get_choice("Please enter selected fields (0 for all fields)");
        APP_INFO0("Get vCards contain");
        APP_INFO0("    0 Any of selected fields");
        APP_INFO0("    1 All of selected fields");
        selector_op = app_get_choice("Please select");
    }

    return app_pbc_get_phonebook(bd_addr, file_name, is_reset_missed_calls, selector, selector_op);
}

/*******************************************************************************
**
** Function         app_pbc_menu_set_chdir
**
** Description      Example of function to perform a set current folder operation
**
** Parameters
**
** Returns          void
**
*******************************************************************************/
static tBSA_STATUS app_pbc_menu_set_chdir()
{
    char    string_dir[20] = {0};
    int     dir_len;
    BD_ADDR bd_addr;

    if (app_pbc_choose_connected_devices(&bd_addr) == FALSE)
    {
        APP_ERROR0("app_pbc_menu_set_chdir cannot find connected device");
        return BSA_ERROR_CLI_BAD_PARAM;
    }

    dir_len = app_get_string("Enter directory name", string_dir, sizeof(string_dir));
    if (dir_len <= 0)
    {
        APP_ERROR0("No valid directory name entered!");
        return BSA_ERROR_CLI_BAD_PARAM;
    }

    return app_pbc_set_chdir(bd_addr, string_dir);
}

/*******************************************************************************
**
** Function         main
**
** Description      This is the main function
**
** Parameters
**
** Returns          void
**
*******************************************************************************/
int main(int argc, char **argv)
{
    int status;
    int choice;

    /* Open connection to BSA Server */
    app_mgt_init();
    if (app_mgt_open(NULL, app_pbc_mgt_callback) < 0)
    {
        APP_ERROR0("Unable to connect to server");
        return -1;
    }

    /* Example of function to start PBC application */
    status = app_pbc_enable(app_pbc_evt_cback);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR0("main: Unable to start PBC");
        app_mgt_close();
        return status;
    }

    do
    {
        app_pbc_display_main_menu();
        choice = app_get_choice("Select action");

        APPL_TRACE_DEBUG1("PBC Main Choise: %d", choice);

        switch (choice)
        {
        case APP_PBC_MENU_NULL:
            break;

        case APP_PBC_MENU_OPEN:
            app_pbc_menu_open();
            break;

        case APP_PBC_MENU_GET_VCARD:
            app_pbc_menu_get_vcard();
            break;

        case APP_PBC_MENU_GET_LISTVCARDS:
            app_pbc_menu_get_list_vcards();
            break;

        case APP_PBC_MENU_GET_PHONEBOOK:
            app_pbc_menu_get_phonebook();
            break;

        case APP_PBC_MENU_SET_CHDIR:
            app_pbc_menu_set_chdir();
            break;

        case APP_PBC_MENU_CLOSE:
            app_pbc_menu_close();
            break;

        case APP_PBC_MENU_ABORT:
            app_pbc_menu_abort();
            break;

        case APP_PBC_MENU_DISC:
            /* Example to perform Device discovery (in blocking mode) */
            app_disc_start_regular(NULL, 0);
            break;

        case APP_PBC_MENU_QUIT:
            break;

        default:
            APP_ERROR1("main: Unknown choice:%d", choice);
            break;
        }
    } while (choice != APP_PBC_MENU_QUIT); /* While user don't exit application */

    /* example to stop PBC service */
    app_pbc_disable();

    /* Just to make sure we are getting the disable event before close the
    connection to the manager */
    sleep(2);

    /* Close BSA Connection before exiting (to release resources) */
    app_mgt_close();

    return 0;
}
