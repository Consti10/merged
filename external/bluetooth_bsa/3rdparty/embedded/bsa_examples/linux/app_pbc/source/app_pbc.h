/*****************************************************************************
**
**  Name:           app_pbc.h
**
**  Description:    Bluetooth Phonebook client application
**
**  Copyright (c)   2013, Broadcom Corp., All Rights Reserved.
**  Broadcom Bluetooth Core. Proprietary and confidential.
**
**  Copyright (C) 2018 Cypress Semiconductor Corporation
**
*****************************************************************************/

#ifndef APP_PBC_H_
#define APP_PBC_H_

#define APP_PBC_MAX_CONN                2

/* Callback to application for connection events */
typedef void (tPbcCallback)(tBSA_PBC_EVT event, tBSA_PBC_MSG *p_data);

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
int app_pbc_enable(tPbcCallback pcb);

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
tBSA_STATUS app_pbc_disable(void);

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
tBSA_STATUS app_pbc_open(BD_ADDR bd_addr);

/*******************************************************************************
**
** Function         app_pbc_close
**
** Description      Function to close PBC connection
**
** Returns          0 if successful, error code otherwise
**
*******************************************************************************/
tBSA_STATUS app_pbc_close(BD_ADDR bd_addr);

/*******************************************************************************
**
** Function         app_pbc_close_all
**
** Description      release all pbc connections
**
** Returns          0 if successful, error code otherwise
**
*******************************************************************************/
tBSA_STATUS app_pbc_close_all(void);

/*******************************************************************************
**
** Function         app_pbc_abort
**
** Description      Example of function to abort current actions
**
** Parameters
**
** Returns          0 if successful, error code otherwise
**
*******************************************************************************/
tBSA_STATUS app_pbc_abort(BD_ADDR bd_addr);

/*******************************************************************************
**
** Function         app_pbc_cancel
**
** Description      cancel connections
**
** Returns          0 if successful, error code otherwise
**
*******************************************************************************/
tBSA_STATUS app_pbc_cancel(BD_ADDR bd_addr);

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
tBSA_STATUS app_pbc_get_list_vcards(BD_ADDR bd_addr, const char* string_dir, tBSA_PBC_ORDER  order,
    tBSA_PBC_ATTR  attr, const char* searchvalue, UINT16 max_list_count,
    BOOLEAN is_reset_missed_calls, tBSA_PBC_FILTER_MASK selector, tBSA_PBC_OP selector_op);

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
tBSA_STATUS app_pbc_get_vcard(BD_ADDR bd_addr, const char* file_name);

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
    BOOLEAN is_reset_missed_calls, tBSA_PBC_FILTER_MASK selector, tBSA_PBC_OP selector_op);

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
tBSA_STATUS app_pbc_set_chdir(BD_ADDR bd_addr, const char *string_dir);

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
BOOLEAN app_pbc_choose_connected_devices(BD_ADDR *addr);
#endif /* APP_PBC_H_ */
