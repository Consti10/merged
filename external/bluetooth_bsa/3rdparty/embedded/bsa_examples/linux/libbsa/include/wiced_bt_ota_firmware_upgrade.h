/***************************************************************************//**
* \file <wiced_bt_ota_firmware_upgrade.h>
*
* WICED OTA Firmware Upgrade Service over GATT
*
* \brief Provides definitions and APIs that exposes an OTA Firmware Upgrade
* GATT service and managed by the OTA Firmware Upgrade library, e.g.
* fw_upgrade_lib. This service can be utilized by peer Firmware Upgrade
* applications to perform a firmware upgrade.
*
*//*****************************************************************************
* $ Copyright Cypress Semiconductor $
*******************************************************************************/
#ifndef WICED_BT_OTA_FIRMWARE_UPGRADE__H
#define WICED_BT_OTA_FIRMWARE_UPGRADE__H

#include "wiced_bt_gatt.h"

/**
* \defgroup group_ota_wiced_firmware_upgrade   WICED OTA Firmware Upgrade
* @{
*
*  The WICED OTA Firmware Upgrade Service is used by peer applications to
*  upgrade firmware on WICED Bluetooth devices over a GATT connection.
*
* \defgroup group_ota_fw_upgrade_macros Macros
* \defgroup group_ota_fw_upgrade_cback_functions Callback Functions
* \defgroup group_ota_fw_upgrade_functions Functions
*/

/**
* \addtogroup group_ota_fw_upgrade_macros
* \{
*/
/* NOTE All UUIDs need to be reversed when publishing in the database. */

#ifdef WICED_OTA_VERSION_2
/* {10022922-ccf5-11e8-b680-025041000001}
   static const GUID GUID_OTA_FW_UPGRADE_SERVICE =
    { 0x10022922, 0xccf5, 0x11e8, { 0xb6, 0x80, 0x02, 0x50, 0x41, 0x00, 0x00, 0x01 } }; */
#define UUID_OTA_FW_UPGRADE_SERVICE                             0x01, 0x00, 0x00, 0x41, 0x50, 0x02, 0x80, 0xb6, 0xe8, 0x11, 0xf5, 0xcc, 0x22, 0x29, 0x02, 0x10

/* {10270b52-ccf5-11e8-8c28-025041000001}
   static const GUID GUID_OTA_SEC_FW_UPGRADE_SERVICE =
   { 0x10270b52, 0xccf5, 0x11e8, { 0x8c, 0x28, 0x02, 0x50, 0x41, 0x00, 0x00, 0x01 } }; */
#define UUID_OTA_SEC_FW_UPGRADE_SERVICE                         0x01, 0x00, 0x00, 0x41, 0x50, 0x02, 0x28, 0x8c, 0xe8, 0x11, 0xf5, 0xcc, 0x52, 0x0b, 0x27, 0x10

/* {1058fcfc-ccf5-11e8-b112-025041000001}
   static const GUID GUID_OTA_FW_UPGRADE_CHARACTERISTIC_CONTROL_POINT =
   { 0x1058fcfc, 0xccf5, 0x11e8, { 0xb1, 0x12, 0x02, 0x50, 0x41, 0x00, 0x00, 0x01 } }; */
#define UUID_OTA_FW_UPGRADE_CHARACTERISTIC_CONTROL_POINT        0x01, 0x00, 0x00, 0x41, 0x50, 0x02, 0x12, 0xb1, 0xe8, 0x11, 0xf5, 0xcc, 0xfc, 0xfc, 0x58, 0x10

/* {107163c8-ccf5-11e8-9b81-025041000001}
   static const GUID GUID_OTA_FW_UPGRADE_CHARACTERISTIC_DATA =
   { 0x107163c8, 0xccf5, 0x11e8, { 0x9b, 0x81, 0x02, 0x50, 0x41, 0x00, 0x00, 0x01 } }; */
#define UUID_OTA_FW_UPGRADE_CHARACTERISTIC_DATA                 0x01, 0x00, 0x00, 0x41, 0x50, 0x02, 0x81, 0x9b, 0xe8, 0x11, 0xf5, 0xcc, 0xc8, 0x63, 0x71, 0x10

/* {10a51326-ccf5-11e8-ab31-025041000001}
   static const GUID GUID_OTA_FW_UPGRADE_SERVICE_CHARACTERISTIC_APP_INFO =
   { 0x10a51326, 0xccf5, 0x11e8, { 0xab, 0x31, 0x02, 0x50, 0x41, 0x00, 0x00, 0x01 } }; */

