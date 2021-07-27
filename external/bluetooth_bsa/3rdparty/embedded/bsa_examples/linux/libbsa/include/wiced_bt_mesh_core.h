/*
 * $ Copyright Cypress Semiconductor $
 */

/**************************************************************************//**
* \file <wiced_bt_mesh_core.h>
*
* Definitions for interface between Bluetooth Mesh Models and Mesh Core
*
*/

#ifndef __WICED_BT_MESH_CORE_H__
#define __WICED_BT_MESH_CORE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "wiced_bt_mesh_event.h"
#include "wiced_bt_mesh_cfg.h"
#include "wiced_bt_ble.h"

/* The Identification Type field values of the proxy service advertisement */
#define WICED_BT_MESH_PROXY_IDENTIFICATION_TYPE_NETWORK_ID      0
#define WICED_BT_MESH_PROXY_IDENTIFICATION_TYPE_NODE_IDENTITY   1

/**
 * @addtogroup  wiced_bt_mesh               BLE Mesh
 * @ingroup     wicedbt
 *
 * Mesh API provides a developer a simple way to incorporate BLE mesh functionality in a WICED application.
 * Most application will only need access to @ref wiced_bt_mesh_models "Mesh Models" library. The applications that
 * supports adding new devices to the mesh network (provisioning) and network configuration may also use @ref wiced_bt_mesh_provisioning "Mesh Provisioning"
 * library.  Applications that use Vendor specific models in addition to standard Bluetooth SIG models, may also need to access
 * @ref wiced_bt_mesh_core "Mesh Core" library functions directly.
 *
 * @{
 */

/**
 * @addtogroup  wiced_bt_mesh_core          Mesh Core Library API
 * @ingroup     wiced_bt_mesh
 *
 * Mesh Core library of the WICED SDK provides access to the BLE Mesh Core.  Typically
 * application will not access BLE Mesh Core directly but through the BLE Mesh Models
 * library functions.
 *
 * @{
 */

/**
* @anchor WICED_BT_MESH_CORE_CMD_SPECIAL
* @name Special Commands of the Proxy Filter, Heartbeat and other
* \details The following is the list of the special commands of the Proxy Filter, Heartbeat and other that came from Network and Transport layers into wiced_bt_mesh_core_received_msg_handler_t.
* @{
*/
/**
* Opcodes of the Proxy Configuration Messages
*/
#define WICED_BT_MESH_CORE_CMD_SPECIAL_PROXY_FLT_SET_TYPE   0x00    /**<  Sent by a Proxy Client to set the proxy filter type. */
#define WICED_BT_MESH_CORE_CMD_SPECIAL_PROXY_FLT_ADD_ADDR   0x01    /**<  Sent by a Proxy Client to add addresses to the proxy filter list. */
#define WICED_BT_MESH_CORE_CMD_SPECIAL_PROXY_FLT_DEL_ADDR   0x02    /**<  Sent by a Proxy Client to remove addresses from the proxy filter list. */
#define WICED_BT_MESH_CORE_CMD_SPECIAL_PROXY_FLT_STATUS     0x03    /**<  Acknowledgment by a Proxy Server to a Proxy Client to report the status of the proxy filter list. */
#define WICED_BT_MESH_CORE_CMD_SPECIAL_HEARTBEAT            0x0a    /**<  Heartbeat message came from Transport layer. */
/** @} WICED_BT_MESH_CORE_CMD_SPECIAL */

#pragma pack(1)
#ifndef PACKED
#define PACKED
#endif

/* Data for HCI_CONTROL_MESH_COMMAND_CORE_NETWORK_LAYER_TRNSMIT */
typedef PACKED struct
{
    uint8_t     ctl;                        /**< CTL field. can be 0 or 1 */
    uint8_t     ttl;                        /**< TTL field. can be between 0 and 0x7f */
    uint8_t     dst[2];                     /**< Destination address (LE) */
    uint8_t     pdu[1];                     /**< TransportPDU set to between 1 and 16 inclusive octets of data */
} wiced_bt_mesh_core_hci_cmd_network_layer_transmit_t;

/* Data for HCI_CONTROL_MESH_COMMAND_CORE_TRANSPORT_LAYER_TRNSMIT */
typedef PACKED struct
{
    uint8_t     ctl;                        /**< CTL field. can be 0 or 1 */
    uint8_t     ttl;                        /**< TTL field. can be between 0 and 0x7f */
    uint8_t     dst_len;                    /**< Length of the destination address. It can be 2 or 16 for virtual address */
    uint8_t     dst[16];                    /**< BigEndian 2 bytes destination address or 16 bytes label UUID of the virtual address */
    uint8_t     szmic;                      /**< 0 - 4 bytes TransMIC size, 1 - 8 bytes TransMIC size */
    uint8_t     pdu[1];                     /**< AccessPDU set to between 1 and 380 inclusive octets of data */
} wiced_bt_mesh_core_hci_cmd_transport_layer_transmit_t;

/* Data for HCI_CONTROL_MESH_COMMAND_CORE_IVUPDATE_SIGNAL_TRNSIT */
typedef PACKED struct
{
    uint8_t     in_progress;                /**< 1 - Transit to IV Update in Progress; 0 - Transit to Normal */
} wiced_bt_mesh_core_hci_cmd_ivupdate_signal_transit_t;

/* Data for HCI_CONTROL_MESH_COMMAND_CORE_PROVISION */
typedef PACKED struct
{
    uint8_t     conn_id[4];             /**< connection id for provisioning. Better to use some random value. */
    uint8_t     addr[2];                /**< unicast address to assign to provisioning node */
    uint8_t     uuid[16];               /**< UUID of the node to provision */
    uint8_t     identify_duration;      /**< identify duration to pass to that provision start command */
} wiced_bt_mesh_core_hci_cmd_provision_t;

/* Data for HCI_CONTROL_MESH_COMMAND_CORE_HEALTH_SET_FAULTS */
typedef PACKED struct
{
    uint8_t     test_id;                /**< Test ID of the faults array */
    uint8_t     company_id[2];          /**< Company ID */
    uint8_t     faults[1];              /**< faults array. can be empty or contain up to 16 fault codes */
} wiced_bt_mesh_core_hci_cmd_health_set_faults_t;

/* Data for HCI_CONTROL_MESH_COMMAND_CORE_ACCESS_PDU */
typedef PACKED struct
{
    uint8_t     ttl;                        /**< TTL field. can be between 0 and 0x7f */
    uint8_t     app_key_idx[2];             /**< Global Index of the Application Key (LE). 0xffff - device Key */
    uint8_t     src[2];                     /**< Source address (LE) */
    uint8_t     dst[2];                     /**< Destination address (LE) */
    uint8_t     pdu[1];                     /**< Access PDU */
} wiced_bt_mesh_core_hci_cmd_access_pdu_t;

/* Data for HCI_CONTROL_MESH_COMMAND_CORE_SEND_SUBS_UPDT */
typedef PACKED struct
{
    uint8_t     add;                        /**< WICED_TRUE/WICED_FALSE - add/remove */
    uint8_t     addr[2];                    /**< Subscription address (LE) */
} wiced_bt_mesh_core_hci_cmd_send_subs_updt_t;

#pragma pack()

/**
 * \brief Received message callback.
 * \details Each model implements that function to handle received messages.
 *
 * @param[in]   p_event         Parameters related to received message. It should be released by call to wiced_bt_mesh_release_event() or it can be used to send reply by call to wiced_bt_mesh_core_send()
 * @param[in]   params          Application Parameters - extracted from the Access Payload
 * @param[in]   params_len      Length of the Application Parameters
 *
 * @return      WICED_TRUE if message handled and handler took ownership of the p_event. Otherwise returns WICED_FALSE - then caller keeps ownership of p_event.
*/
typedef wiced_bool_t (*wiced_bt_mesh_core_received_msg_handler_t)(wiced_bt_mesh_event_t *p_event, const uint8_t *params, uint16_t params_len);

