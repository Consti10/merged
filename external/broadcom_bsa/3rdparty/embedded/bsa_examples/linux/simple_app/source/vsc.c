/*****************************************************************************
**
**  Name:           vsc.c
**
**  Description:    Bluetooth VSC functions
**
**  Copyright (c) 2011-2015, Broadcom Corp., All Rights Reserved.
**  Broadcom Bluetooth Core. Proprietary and confidential.
**
*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bsa_api.h"
#include "vsc.h"

/* #define VSC_DEBUG TRUE */

/*******************************************************************************
 **
 ** Function         vsc_send_init_param
 **
 ** Description      This function is used to initialize the VSC params
 **
 ** Parameters       Pointer to structure containing API parameters
 **
 ** Returns          int
 **
 *******************************************************************************/
int vsc_send_init_param(tBSA_TM_VSC *vsc_param)
{
#if defined(VSC_DEBUG) && (VSC_DEBUG == TRUE)
    printf("vsc_send_init_param\n");
#endif

    BSA_TmVscInit(vsc_param);
    return 0;
}

/*******************************************************************************
 **
 ** Function         vsc_send
 **
 ** Description      This function is used to send a Vendor Specific Command
 **
 ** Parameters       Pointer to structure containing API parameters
 **
 ** Returns          int
 **
 *******************************************************************************/
int vsc_send(tBSA_TM_VSC *vsc_param)
{
    tBSA_STATUS bsa_status;
    UINT8 op1,op2;
    UINT8 *p;
#if defined(VSC_DEBUG) && (VSC_DEBUG == TRUE)
    int index;

    printf("vsc_send\n");

    printf("VSC opcode:0x%03X\n", vsc_param->opcode);
    printf("VSC length:%d\n", vsc_param->length);
    if (vsc_param->length > 0)
    {
        printf("VSC Data:");
        for (index = 0 ; index < vsc_param->length ; index++)
        {
            printf ("%02X ", vsc_param->data[index]);
        }
        printf("\n");
    }
#endif

    bsa_status = BSA_TmVsc(vsc_param);
    if (bsa_status != BSA_SUCCESS)
    {
        fprintf(stderr, "vsc_send: Unable to Send VSC (status:%d)\n", bsa_status);
        return(-1);
    }

#if defined(VSC_DEBUG) && (VSC_DEBUG == TRUE)
    printf("VSC Server status:%d\n", vsc_param->status);
#endif
    printf("04\n");
    printf("0E 04\n");
    p = (UINT8 *)&vsc_param->opcode;
    STREAM_TO_UINT8(op1,p);
    STREAM_TO_UINT8(op2,p);
    printf("01 %02x %02x 00\n",op1,op2);
 
    return 0;
}

/*******************************************************************************
 **
 ** Function         vse_callback
 **
 ** Description      This callback function is used for
 **                  Vendor Specific Events
 **
 ** Parameters       event: TM event received
 **                  p_data: TM event data
 **
 ** Returns          void
 **
 ********************************************************************************/
void vse_callback(tBSA_TM_EVT event, tBSA_TM_MSG *p_data)
{
    tBSA_TM_VSE_MSG *p_vse;
    UINT8 length;
    int   index;

    switch(event)
    {
    case BSA_TM_VSE_EVT:
        printf("\nvse_callback BSA_TM_VSC_EVT\n");
        p_vse = (tBSA_TM_VSE_MSG *)p_data;
        length = p_vse->length;
        if (length > 0)
        {
            printf("04\n");
            printf("FF 21\n");
            printf("%02x ", p_vse->sub_event);
            for(index=0;index<length;index++)
            {
                printf("%02x ",p_vse->data[index]);
            }
            printf("\n");
        }

        break;

    default:
        printf("Unsupported TM event\n");
        break;
    }

}

/*******************************************************************************
 **
 ** Function         vse_register
 **
 ** Description      This function is used to register for Vendor Specific Events
 **
 ** Parameters
 **
 ** Returns          int
 **
 *******************************************************************************/

int vse_register(void)
{
    tBSA_TM_VSE_REGISTER  vse_register_param;
    tBSA_STATUS status;

    BSA_TmVseRegisterInit(&vse_register_param);
    vse_register_param.p_callback = vse_callback;
    vse_register_param.sub_event = BTA_TM_VSE_SUB_EVENT_ALL;
    status = BSA_TmVseRegister(&vse_register_param);
    if (status != BSA_SUCCESS)
    {
        fprintf(stderr, "BSA_TmVseRegister: Unable to register for VSE\n");
        return(status);
    }

    return 0;
}

/*******************************************************************************
 **
 ** Function         vse_deregister
 **
 ** Description      This function is used to deregister for Vendor Specific Events
 **
 ** Parameters
 **
 ** Returns          int
 **
 *******************************************************************************/
int vse_deregister(void)
{
    tBSA_TM_VSE_DEREGISTER  vse_deregister_param;
    tBSA_STATUS status;

    BSA_TmVseDeregisterInit(&vse_deregister_param);
    vse_deregister_param.sub_event = BTA_TM_VSE_SUB_EVENT_ALL;
    vse_deregister_param.deregister_callback = FALSE;
    status = BSA_TmVseDeregister(&vse_deregister_param);
    if (status != BSA_SUCCESS)
    {
        fprintf(stderr, "BSA_TmVseDeregister: Unable to deregister for VSE\n");
        return(status);
    }

    return 0;
}

