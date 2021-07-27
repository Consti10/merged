/*****************************************************************************
**
**  Name:           app_ble.c
**
**  Description:    Bluetooth BLE application
**
**  Copyright (c) 2014, Broadcom Corp., All Rights Reserved.
**  Broadcom Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/

#include "app_ble.h"
#include "app_xml_utils.h"
#include "app_utils.h"
#include "app_mgt.h"
#include "app_disc.h"
#include "app_dm.h"
#include "app_ble_client_db.h"
#include "app_ble_client.h"
#include "app_ble_server.h"

/*
 * Global Variables
 */
tAPP_BLE_CB app_ble_cb;

/*
 * Local functions
 */
static int app_ble_config_bdaddr_for_wakeup(BD_ADDR tgt_addr);

static void app_ble_common_cback(tBSA_BLE_EVT event, tBSA_BLE_MSG *p_data);
/*
 * BLE common functions
 */


/*
 * Defines
 */
#define APP_BLE_ENABLE_WAKE_GPIO_VSC 0xFD60
#define APP_BLE_PF_MANU_DATA_CO_ID   0x000F  /* Company ID for BRCM */
#define APP_BLE_PF_ADDR_FILTER_COND_TYPE 0
#define APP_BLE_PF_MANU_DATA_COND_TYPE 5
 
/*******************************************************************************
 **
 ** Function         app_ble_display_service_name
 **
 ** Description      This function display the name of vendor dependent command
 **
 ** Returns          void
 **
 *******************************************************************************/
char *app_ble_display_service_name(UINT16 uuid)
{
    switch(uuid)
    {
    case BSA_BLE_UUID_SERVCLASS_GAP_SERVER:
        return ("UUID_SERVCLASS_GAP_SERVER");
    case BSA_BLE_UUID_SERVCLASS_GATT_SERVER:
        return ("UUID_SERVCLASS_GATT_SERVER");
    case BSA_BLE_UUID_SERVCLASS_IMMEDIATE_ALERT:
        return("UUID_SERVCLASS_IMMEDIATE_ALERT");
    case BSA_BLE_UUID_SERVCLASS_LINKLOSS:
        return ("UUID_SERVCLASS_LINKLOSS");
    case BSA_BLE_UUID_SERVCLASS_TX_POWER:
        return ("UUID_SERVCLASS_TX_POWER");
    case BSA_BLE_UUID_SERVCLASS_CURRENT_TIME:
        return ("UUID_SERVCLASS_CURRENT_TIME");
    case BSA_BLE_UUID_SERVCLASS_DST_CHG:
        return ("UUID_SERVCLASS_DST_CHG");
    case BSA_BLE_UUID_SERVCLASS_REF_TIME_UPD:
        return ("UUID_SERVCLASS_REF_TIME_UPD");
    case BSA_BLE_UUID_SERVCLASS_GLUCOSE:
        return ("UUID_SERVCLASS_GLUCOSE");
    case BSA_BLE_UUID_SERVCLASS_HEALTH_THERMOMETER:
        return ("UUID_SERVCLASS_HEALTH_THERMOMETER");
    case BSA_BLE_UUID_SERVCLASS_DEVICE_INFORMATION:
        return ("UUID_SERVCLASS_DEVICE_INFORMATION");
    case BSA_BLE_UUID_SERVCLASS_NWA:
        return ("UUID_SERVCLASS_NWA");
    case BSA_BLE_UUID_SERVCLASS_PHALERT:
        return ("UUID_SERVCLASS_PHALERT");
    case BSA_BLE_UUID_SERVCLASS_HEART_RATE:
        return ("UUID_SERVCLASS_HEART_RATE");
    case BSA_BLE_UUID_SERVCLASS_BATTERY_SERVICE:
        return ("UUID_SERVCLASS_BATTERY_SERVICE");
    case BSA_BLE_UUID_SERVCLASS_BLOOD_PRESSURE:
        return ("UUID_SERVCLASS_BLOOD_PRESSURE");
    case BSA_BLE_UUID_SERVCLASS_ALERT_NOTIFICATION_SERVICE:
        return ("UUID_SERVCLASS_ALERT_NOTIFICATION_SERVICE");
    case BSA_BLE_UUID_SERVCLASS_HUMAN_INTERFACE_DEVICE:
        return ("UUID_SERVCLASS_HUMAN_INTERFACE_DEVICE");
    case BSA_BLE_UUID_SERVCLASS_SCAN_PARAMETERS:
        return ("UUID_SERVCLASS_SCAN_PARAMETERS");
    case BSA_BLE_UUID_SERVCLASS_RUNNING_SPEED_AND_CADENCE:
        return ("UUID_SERVCLASS_RUNNING_SPEED_AND_CADENCE");
    case BSA_BLE_UUID_SERVCLASS_CYCLING_SPEED_AND_CADENCE:
        return ("UUID_SERVCLASS_CYCLING_SPEED_AND_CADENCE");
    case BSA_BLE_UUID_SERVCLASS_TEST_SERVER:
        return ("UUID_SERVCLASS_TEST_SERVER");

    default:
        break;
    }

    return ("UNKNOWN");
}