/**
 * \brief Get message handler callback.
 * \details The WICED Mesh Application library implements that function to find and return the message handler corresponding to the opcode of received message.
 * If opcode does not correspond to any supported model then the function returns NULL. Otherwise it returns the message handler.
 *
 * The 0xffff value of the comany_id means special case when message came from Network or Transport layer (see Special Commands
 * of the Proxy Filter, Heartbeat and other).
 *
 * @param[in]   company_id      Opcode company ID
 * @param[in]   opcode          Opcode
 * @param[out]  p_model_id      Variable for found Model ID for that opcode
 * @param[out]  p_dont_save_rpl WICED_TRUE - don't save RPL on messages from that model
 *
 * @return      Message handler of the model for that opcode. If model isn't found then returns NULL.
*/
typedef wiced_bt_mesh_core_received_msg_handler_t (*wiced_bt_mesh_core_get_msg_handler_callback_t)(uint16_t company_id, uint16_t opcode, uint16_t *p_model_id, wiced_bool_t *p_dont_save_rpl);

/**
 * \brief Publication request callback.
 * \details The WICED Mesh Application library implements that function to be notified when periodic publication configuration has changed or when a model need to publish its status.
 * If period is set to 0, the publication needs to stop. Period between 1 and 0xFFFE indicates that Periodic Publication for the specified model is being changed.
 * Value 0xFFFF indicates that the value needs to be published now.
 *
 * @param[in]   elem_idx        Element index
 * @param[in]   company_id      Company ID
 * @param[in]   model_id        Model ID
 * @param[in]   period          New value of the publication period in 100 ms units or indication that model has to publish status now.
 *
 * @return      When core calls application to setup publication period, return WICED_TRUE indicates, that application will take care
 *              of the periodic publication.  WICED_FALSE means that core shall take care of periodic publications and execute callback every period seconds.
 */
typedef wiced_bool_t (*wiced_bt_mesh_core_publication_callback_t)(uint8_t elem_idx, uint16_t company_id, uint16_t model_id, uint16_t period);

/**
 * \brief Application provided function to read/write information from/to NVRAM.
 * \details Application should provide the address of this callback function that the core library can execute to store or retrieve a memory chunk from the NVRAM.
 *
 * @param[in]       write       WICED_TRUE - write; WICED_FALSE - read.
 * @param[in]       inx         Index of the memory chunk to read/write
 * @param[in,out]   node_info   Buffer to read/write from/to.
 * @param[in]       len         The size of memory chunk to read/write. Write 0 means delete.
 * @param[in]       p_result    Pointer to variable to receive the result of operation.
 *
 * @return          Return the number of bytes red/written from/into NVRAM
 */
typedef uint32_t(*wiced_bt_core_nvram_access_t)(wiced_bool_t write, int inx, uint8_t* node_info, uint16_t len, wiced_result_t *p_result);

/**
* \brief Start/Stop scan callback.
* \details Application implements that function if it can provide service to start/stop scan.
*
* @param[in]   start               Call will set this to WICED_TRUE to start the scan, WICED_FALSE to stop the scan
* @param[in]   is_scan_type_active  WICED_TRUE - Active Scan; WICED_FALSE - Passive Scan.
* @return      None
*/
typedef void(*wiced_bt_mesh_core_scan_callback_t)(wiced_bool_t start, wiced_bool_t is_scan_type_active);

/**
 * \brief Definition of the callback function to send proxy messages.
 * \details Application implements that function if it want to use
 * GATT notification, GATT write command or any other type of a proxy
 * connection.
 *
 * @param[in]   conn_id             Connection ID
 * @param[in]   ref_data            The reference data that was passed to the core when connection was established
 * @param[in]   packet              Packet to send
 * @param[in]   packet_len          Length of the packet to send
 *
 * @return      None
 */
typedef void(*wiced_bt_mesh_core_proxy_send_cb_t)(uint32_t conn_id, uint32_t ref_data, const uint8_t *packet, uint32_t packet_len);

/**
 * \brief Definition of the callback function to send proxy packet.
 * \details The application shall implement this function to support Health Server Model. The function is called by the Mesh Core library to get Current Fault.
 *
 * @param[in]   element             Index of the element
 * @param[in]   test_id             Identifier of a specific test to be performed
 * @param[in]   company_id          Company ID of the test
 * @param[in]   fault_array_size    Size of the buffer fault_array
 * @param[out]  fault_array         Buffer to receive FaultArray
 *
 * @return      number of error codes filled into fault_array. Value 0xff means invalid test_id.
 */
typedef uint8_t (*wiced_bt_mesh_core_health_fault_test_cb_t)(uint8_t element, uint8_t test_id, uint16_t company_id, uint8_t fault_array_size, uint8_t *fault_array);

/**
 * \brief Definition of the callback function to attract human attention by LED blinks or other.
 * \details The application shall implement this function to support Health Server Model. The function is executed by the Mesh Core library to attract human attention.
 *
 * @param[in]   element     Index of the element.
 * @param[in]   time        0 - Off; 0x01-0xff - On, remaining time in seconds.
 *
 * @return      None
 */
typedef void(*wiced_bt_mesh_core_attention_cb_t)(uint8_t element, uint8_t time);

typedef enum
{
    WICED_BT_MESH_CORE_STATE_TYPE_SEQ,              /**< Own SEQ or RPL SEQ is changed */
    WICED_BT_MESH_CORE_STATE_DEFAULT_TTL,           /**< Default TTL is changed */
    WICED_BT_MESH_CORE_STATE_NET_TRANSMIT,          /**< Network Transmit is changed */
    WICED_BT_MESH_CORE_STATE_NET_KEY_ADD,           /**< Net Key is added */
    WICED_BT_MESH_CORE_STATE_NET_KEY_DELETE,        /**< Net Key is deleted */
    WICED_BT_MESH_CORE_STATE_NET_KEY_UPDATE,        /**< Net Key is updated */
    WICED_BT_MESH_CORE_STATE_APP_KEY_ADD,           /**< App Key is added */
    WICED_BT_MESH_CORE_STATE_APP_KEY_DELETE,        /**< App Key is deleted */
    WICED_BT_MESH_CORE_STATE_APP_KEY_UPDATE,        /**< App Key is updated */
    WICED_BT_MESH_CORE_STATE_MODEL_APP_BIND,        /**< App is Bound */
    WICED_BT_MESH_CORE_STATE_MODEL_APP_UNBIND,      /**< App is UnBound */
    WICED_BT_MESH_CORE_STATE_IV,                    /**< IV Index or/and IV UPDATE update flag is changed is changed */
    WICED_BT_MESH_CORE_STATE_KR,                    /**< KR phase is changed to 2 (WICED_TRUE) or to 9(WICED_FALSE) */
    WICED_BT_MESH_CORE_STATE_NODE_STATE,            /**< Node state is changed. Called on Node Reset and successfull Provisioning. App should change GATT DB */
    WICED_BT_MESH_CORE_STATE_LPN_FRIENDSHIP,        /**< LPN friendship state is changed - established or aborted */
    WICED_BT_MESH_CORE_STATE_LPN_SLEEP,             /**< LPN application can go to sleep mode for a specific sleep time or can return
                                                         with same or changed sleep time to let core to procceed with poll after that time.*/
    WICED_BT_MESH_CORE_STATE_LPN_SCAN,              /**< LPN layer requests stop(or start) scanning when it doesn't expect(or expects) any packets */
    WICED_BT_MESH_CORE_STATE_FRND_FRIENDSHIP,       /**< Friend friendship state is changed - established or aborted */
    WICED_BT_MESH_CORE_STATE_TYPE_CLR_RPL,          /**< RPL is cleared */
} wiced_bt_mesh_core_state_type_t;

/**
* Data for state type WICED_BT_MESH_CORE_STATE_IV.
* It represents IV Index and IV-UPDATE flag
*/
typedef struct
{
    uint32_t        index;              /**< IV Index */
    uint8_t         update_flag;        /**< IV-UPDATE flag */
} wiced_bt_mesh_core_state_iv_t;


/**
 * Data for state type WICED_BT_MESH_CORE_STATE_TYPE_SEQ.
 * It represents the own Sequence Number(SEQ) or SEQ in any entry of Replay Protection Lis(RPL).
 */
