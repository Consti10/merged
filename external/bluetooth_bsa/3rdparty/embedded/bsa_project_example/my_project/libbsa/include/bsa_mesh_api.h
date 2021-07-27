/*****************************************************************************
**
**  Name:           bsa_mesh_api.h
**
**  Description:    This is the public interface file for Mesh client part of
**                  the Bluetooth simplified API
**
**  Copyright (C) 2018 Cypress Semiconductor Corporation
**
*****************************************************************************/
#ifndef BSA_MESH_API_H
#define BSA_MESH_API_H

#include "bt_types.h"
#include "uipc.h"

/* for tBSA_STATUS */
#include "bsa_status.h"

/*****************************************************************************
**  Constants and Type Definitions
*****************************************************************************/

/* Number of MESH connections */
#define BSA_MESH_NUM_CONN                1

#define BSA_MESH_EVT_DATA_MAX            1024

/* BSA Mesh Client callback events */
typedef enum
{
    BSA_MESH_DISABLE_EVT,    /* Connection Disable */
    BSA_MESH_CORE_EVT        /* Event contains a response to Get request */

} tBSA_MESH_EVT;

#define BSA_MESH_OK              0
#define BSA_MESH_FAIL            1
#define BSA_MESH_NO_PERMISSION   2
#define BSA_MESH_NOT_FOUND       3
#define BSA_MESH_FULL            4
#define BSA_MESH_BUSY            5
#define BSA_MESH_ABORTED         6


typedef struct
{
    tBSA_STATUS      status;
    UINT16           evt_type;
    UINT8            data[BSA_MESH_EVT_DATA_MAX];
    UINT16           len;   // Actual Length of event data
} tBSA_MESH_CORE_EVT;


/* BSA_MESH_DISABLE_EVT callback event data */
typedef struct
{
    tBSA_STATUS         status;
} tBSA_MESH_DISABLE_MSG;

/* Data for all MESH events */
typedef union
{
    tBSA_MESH_DISABLE_MSG      disable;
    tBSA_MESH_CORE_EVT         core_evt;
} tBSA_MESH_EVT_MSG;


/* BSA MESH callback function */
typedef void (tBSA_MESH_CBACK)(tBSA_MESH_EVT event, tBSA_MESH_EVT_MSG *p_data);

/*
* Structures used to pass parameters to BSA API functions
*/

typedef struct
{
    tBSA_MESH_CBACK  *p_cback;
    UINT8 local_features;
} tBSA_MESH_ENABLE;

typedef struct
{
    int dummy;
} tBSA_MESH_DISABLE;

typedef struct
{
    UINT8 bt_rate;
    UINT8 scan_mode;
} tBSA_MESH_BTRATE;

typedef struct
{
    UINT16           opcode;
    UINT8            data[BSA_MESH_EVT_DATA_MAX];
    UINT16           len;   // Actual Length of event data
} tBSA_MESH_SENDMSG;


/*****************************************************************************
**  External Function Declarations
*****************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

/*******************************************************************************
**
** Function            BSA_MeshEnableInit
**
** Description         Initialize structure containing API parameters with default values
**
** Parameters          Pointer to structure containing API parameters
**
** Returns             tBSA_STATUS
**
*******************************************************************************/
tBSA_STATUS BSA_MeshEnableInit(tBSA_MESH_ENABLE *p_enable);

/*******************************************************************************
**
** Function            BSA_MeshEnable
**
** Description         Enable the mesh feature support.  This function must be
**                     called before any other functions in the MESH API are called.
**
** Parameters          Pointer to structure containing API parameters
**
** Returns             tBSA_STATUS
**
*******************************************************************************/
tBSA_STATUS BSA_MeshEnable(tBSA_MESH_ENABLE *p_enable);

/*******************************************************************************
**
** Function            BSA_MeshDisableInit
**
** Description         Initialize structure containing API parameters with default values
**
** Parameters          Pointer to structure containing API parameters
**
** Returns             tBSA_STATUS
**
*******************************************************************************/
tBSA_STATUS BSA_MeshDisableInit(tBSA_MESH_DISABLE *p_disable);

/*******************************************************************************
**
** Function            BSA_MeshDisable
**
** Description         Disable the mesh feature support.
**
** Parameters          Pointer to structure containing API parameters
**
** Returns             tBSA_STATUS
**
*******************************************************************************/
tBSA_STATUS BSA_MeshDisable(tBSA_MESH_DISABLE *p_disable);

/*******************************************************************************
**
** Function            BSA_MeshSendMsgInit
**
** Description         Initialize structure containing API parameters with default values
**
** Parameters          Pointer to structure containing API parameters
**
** Returns             tBSA_STATUS
**
*******************************************************************************/
tBSA_STATUS BSA_MeshSendMsgInit(tBSA_MESH_SENDMSG *p_get);

/*******************************************************************************
**
** Function            BSA_MeshSendMsg
**
** Description         Send Wiced Mesh message to BSA Server
**
** Parameters          Pointer to structure containing API parameters
**
** Returns             tBSA_STATUS
**
*******************************************************************************/
tBSA_STATUS BSA_MeshSendMsg(tBSA_MESH_SENDMSG *p_get);

#if (defined(BSA_MESH_ALT_BRANCH) && BSA_MESH_ALT_BRANCH == TRUE)
/*******************************************************************************
**
** Function            BSA_MeshCOEXBTRateInit
**
** Description         Initialize structure containing API parameters with default values
**
** Parameters          Pointer to structure containing API parameters
**
** Returns             tBSA_STATUS
**
*******************************************************************************/
tBSA_STATUS BSA_MeshCOEXBTRateInit(tBSA_MESH_BTRATE *p_btrate);

/*******************************************************************************
**
** Function            BSA_MeshCOEXBTRate
**
** Description         Set COEX BT rate to BSA Server for Mesh
**
** Parameters          Pointer to structure containing API parameters
**
** Returns             tBSA_STATUS
**
*******************************************************************************/
tBSA_STATUS BSA_MeshCOEXBTRate(tBSA_MESH_BTRATE *p_btrate);
#endif


#ifdef __cplusplus
}
#endif

#endif
