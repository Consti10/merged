/*****************************************************************************
**
**  Name:           app_pbc.c
**
**  Description:    Bluetooth Phonebook client sample application
**
**  Copyright (c) 2013, Broadcom Corp., All Rights Reserved.
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

#define MAX_PATH_LENGTH         512
#define DEFAULT_SEC_MASK        (BSA_SEC_AUTHENTICATION | BSA_SEC_ENCRYPTION)

#define APP_PBC_FEATURES        (BSA_PBC_FEA_DOWNLOADING | BSA_PBC_FEA_BROWSING | \
                                BSA_PBC_FEA_DATABASE_ID | BSA_PBC_FEA_FOLDER_VER_COUNTER | \
                                BSA_PBC_FEA_VCARD_SELECTING | BSA_PBC_FEA_ENH_MISSED_CALLS | \
                                BSA_PBC_FEA_UCI_VCARD_FIELD | BSA_PBC_FEA_UID_VCARD_FIELD | \
                                BSA_PBC_FEA_CONTACT_REF | BSA_PBC_FEA_DEF_CONTACT_IMAGE_FORMAT)

#define APP_PBC_SETPATH_ROOT    "/"
#define APP_PBC_SETPATH_UP      ".."

/*
 * Types
 */
typedef struct
{
    BOOLEAN             is_open;
    volatile BOOLEAN    open_pending; /* Indicate that there is an open pending */
    BD_ADDR             peer_addr;
    BOOLEAN             is_channel_open;
    tUIPC_CH_ID         uipc_pbc_channel;
    BOOLEAN             remove;
} tAPP_PBC_CONN;


typedef struct
{
    tAPP_PBC_CONN       conn[APP_PBC_MAX_CONN];
    tPbcCallback        *s_pPbcCallback;
} tAPP_PBC_CB;

/*
 * Global Variables
 */
tAPP_PBC_CB app_pbc_cb;

/*******************************************************************************
**
** Function         app_pbc_get_conn_by_bd_addr
**
** Description      Get the connection control block by BD address
**
** Parameters       bd_addr - BD address of the device
**
** Returns          tAPP_PBC_CONN *
**
*******************************************************************************/
static tAPP_PBC_CONN *app_pbc_get_conn_by_bd_addr(BD_ADDR bd_addr)
{
    tAPP_PBC_CONN *p_conn = NULL;
    UINT8 i;

    for (i = 0; i < APP_PBC_MAX_CONN; i++)
    {
        if (!bdcmp(app_pbc_cb.conn[i].peer_addr, bd_addr))
        {
            p_conn = &app_pbc_cb.conn[i];
            break;
        }
    }

    if (p_conn == NULL)
    {
        APP_INFO0("app_pbc_get_conn_by_bd_addr could not find connection");
    }

    return p_conn;
}

/*******************************************************************************
**
** Function         app_pbc_get_conn_by_uipc_channel_id
**
** Description      Get the connection control block by UIPC channel id
**
** Parameters       channel_id - UIPC channel ID
**
** Returns          tAPP_PBC_CONN *
**
*******************************************************************************/
static tAPP_PBC_CONN *app_pbc_get_conn_by_uipc_channel_id(tUIPC_CH_ID channel_id)
{
    tAPP_PBC_CONN *p_conn = NULL;
    UINT8 i;

    for (i = 0 ; i < APP_PBC_MAX_CONN; i++)
    {
        if (app_pbc_cb.conn[i].uipc_pbc_channel == channel_id)
        {
            p_conn = &app_pbc_cb.conn[i];
            break;
        }
    }

    if (p_conn == NULL)
    {
        APP_INFO0("app_pbc_get_conn_by_uipc_channel_id could not find connection");
    }

    return p_conn;
}

/*******************************************************************************
**
** Function         app_pbc_get_available_conn
**
** Description      Get an available connection control block
**
** Parameters       bd_addr - BD address of the device
**
** Returns          tAPP_PBC_CONN *
**
*******************************************************************************/
static tAPP_PBC_CONN *app_pbc_get_available_conn(BD_ADDR bd_addr)
{
    tAPP_PBC_CONN *p_conn = NULL;
    UINT8 i;

    for (i = 0; i < APP_PBC_MAX_CONN; i++)
    {
        if ((bdcmp(app_pbc_cb.conn[i].peer_addr, bd_addr) != 0) &&
            (app_pbc_cb.conn[i].is_open == FALSE) && (app_pbc_cb.conn[i].open_pending == FALSE))
        {
            p_conn = &app_pbc_cb.conn[i];
            break;
        }
    }

    if (p_conn == NULL)
    {
        APP_INFO0("app_pbc_get_available_conn could not find empty entry");
    }

    return p_conn;
}