typedef struct
{
    uint16_t        addr;               /**< Unicast device address. 0 means own address */
    uint32_t        seq;                /**< Sequence Number (SEQ). It should be <= 0x00ffffff (3 bytes length). */
    uint8_t         previous_iv_idx;    /**< FALSE  - it is SEQ for current IV INDEX. TRUE - for previous. If addr is 0 then it is ignored. */
    uint16_t        rpl_entry_idx;      /**< Index of this entry in the RPL. It is needed to do direct access to the NVRAM IDX for write. If addr is 0 then it is ignored. */
} wiced_bt_mesh_core_state_seq_t;

/**
 * Data for state types WICED_BT_MESH_CORE_STATE_NET_KEY_XXX.
 */
typedef struct
{
    uint16_t        index;              /**< Net Key Index */
    const uint8_t   *key;               /**< Net Key */
} wiced_bt_mesh_core_state_net_key_t;

/**
 * Data for state types WICED_BT_MESH_CORE_STATE_APP_KEY_XXX.
 */
typedef struct
{
    uint16_t        net_key_index;      /**< Net Key Index */
    uint16_t        index;              /**< App Key Index */
    const uint8_t   *key;               /**< App Key */
} wiced_bt_mesh_core_state_app_key_t;

/**
 * Data for state types WICED_BT_MESH_CORE_STATE_APP_XXX.
 */
typedef struct
{
    uint16_t        element_address;    /**< Element Address */
    uint16_t        app_key_index;      /**< App Key Index */
    uint16_t        model_id;           /**< Model ID */
    uint16_t        vendor_id;          /**< Vendor ID. 0 means SIG model */
} wiced_bt_mesh_core_state_model_app_t;

/**
* Data for state types WICED_BT_MESH_CORE_STATE_NODE_STATE.
*/
typedef struct
{
    wiced_bool_t    provisioned;        /**< GATT DB for provisioned state */
    wiced_bool_t    proxy_on;           /**< Proxy is ON */
    wiced_bool_t    pb_adv;             /** <PB-ADV is in progress (between Link Opened and Link Closed) LPN application may want to change scanning parameters in unprovisioned state before provisioning start.*/
} wiced_bt_mesh_core_state_node_state_t;

/**
* Data for state types WICED_BT_MESH_CORE_STATE_LPN_FRIENDSHIP and WICED_BT_MESH_CORE_STATE_FRND_FRIENDSHIP.
*/
typedef struct
{
    wiced_bool_t    established;        /**< WICED_TRUE/WICED_FALSE - friendship is established/aborted. */
    uint16_t        addr;               /**< Peer address friendship is established or aborted with */
} wiced_bt_mesh_core_state_friendship_t;

typedef union
{
    wiced_bt_mesh_core_state_seq_t              seq;                        /**< State for type WICED_BT_MESH_CORE_STATE_TYPE_SEQ */
    wiced_bt_mesh_core_state_net_key_t          net_key;                    /**< State for types WICED_BT_MESH_CORE_STATE_NET_KEY_XXX */
    wiced_bt_mesh_core_state_app_key_t          app_key;                    /**< State for types WICED_BT_MESH_CORE_STATE_APP_KEY_XXX */
    wiced_bt_mesh_core_state_model_app_t        model_app;                  /**< State for types WICED_BT_MESH_CORE_STATE_MODEL_APP_XXX */
    wiced_bool_t                                kr_flag;                    /**< State for type WICED_BT_MESH_CORE_STATE_KR */
    wiced_bool_t                                lpn_scan;                   /**< request for the application to start/stop scan in the lpn mode */
    wiced_bt_mesh_core_state_node_state_t       node_state;                 /**< State for type WICED_BT_MESH_CORE_STATE_NODE_STATE */
    wiced_bt_mesh_core_state_iv_t               iv;                         /**< New IV Index and IV-UPDATE flag values for WICED_BT_MESH_CORE_STATE_IV */
    wiced_bt_mesh_core_state_friendship_t       lpn;                        /**< LPN friendship state is changed - established or aborted */
    uint32_t                                    lpn_sleep;                  /**< LPN application can go to sleep mode for a specific sleep time or can return
                                                                                 with same or changed sleep time to let core to procceed with poll after that time. */
    wiced_bt_mesh_core_state_friendship_t       frnd;                       /**< Friend friendship state is changed - established or aborted */

} wiced_bt_mesh_core_state_t;

typedef void(*wiced_bt_mesh_core_state_changed_callback_t)(wiced_bt_mesh_core_state_type_t type, wiced_bt_mesh_core_state_t *p_state);

/**
* Data for wiced_bt_mesh_core_init
*/
typedef struct
{
    uint8_t device_uuid[16];                                    /**< 128-bit Device UUID. Device manufacturers shall follow the standard UUID format and generation procedure to ensure the uniqueness of each Device UUID */
    uint8_t non_provisioned_bda[6];                             /**< BD address in non-provisioned state */
    uint8_t provisioned_bda[6];                                 /**< BD address in provisioned state */
    wiced_bt_mesh_core_config_t *p_config_data;                 /**< Configuration data */
    wiced_bt_mesh_core_get_msg_handler_callback_t callback;     /**< Callback function to be called by the Core at received message to get message handler */
    wiced_bt_mesh_core_publication_callback_t pub_callback;     /**< Callback function to be called by the Core at time when publication is required by periodic publication or when configuration changes */
    wiced_bt_mesh_core_scan_callback_t scan_callback;           /**< Callback function to be called by the Core when scan needs to be started or stopped */
    wiced_bt_mesh_core_proxy_send_cb_t proxy_send_callback;     /**< Callback function to send proxy packet over GATT */
    wiced_bt_core_nvram_access_t nvram_access_callback;         /**< Callback function to read/write from/to NVRAM */
    wiced_bt_mesh_core_health_fault_test_cb_t fault_test_cb;    /**< Callback function to be called to invoke a self test procedure of an Element */
    wiced_bt_mesh_core_attention_cb_t attention_cb;             /**< Callback function to be called to attract human attention */
    wiced_bt_mesh_core_state_changed_callback_t state_changed_cb;   /**< Callback function to be called on any change in the mesh state */
} wiced_bt_mesh_core_init_t;


/**
 * \brief Mesh Core initialization.
 * \details The wiced_bt_mesh_core_init function should be called at device startup before calling wiced_bt_mesh_core_register() for each supported model.
 * The function should be called just once with non-NULL p_init and non-NULL p_init->p_config_data at the startup time before calling mesh_app_init.
 * Call this function with NULL p_init or with NULL p_init->p_config_data to reset device into unprovisioned state. In that mode if p_init isn't NULL
 * then following members should contain new values: device_uuid, non_provisioned_bda and provisioned_bda.
 *
 * @param[in]   p_init          Initialization parameters
 *
 * @return      wiced_result_t WICED_BT_SUCCESS means device provisioned, WICED_BT_PENDING - it is not provisioned, otherwise it is not functional.
 */
wiced_result_t wiced_bt_mesh_core_init(wiced_bt_mesh_core_init_t *p_init);

/**
 * \brief Mesh Core Start.
 * \details The wiced_bt_mesh_core_start function should be called at device startup after calling wiced_bt_mesh_core_init().
 * The function starts beacons and service advertisments. It is usually called by application from mesh_app_init().
 */
void wiced_bt_mesh_core_start(void);

/**
 * \brief Sets GATT MTU for the provisioning or proxy connection.
 * \details Called by the app to set max sending packet length in the core and provisioning layer.
 *
 * @param[in]   gatt_mtu       Maximum sizeo of the packet to be used over GATT. 0 means default value (20).
 */
void wiced_bt_mesh_core_set_gatt_mtu(uint16_t gatt_mtu);

/**
 * \brief Gets local mesh address set by provisioner
 *
 * @return Local mesh address
 */
uint16_t wiced_bt_mesh_core_get_local_addr(void);

/**
 * \brief Send message complete callback.
 * \details While sending message to the core for transmission over the mesh, the application may provide this function
 * to be notified when the operation has been completed. The Mesh Core library passes the pointer to the same event which
 * was included in the send call.  When callback is executed the application should take the ownership of the mesh event
 * and shell release or reuse it.
 *
 * @param[in]   p_event       Mesh event passed to wiced_bt_mesh_core_send()
 *
 * @return      None.
 */
typedef void(*wiced_bt_mesh_core_send_complete_callback_t)(wiced_bt_mesh_event_t *p_event);

