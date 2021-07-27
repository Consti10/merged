/*
 * $ Copyright Cypress Semiconductor $
 */
#pragma once

#include "fx_api.h"
#include "brcm_fw_types.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif


/******************************************************
 *                      Macros
 ******************************************************/
#define IMAGE_FILENAME          "image.bin"
/* Only enable ONE of the following options */

/* Enable to make File-X read-only */
//#define FILEX_WRITE_STRATEGY  BLOCK_DEVICE_READ_ONLY

#define FILEX_WRITE_STRATEGY  BLOCK_DEVICE_WRITE_IMMEDIATELY

/* Enable to allow write-behind in File-X */
//#define FILEX_WRITE_STRATEGY  BLOCK_DEVICE_WRITE_BEHIND_ALLOWED

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/
typedef struct
{
    char* filename;
    FILE* image_file_handle;
    wiced_block_device_write_mode_t write_mode;
} tester_block_device_specific_data_t;

/******************************************************
 *                 Global Variables
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/
wiced_result_t  spi_block_device_init( wiced_block_device_t* device, wiced_block_device_write_mode_t write_mode );
wiced_result_t  spi_block_device_deinit( wiced_block_device_t* device );
wiced_result_t  spi_block_write( wiced_block_device_t* device, uint32_t start_address, const uint8_t* data, uint32_t size );
wiced_result_t  spi_block_flush( wiced_block_device_t* device );
wiced_result_t  spi_block_read( wiced_block_device_t* device, uint32_t start_address, uint8_t* data, uint32_t size );
wiced_result_t  spi_block_register_callback( wiced_block_device_t* device, wiced_block_device_status_change_callback_t callback );
wiced_result_t  spi_block_status( wiced_block_device_t* device, wiced_block_device_status_t* status );
/******************************************************
 *               Function Definitions
 ******************************************************/

VOID  wiced_filex_driver( FX_MEDIA* media_ptr );

#ifdef __cplusplus
} /*extern "C" */
#endif