/*******************************************************************************
 **
 ** Function        app_ble_init
 **
 ** Description     This is the main init function
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_init(void)
{
    int index;

    memset(&app_ble_cb, 0, sizeof(app_ble_cb));

    for (index = 0;index < BSA_BLE_CLIENT_MAX;index++)
    {
        app_ble_cb.ble_client[index].client_if = BSA_BLE_INVALID_IF;
        app_ble_cb.ble_client[index].conn_id = BSA_BLE_INVALID_CONN;
    }

    for (index = 0;index < BSA_BLE_SERVER_MAX;index++)
    {
        app_ble_cb.ble_server[index].server_if = BSA_BLE_INVALID_IF;
        app_ble_cb.ble_server[index].conn_id = BSA_BLE_INVALID_CONN;
    }

    app_xml_init();

    app_ble_client_db_init();

    return 0;
}

/*******************************************************************************
 **
 ** Function        app_ble_exit
 **
 ** Description     This is the ble exit function
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_exit(void)
{
    tBSA_STATUS status;
    tBSA_BLE_DISABLE param;

    /* make device non discoverable & non connectable */
    app_dm_set_ble_visibility(FALSE, FALSE);

    /* Deregister all applications */
    app_ble_client_deregister_all();

    BSA_BleDisableInit(&param);

    status = BSA_BleDisable(&param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("Unable to disable BLE status:%d", status);
        return -1;
    }

    return 0;
}

