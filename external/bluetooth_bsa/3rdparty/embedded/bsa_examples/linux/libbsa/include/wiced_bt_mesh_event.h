/*
 * $ Copyright Cypress Semiconductor $
 */


/**************************************************************************//**
* \file <wiced_bt_mesh_event.h>
*
* Mesh Model definitions
*
*/
#ifndef __MESH_EVENT_H__
#define __MESH_EVENT_H__

#include "wiced_timer.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @addtogroup  wiced_bt_mesh_core          Mesh Core Library API
 * @ingroup     wiced_bt_mesh
 *
 * Mesh Core library of the WICED SDK provide a simple method for an application to integrate
 * Bluetooth Mesh functionality.
 *
 * @{
 */

/**
 * Mesh event structure is exchanged between the app and the mesh models library
 */
typedef struct wiced_bt_mesh_event__t
{
    struct wiced_bt_mesh_event__t *p_next;              /**< Pointer to the next event in the queue */
    void            *complete_callback; /**< pointer to the send complete callback */
    uint16_t        company_id;     /**< Company ID for the Model ID */
    uint16_t        model_id;       /**< Model ID */
    uint16_t        opcode;         /**< Opcode of the message to be transmitted or that has been received */
    uint16_t        hci_opcode;     /**< HCI Opcode received of the message from the MCU being processed */
    uint8_t         element_idx;    /**< Element Index of the source or destination of the message */
    uint8_t         ttl;            /**< Time to leave to be used when transmitting or as received from the peer */
    uint16_t        src;            /**< Address of the source mesh node */
    uint16_t        dst;            /**< Address of the destination mesh node */
    uint16_t        app_key_idx;    /**< Application key index used to decrypt when message was received or which should be used to encrypt to send the message */
    uint16_t        num_in_group;   /**< number of devices in the group list */
    uint16_t        *group_list;    /**< If dst is a group address, this points to the array of with individual unicast addresses */
    uint16_t        data_len;       /**< Length of data corresponding to the event */
    uint8_t         credential_flag;/**< Value of the Friendship Credential Flag */
    uint8_t         retrans_cnt;    /**< Number of retransmissions for each message. Should be <= 0x7f. It is ignored using 0 value or if it is reply (reply == WICED_TRUE). */
                                    /**< If bit 0x80 is set, the retransmission count and time applies to number of network rexmits, no access layer rexmits, the time is in 10ms ticks */
#define RETRANSMIT_TIMER_TICK   50
    uint8_t         retrans_time;   /**< Interval between retransmissions in 50-millisecond steps */
    uint8_t         reply;          /**< If TRUE the reply is expected */
#define REPLY_TIMER_TICK        50
    uint8_t         reply_timeout;  /**< Time to wait for the peer model layer acknowledgment in 50-millisecond steps */
#define TX_STATUS_COMPLETED     0
#define TX_STATUS_FAILED        1
#define TX_STATUS_ACK_RECEIVED  2
    uint8_t         tx_status;      /**< Transmission failed or timeout occurred waiting for peer reply */
    wiced_timer_t   timer;
    uint16_t        friend_addr;    /**< core sets it to friend address when calls complete_callback if segmented message has been sent and acked by the friend OnBehalfOf LPN. Otherwise it is 0 */
    uint8_t         send_segmented; /**< if non-0 then core uses segmentation to send that message even if it fits into unsegmented message. */
    int8_t          rssi;           /**< RSSI of the received message */
} wiced_bt_mesh_event_t;

/**
 * \brief Create message reply mesh event from the received mesh event
 * \details This function doesn't create but just updates received mesh event to be used to send message back to the originator.
 * After application or a models library calls Mesh Models Library or Mesh Core library passing the pointer to the mesh event, it loses the ownership of the event and should not
 * use it again.
 *
 * @param       p_event information about the message received.
 *
 * @return      Pointer to the input p_event updated for response
 */
wiced_bt_mesh_event_t *wiced_bt_mesh_create_reply_event(wiced_bt_mesh_event_t *p_event);

/**
 * \brief Create the copy of the existing mesh event
 * \details This function creates new mesh event with same content as input mesh event.
 *
 * @param       p_event mesh event to copy.
 *
 * @return      Pointer to the created mesh event
 */
wiced_bt_mesh_event_t *wiced_bt_mesh_copy_event(wiced_bt_mesh_event_t *p_event);

/**
 * \brief Create mesh event for an unsolicited message.
 * \details In case dst is 0 the function takes all information from the model's publication or fails if publication is not configured for the specified model.
 * In case non-0 dst the function uses specified dst and app_key_idx and fills all other fields with default values.
 * In special case with company_id equals to 0xffff the function creates a message event with default ttl.
 *
 * @param       element_index   Element index.
 * @param       company_id      Company ID.
 * @param       model_id        Model ID.
 * @param       dst             Destination address. If parameter is 0, the function finds publication and take uses its fields for the mesh event.
 * @param       app_key_idx     Global app key index. The parameter is ignored if dst is equal to 0. The 0xffff means device key.
 *
 * @return      p_event Pointer to a newly allocated mesh event.
 */
wiced_bt_mesh_event_t *wiced_bt_mesh_create_event(uint8_t element_index, uint16_t company_id, uint16_t model_id, uint16_t dst, uint16_t app_key_idx);

/**
 * \brief Release mesh event
 * \details The application should call this function when it receives the mesh event in the callback and the reply value is set to 0.
 *
 * @param       p_event information for the message.
 *
 * @return      None
 */
void wiced_bt_mesh_release_event(wiced_bt_mesh_event_t *p_event);

/* @} wiced_bt_mesh_core */

#ifdef __cplusplus
}
#endif

#endif /* __MESH_EVENT_H__ */
