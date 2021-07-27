/*
 * $ Copyright Cypress Semiconductor $
 */

/** @file
 *
 *  WICED Generic Attribute (GATT) Application Programming Interface
 */
#pragma once


/*****************************************************************************
 *  External Function Declarations
 ****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @addtogroup  wicedbt_gatt_utils   GATT Utilities
 * @ingroup     wicedbt_gatt
 *
 * Generic Attribute (GATT) Utility Functions.
 *
 * @{
 */

/**
 * Function     wiced_bt_util_set_gatt_client_config_descriptor
 *
 *              Set value of Client Configuration Descriptor
 *
 * @param[in]   conn_id   : GATT connection ID
 * @param[in]   handle    : Handle of the descriptor to modify
 * @param[in]   value     : Value to set
 *
 * @return @link wiced_bt_gatt_status_e wiced_bt_gatt_status_t @endlink
 *
 */
wiced_bt_gatt_status_t wiced_bt_util_set_gatt_client_config_descriptor(uint16_t conn_id, uint16_t handle, uint16_t value);

/**
 *
 * Function     wiced_bt_util_send_gatt_discover
 *
 *              Format and send GATT discover request
 *
 *  @param[in]  conn_id     : connection identifier.
 *  @param[in]  type        : GATT discovery type.
 *  @param[in]  uuid        : UUID of the attribute to search for.
 *  @param[in]  s_handle    : Start handle.
 *  @param[in]  e_handle    : Start handle.
 *
 *  @return @link wiced_bt_gatt_status_e wiced_bt_gatt_status_t @endlink
 *
 */
wiced_bt_gatt_status_t wiced_bt_util_send_gatt_discover(uint16_t conn_id, wiced_bt_gatt_discovery_type_t type, uint16_t uuid, uint16_t s_handle, uint16_t e_handle);

/**
 * Function     wiced_bt_gatt_status_t wiced_bt_util_send_gatt_read_by_handle(uint16_t conn_id, uint16_t handle)
 *
 *              Format and send Read By Handle GATT request.
 *
 *  @param[in]  conn_id     : connection identifier.
 *  @param[in]  handle      : Attribute handle of the attribute to read.
 *
 *  @return @link wiced_bt_gatt_status_e wiced_bt_gatt_status_t @endlink
 *
 */
wiced_bt_gatt_status_t wiced_bt_util_send_gatt_read_by_handle(uint16_t conn_id, uint16_t handle);

/**
 * Function       wiced_bt_util_send_gatt_read_by_type
 *
 *                Format and send Read by Type GATT request
 *
 *  @param[in]  conn_id     : Connection handle
 *  @param[in]  s_handle    : Start handle
 *  @param[in]  e_handle    : End handle
 *  @param[in]  uuid        : UUID of the attribute to read
 *
 *  @return @link wiced_bt_gatt_status_e wiced_bt_gatt_status_t @endlink
 */
wiced_bt_gatt_status_t wiced_bt_util_send_gatt_read_by_type(uint16_t conn_id, uint16_t s_handle, uint16_t e_handle, uint16_t uuid);

/**
 * Function       wiced_bt_util_uuid_cpy
 *
 *                This utility function copies an UUID
 *
 *  @param[out] p_dst       : Destination UUID
 *  @param[in]  p_src       : Source UUID
 *
 *  @return  int 0 if success, -1 if error
 */
int wiced_bt_util_uuid_cpy(wiced_bt_uuid_t *p_dst, wiced_bt_uuid_t *p_src);

/*
 * wiced_bt_util_uuid_cmp
 * This utility function Compares two UUIDs.
 * Note: This function can only compare UUIDs of same length
 * Return value: 0 if UUID are equal; 1 other with
 */
/**
 * Function       wiced_bt_util_uuid_cmp
 *
 *                This utility function Compares two UUIDs.
 *                Note: This function can only compare UUIDs of same length
 *
 *  @param[out] p_uuid1     : First UUID
 *  @param[in]  p_uuid2     : Second UUID
 *
 *  @return  int 0 if UUID are equal; -1 if error, 1 if UUIDs are different
 */
int wiced_bt_util_uuid_cmp(wiced_bt_uuid_t *p_uuid1, wiced_bt_uuid_t *p_uuid2);

/**@} wicedbt_gatt_utils */

#ifdef __cplusplus
}
#endif