/*******************************************************************************
 **
 ** Function        app_ble_start
 **
 ** Description     DTV Start function
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_start(void)
{
    tBSA_STATUS status;
    tBSA_BLE_ENABLE enable_param;

    APP_INFO0("app_ble_start");

    BSA_BleEnableInit(&enable_param);

    enable_param.p_cback = app_ble_common_cback;

    status = BSA_BleEnable(&enable_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("Unable to enable BLE status:%d", status);
        return -1;
    }

    return 0;
}

/*******************************************************************************
 **
 ** Function        app_ble_wake_configure
 **
 ** Description     Configure for Wake on BLE
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_wake_configure(void)
{
    tBSA_BLE_WAKE_CFG bsa_ble_wake_cfg;
    tBSA_BLE_WAKE_ENABLE bsa_ble_wake_enable;
    tBSA_STATUS bsa_status;
    tBSA_TM_VSC bsa_vsc;
    char manu_pattern[] = "WAKEUP";
    int device_index;
    int choice;

    /* Configure Manufacturer data pattern */
    bsa_status = BSA_BleWakeCfgInit(&bsa_ble_wake_cfg);

    bsa_ble_wake_cfg.cond.manu_data.company_id = APP_BLE_PF_MANU_DATA_CO_ID; 
    memcpy(bsa_ble_wake_cfg.cond.manu_data.pattern, manu_pattern,sizeof(manu_pattern));
    bsa_ble_wake_cfg.cond.manu_data.data_len = sizeof(manu_pattern) - 1;
    bsa_ble_wake_cfg.cond_type = APP_BLE_PF_MANU_DATA_COND_TYPE;

    bsa_status = BSA_BleWakeCfg(&bsa_ble_wake_cfg);
    if (bsa_status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_BleWakeCfg failed status:%d", bsa_status);
        return(-1);
    }

    /* Configure BDADDR of remote device allowed to do wake-up */
    do {

        choice = app_get_choice("Configure a BLE BDADDR to wake-up Host? 0:No, 1:Yes");
        if (choice == 1)
        {
            /* Read the XML file which contains all the bonded devices */
            app_read_xml_remote_devices();
            app_xml_display_devices(app_xml_remote_devices_db,
                  APP_NUM_ELEMENTS(app_xml_remote_devices_db));
            device_index = app_get_choice("Select device");
            if ((device_index >= 0) &&
                (device_index < APP_NUM_ELEMENTS(app_xml_remote_devices_db)) &&
                (app_xml_remote_devices_db[device_index].in_use != FALSE))
            {
                /* Configure BDADDR of additional remote device allowed to do wake-up */
                bsa_status = app_ble_config_bdaddr_for_wakeup(app_xml_remote_devices_db[device_index].bd_addr);
                if (bsa_status != BSA_SUCCESS)
                {
                    APP_ERROR1("Config 2nd remote bdaddr failed status:%d", bsa_status);
                    return(-1);
                }
            }
        }
    }while (choice == 1);

    /* Enable Wake on BLE */
    bsa_status = BSA_BleWakeEnableInit(&bsa_ble_wake_enable);
    bsa_ble_wake_enable.enable = TRUE;
    bsa_status = BSA_BleWakeEnable(&bsa_ble_wake_enable);
    if (bsa_status != BSA_SUCCESS)
    {
        APP_ERROR1("BSA_BleWake failed status:%d", bsa_status);
        return(-1);
    }

    bsa_status = BSA_TmVscInit(&bsa_vsc);
    bsa_vsc.opcode = APP_BLE_ENABLE_WAKE_GPIO_VSC;
    bsa_vsc.length = 1;
    bsa_vsc.data[0] = 1; /* 1=ENABLE 0=DISABLE */

    bsa_status = BSA_TmVsc(&bsa_vsc);
    if (bsa_status != BSA_SUCCESS)
    {
        APP_ERROR1("Unable to Send Enable Wake on BLE GPIO VSC status:%d", bsa_status);
        return(-1);
    }

    return 0;
}

/*******************************************************************************
 **
 ** Function        app_ble_config_bdaddr_for_wakeup
 **
 ** Description     Configure bdaddr of remote device allowed to do wakeup
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
static int app_ble_config_bdaddr_for_wakeup(BD_ADDR tgt_addr)
{
    tBSA_BLE_WAKE_CFG bsa_ble_wake_cfg;
    tBSA_STATUS bsa_status;

    bsa_status = BSA_BleWakeCfgInit(&bsa_ble_wake_cfg);
    memcpy(&bsa_ble_wake_cfg.cond.target_addr.bda, tgt_addr,sizeof(BD_ADDR));
    bsa_ble_wake_cfg.cond.target_addr.type = 0;  /* BLE addr type = PUBLIC*/
    bsa_ble_wake_cfg.cond_type = APP_BLE_PF_ADDR_FILTER_COND_TYPE;

    bsa_status = BSA_BleWakeCfg(&bsa_ble_wake_cfg);
    if (bsa_status != BSA_SUCCESS)
    {
        APP_ERROR1("app_ble_config_bdaddr_for_wakeup: BSA_BleWakeCfg failed status:%d", bsa_status);
        return(-1);
    }

    return 0;
}