/*******************************************************************************
 **
 ** Function        app_pbc_uipc_cback
 **
 ** Description     uipc pbc call back function.
 **
 ** Parameters      pointer on a buffer containing Phonebook data with a BT_HDR header
 **
 ** Returns          void
 **
 *******************************************************************************/
static void app_pbc_uipc_cback(BT_HDR *p_msg)
{
    tAPP_PBC_CONN   *p_conn;
    UINT8           *p_buffer = NULL;
    int             dummy = 0;
    int             fd = -1;
    static char     path[260];
    static char     file_name[100];
    tUIPC_CH_ID     uipc_channel;
    UINT8           channel_offset;

    APP_INFO1("app_pbc_uipc_cback response received... p_msg = %x", p_msg);

    if (p_msg == NULL)
    {
        return;
    }

    uipc_channel = p_msg->layer_specific;
    if (((p_conn = app_pbc_get_conn_by_uipc_channel_id(uipc_channel)) == NULL) ||
        (uipc_channel < UIPC_CH_ID_PBC_FIRST) || (uipc_channel > UIPC_CH_ID_PBC_LAST))
    {
        APP_ERROR1("app_pbc_uipc_cback response received...event = %d, uipc channel = %d", p_msg->event, uipc_channel);
        GKI_freebuf(p_msg);
        return;
    }

    /* Make sure we are only handling UIPC_RX_DATA_EVT and other UIPC events. Ignore UIPC_OPEN_EVT and UIPC_CLOSE_EVT */
    if(p_msg->event != UIPC_RX_DATA_EVT)
    {
        APP_ERROR1("app_pbc_uipc_cback response received...event = %d",p_msg->event);
        GKI_freebuf(p_msg);
        return;
    }

    APP_INFO1("app_pbc_uipc_cback response received...event = %d, msg len = %d",p_msg->event, p_msg->len);

    /* Check msg len */
    if(!(p_msg->len))
    {
        APP_INFO0("app_pbc_uipc_cback response received. DATA Len = 0");
        GKI_freebuf(p_msg);
        return;
    }

    /* Get to the data buffer */
    p_buffer = ((UINT8 *)(p_msg + 1)) + p_msg->offset;

    channel_offset = uipc_channel - UIPC_CH_ID_PBC_FIRST;
    /* Write received buffer to file */
    memset(path, 0, sizeof(path));
    memset(file_name, 0, sizeof(file_name));
    if (!getcwd(path, 260))
    {
        strcat(path, ".");
    }
    sprintf(file_name, "/pb_data_%d.vcf", channel_offset);
    strcat(path, file_name);

    APP_INFO1("app_pbc_uipc_cback file path = %s", path);

    /* Delete temporary file */
    if(p_conn->remove)
    {
        unlink(path);
        p_conn->remove = FALSE;
    }

    /* Save incoming data in temporary file */
    fd = open(path, O_WRONLY | O_CREAT | O_APPEND, 0666);
    APP_INFO1("app_pbc_uipc_cback fd = %d", fd);

    if(fd != -1)
    {
        dummy = write(fd, p_buffer, p_msg->len);
        APP_INFO1("app_pbc_uipc_cback dummy = %d err = %d", dummy, errno);
        (void)dummy;
        close(fd);
    }

    /* APP_DUMP("PBC Data", p_buffer, p_msg->len); */
    GKI_freebuf(p_msg);
}

/*******************************************************************************
**
** Function         app_pbc_handle_list_evt
**
** Description      Example of list event callback function
**
** Parameters
**
** Returns          void
**
*******************************************************************************/
static void app_pbc_handle_list_evt(tBSA_PBC_LIST_MSG *p_data)
{
    UINT8   *p_buf = p_data->data;
    UINT16  Index;

    if(p_data->is_xml)
    {
        p_buf[p_data->len] = 0;
        APP_INFO1("%s", p_buf);
    }
    else
    {
        for (Index = 0;Index < p_data->num_entry * 2; Index++)
        {
            APP_INFO1("%s", p_buf);
            p_buf += 1+strlen((char *) p_buf);
        }
    }

    APP_INFO1("BSA_PBC_LIST_EVT num entry %d,is final %d, xml %d, len %d",
        p_data->num_entry, p_data->final, p_data->is_xml, p_data->len);
}