#define UUID_OTA_FW_UPGRADE_SERVICE_CHARACTERISTIC_APP_INFO     0x01, 0x00, 0x00, 0x41, 0x50, 0x02, 0x31, 0xab, 0xe8, 0x11, 0xf5, 0xcc, 0x26, 0x13, 0xa5, 0x10
#else
/* {aE5D1E47-5C13-43A0-8635-82AD38A1381F}
   static const GUID WSRU_OTA_SERVICE =
   { 0xae5d1e47, 0x5c13, 0x43a0, { 0x86, 0x35, 0x82, 0xad, 0x38, 0xa1, 0x38, 0x1f } }; */
#define UUID_OTA_FW_UPGRADE_SERVICE                             0x1f, 0x38, 0xa1, 0x38, 0xad, 0x82, 0x35, 0x86, 0xa0, 0x43, 0x13, 0x5c, 0x47, 0x1e, 0x5d, 0xae

/* {C7261110-F425-447A-A1BD-9D7246768BD8}
   static const GUID GUID_OTA_SEC_FW_UPGRADE_SERVICE =
   { 0xc7261110, 0xf425, 0x447a,{ 0xa1, 0xbd, 0x9d, 0x72, 0x46, 0x76, 0x8b, 0xd8 } }; */
#define UUID_OTA_SEC_FW_UPGRADE_SERVICE                         0xd8, 0x8b, 0x76, 0x46, 0x72, 0x9d, 0xbd, 0xa1, 0x7a, 0x44, 0x25, 0xf4, 0x10, 0x11, 0x26, 0xc7

/* {a3DD50BF-F7A7-4E99-838E-570A086C661B}
   static const GUID WSRU_OTA_CHARACTERISTIC_CONTROL_POINT =
   { 0xa3dd50bf, 0xf7a7, 0x4e99, { 0x83, 0x8e, 0x57, 0xa, 0x8, 0x6c, 0x66, 0x1b } }; */
#define UUID_OTA_FW_UPGRADE_CHARACTERISTIC_CONTROL_POINT        0x1b, 0x66, 0x6c, 0x08, 0x0a, 0x57, 0x8e, 0x83, 0x99, 0x4e, 0xa7, 0xf7, 0xbf, 0x50, 0xdd, 0xa3

/* {a2E86C7A-D961-4091-B74F-2409E72EFE26}
   static const GUID WSRU_OTA_CHARACTERISTIC_DATA =
   { 0xa2e86c7a, 0xd961, 0x4091, { 0xb7, 0x4f, 0x24, 0x9, 0xe7, 0x2e, 0xfe, 0x26 } }; */
#define UUID_OTA_FW_UPGRADE_CHARACTERISTIC_DATA                 0x26, 0xfe, 0x2e, 0xe7, 0x09, 0x24, 0x4f, 0xb7, 0x91, 0x40, 0x61, 0xd9, 0x7a, 0x6c, 0xe8, 0xa2

/* {a47F7608-2E2D-47EB-91E9-75D4EDC4DE4B}
   static const GUID WSRU_OTA_CHARACTERISTIC_APP_INFO =
   { 0xa47f7608, 0x2e2d, 0x47eb, { 0x91, 0xe9, 0x75, 0xd4, 0xed, 0xc4, 0xde, 0x4b } }; */

#define UUID_OTA_FW_UPGRADE_SERVICE_CHARACTERISTIC_APP_INFO     0x4b, 0xde, 0xc4, 0xed, 0xd4, 0x75, 0x3b, 0x91, 0xeb, 0x47, 0x2d, 0x2e, 0x08, 0x76, 0x7f, 0xa4
#endif

/* Maximum data packet length used during FW download */
#define WICED_OTA_FW_UPGRADE_MAX_DATA_LEN                        128

/* Command definitions for the OTA FW upgrade */
#define WICED_OTA_UPGRADE_COMMAND_PREPARE_DOWNLOAD               1
#define WICED_OTA_UPGRADE_COMMAND_DOWNLOAD                       2
#define WICED_OTA_UPGRADE_COMMAND_VERIFY                         3
#define WICED_OTA_UPGRADE_COMMAND_FINISH                         4
#define WICED_OTA_UPGRADE_COMMAND_GET_STATUS                     5 /* Not currently used */
#define WICED_OTA_UPGRADE_COMMAND_CLEAR_STATUS                   6 /* Not currently used */
#define WICED_OTA_UPGRADE_COMMAND_ABORT                          7
#define WICED_OTA_UPGRADE_COMMAND_APPLY                          8