/*******************************************************************************
 **
 ** Function        app_ble_apcf_enable
 **
 ** Description     Enable APCF
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_apcf_enable(void)
{
    tBSA_BLE_APCF_ENABLE bsa_ble_apcf_enable;
    tBSA_STATUS bsa_status;

    BSA_BleApcfEnableInit(&bsa_ble_apcf_enable);
    bsa_ble_apcf_enable.enable = TRUE;
    bsa_status = BSA_BleApcfEnable(&bsa_ble_apcf_enable);
    if (bsa_status != BSA_SUCCESS)
    {
        APP_ERROR1("Unable to Enable APCF status:%d", bsa_status);
        return -1;
    }

    return 0;
}

/*******************************************************************************
 **
 ** Function        app_ble_apcf_disable
 **
 ** Description     Disable APCF
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_apcf_disable(void)
{
    tBSA_BLE_APCF_ENABLE bsa_ble_apcf_enable;
    tBSA_STATUS bsa_status;

    BSA_BleApcfEnableInit(&bsa_ble_apcf_enable);
    bsa_ble_apcf_enable.enable = FALSE;
    bsa_status = BSA_BleApcfEnable(&bsa_ble_apcf_enable);
    if (bsa_status != BSA_SUCCESS)
    {
        APP_ERROR1("Unable to Disable APCF status:%d", bsa_status);
        return -1;
    }

    return 0;
}

/*******************************************************************************
 **
 ** Function         app_ble_apcf_cond_type_menu
 **
 ** Description      This is the BLE APCF Condition Type menu
 **
 ** Returns          void
 **
 *******************************************************************************/
static void app_ble_apcf_cond_type_menu(void)
{
    APP_INFO1("\t%d => Set Filtering Parameters", BSA_BLE_APCF_FILTER_SETTING);
    APP_INFO1("\t%d => Broadcast Address", BSA_BLE_APCF_ADDR_FILTER);
    APP_INFO1("\t%d => Service UUID", BSA_BLE_APCF_SRVC_UUID);
    APP_INFO1("\t%d => Solicitation UUID", BSA_BLE_APCF_SRVC_SOL_UUID);
    APP_INFO1("\t%d => Local Name", BSA_BLE_APCF_LOCAL_NAME);
    APP_INFO1("\t%d => Manufacturer Data", BSA_BLE_APCF_MANU_DATA);
    APP_INFO1("\t%d => Service Data", BSA_BLE_APCF_SRVC_DATA);
}