/*******************************************************************************
**
** Function         app_pbc_auth
**
** Description      Respond to an Authorization Request
**
** Parameters
**
** Returns          void
**
*******************************************************************************/
static void app_pbc_auth(tBSA_PBC_MSG *p_data)
{
    tBSA_PBC_AUTHRSP auth_cmd;

    BSA_PbcAuthRspInit(&auth_cmd);

    bdcpy(auth_cmd.bd_addr, p_data->auth.bd_addr);

    /* Provide userid and password for authorization */
    strcpy(auth_cmd.password, "0000");
    strcpy(auth_cmd.userid, "guest");

    BSA_PbcAuthRsp(&auth_cmd);

    /*
    OR Call BSA_PbcClose() to close the connection
    */
}

/*******************************************************************************
**
** Function         app_pbc_get_evt
**
** Description      Response events received in response to GET command
**
** Parameters
**
** Returns          void
**
*******************************************************************************/
static void app_pbc_get_evt(tBSA_PBC_MSG *p_data)
{
    APP_INFO1("app_pbc_get_evt GET EVT response received... p_data = %x, type = %x", p_data, p_data->get.type);

    if(!p_data)
        return;

    switch(p_data->get.type)
    {
    case BSA_PBC_GET_PARAM_STATUS:          /* status only*/
        break;

    case BSA_PBC_GET_PARAM_LIST:            /* List message */
        app_pbc_handle_list_evt(&(p_data->get.param.list));
        break;

    case BSA_PBC_GET_PARAM_PROGRESS:        /* Progress Message*/
        break;

    case BSA_PBC_GET_PARAM_PHONEBOOK:       /* Phonebook paramm - Indicates Phonebook transfer complete */
        APP_INFO0("app_pbc_get_evt GET EVT  BSA_PBC_GET_PARAM_PHONEBOOK received");
        break;

    case BSA_PBC_GET_PARAM_FILE_TRANSFER_STATUS:    /* File transfer status */
        APP_INFO0("app_pbc_get_evt GET EVT  BSA_PBC_GET_PARAM_FILE_TRANSFER_STATUS received");
        break;

    default:
        break;
    }
}

/*******************************************************************************
**
** Function         app_pbc_set_evt
**
** Description      Response events received in response to SET command
**
** Parameters
**
** Returns          void
**
*******************************************************************************/
static void app_pbc_set_evt(tBSA_PBC_MSG *p_data)
{
    APP_INFO0("app_pbc_set_evt SET EVT response received...");
}

/*******************************************************************************
**
** Function         app_pbc_abort_evt
**
** Description      Response events received in response to ABORT command
**
** Parameters
**
** Returns          void
**
*******************************************************************************/
static void app_pbc_abort_evt(tBSA_PBC_MSG *p_data)
{
    APP_INFO0("app_pbc_abort_evt received...");
}

/*******************************************************************************
**
** Function         app_pbc_open_evt
**
** Description      Response events received in response to OPEN command
**
** Parameters
**
** Returns          void
**
*******************************************************************************/
static void app_pbc_open_evt(tBSA_PBC_MSG *p_data)
{
    tAPP_PBC_CONN *p_conn = NULL;

    APP_INFO0("app_pbc_open_evt received...");

    if ((p_conn = app_pbc_get_conn_by_bd_addr(p_data->open.bd_addr)) == NULL)
    {
        APP_ERROR0("Could not find connection control block");
        return;
    }

    if (p_data->open.status == BSA_SUCCESS)
    {
        p_conn->is_open = TRUE;
    }
    else
    {
        p_conn->is_open = FALSE;
        bdcpy(p_conn->peer_addr, bd_addr_null);
    }

    p_conn->open_pending = FALSE;

    if(p_data->open.status == BSA_SUCCESS && !p_conn->is_channel_open )
    {
        /* Once connected to PBAP Server, UIPC channel is returned. */
        /* Open the channel and register a callback to get phone book data. */
        /* Save UIPC channel */
        p_conn->uipc_pbc_channel = p_data->open.uipc_channel;
        p_conn->is_channel_open = TRUE;

        /* open the UIPC channel to receive data */
        if (UIPC_Open(p_data->open.uipc_channel, app_pbc_uipc_cback) == FALSE)
        {
            p_conn->is_channel_open = FALSE;
            APP_ERROR0("Unable to open UIPC channel");
            return;
        }
    }
}

