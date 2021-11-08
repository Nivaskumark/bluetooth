/******************************************************************************
 *
 *  Copyright (c) 2014 The Android Open Source Project
 *  Copyright (C) 2009-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

#ifndef BTIF_COMMON_H
#define BTIF_COMMON_H

#include "data_types.h"
#include "bt_types.h"
#include "bta_api.h"

#ifndef LOG_TAG
#error "LOG_TAG not defined, please add in .c file prior to including bt_common.h"
#endif

#include <utils/Log.h>

/*******************************************************************************
**  Constants & Macros
********************************************************************************/

#define ASSERTC(cond, msg, val) if (!(cond)) { ALOGE( \
    "### ASSERT : %s line %d %s (%d) ###", __FILE__, __LINE__, msg, val);}

/* Calculate start of event enumeration; id is top 8 bits of event */
#define BTIF_SIG_START(id)       ((id) << 8)

/* For upstream the MSB bit is always SET */
#define BTIF_SIG_CB_BIT   (0x8000)
#define BTIF_SIG_CB_START(id)    (((id) << 8) | BTIF_SIG_CB_BIT)

/* BTIF sub-systems */
#define BTIF_CORE           0
#define BTIF_DM             1
#define BTIF_HFP            2
#define BTIF_AV             3
#define BTIF_PAN            4
#define BTIF_HF_CLIENT      5

extern bt_callbacks_t *bt_hal_cbacks;

#define HAL_CBACK(P_CB, P_CBACK, ...)\
    if (P_CB && P_CB->P_CBACK) {            \
        BTIF_TRACE_API("HAL %s->%s", #P_CB, #P_CBACK); \
        P_CB->P_CBACK(__VA_ARGS__);         \
    }                                       \
    else {                                  \
        ASSERTC(0, "Callback is NULL", 0);  \
    }

/**
 * BTIF events for requests that require context switch to btif task
 * on downstreams path
 */
enum
{
    BTIF_CORE_API_START = BTIF_SIG_START(BTIF_CORE),
    BTIF_CORE_STORAGE_NO_ACTION,
    BTIF_CORE_STORAGE_ADAPTER_WRITE,
    BTIF_CORE_STORAGE_ADAPTER_READ,
    BTIF_CORE_STORAGE_ADAPTER_READ_ALL,
    BTIF_CORE_STORAGE_REMOTE_WRITE,
    BTIF_CORE_STORAGE_REMOTE_READ,
    BTIF_CORE_STORAGE_REMOTE_READ_ALL,
    BTIF_CORE_STORAGE_READ_ALL,
    BTIF_CORE_STORAGE_NOTIFY_STATUS,
    /* add here */

    BTIF_DM_API_START = BTIF_SIG_START(BTIF_DM),
    BTIF_DM_ENABLE_SERVICE,
    BTIF_DM_DISABLE_SERVICE,
    /* add here */

    BTIF_HFP_API_START = BTIF_SIG_START(BTIF_HFP),
    /* add here */

    BTIF_AV_API_START = BTIF_SIG_START(BTIF_AV),
    /* add here */
};

/**
 * BTIF events for callbacks that require context switch to btif task
 * on upstream path - Typically these would be non-BTA events
 * that are generated by the BTIF layer.
 */
enum
{
    BTIF_CORE_CB_START = BTIF_SIG_CB_START(BTIF_CORE),
    /* add here */

    BTIF_DM_CB_START = BTIF_SIG_CB_START(BTIF_DM),
    BTIF_DM_CB_DISCOVERY_STARTED, /* Discovery has started */
    BTIF_DM_CB_CREATE_BOND,   /* Create bond */
    BTIF_DM_CB_REMOVE_BOND,   /*Remove bond */
    BTIF_DM_CB_HID_REMOTE_NAME,   /* Remote name callback for HID device */
    BTIF_DM_CB_BOND_STATE_BONDING,
    BTIF_DM_CB_LE_TX_TEST,    /* BLE Tx Test command complete callback */
    BTIF_DM_CB_LE_RX_TEST,    /* BLE Rx Test command complete callback */
    BTIF_DM_CB_LE_TEST_END,   /* BLE Test mode end callback */

    BTIF_HFP_CB_START  = BTIF_SIG_CB_START(BTIF_HFP),
    BTIF_HFP_CB_AUDIO_CONNECTING, /* HF AUDIO connect has been sent to BTA successfully */

    BTIF_PAN_CB_START = BTIF_SIG_CB_START(BTIF_PAN),
    BTIF_PAN_CB_DISCONNECTING, /* PAN Disconnect has been sent to BTA successfully */

    BTIF_HF_CLIENT_CLIENT_CB_START  = BTIF_SIG_CB_START(BTIF_HF_CLIENT),
    BTIF_HF_CLIENT_CB_AUDIO_CONNECTING, /* AUDIO connect has been sent to BTA successfully */
};

/* Macro definitions for BD ADDR persistence */

/**
 * PROPERTY_BT_BDADDR_PATH
 * The property key stores the storage location of Bluetooth Device Address
 */
#ifndef PROPERTY_BT_BDADDR_PATH
#define PROPERTY_BT_BDADDR_PATH         "ro.bt.bdaddr_path"
#endif

/**
 * PERSIST_BDADDR_PROPERTY
 * If there is no valid bdaddr available from PROPERTY_BT_BDADDR_PATH,
 * generating a random BDADDR and keeping it in the PERSIST_BDADDR_DROP.
 */
#ifndef PERSIST_BDADDR_PROPERTY
#define PERSIST_BDADDR_PROPERTY         "persist.service.bdroid.bdaddr"
#endif

#define FACTORY_BT_BDADDR_STORAGE_LEN   17


/*******************************************************************************
**  Type definitions for callback functions
********************************************************************************/

typedef void (tBTIF_CBACK) (UINT16 event, char *p_param);
typedef void (tBTIF_COPY_CBACK) (UINT16 event, char *p_dest, char *p_src);


/*******************************************************************************
**  Type definitions and return values
********************************************************************************/

/* this type handles all btif context switches between BTU and HAL */
typedef struct
{
    BT_HDR               hdr;
    tBTIF_CBACK*         p_cb;    /* context switch callback */

    /* parameters passed to callback */
    UINT16               event;   /* message event id */
    char                 p_param[0]; /* parameter area needs to be last */
} tBTIF_CONTEXT_SWITCH_CBACK;


/*******************************************************************************
**  Functions
********************************************************************************/

bt_status_t btif_transfer_context (tBTIF_CBACK *p_cback, UINT16 event, char* p_params,
                                    int param_len, tBTIF_COPY_CBACK *p_copy_cback);
tBTA_SERVICE_MASK btif_get_enabled_services_mask(void);
bt_status_t btif_enable_service(tBTA_SERVICE_ID service_id);
bt_status_t btif_disable_service(tBTA_SERVICE_ID service_id);
int btif_is_enabled(void);
void btif_data_profile_register(int value);

/**
 * BTIF_Events
 */
void btif_enable_bluetooth_evt(tBTA_STATUS status, BD_ADDR local_bd);
void btif_disable_bluetooth_evt(void);
void btif_adapter_properties_evt(bt_status_t status, uint32_t num_props, bt_property_t *p_props);
void btif_remote_properties_evt(bt_status_t status, bt_bdaddr_t *remote_addr,
                                   uint32_t num_props, bt_property_t *p_props);
#endif /* BTIF_COMMON_H */