/**
 * \brief Sends the message to the dst address.
 * \details This function encrypts and authenticates the application payload and then passes it to transport layer
 * and then to the network layer for sending out. If the function is used for sending the response to the received
 * message then it should use app_key_idx and SRC of the received message and default TTL. If the function is used
 * for sending unsolicited message and application does not know the destination address, the publication information
 * for the models should be used.
 *
 * If the complete_callback is set to NULL the Core library will release the mesh event when it is done processing of
 * the event. Otherwise the Core library will call complete_callback at the end of retransmission. At that time the
 * caller takes ownership of the mesh event and shall reuse it or release.
 *
 * @param[in]   p_event         All details of the message to be sent
 * @param[in]   params          Parameters of the Access Payload
 * @param[in]   params_len      Length of the Parameters of the Access Payload
 * @param[in]   complete_callback   Callback function to be called at the end of all retransmissions. Can be NULL.
 *
 * @return      wiced_result_t
 */
wiced_result_t wiced_bt_mesh_core_send(wiced_bt_mesh_event_t *p_event, const uint8_t* params, uint16_t params_len, wiced_bt_mesh_core_send_complete_callback_t complete_callback);

/**
 * \brief Stop sending retransmissions.
 * \details Mesh Core library when sending mesh packets over the advertising channel typically retransmits every
 * packet several times to improve reliability of the delivery. When it is a client request (for example a Get or
 * a Set message), the peer may receive the earlier transmission and reply to it. When a Model library receives
 * the reply, and detects that there is no need to retransmit the packet, it can call the wiced_bt_mesh_core_cancel_send
 * function to cancel the retransmission of the packet.
 *
 * @param[in]   p_event       Mesh event used to send message by wiced_bt_mesh_core_send() for which retransitions should be stoped.
 *
 * @return      None
 */
void wiced_bt_mesh_core_cancel_send(wiced_bt_mesh_event_t *p_event);

/**
 * \brief Handles received advertising packet.
 * \details The Mesh Application library calls this function for each received advertisement packet.
 *
 * @param[in]   rssi             RSSI of the received packet
 * @param[in]   p_adv_data       Advertisement packet data
 *
 * @return      wiced_result_t
 */
wiced_result_t wiced_bt_mesh_core_adv_packet(int8_t rssi, const uint8_t *p_adv_data);

/**
 * \brief Handle packet received by proxy via GATT.
 * \details Application should call that function on each received packet from the GATT data in characteristic of the proxy service.
 *
 * @param[in]   p_data          Received packet
 * @param[in]   data_len        Length of the received packet.
 *
 * @return      None
 */
void wiced_bt_mesh_core_proxy_packet(const uint8_t* p_data, uint8_t data_len);

/**
 * \brief Get Network Key Index.
 * \details Application can call this function to verify that Network ID is provisioned on the device.
 *
 * @param[in]   p_network_id    :Network ID in question
 * @param[out]  p_net_key_idx   :Valid Natwork Key Index
 *
 * @return      WICED_TRUE if the Network ID is known, otherwise WICED_FALSE.
 */
wiced_bool_t wiced_bt_mesh_core_is_valid_network_id(uint8_t *p_network_id, uint16_t * p_net_key_idx);

/**
 * \brief Handles connection up and down events.
 * \details Application should call that function on each connection up and down state. Only one connection is alowed.
 *
 * @param[in]   conn_id             Connection ID if connected. 0 if disconnected
 * @param[in]   connected_to_proxy  If TRUE, core will send all the messages to this connection.  Otherwise core will send packet to all bearers.
 * @param[in]   ref_data            Core passes this parameter back to the application when packet needs to be sent out.
 *                                  On disconnection (if conn_id == 0) this parameter contains disconnection reason wiced_bt_gatt_disconn_reason_t
 * @param[in]   mtu                 When sending a message, core should fragment the packet to fit into the mtu.
 *
 * @return      None
 */
void wiced_bt_mesh_core_connection_status(uint32_t conn_id, wiced_bool_t connected_to_proxy, uint32_t ref_data, uint16_t mtu);

/**
 * \brief Checks if received Service Data for Mesh Proxy Service with Node Identity corresponds to that node address
 * \details Application should call that function on each received advertizing with proxy service data for Mesh Proxy Service with Node Identity to check if it is came from specific node address.
 *  Example:
 *  if (NULL != (p_data = wiced_bt_ble_check_advertising_data(received_adv_data, 0x16, &len)))
 *      wiced_bt_mesh_core_check_node_identity(address, p_data, len, NULL);
 *
 * @param[in]   addr            Node address to check Identity for
 * @param[in]   p_data          Received service data returned by wiced_bt_ble_check_advertising_data()
 * @param[in]   len             Length of the received service data.
 * @param[out]  net_key_idx     Optional (can be NULL) pointer to variable to receive matched global network index - it is valid on success only.
 *
 * @return      WICED_TRUE/WICED_FALSE - success/failed
 */
wiced_bool_t wiced_bt_mesh_core_check_node_identity(uint16_t addr, const uint8_t *data, uint8_t len, uint16_t *p_net_key_idx);

 /**
 * @anchor WICED_BT_MESH_CORE_TEST_MODE_SIGNAL
 * @name Test Mode Signals.
 * \details The following is the list of signals used for certification and compliance testing.
 * @{
 */
#define WICED_BT_MESH_CORE_TEST_MODE_SIGNAL_IV_UPDATE_BEGIN     0   /**< the node shall transition to the IV Update in Progress state, ignoring the 96 hour limit. */
#define WICED_BT_MESH_CORE_TEST_MODE_SIGNAL_IV_UPDATE_END       1   /**< the node shall transition to the Normal state, ignoring the 96 hour limit. */
#define WICED_BT_MESH_CORE_TEST_MODE_SIGNAL_TEST                2   /**< test signal for internal use. */
 /** @} WICED_BT_MESH_CORE_TEST_MODE_SIGNAL */

/**
 * \brief The signals for different test modes.
 * \details Application shall support some test modes (for example IV update) used for certification and compliance testing.
 * The activation of the test mode shall be carried out locally (via a HW or SW interface).
 *
 * @param[in]   opcode       opcode (see HCI_CONTROL_MESH_COMMAND_CORE_XXX in hci_control_api.h)
 * @param[in]   p_data       data
 * @param[in]   data_len     length of the data
 *
 * @return      WICED_BT_SUCCESS if opcode is recognized and handles. Otherwise caller has to call some other handler
 */
wiced_result_t wiced_bt_mesh_core_test_mode_signal(uint16_t opcode, const uint8_t *p_data, uint16_t data_len);

/*
 * \breaf Calculates URI hash URI Hash: s1(URI Data)[0-3]
 *
 * @param[in]   uri         URI data: <scheme(1byte: 0x16-"http:" or 0x17-"https:")><uri with removed scheme(max 29bytes)>
 * @param[in]   uri_len     Length of the URI data <= 29
 * @param[out]  hash        buffer to received calculated URI hash
*/
void wiced_bt_mesh_core_calc_uri_hash(const uint8_t *uri, uint8_t len, uint8_t *hash);

/**
* \brief Sets node identity state as a result of user interaction.
* @param        None
* @return       None
*/
void wiced_bt_mesh_core_set_node_identity(void);

/**
 * \brief Indicates the fault happened.
 * \details Application shall call that function to indicate the fault
 *
 * @param[in]   element_idx         Index of the elemnt
 * @param[in]   test_id             Identifier of a specific test to be performed
 * @param[in]   company_id          Company ID of the test
 * @param[in]   faults_number       Number of faults in FaultArray
 * @param[out]  fault_array         FaultArray
 *
 * @return      WICED_TRUE if accepted. Otherwise returns WICED_FALSE.
 */
wiced_bool_t wiced_bt_mesh_core_health_set_faults(uint8_t element_idx, uint8_t test_id, uint16_t company_id, uint8_t faults_number, uint8_t *fault_array);

#define WICED_BT_MESH_PROVISION_PRIV_KEY_LEN        32      /**< Size of the private key in bytes */

#define WICED_BT_MESH_KEY_LEN                       16      /**< Size of the mesh keys */

/**
 * @anchor BT_MESH_PROVISION_RESULT
 * @name Provisioning Result codes
 * \details The following is the list of Provisioning Result codes that a provisioning layer can return in wiced_bt_mesh_provision_end_cb_t().
 * @{
 */
