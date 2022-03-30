#include "app_config.h"
#include "bt_common.h"
#include "btstack/ble_data_types.h"
#include "app_mesh.h"

#if (TCFG_BLE_DEMO_SELECT == DEF_BLE_DEMO_MESH)

#define BYTE_LEN(x...)                  sizeof((u8 []) {x})

/**
 * @brief Conifg complete local name
 */
/*-----------------------------------------------------------*/
#define BLE_DEV_NAME        'H', 'H', 'Y', '_','M', 'E', 'S', 'H'

const uint8_t mesh_name[] = {
    // Name
    BYTE_LEN(BLE_DEV_NAME) + 1, BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME, BLE_DEV_NAME,
};

void get_mesh_adv_name(u8 *len, u8 **data)
{
    *len = sizeof(mesh_name);

    *data = mesh_name;
}

/**
 * @brief Config current node features(Relay/Proxy/Friend/Low Power)
 */
/*-----------------------------------------------------------*/
const int config_bt_mesh_features = BT_MESH_FEAT_SUPPORTED;

/**
 * @brief Config adv bearer hardware param when node send messages
 */
/*-----------------------------------------------------------*/
const u16 config_bt_mesh_node_msg_adv_interval = ADV_SCAN_UNIT(10); // unit: ms
const u16 config_bt_mesh_node_msg_adv_duration = 100; // unit: ms

/**
 * @brief Config proxy connectable adv hardware param
 */
/*-----------------------------------------------------------*/
const u16 config_bt_mesh_proxy_unprovision_adv_interval = ADV_SCAN_UNIT(30); // unit: ms
const u16 config_bt_mesh_proxy_pre_node_adv_interval = ADV_SCAN_UNIT(10); // unit: ms
#if BT_MESH_FEATURES_GET(BT_MESH_FEAT_LOW_POWER)
const u16 config_bt_mesh_proxy_node_adv_interval = ADV_SCAN_UNIT(3000); // unit: ms
#else
const u16 config_bt_mesh_proxy_node_adv_interval = ADV_SCAN_UNIT(300); // unit: ms
#endif /* BT_MESH_FEATURES_GET(BT_MESH_FEAT_LOW_POWER) */

/**
 * @brief Config lpn node character
 */
/*-----------------------------------------------------------*/
const u8 config_bt_mesh_lpn_auto_timeout = 0; // unit: s
const u8 config_bt_mesh_lpn_retry_timeout = 10; // unit: s
const int config_bt_mesh_lpn_scan_latency = 10; // unit: ms
const u32 config_bt_mesh_lpn_init_poll_timeout = 300; // unit: 100ms
const u8 config_bt_mesh_lpn_powerup_add_sub_list = 1;
//< 3.6.5.3 Friend Request
const u8 config_bt_mesh_lpn_recv_delay = 100; // unit: ms
const u32 config_bt_mesh_lpn_poll_timeout = 200; // unit: 100ms
const u8 config_bt_mesh_lpn_rssi_factor = 0;
const u8 config_bt_mesh_lpn_recv_win_factor = 0;
const u8 config_bt_mesh_lpn_min_queue_size = 1;

/**
 * @brief Config friend node character
 */
/*-----------------------------------------------------------*/
const u8 config_bt_mesh_friend_lpn_count = 1;
//< 3.6.5.4 Friend Offer
const u8 config_bt_mesh_friend_recv_win = 250;
const u8 config_bt_mesh_friend_sub_list_size = 2;
const u8 config_bt_mesh_friend_queue_size = 2;

/**
 * @brief Config cache buffer
 */
/*-----------------------------------------------------------*/
const u8 config_bt_mesh_adv_buf_count = 4;

/**
 * @brief Ble Mesh Log
 */
/*-----------------------------------------------------------*/
const char log_tag_const_v_MESH_JOINT AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_i_MESH_JOINT AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_w_MESH_JOINT AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_d_MESH_JOINT AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_e_MESH_JOINT AT(.LOG_TAG_CONST) = 1;

const char log_tag_const_v_MESH_SET AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_i_MESH_SET AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_w_MESH_SET AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_d_MESH_SET AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_e_MESH_SET AT(.LOG_TAG_CONST) = 1;

const char log_tag_const_v_MESH_NETB AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_i_MESH_NETB AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_w_MESH_NETB AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_d_MESH_NETB AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_e_MESH_NETB AT(.LOG_TAG_CONST) = 1;

const char log_tag_const_v_MESH_LAYER AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_i_MESH_LAYER AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_w_MESH_LAYER AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_d_MESH_LAYER AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_e_MESH_LAYER AT(.LOG_TAG_CONST) = 1;

const char log_tag_const_v_MESH_ENC AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_i_MESH_ENC AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_w_MESH_ENC AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_d_MESH_ENC AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_e_MESH_ENC AT(.LOG_TAG_CONST) = 1;

const char log_tag_const_v_MESH_LPN AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_i_MESH_LPN AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_w_MESH_LPN AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_d_MESH_LPN AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_e_MESH_LPN AT(.LOG_TAG_CONST) = 1;

const char log_tag_const_v_MESH_FRIEND AT(.LOG_TAG_CONST) = 0;
const char log_tag_const_i_MESH_FRIEND AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_w_MESH_FRIEND AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_d_MESH_FRIEND AT(.LOG_TAG_CONST) = 1;
const char log_tag_const_e_MESH_FRIEND AT(.LOG_TAG_CONST) = 1;

#if 0
#define Provisioning_Service(x)     provisioning_service_##x
#define Proxy_Service(x)            proxy_service_##x

const uint8_t Proxy_Service(profile_data)[] = {1, 2, 3};
const uint8_t Provisioning_Service(profile_data)[] = {1, 2, 3};

void Provisioning_Service(change_adv_info)(void)
{
    printf("--func=%s", __FUNCTION__);
}
#endif

#endif /* (TCFG_BLE_DEMO_SELECT == DEF_BLE_DEMO_MESH) */