/*******************************************************************************
 **
 ** Function        app_ble_apcf_cfg
 **
 ** Description     Config APCF
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_apcf_cfg(void)
{
    tBSA_BLE_APCF_CFG bsa_ble_apcf_cfg;
    tBSA_STATUS bsa_status;
    int i;
    tBSA_BLE_APCF_COND_TYPE cond_type;
    tBSA_BLE_APCF_COND_OP action;

    BSA_BleApcfCfgInit(&bsa_ble_apcf_cfg);

    bsa_ble_apcf_cfg.filter_index = app_get_choice("Enter Filtering Index (0 - 15):");
    if (bsa_ble_apcf_cfg.filter_index > 15) /* Current Max Filtering Index is 15 */
    {
        APP_ERROR1("Incorrect Filtering Index: %d", bsa_ble_apcf_cfg.filter_index);
        return -1;
    }
    app_ble_apcf_cond_type_menu();
    cond_type = app_get_choice("Select condition type:");
    action = app_get_choice("Enter Action(0:Add,1:Delete,2:Clear):");
    if (action > BSA_BLE_APCF_COND_CLEAR)
    {
        APP_ERROR1("Unknown action: %d", action);
        return -1;
    }

    if ((action != BSA_BLE_APCF_COND_CLEAR) &&
         !(action == BSA_BLE_APCF_COND_DELETE &&
            cond_type == BSA_BLE_APCF_FILTER_SETTING))
    {
        switch(cond_type)
        {
            case BSA_BLE_APCF_FILTER_SETTING:
                bsa_ble_apcf_cfg.cond.filter_setting.feat_seln =
                    app_get_choice("Enter Feature Selection:");
                bsa_ble_apcf_cfg.cond.filter_setting.logic_type =
                    app_get_choice("Enter Feature Logic Type:");
                bsa_ble_apcf_cfg.cond.filter_setting.filter_logic_type =
                    app_get_choice("Enter Filter Logic Type:");
                bsa_ble_apcf_cfg.cond.filter_setting.rssi_high_thres =
                    app_get_choice("Enter RSSI High Threshold:");
                break;

            case BSA_BLE_APCF_ADDR_FILTER:
                bsa_ble_apcf_cfg.cond.target_addr.type =
                    app_get_choice("Enter Broadcaster Address Type(0:Public,1:Random,2:NA):");
                if (bsa_ble_apcf_cfg.cond.target_addr.type > 2)
                {
                    APP_ERROR1("Incorrect  Broadcaster Address Type: %d",
                        bsa_ble_apcf_cfg.cond.target_addr.type);
                    return -1;
                }

                APP_INFO0("Enter the Broadcaster Address(AA.BB.CC.DD.EE.FF): ");
                if (scanf("%hhx.%hhx.%hhx.%hhx.%hhx.%hhx",
                    &bsa_ble_apcf_cfg.cond.target_addr.bda[0],
                    &bsa_ble_apcf_cfg.cond.target_addr.bda[1],
                    &bsa_ble_apcf_cfg.cond.target_addr.bda[2],
                    &bsa_ble_apcf_cfg.cond.target_addr.bda[3],
                    &bsa_ble_apcf_cfg.cond.target_addr.bda[4],
                    &bsa_ble_apcf_cfg.cond.target_addr.bda[5]) != 6)
                {
                    APP_ERROR0("BD address not entered correctly");
                    return -1;
                }
                break;

            case BSA_BLE_APCF_MANU_DATA:
            case BSA_BLE_APCF_SRVC_DATA:
                bsa_ble_apcf_cfg.cond.data.data_len =
                    app_get_choice("Enter Data Length(<=29):");
                if (bsa_ble_apcf_cfg.cond.data.data_len > BSA_BLE_APCF_COND_MAX_LEN)
                    bsa_ble_apcf_cfg.cond.data.data_len = BSA_BLE_APCF_COND_MAX_LEN;
                for (i = 0; i < bsa_ble_apcf_cfg.cond.data.data_len; i++)
                {
                    bsa_ble_apcf_cfg.cond.data.data[i] =
                        app_get_choice("Enter data in byte (eg. 1 or 2)");
                }
                if (action == BSA_BLE_APCF_COND_ADD)
                    for (i = 0; i < bsa_ble_apcf_cfg.cond.data.data_len; i++)
                    {
                        bsa_ble_apcf_cfg.cond.data.data_mask[i] =
                            app_get_choice("Enter data mask in byte (eg. 1 or 2)");
                    }
                break;

            case BSA_BLE_APCF_LOCAL_NAME:
                bsa_ble_apcf_cfg.cond.local_name.data_len =
                    app_get_choice("Enter Local Name Length(<=29):");
                if (bsa_ble_apcf_cfg.cond.local_name.data_len > BSA_BLE_APCF_COND_MAX_LEN)
                    bsa_ble_apcf_cfg.cond.local_name.data_len = BSA_BLE_APCF_COND_MAX_LEN;
                for (i = 0; i < bsa_ble_apcf_cfg.cond.local_name.data_len; i++)
                {
                    bsa_ble_apcf_cfg.cond.local_name.data[i] =
                        app_get_choice("Enter local name in byte (eg. 1 or 2)");
                }
                break;

            case BSA_BLE_APCF_SRVC_UUID:
            case BSA_BLE_APCF_SRVC_SOL_UUID:
                bsa_ble_apcf_cfg.cond.uuid.uuid.uu.uuid16 =
                    app_get_choice("Enter UUID (eg. x180d, current sample app only support 16 bits)");
                bsa_ble_apcf_cfg.cond.uuid.uuid.len = 2;
                if (action == BSA_BLE_APCF_COND_ADD)
                     bsa_ble_apcf_cfg.cond.uuid.uuid_mask.uuid16_mask =
                         app_get_choice("Enter UUID mask (eg. xFFFF, current sample app only support 16 bits)");
                break;

            default:
                APP_ERROR1("Unknown condition type: %d", cond_type);
                return -1;
                break;
        }
    }

    bsa_ble_apcf_cfg.cond_type = cond_type;
    bsa_ble_apcf_cfg.action = action;
    bsa_status = BSA_BleApcfCfg(&bsa_ble_apcf_cfg);
    if (bsa_status != BSA_SUCCESS)
    {
        APP_ERROR1("Unable to Config APCF status:%d", bsa_status);
        return -1;
    }

    return 0;
}