#define WICED_BT_MESH_PROVISION_RESULT_SUCCESS   0   /**< Provisioning succeeded */
#define WICED_BT_MESH_PROVISION_RESULT_TIMEOUT   1   /**< Provisioning failed due to timeout */
#define WICED_BT_MESH_PROVISION_RESULT_FAILED    2   /**< Provisioning  failed */
/** @} BT_MESH_PROVISION_RESULT */

/**
 * @anchor BT_MESH_OUT_OOB_ACT
 * @name Output OOB Action field values
 * @{
 */
#define WICED_BT_MESH_PROVISION_OUT_OOB_ACT_BLINK                 0x00  /**< Blink */
#define WICED_BT_MESH_PROVISION_OUT_OOB_ACT_BEEP                  0x01  /**< Beep */
#define WICED_BT_MESH_PROVISION_OUT_OOB_ACT_VIBRATE               0x02  /**< Vibrate */
#define WICED_BT_MESH_PROVISION_OUT_OOB_ACT_DISP_NUM              0x03  /**< Output Numeric */
#define WICED_BT_MESH_PROVISION_OUT_OOB_ACT_DISP_ALPH             0x04  /**< Output Alphanumeric */
#define WICED_BT_MESH_PROVISION_OUT_OOB_ACT_MAX                   0x15  /**< Max number of supported actions */
/** @} BT_MESH_OUT_OOB_ACT */

/**
 * @anchor BT_MESH_IN_OOB_ACT
 * @name Input OOB Action field values
 * @{
 */
#define WICED_BT_MESH_PROVISION_IN_OOB_ACT_PUSH                   0x00  /**< Push */
#define WICED_BT_MESH_PROVISION_IN_OOB_ACT_TWIST                  0x01  /**< Twist */
#define WICED_BT_MESH_PROVISION_IN_OOB_ACT_ENTER_NUM              0x02  /**< Input Number */
#define WICED_BT_MESH_PROVISION_IN_OOB_ACT_ENTER_STR              0x03  /**< Input Alphanumeric */
#define WICED_BT_MESH_PROVISION_IN_OOB_ACT_MAX                    0x15  /**< Max number of supported actions */
/** @} BT_MESH_IN_OOB_ACT */

/**
 * @anchor BT_MESH_PROVISION_DEFS
 * @name Provisioning Capabilities and Start common definitions
 * \details The following is the list of Provisioning in/out action codes and max sizes are the same for both capabilities and start messages.
 * @{
 */
#define WICED_BT_MESH_PROVISION_OUT_OOB_MAX_SIZE                  0x08
#define WICED_BT_MESH_PROVISION_IN_OOB_MAX_SIZE                   0x08
#define WICED_BT_MESH_PROVISION_STATIC_OOB_MAX_SIZE               0x10
 /** @} BT_MESH_PROVISION_DEFS */

/**
 * @anchor BT_MESH_PROVISION_CAPABILITIES
 * @name Provisioning Capabilities definitions
 * \details The following is the list of the definitions for capabilities message.
 * @{
 */
#define WICED_BT_MESH_PROVISION_ALG_FIPS_P256_ELLIPTIC_CURVE      0x0001
#define WICED_BT_MESH_PROVISION_CAPS_PUB_KEY_TYPE_AVAILABLE       0x01
#define WICED_BT_MESH_PROVISION_CAPS_PUB_KEY_TYPE_PROHIBITED      0xfe
#define WICED_BT_MESH_PROVISION_CAPS_STATIC_OOB_TYPE_AVAILABLE    0x01
#define WICED_BT_MESH_PROVISION_CAPS_STATIC_OOB_TYPE_PROHIBITED   0xfe
/** @} BT_MESH_PROVISION_CAPABILITIES */

/**
 * Defines Capabilities data of the provisioning device
 */
typedef struct
{
    uint8_t   elements_num;             /**< Number of elements supported by the device (1-255) */
    uint8_t   algorithms[2];            /**< Bitmap of Supported algorithms and other capabilities (FIPS_P256_EC is the only one supported at 1.0 time) */
    uint8_t   pub_key_type;             /**< Bitmap of Supported public key types (Static OOB information available 0x01 is the only one supported at 1.0 time) */
    uint8_t   static_oob_type;          /**< Supported static OOB Types (1 if available) */
    uint8_t   output_oob_size;          /**< Maximum size of Output OOB supported (0 - device does not support output OOB, 1-8 max size in octets supported by the device) */
    uint8_t   output_oob_action[2];     /**< Output OOB Action field values (see @ref BT_MESH_OUT_OOB_ACT "Output OOB Action field values") */
    uint8_t   input_oob_size;           /**< Maximum size in octets of Input OOB supported */
    uint8_t   input_oob_action[2];      /**< Supported Input OOB Actions (see @ref BT_MESH_IN_OOB_ACT "Input OOB Action field values") */
} wiced_bt_mesh_core_provision_capabilities_t;

/**
 * @anchor BT_MESH_PROVISION_START
 * @name Provisioning Start definitions
 * \details The following is the list of the definitions for start message.
 * @{
 */
#define WICED_BT_MESH_PROVISION_START_ALG_FIPS_P256               0x00
#define WICED_BT_MESH_PROVISION_START_PUB_KEY_NO                  0x00
#define WICED_BT_MESH_PROVISION_START_PUB_KEY_USED                0x01
#define WICED_BT_MESH_PROVISION_START_AUTH_METHOD_NO              0x00
#define WICED_BT_MESH_PROVISION_START_AUTH_METHOD_STATIC          0x01
#define WICED_BT_MESH_PROVISION_START_AUTH_METHOD_OUTPUT          0x02
#define WICED_BT_MESH_PROVISION_START_AUTH_METHOD_INPUT           0x03
/** @} BT_MESH_PROVISION_START */

/**
 * Defines Start data sent by Provisioner to the provisioning device
 */
typedef struct
{
    uint8_t   algorithm;                    /**< The algorithm used for provisioning (see @ref BT_MESH_PROVISION_START) */
    uint8_t   public_key_oob_available;     /**< Public Key used (if 0, public key is not used */
    uint8_t   auth_method;                  /**< Authentication Method used */
    uint8_t   auth_action;                  /**< Selected Output OOB Action (see @ref BT_MESH_OUT_OOB_ACT) or Input OOB Action (see @ref BT_MESH_IN_OOB_ACT) or 0x00 */
    uint8_t   auth_size;                    /**< Size of the Output OOB used or size of the Input OOB used or 0x00 */
} wiced_bt_mesh_core_provision_start_t;

/**
* @anchor WICED_BT_MESH_PROVISION_TYPE
* @name Provisioning Type definitions
* \details The following is the list of the definitions for provisioning type.
* @{
*/
#define WICED_BT_MESH_PROVISION_TYPE_PROVISIONING_NODE  0   // it is a provisioning node
#define WICED_BT_MESH_PROVISION_TYPE_PROVISIONER        1   // this is the provisioner side
#define WICED_BT_MESH_PROVISION_TYPE_SELF_PROVISIONER   2   // it is self provisioner sizde (both privisioner side and provisioning node)
/** @} WICED_BT_MESH_PROVISION_TYPE */

/**
 * Mesh Core Provisioning Control Block
 */