/* Event definitions for the OTA FW upgrade */
#define WICED_OTA_UPGRADE_STATUS_OK                              0
#define WICED_OTA_UPGRADE_STATUS_UNSUPPORTED_COMMAND             1
#define WICED_OTA_UPGRADE_STATUS_ILLEGAL_STATE                   2
#define WICED_OTA_UPGRADE_STATUS_VERIFICATION_FAILED             3
#define WICED_OTA_UPGRADE_STATUS_INVALID_IMAGE                   4
#define WICED_OTA_UPGRADE_STATUS_INVALID_IMAGE_SIZE              5
#define WICED_OTA_UPGRADE_STATUS_MORE_DATA                       6
#define WICED_OTA_UPGRADE_STATUS_INVALID_APPID                   7
#define WICED_OTA_UPGRADE_STATUS_INVALID_VERSION                 8
#define WICED_OTA_UPGRADE_STATUS_CONTINUE                        9
#define WICED_OTA_UPGRADE_STATUS_BAD_PARAM                       10

/* GATT handles used for by the FW upgrade service */
#define HANDLE_OTA_FW_UPGRADE_SERVICE                           0xff00 /* OTA firmware upgrade service handle */
#define HANDLE_OTA_FW_UPGRADE_CHARACTERISTIC_CONTROL_POINT      0xff01 /* OTA firmware upgrade characteristic control point handle */
#define HANDLE_OTA_FW_UPGRADE_CONTROL_POINT                     0xff02 /* OTA firmware upgrade control point value handle */
#define HANDLE_OTA_FW_UPGRADE_CLIENT_CONFIGURATION_DESCRIPTOR   0xff03 /* OTA firmware upgrade client configuration description handle */
#define HANDLE_OTA_FW_UPGRADE_CHARACTERISTIC_DATA               0xff04 /* OTA firmware upgrade data characteristic handle */
#define HANDLE_OTA_FW_UPGRADE_DATA                              0xff05 /* OTA firmware upgrade data value handle */
#define HANDLE_OTA_FW_UPGRADE_CHARACTERISTIC_APP_INFO           0xff06 /* OTA firmware upgrade app info characteristic handle */
#define HANDLE_OTA_FW_UPGRADE_APP_INFO                          0xff07 /* OTA firmware upgrade app info value handle */

/* OTA firmware upgrade status codes */
#define OTA_FW_UPGRADE_STATUS_STARTED                           1   /* Client started OTA firmware upgrade process */
#define OTA_FW_UPGRADE_STATUS_ABORTED                           2   /* OTA firmware upgrade process was aborted or failed verification */
#define OTA_FW_UPGRADE_STATUS_COMPLETED                         3   /* OTA firmware upgrade completed, will reboot */
#define OTA_FW_UPGRADE_STATUS_VERIFICATION_START                4   /**< OTA firmware upgrade starts verification */
/** \} group_ota_fw_upgrade_macros */