/*******************************************************************************
**
** Function         app_ble_common_cback
**
** Description      BLE common events callback.
**
** Returns          void
**
*******************************************************************************/
static void app_ble_common_cback(tBSA_BLE_EVT event, tBSA_BLE_MSG *p_data)
{
    APP_DEBUG1("app_ble_common_cback event = %d ", event);

    switch (event)
    {
    case BSA_BLE_APCF_ENABLE_EVT:
        APP_INFO1("APCF %s, status %d",
            (p_data->apcf_enable.enable ? "Enable" : "Disable"),
            p_data->apcf_enable.status);
        break;

    case BSA_BLE_APCF_CFG_EVT:
        if (p_data->apcf_cfg.status == BSA_SUCCESS)
        {
            if (p_data->apcf_cfg.cond_type == BSA_BLE_APCF_FILTER_SETTING)
            {
                APP_INFO1("status : %d cond : %d action : %d the remainder filter is : %d",
                    p_data->apcf_cfg.status, p_data->apcf_cfg.cond_type,
                    p_data->apcf_cfg.action, p_data->apcf_cfg.num_avail);
            }
            else
            {
                APP_INFO1("status : %d cond : %d action : %d the remainder condition is : %d",
                    p_data->apcf_cfg.status, p_data->apcf_cfg.cond_type,
                    p_data->apcf_cfg.action, p_data->apcf_cfg.num_avail);
            }
        }
        else
        {
            APP_INFO1("status : %d cond : %d action : %d",
                p_data->apcf_cfg.status, p_data->apcf_cfg.cond_type,
                p_data->apcf_cfg.action);
        }
        break;
#if (defined APP_BLE_2M_PHY_INCLUDED) && (APP_BLE_2M_PHY_INCLUDED == TRUE)
    case BSA_BLE_PHY_INFO_EVT:
        APP_INFO0("BSA_BLE_PHY_INFO_EVT");
        APP_INFO1("%02X:%02X:%02X:%02X:%02X:%02X",
            p_data->phy_info.bd_addr[0], p_data->phy_info.bd_addr[1],
            p_data->phy_info.bd_addr[2], p_data->phy_info.bd_addr[3],
            p_data->phy_info.bd_addr[4], p_data->phy_info.bd_addr[5]);
        APP_DEBUG1("status:0x%x, tx phy:0x%x, rx phy:0x%x",
            p_data->phy_info.status, p_data->phy_info.tx_phy, p_data->phy_info.rx_phy);
        break;
    case BSA_BLE_PHY_UPDATE_EVT:
        APP_INFO1("BSA_BLE_PHY_UPDATE_EVT, is_preference:%d", p_data->phy_info.is_preference);
        if (p_data->phy_info.is_preference == TRUE)
        {
            APP_DEBUG1("set preference, status:0x%x", p_data->phy_info.status);
        }
        else
        {
            APP_INFO1("%02X:%02X:%02X:%02X:%02X:%02X",
                p_data->phy_info.bd_addr[0], p_data->phy_info.bd_addr[1],
                p_data->phy_info.bd_addr[2], p_data->phy_info.bd_addr[3],
                p_data->phy_info.bd_addr[4], p_data->phy_info.bd_addr[5]);
            APP_DEBUG1("status:0x%x, tx phy:0x%x, rx phy:0x%x",
                p_data->phy_info.status, p_data->phy_info.tx_phy, p_data->phy_info.rx_phy);
        }
        break;
#endif
    case BSA_BLE_CONN_PARAM_UPDATE_EVT:
        APP_INFO0("BSA_BLE_CONN_PARAM_UPDATE_EVT");
        APP_INFO1("%02X:%02X:%02X:%02X:%02X:%02X",
            p_data->conn_update.bd_addr[0], p_data->conn_update.bd_addr[1],
            p_data->conn_update.bd_addr[2], p_data->conn_update.bd_addr[3],
            p_data->conn_update.bd_addr[4], p_data->conn_update.bd_addr[5]);
        APP_INFO1("status:%d, conn_interval:%d, conn_latency:%d conn_timeout:%d",
            p_data->conn_update.status,
            p_data->conn_update.conn_interval,
            p_data->conn_update.conn_latency,
            p_data->conn_update.conn_timeout);
        break;
    default:
        break;
    }
}