/*******************************************************************************
**
** Function         app_pbc_close_evt
**
** Description      Response events received in response to CLOSE command
**
** Parameters
**
** Returns          void
**
*******************************************************************************/
static void app_pbc_close_evt(tBSA_PBC_MSG *p_data)
{
    tAPP_PBC_CONN *p_conn = NULL;

    APP_INFO1("app_pbc_close_evt received... Reason = %d", p_data->close.status);

    if ((p_conn = app_pbc_get_conn_by_bd_addr(p_data->close.bd_addr)) == NULL)
    {
        APP_ERROR0("Could not find connection control block");
        return;
    }

    p_conn->is_open = FALSE;
    p_conn->open_pending = FALSE;
    bdcpy(p_conn->peer_addr, bd_addr_null);

    if(p_conn->is_channel_open)
    {
        /* close the UIPC channel receiving data */
        UIPC_Close(p_conn->uipc_pbc_channel);
        p_conn->uipc_pbc_channel = UIPC_CH_ID_BAD;
        p_conn->is_channel_open = FALSE;
    }
}

/*******************************************************************************
**
** Function         app_pbc_cback
**
** Description      Example of PBC callback function to handle events related to PBC
**
** Parameters
**
** Returns          void
**
*******************************************************************************/
static void app_pbc_cback(tBSA_PBC_EVT event, tBSA_PBC_MSG *p_data)
{
    switch (event)
    {
    case BSA_PBC_OPEN_EVT: /* Connection Open (for info) */
        APP_INFO1("BSA_PBC_OPEN_EVT Status %d", p_data->open.status);
        app_pbc_open_evt(p_data);
        break;

    case BSA_PBC_CLOSE_EVT: /* Connection Closed (for info)*/
        APP_INFO0("BSA_PBC_CLOSE_EVT");
        app_pbc_close_evt(p_data);
        break;

    case BSA_PBC_DISABLE_EVT: /* PBAP Client module disabled*/
        APP_INFO0("BSA_PBC_DISABLE_EVT");
        break;

    case BSA_PBC_AUTH_EVT: /* Authorization */
        APP_INFO0("BSA_PBC_AUTH_EVT");
        app_pbc_auth(p_data);
        break;

    case BSA_PBC_SET_EVT:
        APP_INFO1("BSA_PBC_SET_EVT status %d", p_data->set.status);
        app_pbc_set_evt(p_data);
        break;

    case BSA_PBC_GET_EVT:
        APP_INFO1("BSA_PBC_GET_EVT status %d", p_data->get.status);
        app_pbc_get_evt(p_data);
        break;

    case BSA_PBC_ABORT_EVT:
        APP_INFO0("BSA_PBC_ABORT_EVT");
        app_pbc_abort_evt(p_data);
        break;

    default:
        APP_ERROR1("app_pbc_cback unknown event:%d", event);
        break;
    }

    /* forward the callback to registered applications */
    if(app_pbc_cb.s_pPbcCallback)
        app_pbc_cb.s_pPbcCallback(event, p_data);
}


/*******************************************************************************
**
** Function         app_pbc_enable
**
** Description      Example of function to enable PBC functionality
**
** Parameters
**
** Returns          0 if successful, error code otherwise
**
*******************************************************************************/
int app_pbc_enable(tPbcCallback pcb)
{
    tBSA_STATUS     status = BSA_SUCCESS;
    tBSA_PBC_ENABLE enable_param;
    UINT8           i;

    APP_INFO0("app_pbc_enable");

    /* Initialize the control structure */
    memset(&app_pbc_cb, 0, sizeof(app_pbc_cb));
    app_pbc_cb.s_pPbcCallback = pcb;

    for (i = 0; i < APP_PBC_MAX_CONN; i++)
    {
        app_pbc_cb.conn[i].uipc_pbc_channel = UIPC_CH_ID_BAD;
    }

    status = BSA_PbcEnableInit(&enable_param);

    enable_param.p_cback = app_pbc_cback;
    enable_param.local_features = APP_PBC_FEATURES;

    status = BSA_PbcEnable(&enable_param);
    APP_INFO1("app_pbc_enable Status: %d", status);

    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("app_pbc_enable: Unable to enable PBC status:%d", status);
    }
    return status;
}

