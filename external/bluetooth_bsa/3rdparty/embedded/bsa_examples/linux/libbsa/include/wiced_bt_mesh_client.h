/*
 * $ Copyright Cypress Semiconductor $
 */

/**************************************************************************//**
* \file <wiced_bt_mesh_client.h>
*
* Mesh Provisioning definitions.
*/
#ifndef __WICED_BT_MESH_CLIENT_H__
#define __WICED_BT_MESH_CLIENT_H__

#include "wiced_bt_mesh_event.h"
#include "wiced_bt_mesh_provision.h"

#ifdef _WIN32
#define PACKED
#include <pshpack1.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @addtogroup  wiced_bt_mesh_client        Mesh Client Library API
 * @ingroup     wiced_bt_mesh
 *
 * The mesh client API
 *
 * @{
 */

/**
 * \brief Process Advertising Packet.
 * \details The application can call this function to when it receives advertising packet.
 *
 * @param    p_adv_report Advertising report paremeters
 * @param    p_adv_data Advertising data
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_remote_provisioning_adv_packet(wiced_bt_ble_scan_results_t *p_adv_report, uint8_t *p_adv_data);


/**
 * \brief Checking GATT connection.
 * \details This function returns TRUE if remote provisioning started GATT connection
 *
 * @param    none
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_remote_provisioning_is_gatt_connecting(void);

/**
 * \brief Connection State Changed.
 * \details The application shall call this function when connection is established or dropped.
 *
 * @param  conn_id GATT connection ID, or 0 if connection is lost.
 * @param  reason Disconnection reason
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
void wiced_bt_mesh_remote_provisioning_connection_state_changed(uint16_t conn_id, uint16_t reason);

/*
 */
/**
 * \brief Search for Proxy device.
 * \details Process command from the MCU to start or stop scanning for GATT Proxy devices.
 *
 * @param    p_data Pointer to the data structure with start/stop information.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
void wiced_bt_mesh_client_search_proxy(uint8_t start);

/**
 * \brief Proxy Connect.
 * \details The application can call this function to establish GATT Proxy connection.
 * Depending on the parameters the connection should be establishing either to a device
 * advertising it's identity, or a device advertising a mesh network
 *
 * @param  p_data Pointer to the data structure with the node identity or network.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_client_proxy_connect(wiced_bt_mesh_proxy_connect_data_t *p_data);

/**
 * \brief Proxy Disonnect.
 * \details The application can call this function to terminat GATT Proxy connection.
 *
 * @param    none
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_client_proxy_disconnect(void);

/*
 * \brief Provision Connect.
 * Command from MCU to start connection.  If use PB-GATT we will be scanning for
 * connectable unprovisioned devices until we figure out the BDADDR.  Otherwise
 * pass the connect request to provisioner library
 *
 * @param  p_event Pointer to the event structure which identified the remote provisioning server.
 * @param  p_data Pointer to the data structure with the connection information.
 * @param  use_pb_gatt If true, establish GATT connection for provisioning
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_client_provision_connect(wiced_bt_mesh_event_t *p_event, wiced_bt_mesh_provision_connect_data_t *p_connect, wiced_bool_t use_pb_gatt);

/**
 * \brief Provisioner Disconnect.
 * \details The application can call this function to terminate GATT Provisioning connection.
 *
 * @param    p_event Pointer to the event structure which identified the remote provisioning server.
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_provision_disconnect(wiced_bt_mesh_event_t *p_event);


/**
 * \brief Set Scan Mode.
 * \details The application can call this function to switch between passive and active scan.
 *
 * @param  is_active If set to 1, the stack will perform the active scan
 *
 * @return none
 */
void wiced_bt_ble_set_scan_mode(uint8_t is_active);

/**
 * \brief Send Vendor Data message
 * \details The application can call this function to send Vendor Data message to the Client
 *
 * @param  p_event  Pointer to the event structure which identified the remote provisioning server.
 * @param  p_data   Pointer to the vendor data
 * @param  data_len Length of the vendor data
 *
 * @return none
 */
void wiced_bt_mesh_client_vendor_data(wiced_bt_mesh_event_t *p_event, uint8_t *p_data, uint16_t data_len);

#ifdef __cplusplus
}
#endif

#ifdef _WIN32
#include <poppack.h>
#endif


/* @} wiced_bt_mesh_provisioning */

#endif /* __WICED_BT_MESH_PROVISION_H__ */