typedef struct
{
    uint32_t  conn_id;            /**< link id (connection id) for PB_ADV */
    uint8_t   state;              /**< one of the BP_ADV_TRANSPORT_STATE_XXX */
    uint8_t   client_pdu_sent;    /**< TRUE(>0) during the time when RPR client is senfing PDU to the provisioning node */
    uint8_t   pb_type;            /**< type of the provisioning. one of the PB_TYPE_XXX */
    uint16_t  node_id;            /**< address of the provisioning node */
    uint32_t  timeout;            /**< end timeout between PDUs in seconds - disconnect after that */
    uint8_t   identify_duration;  /**< Identify duration field of the Invite PDU */
    uint8_t   type;               /**< Provisioning type (see @ref WICED_BT_MESH_PROVISION_TYPE) */
    wiced_bt_mesh_core_provision_capabilities_t capabilities;
    wiced_bt_mesh_core_provision_start_t        start;
    uint8_t   peer_public_key_or_ecdh_secret[WICED_BT_MESH_PROVISION_PUBLIC_KEY_LEN];  /**< we will use it for ECDH secret too */
    uint8_t   conf_key_or_dev_key[WICED_BT_MESH_KEY_LEN];
    uint8_t   confirmation[WICED_BT_MESH_PROVISION_CONFIRMATION_LEN];
    uint8_t   prov_salt[WICED_BT_MESH_KEY_LEN];
    uint8_t   session_key[WICED_BT_MESH_KEY_LEN];
    uint8_t   random[WICED_BT_MESH_PROVISION_RANDOM_LEN];
    uint8_t   oob_value[16];    /**< max static OOB value len is 256 bits(16bytes). Max input and output OOB value length is 8 bytes */
    uint8_t   oob_value_len;    /**< length of the OOB value */
    uint16_t  net_key_idx;      /**< NetKeyIdx provisioned */
} mesh_core_provision_cb_t;


/*------------------- API for provisioner and provisioning node -------------------------------------*/

/**
 * \brief Definition of the callback function to send provisioning packet.
 * \details Application implements that function if it want to use
 * GATT notification or even external function (for example Windows provisioner).
 * Called by provisioning layer to send packet to the provisioner or provisioning node.
 *
 * @param[in]   conn_id         GATT connection ID
 * @param[in]   packet          Packet to send
 * @param[in]   packet_len      Length of the packet to send
 *
 * @return      None
 */
typedef void(*wiced_bt_mesh_core_provision_gatt_send_cb_t)(uint16_t conn_id, const uint8_t *packet, uint32_t packet_len);

/**
 * \brief Definition of the callback function on Starts provisioning.
 * \details Provisioner or/and provisioning application implements that function to be called on successful start of provisioning.
 *
 * @param[in]   conn_id         Connection ID of the provisioning connection
 *
 * @return   None
 */
typedef void (*wiced_bt_mesh_core_provision_started_cb_t)(
    uint32_t  conn_id);

/**
 * \brief Definition of the callback function on provisioning end.
 * \details Provisioner or/and provisioning application implements that function to be called on successfull or failed end of provisioning.
 *
 * @param[in]   conn_id     Connection ID of the provisioning connection
 * @param[in]   addr        Address assigned to the first element of the device
 * @param[in]   net_key_idx Net Key Index configured to the device
 * @param[in]   result      Provisioning Result code (see @ref BT_MESH_PROVISION_RESULT "Provisioning Result codes")
 * @param[in]   p_dev_key   Pointer to the Device Key generated during provisioning. It is valid only if result is SUCCESS(0).
 *                          On provisioner side that pointer becomes invalid after return. On provisioning node side it can be use at any time.
 *
 * @return   None
 */
typedef void (*wiced_bt_mesh_core_provision_end_cb_t)(
    uint32_t        conn_id,
    uint16_t        addr,
    uint16_t        net_key_idx,
    uint8_t         result,
    const uint8_t   *p_dev_key);

/**
 * @anchor BT_MESH_PROVISION_GET_OOB
 * @name OOB callback definitions
 * \details Defines possible values for OOB callback wiced_bt_mesh_provision_get_oob_cb_t.
 * @{
 */
#define WICED_BT_MESH_PROVISION_GET_OOB_TYPE_NONE            0   /**< Provisioner: OOB not used */
#define WICED_BT_MESH_PROVISION_GET_OOB_TYPE_ENTER_PUB_KEY   1   /**< Provisioner: Enter public key() */
#define WICED_BT_MESH_PROVISION_GET_OOB_TYPE_ENTER_OUTPUT    2   /**< Provisioner: Enter output OOB value(size, action) displaied on provisioning node */
#define WICED_BT_MESH_PROVISION_GET_OOB_TYPE_ENTER_STATIC    3   /**< Provisioner: Enter static OOB value(size)*/
#define WICED_BT_MESH_PROVISION_GET_OOB_TYPE_ENTER_INPUT     4   /**< Provisioning node: Enter input OOB value(size, action) displaied on provisioner */
#define WICED_BT_MESH_PROVISION_GET_OOB_TYPE_DISPLAY_INPUT   5   /**< Provisioner: Select and display input OOB value(size, action)*/
#define WICED_BT_MESH_PROVISION_GET_OOB_TYPE_DISPLAY_OUTPUT  6   /**< Provisioning node: Select and display output OOB value(size, action) */
#define WICED_BT_MESH_PROVISION_GET_OOB_TYPE_DISPLAY_STOP    7   /**< Provisioner and Provisioning node: Stop displaying OOB value */
#define WICED_BT_MESH_PROVISION_GET_OOB_TYPE_GET_STATIC      8   /**< Provisioning node: Requests static OOB value(size) */
/** @} BT_MESH_PROVISION_GET_OOB */

/**
 * \brief Definition of the callback function on get OOB.
 * \details Provisioner application implements that function to be called on get OOB
 * if it supposed to enter public key, output OOB, or static OOB or display input OOB.
 * Provisioning application implements that function to be called on get OOB
 * if it supposed to enter input OOB or display output OOB
 * App should call wiced_bt_mesh_provision_set_oob() to proceed.
 * Provisioner layer calls that function to request app to enter requested value and call pb_put_oob with entered value.
 *
 * @param[in]   conn_id     Connection ID of the provisioning connection
 * @param[in]   type        OOB type requested (see @ref BT_MESH_PROVISION_GET_OOB "OOB callback definitions")
 * @param[in]   size        Ignored for PUB_KEY, size for STATIC, INPUT and OUTPUT, ignored for DISPLAY_STOP (see spec for possible values)
 * @param[in]   action      Ignored for PUB_KEY and STATIC, action for INPUT and OUTPUT, ignored for DISPLAY_STOP(see spec for possible values)
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
typedef wiced_bool_t (*wiced_bt_mesh_core_provision_get_oob_cb_t)(
    uint32_t  conn_id,
    uint8_t   type,
    uint8_t   size,
    uint8_t   action);

/**
 * \brief Definition of the callback function on Capabilities.
 * \details Provisioner application implements that function to be called on receiving the capabilities of provisioning node.
 * App should call wiced_bt_mesh_provision_set_mode() to proceed.
 *
 * @param[in]   conn_id         Connection ID of the provisioning connection
 * @param[in]   capabilities    Capabilities received from provisioning node
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
typedef wiced_bool_t (*wiced_bt_mesh_core_provision_on_capabilities_cb_t)(
    uint32_t                                        conn_id,
    const wiced_bt_mesh_core_provision_capabilities_t    *capabilities);

/**
 * \brief Definition of the callback function on get Capabilities.
 * \details Provisioning application implements that function to be called by provisioning layer to get capabilities of provisioning node from the app.
 * App should call wiced_bt_mesh_core_provision_set_capabilities() to proceed
 *
 * @param[in]   conn_id         Connection ID of the provisioning connection
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
typedef wiced_bool_t (*wiced_bt_mesh_core_provision_get_capabilities_cb_t)(
    uint32_t                              conn_id);

/**
 * \brief Initializes provisioning layer for the node(server side).
 * \details Called by provisioning app to initialize provisioning layer for both PB_ADV and PB_GATT.
 *
 * @param[in]   priv_key                Private key(32 bytes) to use for ECDH authentication. It should be same is both client and server are initialized
 * @param[in]   started_cb              Callback function to be called on provisioning start.
 * @param[in]   end_cb                  Callback function to be called on provisioning end, or error.
 * @param[in]   get_capabilities_cb     Callback function to be called on Capabilities get
 * @param[in]   get_oob_cb              Callback function to be called on OOB get
 * @param[in]   gatt_send_cb            Callback function to send provisioning packet ofer GATT
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_core_provision_server_init(
    uint8_t                                              *priv_key,
    wiced_bt_mesh_core_provision_started_cb_t            started_cb,
    wiced_bt_mesh_core_provision_end_cb_t                end_cb,
    wiced_bt_mesh_core_provision_get_capabilities_cb_t   get_capabilities_cb,
    wiced_bt_mesh_core_provision_get_oob_cb_t            get_oob_cb,
    wiced_bt_mesh_core_provision_gatt_send_cb_t          gatt_send_cb);

/**
 * \brief Sets OOB in response on wiced_bt_mesh_provision_get_oob_cb.
 * \details Called by provisioner and provisioning application as a responce on wiced_bt_mesh_provision_get_oob_cb.
 *
 * @param[in]   value            Requested OOB value
 * @param[in]   value_len        Length of the requested OOB value
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_core_provision_set_oob(
    uint8_t   *value,
    uint8_t   value_len);

/**
 * \brief Sets Capabilities of the provisioning device.
 * \details Called by provisioning app set capabilities of provisioning node.
 *
 * @param[in]   conn_id         Connection ID of the provisioning connection
 * @param[in]   capabilities    Capabilities of the provisioning node
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_core_provision_set_capabilities(  /* ToDo rename to server_set_cap */
    uint32_t                                           conn_id,
    const wiced_bt_mesh_core_provision_capabilities_t  *capabilities);