/*******************************************************************************
**
** Function         app_pbc_disable
**
** Description      Example of function to stop PBC functionality
**
** Parameters
**
** Returns          0 if successful, error code otherwise
**
*******************************************************************************/
tBSA_STATUS app_pbc_disable(void)
{
    tBSA_PBC_DISABLE    disable_param;
    tBSA_STATUS         status;
    UINT8               i;

    APP_INFO0("app_stop_pbc");

    for (i = 0; i < APP_PBC_MAX_CONN; i++)
    {
        if(app_pbc_cb.conn[i].is_channel_open)
        {
            /* close the UIPC channel receiving data */
            UIPC_Close(app_pbc_cb.conn[i].uipc_pbc_channel);
            app_pbc_cb.conn[i].uipc_pbc_channel = UIPC_CH_ID_BAD;
            app_pbc_cb.conn[i].is_channel_open = FALSE;
        }
    }

    status = BSA_PbcDisableInit(&disable_param);
    status = BSA_PbcDisable(&disable_param);

    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("app_stop_pbc: Error:%d", status);
    }

    app_pbc_cb.s_pPbcCallback = NULL;

    return status;
}

/*******************************************************************************
**
** Function         app_pbc_open
**
** Description      Example of function to open up a connection to peer
**
** Parameters
**
** Returns          0 if successful, error code otherwise
**
*******************************************************************************/
tBSA_STATUS app_pbc_open(BD_ADDR bd_addr)
{
    tAPP_PBC_CONN   *p_conn;
    tBSA_STATUS     status = BSA_SUCCESS;
    tBSA_PBC_OPEN   param;

    APP_INFO0("app_pbc_open");

    if ((p_conn = app_pbc_get_available_conn(bd_addr)) == NULL)
        return BSA_ERROR_CLI_BAD_PARAM;

    bdcpy(p_conn->peer_addr, bd_addr);
    p_conn->open_pending = TRUE;

    BSA_PbcOpenInit(&param);

    bdcpy(param.bd_addr, bd_addr);
    param.sec_mask = DEFAULT_SEC_MASK;

    status = BSA_PbcOpen(&param);

    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("app_pbc_open: Error:%d", status);
        bdcpy(p_conn->peer_addr, bd_addr_null);
        p_conn->open_pending = FALSE;
    }
    return status;
}

/*******************************************************************************
**
** Function         app_pbc_close
**
** Description      Example of function to disconnect current connection
**
** Parameters       bd_addr     - BD address of the device
**
** Returns          0 if successful, error code otherwise
**
*******************************************************************************/
tBSA_STATUS app_pbc_close(BD_ADDR bd_addr)
{
    tAPP_PBC_CONN   *p_conn;
    tBSA_PBC_CLOSE  close_param;
    tBSA_STATUS     status;

    APP_INFO0("app_pbc_close");

    if ((p_conn = app_pbc_get_conn_by_bd_addr(bd_addr)) == NULL)
        return BSA_ERROR_CLI_BAD_PARAM;

    status = BSA_PbcCloseInit(&close_param);

    bdcpy(close_param.bd_addr, bd_addr);

    status = BSA_PbcClose(&close_param);

    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("app_pbc_close: Error:%d", status);
    }
    return status;
}

/*******************************************************************************
**
** Function         app_pbc_close_all
**
** Description      release all pbc connections
**
** Returns          0 if successful, error code otherwise
**
*******************************************************************************/
tBSA_STATUS app_pbc_close_all(void)
{
    tBSA_PBC_CLOSE  close_param;
    tBSA_STATUS     status;
    UINT16          cb_index;

    APPL_TRACE_EVENT0("app_pbc_close_all");

    for(cb_index = 0; cb_index < APP_PBC_MAX_CONN; cb_index++)
    {
        /* Prepare parameters */
        BSA_PbcCloseInit(&close_param);
        bdcpy(close_param.bd_addr, app_pbc_cb.conn[cb_index].peer_addr);


        status = BSA_PbcClose(&close_param);
        if (status != BSA_SUCCESS)
        {
            APP_ERROR1("failed with status : %d", status);
        }
    }

    return status;
}

