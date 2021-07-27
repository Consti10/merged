/*****************************************************************************
**
**  Name:           bta_pbc_ci.h
**
**  Description:    This is the implementation file for the phone book access
**                  client call-in functions.
**
**  Copyright (C) 2018 Cypress Semiconductor Corporation
**
*****************************************************************************/

#ifndef BTA_PBC_CI_H
#define BTA_PBC_CI_H

#include "bta_api.h"
#include "bta_pbc_api.h"
#include "bta_fs_ci.h"
#include "bta_pbc_co.h"
#include "bd.h"

/*****************************************************************************
**  Constants and data types
*****************************************************************************/


/*****************************************************************************
**  Function Declarations
*****************************************************************************/

/*******************************************************************************
**
** Function         bta_pbc_ci_write
**
** Description      This function sends an event to IO indicating the phone
**                  has written the number of bytes specified in the call-out
**                  function, bta_pbc_co_write_file_data(), and is ready for more data.
**                  This function is used to control the TX data flow.
**                  Note: The data buffer is released by the stack after
**                        calling this function.
**
** Parameters       fd      - file descriptor passed to the stack in the
**                            bta_mce_ci_open call-in function.
**                  status  - BTA_PBC_CO_OK or BTA_PBC_CO_FAIL
**                  evt     - event that waspassed into the call-out function.
**                  bd_addr - BD address
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void bta_pbc_ci_write(int fd, tBTA_PBC_CO_STATUS status, UINT16 evt,
                                        BD_ADDR bd_addr);

/*******************************************************************************
**
** Function         bta_pbc_ci_open
**
** Description      This function sends an event to BTA indicating the phone has
**                  finished opening a file for reading or writing.
**
** Parameters       fd          - file descriptor passed to the stack in the
**                                bta_pbc_ci_open call-in function.
**                  status      - BTA_PBC_CO_OK if file was opened in mode specified
**                                in the call-out function.
**                                BTA_PBC_CO_EACCES if the file exists, but contains
**                                the wrong access permissions.
**                                BTA_PBC_CO_FAIL if any other error has occurred.
**                  file_size   - The total size of the file
**                  evt         - Used Internally by BTA -> MUST be same value passed
**                                in call-out function.
**                  bd_addr     - BD address
**
** Returns          void
**
*******************************************************************************/
BTA_API extern void bta_pbc_ci_open(int fd, tBTA_PBC_CO_STATUS status, UINT32 file_size,
                                        UINT16 evt, BD_ADDR bd_addr);

#endif /* BTA_PBC_CI_H */