/**
 * \brief Processes received PB_GATT packets
 *
 * @param[in]   is_notification     If TRUE processing GATT Notification, otherwise GATT Write Cmd.
 * @param[in]   conn_id             Connection ID. It is needed in case of few simultaneous provisioning via GATT.
 * @param[in]   packet              Packet to process.
 * @param[in]   packet_len          Length of the packet to process
 *
 * @return      None
 *
 */
void wiced_bt_mesh_core_provision_gatt_packet(uint8_t is_notification, uint32_t conn_id, const uint8_t *packet, uint8_t packet_len);

/**
 * \brief Provision Local Device.
 * \details Called by provisioning and provisioner app to initialize the device.
 *
 * @param[in]   addr       Address of the first element of the device
 * @param[in]   dev_key     Local Device Key.  If we are a provisioner, this key can be used to configure this device.
 * @param[in]   net_key     Network Key
 * @param[in]   net_key_idx Network Key Index
 * @param[in]   iv_idx      Current IV index
 * @param[in]   key_refresh Key refresh phase 2 is in progress
 * @param[in]   iv_update   iv_update is in progress
 *
 * @return   WICED_TRUE/WICED_FALSE - success/failed.
 */
wiced_bool_t wiced_bt_mesh_core_provision_local(uint16_t addr, uint8_t *dev_key, uint8_t *network_key, uint16_t net_key_idx, uint32_t iv_idx, uint8_t key_refresh, uint8_t iv_update);

/**
 * \brief Set Sequence Number (SEQ) own or for RPL(Replay Protection List).
 *
 * @param[in]   addr                Unicast device address. 0 means own address.
 * @param[in]   seq                 Sequence Number (SEQ). It should be <= 0x00ffffff (3 bytes length).
 * @param[in]   prev_iv_idx         FALSE  - it is SEQ for current IV INDEX. TRUE - for previous. If addr is 0 then it is ignored.
 *
 * @return      TRUE on success
 *
 */
wiced_bool_t wiced_bt_mesh_core_set_seq(uint16_t addr, uint32_t seq, wiced_bool_t prev_iv_idx);

/**
 * \brief Removes node from RPL(Replay Protection List).
 *
 * @param[in]   addr                Unicast device address to remove RPL record.
 *
 * @return      TRUE on success
 *
 */
wiced_bool_t wiced_bt_mesh_core_del_seq(uint16_t addr);

/**
 * \brief Calculates size of the memory for configuration.
 *
 * @param[in]   config              Configuration data.
 *
 * @return      size of the memory for configuration
 *
 */
uint16_t wiced_bt_mesh_get_node_config_size(wiced_bt_mesh_core_config_t *config);

/**
 * \brief Stops proxy server advertisement and network secure beacon if they are running
 *
 * @param       None
 *
 * @return      None
 */
void wiced_bt_mesh_core_stop_advert(void);

/**
* Reperesents statistics of messages receiving, sending and handling by network and bearers layers.
*/
typedef struct
{
    uint32_t    received_msg_cnt;                   /**< Number of the all received messages */
    uint32_t    received_proxy_cfg_msg_cnt;         /**< Number of proxy configuration messages */
    uint32_t    relayed_msg_cnt;                    /**< Number of the relayed messages */
    uint32_t    accepted_unicast_msg_cnt;           /**< Number of the unicast messages passed up to transport layer */
    uint32_t    accepted_group_msg_cnt;             /**< Number of the group messages passed up to transport layer */
    uint32_t    received_for_lpn_msg_cnt;           /**< Number of the messages addressed to the friend LPN */

    uint32_t    dropped_invalid_msg_cnt;            /**< Number of dropped invalid messages (too short, 0 or own SRC, not from friend, invalid proxy config, 0 DST */
    uint32_t    dropped_by_nid_msg_cnt;             /**< Number of the messages with unsupported NID */
    uint32_t    dropped_not_decrypted_msg_cnt;      /**< Number of the messages failed to be decryptd */
    uint32_t    dropped_by_net_cache_msg_cnt;       /**< Number of the messages dropped by network cache */
    uint32_t    not_relayed_by_ttl_msg_cnt;         /**< Number of the messages not relayed because TTL <= 1 */
    uint32_t    dropped_group_msg_cnt;              /**< Number of the dropped group messages (no subscriptions) */

    uint32_t    sent_adv_msg_cnt;                   /**< Number of messages sent via adv bearer */
    uint32_t    sent_adv_unicast_msg_cnt;           /**< Number of unicast messages sent via adv bearer */
    uint32_t    sent_adv_group_msg_cnt;             /**< Number of group messages sent via adv bearer */
    uint32_t    sent_gatt_msg_cnt;                  /**< Number of messages sent via gatt bearer */
    uint32_t    sent_proxy_cfg_msg_cnt;             /**< Number of sent proxy configuration messages */
    uint32_t    sent_proxy_clnt_msg_cnt;            /**< Number of sent proxy client messages */
    uint32_t    sent_net_credentials_msg_cnt;       /**< Number of messages sent with network credentials */
    uint32_t    sent_frnd_credentials_msg_cnt;      /**< Number of messages sent with friendship credentials */
} wiced_bt_mesh_core_statistics_t;

/**
* Reperesents statistics of messages receiving, sending and handling by transport layers.
*/
typedef struct
{
    uint32_t    sent_access_layer_msg_cnt;          /**< Number of messages sent by access layer */
    uint32_t    sent_unseg_msg_cnt;                 /**< Number of sent unsegmented messages */
    uint32_t    sent_seg_msg_cnt;                   /**< Number of sent segments */
    uint32_t    sent_ack_msg_cnt;                   /**< Number of sent ACKs */

    uint32_t    received_access_layer_msg_cnt;      /**< Number of messages received by access layer */
    uint32_t    received_unseg_msg_cnt;             /**< Number of received unsegmented messages */
    uint32_t    received_seg_msg_cnt;               /**< Number of received segments */
    uint32_t    received_ack_msg_cnt;               /**< Number of received ACKs */
    uint32_t    dropped_access_layer_msg_cnt;       /**< Number of messages dropped by access layer */

    uint32_t    df_not_relayed_msg_cnt;             /**< Number of messages which don't relay using Directed Forwarding Mode */
    uint32_t    df_relayed_msg_cnt;                 /**< Number of messages which relayed using Directed Forwarding Mode */
    uint32_t    df_sent_ack_msg_cnt;                /**< Number of Segment Acknowledgment message using Directed Forwarding Mode */
    uint32_t    df_sent_msg_cnt;                    /**< Number of messages sent using Directed Forwarding Mode  */
} wiced_bt_mesh_core_transport_statistics_t;

/**
* \brief Requests collected statistics for network layer.
*
* @param[out]   p_data             pointer where to copy network layer statistics
*
* @return       None
*/
void wiced_bt_mesh_core_statistics_get(wiced_bt_mesh_core_statistics_t *p_data);

/**
* \brief Requests collected statistics for transport layer.
*
* @param[out]   p_data             pointer where to copy transport layer statistics
*
* @return       None
*/
void wiced_bt_mesh_core_transport_statistics_get(wiced_bt_mesh_core_transport_statistics_t *p_data);

/**
* \brief Resets statistics of the network layer.
*
* @param       None
*
* @return      None
*/
void wiced_bt_mesh_core_statistics_reset(void);