/*******************************************************************************
**
** Function         app_pbc_abort
**
** Description      Example of function to abort current actions
**
** Parameters       bd_addr     - BD address of the device
**
** Returns          0 if successful, error code otherwise
**
*******************************************************************************/
tBSA_STATUS app_pbc_abort(BD_ADDR bd_addr)
{
    tAPP_PBC_CONN   *p_conn;
    tBSA_PBC_ABORT  abort_param;
    tBSA_STATUS     status;

    APP_INFO0("app_pbc_abort");

    if ((p_conn = app_pbc_get_conn_by_bd_addr(bd_addr)) == NULL)
        return BSA_ERROR_CLI_BAD_PARAM;

    status = BSA_PbcAbortInit(&abort_param);

    bdcpy(abort_param.bd_addr, bd_addr);

    status = BSA_PbcAbort(&abort_param);

    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("app_pbc_abort: Error:%d", status);
    }
    return status;
}

/*******************************************************************************
**
** Function         app_pbc_cancel
**
** Description      cancel connections
**
** Returns          0 if successful, error code otherwise
**
*******************************************************************************/
tBSA_STATUS app_pbc_cancel(BD_ADDR bd_addr)
{
    tAPP_PBC_CONN   *p_conn;
    tBSA_PBC_CANCEL cancel_param;
    tBSA_STATUS     status;

    APP_INFO0("app_pbc_cancel");

    if ((p_conn = app_pbc_get_conn_by_bd_addr(bd_addr)) == NULL)
        return BSA_ERROR_CLI_BAD_PARAM;

    /* Prepare parameters */
    BSA_PbcCancelInit(&cancel_param);

    bdcpy(cancel_param.bd_addr, bd_addr);

    status = BSA_PbcCancel(&cancel_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("app_pbc_cancel - failed with status : %d", status);
    }
    return status;
}

/*******************************************************************************
**
** Function         app_pbc_get_vcard
**
** Description      Example of function to perform a get VCard operation
**
** Parameters
**
** Returns          0 if successful, error code otherwise
**
*******************************************************************************/
tBSA_STATUS app_pbc_get_vcard(BD_ADDR bd_addr, const char* file_name)
{
    tAPP_PBC_CONN           *p_conn;
    tBSA_STATUS             status = 0;
    tBSA_PBC_GET            pbcGet;
    tBSA_PBC_FILTER_MASK    filter = BSA_PBC_FILTER_ALL;
    tBSA_PBC_FORMAT         format =  BSA_PBC_FORMAT_CARD_21;
    UINT16                  len = strlen(file_name) + 1;
    char                    *p_path;

    APP_INFO0("app_pbc_get_vcard");

    if ((p_conn = app_pbc_get_conn_by_bd_addr(bd_addr)) == NULL)
        return BSA_ERROR_CLI_BAD_PARAM;

    if ((p_path = (char *) GKI_getbuf(MAX_PATH_LENGTH + 1)) == NULL)
    {
        return FALSE;
    }

    memset(p_path, 0, MAX_PATH_LENGTH + 1);
    sprintf(p_path, "/%s", file_name); /* Choosing the root folder */
    APP_INFO1("GET vCard Entry p_path = %s,  file name = %s, len = %d",
        p_path, file_name, len);

    p_conn->remove = TRUE;

    status = BSA_PbcGetInit(&pbcGet);

    pbcGet.type = BSA_PBC_GET_CARD;
    bdcpy(pbcGet.bd_addr, bd_addr);
    strncpy(pbcGet.param.get_card.remote_name, file_name, BSA_PBC_FILENAME_MAX - 1);
    pbcGet.param.get_card.filter = filter;
    pbcGet.param.get_card.format = format;

    status = BSA_PbcGet(&pbcGet);

    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("app_pbc_get_vcard: Error:%d", status);
    }

    GKI_freebuf(p_path);

    return status;
}


