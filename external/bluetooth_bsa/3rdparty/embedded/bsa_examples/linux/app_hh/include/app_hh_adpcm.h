/*****************************************************************************
 **
 **  Name:           app_hh_adpcm.h
 **
 **  Description:    APP HID ADPCM function definitions
 **
 **  Copyright (c) 2019, Cypress Semiconductor, All Rights Reserved.
 **  Proprietary and confidential.
 **
 *****************************************************************************/
#ifndef _APP_HH_ADPCM_H_
#define _APP_HH_ADPCM_H_

/*******************************************************************************
 **
 ** Function         app_hh_adpcm_init
 **
 ** Description      This function initialize ADPCM decoder
 **
 ** Parameters
 **
 ** Returns          0 if successful, otherwise the error number
 **
 *******************************************************************************/
int app_hh_adpcm_init(void);

/*******************************************************************************
 **
 ** Function         app_hh_adpcm_decode
 **
 ** Description      This function processes HID reports containing ADPCM encoded
 **                  data.
 **
 ** Parameters
 **
 ** Returns          int
 **
 *******************************************************************************/
int app_hh_adpcm_decode(tAPP_HH_DEVICE *p_hh_dev, UINT8 *p_data, UINT16 length, UINT16 packet_count);

#endif /* _APP_HH_ADPCM_H_ */