#ifdef __cplusplus
extern "C" {
#endif

/**
* \addtogroup group_ota_fw_upgrade_cback_functions
* \{
*/
/** An optional callback to be executed when the OTA Fimware Upgrade state changes.
* The application can register the callback to execute when the client starts,
* aborts or completes the OTA firmware upgrade process. When the firmware
* has been downloaded and verified and before a callback is executed before
* the library restarts. For example, if the application needs to drop all connections.
*
* \param   status : OTA firmware upgrade status (see @ref OTA_FW_UPGRADE_STATUS "OTA firmware upgrade status codes")
*
******************************************************************************/
typedef void (wiced_ota_firmware_upgrade_status_callback_t)(uint8_t status);

/** An optional callback to be executed to before sending data over the air.
* The application can register this callback, for example, when it needs to
* encrypt data before it is sent out as a notification.
*
* \param   is_notification : Set to WICED_TRUE to send notification, WICED_FALSE to send indication.
* \param   conn_id         : Connection ID
* \param   attr_handle     : Handle of the attribute to notify.
* \param   val_len         : The length of the data.
* \param   p_val           : The pointer to the data.
*
******************************************************************************/
typedef wiced_bt_gatt_status_t (wiced_ota_firmware_upgrade_send_data_callback_t)(wiced_bool_t is_notification, uint16_t conn_id, uint16_t attr_handle, uint16_t val_len, uint8_t *p_val);
/** \} group_ota_fw_upgrade_cback_functions */

/**
* \addtogroup group_ota_fw_upgrade_functions
* \{
*/
/******************************************************************************
* Function Name: wiced_ota_fw_upgrade_init
***************************************************************************//**
* \brief Initializes the WICED OTA Firmware Upgrade Service module.
*
* \details The application calls this function during initialization.
*          The none-zero pointer to the public key indicates to the library
*          that the secure OTA firmware update procedure is being used.
*
* \param public_key             The pointer to the public key.
* \param p_pre_reboot_callback  The callback to issue at the end of an upgrade procedure before a chip reboot.
*
* \note  WICED BT platforms that use external flash(i.e not on chip flash) must define the flash size, sector size
*        before calling this API. For example see the wiced_platform.h, for default flash configuration
*        exists on WICED eval kits.
*
******************************************************************************/
wiced_bool_t wiced_ota_fw_upgrade_init(void *public_key, wiced_ota_firmware_upgrade_status_callback_t *p_status_callback, wiced_ota_firmware_upgrade_send_data_callback_t *p_send_data_callback);

/******************************************************************************
* Function Name: wiced_ota_fw_upgrade_is_active
***************************************************************************//**
* \brief Checks the state of the OTA Firmware Upgrade.
*
* \details Returns TRUE if OTA Firmware Upgrade is active.
*
******************************************************************************/
wiced_bool_t wiced_ota_fw_upgrade_is_active(void);

/******************************************************************************
* Function Name: wiced_ota_fw_upgrade_connection_status_event
***************************************************************************//**
* \brief The Connection status notification.
*
* \details The application calls this function to pass the Connection status notification to the
*          OTA Firmware Upgrade library.
*
******************************************************************************/
void wiced_ota_fw_upgrade_connection_status_event(wiced_bt_gatt_connection_status_t *p_status);

/******************************************************************************
* Function Name: wiced_ota_fw_upgrade_read_handler
***************************************************************************//**
* \brief The OTA FW Upgrade read request.
*
* \details The application calls this function to pass a GATT read request to the
*         OTA Firmware Upgrade library for handles that belong to the FW Upgrade Service.
*
******************************************************************************/
wiced_bt_gatt_status_t wiced_ota_fw_upgrade_read_handler(uint16_t conn_id, wiced_bt_gatt_read_t * p_read_data);

/******************************************************************************
* Function Name: wiced_ota_fw_upgrade_write_handler
***************************************************************************//**
* \brief the OTA FW Upgrade write request.
*
* \details The application calls this function to pass a GATT write request to the
*          OTA Firmware Upgrade library for handles that belong to the FW Upgrade Service.
*
******************************************************************************/
wiced_bt_gatt_status_t wiced_ota_fw_upgrade_write_handler(uint16_t conn_id, wiced_bt_gatt_write_t *p_write_data);

/******************************************************************************
* Function Name: wiced_ota_fw_upgrade_indication_cfm_handler
***************************************************************************//**
* \brief The OTA FW Upgrade indication confirmation.
*
* \details The application calls this function to pass a GATT indication confirm request to the
*          OTA Firmware Upgrade library for handles that belong to the FW Upgrade Service.
*
******************************************************************************/

wiced_bt_gatt_status_t wiced_ota_fw_upgrade_indication_cfm_handler(uint16_t conn_id, uint16_t handle);

/******************************************************************************
* Function Name: wiced_ota_fw_upgrade_is_gatt_handle
***************************************************************************//**
* \brief The OTA Firmware Upgrade Service GATT handle check.
*
* \details The application calls this function to check if the handle is a GATT handle
*          within the OTA Firmware Upgrade Service handles
*
* \param[in] handle The OTA Firmware Service GATT handle to check.
*
* \return true      The handle is a GATT handle, otherwise - false.
*
******************************************************************************/
wiced_bool_t wiced_ota_fw_upgrade_is_gatt_handle(uint16_t handle);

/******************************************************************************
* Function Name: wiced_ota_fw_upgrade_get_new_fw_info
***************************************************************************//**
* \brief Gets info of received firmware.
*
* \details The firmware info is available only after the whole image is received and verified.
*
******************************************************************************/
wiced_bool_t wiced_ota_fw_upgrade_get_new_fw_info(uint16_t *company_id, uint8_t *fw_id_len, uint8_t *fw_id);

#ifdef __cplusplus
}
#endif

/** \} group_ota_fw_upgrade_functions */
/** \} group_ota_wiced_firmware_upgrade */
/** @} */

#endif /* WICED_BT_OTA_FIRMWARE_UPGRADE__H */