/*******************************************************************************
**
** Function         app_pbc_get_list_vcards
**
** Description      Example of function to perform a GET - List VCards operation
**
** Parameters
**
** Returns          0 if successful, error code otherwise
**
*******************************************************************************/
tBSA_STATUS app_pbc_get_list_vcards(BD_ADDR bd_addr, const char* string_dir,
    tBSA_PBC_ORDER  order, tBSA_PBC_ATTR  attr, const char* searchvalue, UINT16 max_list_count,
    BOOLEAN is_reset_missed_calls, tBSA_PBC_FILTER_MASK selector, tBSA_PBC_OP selector_op)
{
    tAPP_PBC_CONN   *p_conn;
    UINT16          list_start_offset = 0;
    tBSA_STATUS     status = 0;
    tBSA_PBC_GET    pbcGet;
    int             is_xml = 1;

    APP_INFO0("app_pbc_get_list_vcards");

    if ((p_conn = app_pbc_get_conn_by_bd_addr(bd_addr)) == NULL)
        return BSA_ERROR_CLI_BAD_PARAM;

    status = BSA_PbcGetInit(&pbcGet);

    pbcGet.type = BSA_PBC_GET_LIST_CARDS;
    pbcGet.is_xml = is_xml;
    bdcpy(pbcGet.bd_addr, bd_addr);

    strncpy( pbcGet.param.list_cards.dir, string_dir, BSA_PBC_ROOT_PATH_LEN_MAX - 1);
    pbcGet.param.list_cards.order = order;
    strncpy(pbcGet.param.list_cards.value, searchvalue, BSA_PBC_MAX_VALUE_LEN - 1);
    pbcGet.param.list_cards.attribute =  attr;
    pbcGet.param.list_cards.max_list_count = max_list_count;
    pbcGet.param.list_cards.list_start_offset = list_start_offset;
    pbcGet.param.list_cards.is_reset_miss_calls = is_reset_missed_calls;
    pbcGet.param.list_cards.selector = selector;
    pbcGet.param.list_cards.selector_op = selector_op;

    status = BSA_PbcGet(&pbcGet);

    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("app_pbc_get_list_vcards: Error:%d", status);
    }

    return status;
}

/*******************************************************************************
**
** Function         app_pbc_get_phonebook
**
** Description      Example of function to perform a get phonebook operation
**
** Parameters
**
** Returns          0 if successful, error code otherwise
**
*******************************************************************************/
tBSA_STATUS app_pbc_get_phonebook(BD_ADDR bd_addr, const char* file_name,
    BOOLEAN is_reset_missed_calls, tBSA_PBC_FILTER_MASK selector, tBSA_PBC_OP selector_op)
{
    tAPP_PBC_CONN           *p_conn;
    tBSA_STATUS             status = 0;
    tBSA_PBC_GET            pbcGet;
    tBSA_PBC_FILTER_MASK    filter = BSA_PBC_FILTER_ALL;
    tBSA_PBC_FORMAT         format =  BSA_PBC_FORMAT_CARD_30;
    UINT16                  max_list_count = 65535;
    UINT16                  list_start_offset = 0;
    UINT16                  len = strlen(file_name) + 1;
    char                    *p_path;
    char                    *p = file_name;
    char                    *p_name = NULL;

    APP_INFO0("app_pbc_get_phonebook");

    if ((p_conn = app_pbc_get_conn_by_bd_addr(bd_addr)) == NULL)
        return BSA_ERROR_CLI_BAD_PARAM;

    do
    {
        /* double check the specified phonebook is correct */
        if(strncmp(p, "SIM1/", 5) == 0)
            p += 5;
        if(strncmp(p, "telecom/", 8) != 0)
            break;
        p += 8;
        p_name = p;
        if(strcmp(p, "pb.vcf") && strcmp(p, "fav.vcf") && strcmp(p, "spd.vcf"))
        {
            p++;
            if(strcmp(p, "ch.vcf") != 0) /* this covers rest of the ich,och,mch,cch */
                break;
        }

        if ((p_path = (char *) GKI_getbuf(MAX_PATH_LENGTH + 1)) == NULL)
        {
            break;
        }

        memset(p_path, 0, MAX_PATH_LENGTH + 1);
        sprintf(p_path, "/%s", p_name); /* Choosing the root folder */
        APP_INFO1("GET PhoneBook p_path = %s", p_path);
        APP_INFO1("file name = %s, len = %d", file_name, len);

        p_conn->remove = TRUE;

        status = BSA_PbcGetInit(&pbcGet);
        pbcGet.type = BSA_PBC_GET_PHONEBOOK;
        bdcpy(pbcGet.bd_addr, bd_addr);
        strncpy(pbcGet.param.get_phonebook.remote_name, file_name, BSA_PBC_MAX_REALM_LEN - 1);
        pbcGet.param.get_phonebook.filter = filter;
        pbcGet.param.get_phonebook.format = format;
        pbcGet.param.get_phonebook.max_list_count = max_list_count;
        pbcGet.param.get_phonebook.list_start_offset = list_start_offset;
        pbcGet.param.get_phonebook.is_reset_miss_calls = is_reset_missed_calls;
        pbcGet.param.get_phonebook.selector = selector;
        pbcGet.param.get_phonebook.selector_op = selector_op;

        status = BSA_PbcGet(&pbcGet);

        if (status != BSA_SUCCESS)
        {
            APP_ERROR1("app_pbc_get_phonebook: Error:%d", status);
        }

        GKI_freebuf(p_path);

    } while(0);

    return status;
}