#if (defined APP_BLE_2M_PHY_INCLUDED) && (APP_BLE_2M_PHY_INCLUDED == TRUE)
/*******************************************************************************
 **
 ** Function        app_ble_read_phy
 **
 ** Description     read current phy status for active connection
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_read_phy(void)
{
    tBSA_STATUS status;
    tBSA_BLE_READ_PHY read_phy_param;

    status = BSA_BleReadPhyInit(&read_phy_param);

    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("app_ble_read_phy failed status = %d", status);
        return -1;
    }

    APP_INFO0("Enter the BD address to read phy status (AA.BB.CC.DD.EE.FF):");
    if (scanf("%hhx.%hhx.%hhx.%hhx.%hhx.%hhx",
        &read_phy_param.bd_addr[0], &read_phy_param.bd_addr[1],
        &read_phy_param.bd_addr[2], &read_phy_param.bd_addr[3],
        &read_phy_param.bd_addr[4], &read_phy_param.bd_addr[5]) != 6)
    {
        APP_ERROR0("BD address not entered correctly");
        return -1;
    }

    status = BSA_BleReadPhy(&read_phy_param);

    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("app_ble_read_phy failed status = %d", status);
        return -1;
    }
    return 0;
}

/*******************************************************************************
 **
 ** Function        app_ble_set_phy
 **
 ** Description     set current phy status for active connection/preference
 **
 ** Parameters      None
 **
 ** Returns         status: 0 if success / -1 otherwise
 **
 *******************************************************************************/
int app_ble_set_phy(void)
{
    tBSA_STATUS status;
    tBSA_BLE_SET_PHY set_phy_param;
    UINT8 rx_phy, tx_phy;
    UINT16 phy_opt;

    status = BSA_BleSetPhyInit(&set_phy_param);

    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("app_ble_set_phy failed status = %d", status);
        return -1;
    }

    set_phy_param.preference = app_get_choice("Is this default preference value?(0 = no, 1 = yes)");

    tx_phy = app_get_choice("Set preferred phy mask for TX: (e.g. bit0: no preference, bit1: 1M preferred, bit2: 2M preferrred");
    if (tx_phy > 7)
    {
        APP_ERROR0("app_ble_set_phy : Invalid phy entered, should be 0~7");
        return -1;
    }

    rx_phy = app_get_choice("Set preferred phy mask for RX: (e.g. bit0: no preference, bit1: 1M preferred, bit2: 2M preferrred");
    if (rx_phy > 7)
    {
        APP_ERROR0("app_ble_set_phy : Invalid phy entered, should be 0~7");
        return -1;
    }

    if (set_phy_param.preference != TRUE)
    {
        /*
            0 = the Host has no preferred coding when transmitting on the LE Coded PHY
            1 = the Host prefers that S=2 coding be used when transmitting on the LE Coded PHY
            2 = the Host prefers that S=8 coding be used when transmitting on the LE Coded PHY
        */
        phy_opt = app_get_choice("Enter phy options: (e.g. 0: no coding preference, 1: S2, 2: S8");
        if (phy_opt > 2)
        {
            APP_ERROR0("app_ble_set_phy : Invalid phy option entered, should be 0~2");
            return -1;
        }

        APP_INFO0("Enter the BD address to set phy status (AA.BB.CC.DD.EE.FF):");
        if (scanf("%hhx.%hhx.%hhx.%hhx.%hhx.%hhx",
            &set_phy_param.bd_addr[0], &set_phy_param.bd_addr[1],
            &set_phy_param.bd_addr[2], &set_phy_param.bd_addr[3],
            &set_phy_param.bd_addr[4], &set_phy_param.bd_addr[5]) != 6)
        {
            APP_ERROR0("BD address not entered correctly");
            return -1;
        }
    }

    set_phy_param.tx_phy = tx_phy;
    set_phy_param.rx_phy = rx_phy;
    set_phy_param.phy_opt = phy_opt;

    status = BSA_BleSetPhy(&set_phy_param);
    if (status != BSA_SUCCESS)
    {
        APP_ERROR1("app_ble__set_phy failed status = %d", status);
        return -1;
    }
    return status;
}

#endif