/**
* \brief Resets statistics of the transport layers.
*
* @param       None
*
* @return      None
*/
void wiced_bt_mesh_core_transport_statistics_reset(void);

/* variable with max number of network keys - can be set by app before call to wiced_bt_mesh_get_node_config_size() */
extern uint8_t wiced_bt_mesh_core_net_key_max_num;
/* variable with max number of application keys - can be set by app before call to wiced_bt_mesh_get_node_config_size() */
extern uint8_t wiced_bt_mesh_core_app_key_max_num;
/* variable with size of Network Cache, Default value is 20 */
extern uint8_t wiced_bt_mesh_core_net_cache_size;
/* Maximum number of addresses in the Proxy Output Filter. Default value is 16 */
extern uint8_t wiced_bt_mesh_core_proxy_out_flt_addr_max_num;
/* variable with max number of the lower-transport-layer control blocks. Default value is 8. If needed change it before call to wiced_bt_mesh_core_init() */
extern uint8_t wiced_bt_mesh_core_lower_transport_ctrl_blocks_max_num;

/* variables with NVRAM IDs. If needed change them before call to wiced_bt_mesh_core_init() */
extern uint16_t wiced_bt_mesh_core_nvm_idx_node_data;
extern uint16_t wiced_bt_mesh_core_nvm_idx_virt_addr;
extern uint16_t wiced_bt_mesh_core_nvm_idx_frnd_state;
extern uint16_t wiced_bt_mesh_core_nvm_idx_net_key_begin;
extern uint16_t wiced_bt_mesh_core_nvm_idx_app_key_begin;
extern uint16_t wiced_bt_mesh_core_nvm_idx_health_state;
extern uint16_t wiced_bt_mesh_core_nvm_idx_cfg_data;

/* Advertisements TX Power. Default value is 4. */
extern uint8_t  wiced_bt_mesh_core_adv_tx_power;

/**
* Interval in seconds of advertising of the Proxy Service Network ID. 0 means always advertise.
* Ignored if proxy isn't enabled. Default value is 5.
*/
extern uint8_t wiced_bt_mesh_core_proxy_on_demand_advert_to;

/**
* If the value of that variable is WICED_TRUE then the transport layer always use 32-bit TransMIC.
* Otherwise the transport layer uses 64-bit TransMIC for segmented access messages if it fits into max transport PDU.
*/
extern wiced_bool_t wiced_bt_core_short_trans_mic;

/**
* WICED_TRUE value activates IV Update test mode - only removes the 96-hour limit; all other behavior of the device is unchanged.
* HCI_CONTROL_MESH_COMMAND_CORE_SET_IV_UPDATE_TEST_MODE sets it to TRUE
* Default value is WICED_FALSE.
*/
extern wiced_bool_t wiced_bt_core_iv_update_test_mode;

/**
* Segment Transmission Timer - If the segment transmission timer expires and no valid acknowledgment for the segmented message is received,
* then the lower transport layer shall retransmit all unacknowledged Lower Transport PDUs
* Default value is 1000 (1 sec)
*/
extern uint16_t wiced_bt_core_lower_transport_seg_trans_timeout_ms;

/**
* Number of times the lower transport layer transmits all unacknowledged Lower Transport PDUs
* Default value is 3
*/
extern uint8_t wiced_bt_core_lower_transport_seg_trans_cnt;

/**
* On WICED_TRUE CPU is boost for cpypto operations
* Default value is WICED_TRUE
*/
extern wiced_bool_t wiced_bt_core_boost_cpu_on_crypt_op;

/* Code of the Mesh Proxy request */
#define WICED_MESH_PROXY_REQ 0x0077

/**
 * \brief Fills buffer net_id with 8 bytes Network ID for net_key_idx.
 *
 * @param[in]       net_key_idx         Network Key Index to get Network ID for.
 * @param[out]      network_id          Buffer 8 bytes length for Network ID
 *
 * @return      TRUE on success
 */
wiced_bool_t wiced_bt_mesh_core_get_network_id(uint16_t net_key_idx, uint8_t *network_id);

/**
 * \brief Remote Provisioning Server initialization
 *
 * @param       None
 *
 * @return      None
 */
void wiced_bt_mesh_remote_provisioning_server_init(void);

/**
* Process Advertising Packets to check if the packet is for remote provisioning server.
*
* @param  p_adv_report Advertising report paremeters
* @param  p_adv_data Advertising data
*
* @return   WICED_TRUE/WICED_FALSE - success/failed.
*/
wiced_bool_t wiced_bt_mesh_remote_provisioning_adv_packet(wiced_bt_ble_scan_results_t *p_adv_report, uint8_t *p_adv_data);

/**
* \brief Encrypt or decrypts and authenticates data
* @param[in]    encrypt         TRUE - encrypt, FALSE - decrypt
* @param[in]    p_in_data       Input data
* @param[in]    in_data_len     Length of the input data
* @param[out]   p_out_buf       Buffer for output data. It can be same as p_in_data
* @param[in]    out_buf_len     Length of the buffer for output data
*
* @return      Length of the output data. 0 on error
*/
uint16_t wiced_bt_mesh_core_crypt(wiced_bool_t encrypt, const uint8_t *p_in_data, uint16_t in_data_len, uint8_t *p_out_buf, uint16_t out_buf_len);

/**
* \brief  Set scan response raw data
*
* @param[in] num_elem       number of scan response data element
* @param[in] p_data         scan response raw data
*
* @return      WICED_TRUE on success
*/
wiced_bool_t wiced_bt_mesh_set_raw_scan_response_data(uint8_t num_elem, wiced_bt_ble_advert_elem_t *p_data);

/**
 * \brief Device Key request callback.
 * \details Client application implements that function to handle core requests for device key of the peer device. It is needed if app wants to send/receive configuration messages.
 *
 * @param[in]   addr            Address of the node which device key is requested.
 * @param[out]  p_net_key_idx   Network Key Index to use for message encryption. It can be NULL. Returned value 0xffff means use primary net key.
 *
 * @return      Pointer to the device key. On error returns NULL.
*/
typedef const uint8_t *(*wiced_bt_mesh_core_get_dev_key_callback_t)(uint16_t addr, uint16_t *p_net_key_idx);

/**
 * \brief Sets Device Key request callback.
 * \details Client application implements callback function and sets it to the core for requests for device key of the peer device. It is needed if app wants to send/receive configuration messages.
 *
 * @param[in]   callback    Callback function. It can be NULL to disable callback.
 *
 * @return      None
*/
void wiced_bt_mesh_core_set_dev_key_callback(wiced_bt_mesh_core_get_dev_key_callback_t callback);

/**
 * \brief Gets friend address of the LPN.
 * @param       None
 * @return      Address of the friend node of the LPN. Unassigned address (0) means friendship is not established.
 */
uint16_t wiced_bt_mesh_core_lpn_get_friend_addr(void);

/**
 * \brief Stops all advertisements and timers.
 *
 * @param       None
 *
 * @return      None
*/
void wiced_bt_mesh_core_shutdown(void);

#define WICED_BT_MESH_CORE_DIRECTED_FORWARDING_TEST_FLAGS_DISABLE_DISCOVERY_INIT 0x0001
#define WICED_BT_MESH_CORE_DIRECTED_FORWARDING_TEST_FLAGS_DISABLE_DELEGATED_DISCOVERY_INIT  0x0002
/* sets flags for PTS testing */
extern uint32_t wiced_bt_mesh_core_directed_forwarding_test_flags;

/**
* \brief Advertising interval of the unprovisioned beacon in units 0.5 sec. Default value: 8000 (5 sec)
*/
extern uint16_t wiced_bt_mesh_core_unprovisioned_beacon_interval;
/**
* \brief Advertising interval in MS of the provisioning service in units 0.5 sec. Default value: 800 (0.5 sec)
*/
extern uint16_t wiced_bt_mesh_core_provisioning_srv_adv_interval;
/**
* \brief Advertising interval in MS of the provisioning URL in units 0.5 sec. Default value: 8000 (5 sec)
*/
extern uint16_t wiced_bt_mesh_core_provisioning_url_adv_interval;

/* @} wiced_bt_mesh_core */
/* @} wiced_bt_mesh */

#ifdef __cplusplus
}
#endif

#endif /* __WICED_BT_MESH_CORE_H__ */