/*******************************************************************************
**
** Function         app_pbc_set_chdir
**
** Description      Example of function to perform a set current folder operation
**
** Parameters
**
** Returns          0 if successful, error code otherwise
**
*******************************************************************************/
tBSA_STATUS app_pbc_set_chdir(BD_ADDR bd_addr, const char *string_dir)
{
   tAPP_PBC_CONN    *p_conn;
   tBSA_PBC_SET     set_param;
   tBSA_STATUS      status = BSA_SUCCESS;

    APP_INFO1("app_pbc_set_chdir: path= %s", string_dir);

    if ((p_conn = app_pbc_get_conn_by_bd_addr(bd_addr)) == NULL)
        return BSA_ERROR_CLI_BAD_PARAM;

    status = BSA_PbcSetInit(&set_param);
    if (strcmp(string_dir, APP_PBC_SETPATH_ROOT) == 0)
    {
        set_param.param.ch_dir.flag = BSA_PBC_FLAG_NONE;
    }
    else if (strcmp(string_dir, APP_PBC_SETPATH_UP) == 0)
    {
        set_param.param.ch_dir.flag = BSA_PBC_FLAG_BACKUP;
    }
    else
    {
        strncpy(set_param.param.ch_dir.dir, string_dir, BSA_PBC_ROOT_PATH_LEN_MAX - 1);
        set_param.param.ch_dir.flag = BSA_PBC_FLAG_NONE;
    }
    bdcpy(set_param.bd_addr, bd_addr);
    status = BSA_PbcSet(&set_param);

    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("app_pbc_set_chdir: Error:%d", status);
    }
    return status;
}

/*******************************************************************************
**
** Function         app_pbc_choose_connected_devices
**
** Description      Display connected devices and select a device
**
** Parameters       bd addr to copy
**
** Returns          TRUE if found
**
*******************************************************************************/
BOOLEAN app_pbc_choose_connected_devices(BD_ADDR *addr)
{
    UINT8   i, device_index, num_conn = 0;
    int     choice;
    BOOLEAN is_found = FALSE;

    for (i = 0; i < APP_PBC_MAX_CONN; i++)
    {
        if(app_pbc_cb.conn[i].is_open)
        {
            num_conn++;
            APP_INFO1("Connection index %d:%02X:%02X:%02X:%02X:%02X:%02X",
                i,
                app_pbc_cb.conn[i].peer_addr[0], app_pbc_cb.conn[i].peer_addr[1],
                app_pbc_cb.conn[i].peer_addr[2], app_pbc_cb.conn[i].peer_addr[3],
                app_pbc_cb.conn[i].peer_addr[4], app_pbc_cb.conn[i].peer_addr[5]);
        }
    }

    if (num_conn)
    {
        APP_INFO0("Choose connection index");
        device_index = app_get_choice("\n");
        if ((device_index < APP_PBC_MAX_CONN) && (app_pbc_cb.conn[device_index].is_open == TRUE))
        {
            bdcpy(*addr, app_pbc_cb.conn[device_index].peer_addr);
            is_found = TRUE;
        }
        else
        {
            APP_INFO0("Unknown choice");
        }
    }
    else
    {
        APP_INFO0("No connections");
    }

    return is_found;
}

