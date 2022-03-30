#include "system/app_core.h"
#include "system/includes.h"
#include "server/server_core.h"
#include "app_config.h"
#include "app_action.h"
#include "os/os_api.h"
#include "btcontroller_config.h"
#include "btctrler/btctrler_task.h"
#include "btstack/btstack_task.h"
#include "btstack/bt_profile_config.h"
#include "btstack/avctp_user.h"
#include "btstack/le_user.h"
#include "system/event.h"
#include "btstack/ble_data_types.h"
#include "app_mesh.h"
#include "bt_common.h"

#define LOG_TAG_CONST       MESH
#define LOG_TAG             STR(LOG_TAG_CONST)
#define LOG_ERROR_ENABLE
#define LOG_DEBUG_ENABLE
#define LOG_INFO_ENABLE
/* #define LOG_DUMP_ENABLE */
#define LOG_CLI_ENABLE
#include "debug.h"

#if (TCFG_BLE_DEMO_SELECT == DEF_BLE_DEMO_MESH)

extern void bt_set_osc_cap(u8 sel_l, u8 sel_r);
extern void le_device_db_init(void);

#define SINGLE_CLIENT_EN                0
#define SINGLE_SERVER_EN                0

/*
 * Publication Declarations
 *
 * The publication messages are initialized to the
 * the size of the opcode + content
 *
 * For publication, the message must be in static or global as
 * it is re-transmitted several times. This occurs
 * after the function that called bt_mesh_model_publish() has
 * exited and the stack is no longer valid.
 *
 * Note that the additional 4 bytes for the AppMIC is not needed
 * because it is added to a stack variable at the time a
 * transmission occurs.
 *
 */
BT_MESH_MODEL_PUB_DEFINE(gen_onoff_pub_srv, NULL, 2 + 2);
BT_MESH_MODEL_PUB_DEFINE(gen_onoff_pub_cli, NULL, 2 + 2);

/* Model Operation Codes */
#define BT_MESH_MODEL_OP_GEN_ONOFF_GET			BT_MESH_MODEL_OP_2(0x82, 0x01)
#define BT_MESH_MODEL_OP_GEN_ONOFF_SET			BT_MESH_MODEL_OP_2(0x82, 0x02)
#define BT_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK	BT_MESH_MODEL_OP_2(0x82, 0x03)
#define BT_MESH_MODEL_OP_GEN_ONOFF_STATUS		BT_MESH_MODEL_OP_2(0x82, 0x04)

/* Company Identifiers (see Bluetooth Assigned Numbers) */
#define BT_COMP_ID_LF           0x05D6 // Zhuhai Jieli technology Co.,Ltd

/* LED NUMBER */
#define LED0_GPIO_PIN           0
#define LED1_GPIO_PIN           1

/*
 * Server Configuration Declaration
 */
static struct bt_mesh_cfg_srv cfg_srv = {
    .relay          = BT_MESH_FEATURES_GET(BT_MESH_FEAT_RELAY),
    .frnd           = BT_MESH_FEATURES_GET(BT_MESH_FEAT_FRIEND),
    .gatt_proxy     = BT_MESH_FEATURES_GET(BT_MESH_FEAT_PROXY),

    .beacon         = BT_MESH_BEACON_ENABLED,
    .default_ttl    = 7,
};

struct onoff_state {
    u8_t current;
    u8_t previous;
    u8_t led_gpio_pin;
};

struct _switch {
    u8_t sw_num;
    u8_t onoff_state;
};

static u8_t trans_id;

static struct onoff_state onoff_state[] = {
    { .led_gpio_pin = LED0_GPIO_PIN },
};

const u8 led_use_port[] = {

    IO_PORTA_01,

};

static void relay_sent(u16 *delay, u16 *duration, void *cb_data)
{
    u16 delay_ms;

    pseudo_random_genrate(&delay_ms, 2);

    delay_ms = btctler_get_rand_from_assign_range(delay_ms, 50, 200);

    *delay = delay_ms;

    log_info("Relay_delay = %u ms", delay_ms);
}

const struct bt_mesh_send_cb relay_sent_cb = {
    .user_intercept = relay_sent,
};

static void led_io_init(void)
{
    for (u32 i = 0; i < ARRAY_SIZE(led_use_port); i++) {
        gpio_set_die(led_use_port[i], 1);
        gpio_set_pull_down(led_use_port[i], 0);
        gpio_set_pull_up(led_use_port[i], 0);
        gpio_direction_output(led_use_port[i], 0);
    }
}

static void gpio_pin_write(u8_t led_index, u8_t onoff)
{
    if (led_index >= ARRAY_SIZE(led_use_port)) {
        log_info("led_index over realy led set");
        return;
    }

    onoff ? gpio_direction_output(led_use_port[led_index], 1) : gpio_direction_output(led_use_port[led_index], 0);
}

/*
 * Models in an element must have unique op codes.
 *
 * The mesh stack dispatches a message to the first model in an element
 * that is also bound to an app key and supports the op code in the
 * received message.
 *
 */
static void gen_onoff_status(struct bt_mesh_model *model,
                             struct bt_mesh_msg_ctx *ctx,
                             struct net_buf_simple *buf)
{
    u8_t	state;

    state = buffer_pull_u8_from_head(buf);

    log_info("Node 0x%04x OnOff status from 0x%04x with state 0x%02x\n",
             bt_mesh_model_elem(model)->addr, ctx->addr, state);
}

/*
 * OnOff Model Client Op Dispatch Table
 */

static const struct bt_mesh_model_op gen_onoff_cli_op[] = {
    { BT_MESH_MODEL_OP_GEN_ONOFF_STATUS, 1, gen_onoff_status },
    BT_MESH_MODEL_OP_END,
};

/*
 * Generic OnOff Model Server Message Handlers
 *
 * Mesh Model Specification 3.1.1
 *
 */
static void respond_messsage_schedule(u16 *delay, u16 *duration, void *cb_data)
{
    /*  Mesh_v1.0 <3.7.4.1 Transmitting an access message>
     *
     *	  If the message is sent in response to a received message
     *  that was sent to a unicast address, the node should transmit
     *  the response message with a random delay between 20 and 50 milliseconds.
     *
     *    If the message is sent in response to a received message
     *  that was sent to a group address or a virtual address,
     *  the node should transmit the response message with
     *  a random delay between 20 and 500 milliseconds.
     */
    u16 delay_ms;
    struct bt_mesh_msg_ctx *ctx = cb_data;

    pseudo_random_genrate((u8 *)&delay_ms, 2);
    if (BT_MESH_ADDR_IS_UNICAST(ctx->recv_dst)) {
        delay_ms = btctler_get_rand_from_assign_range(delay_ms, 20, 50);
    } else {
        delay_ms = btctler_get_rand_from_assign_range(delay_ms, 20, 200);
    }

    *delay = delay_ms;
    log_info("respond_messsage delay =%u ms", delay_ms);
}

static const struct bt_mesh_send_cb rsp_msg_cb = {
    .user_intercept = respond_messsage_schedule,
    /* .user_intercept = NULL, */
};

static void gen_onoff_get(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
    NET_BUF_SIMPLE_DEFINE(msg, 2 + 1 + 4);
    struct onoff_state *onoff_state = model->user_data;

    log_info("addr 0x%04x onoff 0x%02x\n",
             bt_mesh_model_elem(model)->addr, onoff_state->current);
    bt_mesh_model_msg_init(&msg, BT_MESH_MODEL_OP_GEN_ONOFF_STATUS);
    buffer_add_u8_at_tail(&msg, onoff_state->current);

    if (bt_mesh_model_send(model, ctx, &msg, &rsp_msg_cb, ctx)) {
        log_info("Unable to send On Off Status response\n");
    }
}

static void gen_onoff_set_unack(struct bt_mesh_model *model,
                                struct bt_mesh_msg_ctx *ctx,
                                struct net_buf_simple *buf)
{
    struct net_buf_simple *msg = model->pub->msg;
    struct onoff_state *onoff_state = model->user_data;
    int err;

    onoff_state->current = buffer_pull_u8_from_head(buf);
    log_info("addr 0x%02x state 0x%02x\n",
             bt_mesh_model_elem(model)->addr, onoff_state->current);
    /* log_info_hexdump((u8 *)onoff_state, sizeof(*onoff_state)); */

    gpio_pin_write(onoff_state->led_gpio_pin,
                   onoff_state->current);

#if 0
    /*
     * If a server has a publish address, it is required to
     * publish status on a state change
     *
     * See Mesh Profile Specification 3.7.6.1.2
     *
     * Only publish if there is an assigned address
     */

    if (onoff_state->previous != onoff_state->current &&
        model->pub->addr != BT_MESH_ADDR_UNASSIGNED) {
        log_info("publish last 0x%02x cur 0x%02x\n",
                 onoff_state->previous, onoff_state->current);
        onoff_state->previous = onoff_state->current;
        bt_mesh_model_msg_init(msg,
                               BT_MESH_MODEL_OP_GEN_ONOFF_STATUS);
        buffer_add_u8_at_tail(msg, onoff_state->current);
        err = bt_mesh_model_publish(model);
        if (err) {
            log_info("bt_mesh_model_publish err %d\n", err);
        }
    }
#endif /*  */
}

static void gen_onoff_set(struct bt_mesh_model *model,
                          struct bt_mesh_msg_ctx *ctx,
                          struct net_buf_simple *buf)
{
    log_info("gen_onoff_set\n");

    gen_onoff_set_unack(model, ctx, buf);
    gen_onoff_get(model, ctx, buf);
}

/*
 * OnOff Model Server Op Dispatch Table
 *
 */
static const struct bt_mesh_model_op gen_onoff_srv_op[] = {
    { BT_MESH_MODEL_OP_GEN_ONOFF_GET, 0, gen_onoff_get },
    { BT_MESH_MODEL_OP_GEN_ONOFF_SET, 2, gen_onoff_set },
    { BT_MESH_MODEL_OP_GEN_ONOFF_SET_UNACK, 2, gen_onoff_set_unack },
    BT_MESH_MODEL_OP_END,
};

/*
 *
 * Element Model Declarations
 *
 * Element 0 Root Models
 */
#if SINGLE_CLIENT_EN
static struct bt_mesh_model root_models[] = {
    BT_MESH_MODEL_CFG_SRV(&cfg_srv),
    BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_ONOFF_CLI, gen_onoff_cli_op, &gen_onoff_pub_cli, &onoff_state[0]),
};
#elif SINGLE_SERVER_EN
static struct bt_mesh_model root_models[] = {
    BT_MESH_MODEL_CFG_SRV(&cfg_srv),
    BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_ONOFF_SRV, gen_onoff_srv_op, &gen_onoff_pub_srv, &onoff_state[0]),
};
#else
static struct bt_mesh_model root_models[] = {
    BT_MESH_MODEL_CFG_SRV(&cfg_srv),
    BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_ONOFF_SRV, gen_onoff_srv_op, &gen_onoff_pub_srv, &onoff_state[0]),
    BT_MESH_MODEL(BT_MESH_MODEL_ID_GEN_ONOFF_CLI, gen_onoff_cli_op, &gen_onoff_pub_cli, &onoff_state[0]),
};
#endif /* SINGLE_CLIENT_EN */

/*
 * Button to Client Model Assignments
 */
static struct bt_mesh_model *mod_cli_sw[] = {
#if SINGLE_CLIENT_EN
    &root_models[1],
#else
    &root_models[2],
#endif /* SINGLE_CLIENT_EN */
};

/*
 * LED to Server Model Assigmnents
 */
static struct bt_mesh_model *mod_srv_sw[] = {
    &root_models[1],
};

/*
 * Root and Secondary Element Declarations
 */
static struct bt_mesh_elem elements[] = {
    BT_MESH_ELEM(0, root_models, BT_MESH_MODEL_NONE),
};

static const struct bt_mesh_comp composition = {
    .cid = BT_COMP_ID_LF,
    .elem = elements,
    .elem_count = ARRAY_SIZE(elements),
};

static u16_t primary_addr;
static u16_t primary_net_idx;
static u8_t dev_uuid[16] = { 0xdd, 0xdd };

static void prov_complete(u16_t net_idx, u16_t addr)
{
    log_info("provisioning complete for net_idx 0x%04x addr 0x%04x\n",
             net_idx, addr);
    primary_addr = addr;
    primary_net_idx = net_idx;
}

static void prov_reset(void)
{
    bt_mesh_prov_enable(BT_MESH_PROV_ADV | BT_MESH_PROV_GATT);
}

static const struct bt_mesh_prov prov = {
    .uuid = dev_uuid,
#if 0
    .output_size = 6,
    .output_actions = (BT_MESH_DISPLAY_NUMBER | BT_MESH_DISPLAY_STRING),
    .output_number = output_number,
    .output_string = output_string,
#else
    .output_size = 0,
    .output_actions = 0,
    .output_number = 0,
#endif
    .complete = prov_complete,
    .reset = prov_reset,
};

static bool server_publish(struct _switch *sw)
{
    struct bt_mesh_model *mod_srv;
    struct bt_mesh_model_pub *pub_srv;

    mod_srv = mod_srv_sw[sw->sw_num];
    pub_srv = mod_srv->pub;

    /* If unprovisioned, just call the set function.
     * The intent is to have switch-like behavior
     * prior to provisioning. Once provisioned,
     * the button and its corresponding led are no longer
     * associated and act independently. So, if a button is to
     * control its associated led after provisioning, the button
     * must be configured to either publish to the led's unicast
     * address or a group to which the led is subscribed.
     */
    if (primary_addr == BT_MESH_ADDR_UNASSIGNED) {
        NET_BUF_SIMPLE_DEFINE(msg, 1);
        struct bt_mesh_msg_ctx ctx = {
            .addr = sw->sw_num + primary_addr,
        };

        /* This is a dummy message sufficient
         * for the led server
         */
        buffer_add_u8_at_tail(&msg, sw->onoff_state);
        gen_onoff_set_unack(mod_srv, &ctx, &msg);
        return TRUE;
    }
    return FALSE;
}

static void client_publish(struct _switch *sw)
{
    int err;
    struct bt_mesh_model *mod_cli;
    struct bt_mesh_model_pub *pub_cli;

    mod_cli = mod_cli_sw[sw->sw_num];
    pub_cli = mod_cli->pub;

    if (pub_cli->addr == BT_MESH_ADDR_UNASSIGNED) {
        return;
    }

    log_info("publish to 0x%04x onoff 0x%04x sw->sw_num 0x%04x\n",
             pub_cli->addr, sw->onoff_state, sw->sw_num);
    bt_mesh_model_msg_init(pub_cli->msg,
                           BT_MESH_MODEL_OP_GEN_ONOFF_SET);
    buffer_add_u8_at_tail(pub_cli->msg, sw->onoff_state);
    buffer_add_u8_at_tail(pub_cli->msg, trans_id++);
    err = bt_mesh_model_publish(mod_cli);
    if (err) {
        log_info("bt_mesh_model_publish err %d\n", err);
    }
}

/*
 * Button Pressed Worker Task
 */
static void button_pressed_worker(struct _switch *sw)
{
    if (sw->sw_num >= composition.elem_count) {
        log_info("sw_num over elem_count");
        return;
    }

    if (server_publish(sw)) {
        return;
    }
    client_publish(sw);
}

static void mesh_init(void)
{
    log_info("--func=%s", __FUNCTION__);

    int err = bt_mesh_init(&prov, &composition);
    if (err) {
        log_error("Initializing mesh failed (err %d)\n", err);
        return;
    }

    settings_load();

    bt_mesh_prov_enable(BT_MESH_PROV_GATT | BT_MESH_PROV_ADV);
}

#define FIRST_POWER_UP_USE_RANDOM_MAC_ADDR      1

#define APPS_VM_START_INDEX                     80

//< max index : 128
typedef enum _INFO_SETTING_INDEX {

    MAC_ADDR_INDEX = APPS_VM_START_INDEX,

    VM_INDEX_DEMO_0,
    VM_INDEX_DEMO_1,

    // ...more to add
} INFO_SETTING_INDEX;

static bool info_load(INFO_SETTING_INDEX index, void *buf, u16 len)
{
    int ret;

    vm_hdl hdl = vm_open(index);
    log_info("vm hdl=0x%x", hdl);
    ret = vm_read(hdl, (u8 *)buf, len);
    if (ret != len) {
        log_info("vm_read err\n");
        log_info("ret = %d\n", ret);
        return 1;
    }

    return 0;
}

static void info_store(INFO_SETTING_INDEX index, void *buf, u16 len)
{
    int ret;

    vm_check_all(0);

    vm_hdl hdl = vm_open(index);
    log_info("vm hdl=0x%x", hdl);
    ret = vm_write(hdl, (u8 *)buf, len);
    if (ret != len) {
        log_info("vm_write err\n");
        log_info("ret = %d\n", ret);
    }
}

static void generate_bt_address(u8 addr[6])
{
    u8 i;

    for (i = 0; i < 6;) {
        addr[i++] = JL_RAND->R64L;
        addr[i++] = JL_RAND->R64H;
    }
}

static void bt_mac_addr_set(void)
{
    int err;
    u8 bt_addr[6] = {0x21, 0x22, 0x33, 0x44, 0x55, 0x78};

    if (FIRST_POWER_UP_USE_RANDOM_MAC_ADDR) {
        err = info_load(MAC_ADDR_INDEX, bt_addr, sizeof(bt_addr));
        if (err) {
            log_info(RedBoldBlink "first setup bt mac addr store" Reset);
            generate_bt_address(bt_addr);
            info_store(MAC_ADDR_INDEX, bt_addr, sizeof(bt_addr));
        } else {
            log_info(BlueBoldBlink "load exist bt mac addr" Reset);
        }
    }

    le_controller_set_mac(bt_addr);

    log_info(PurpleBold "CHIP bt MAC : %s" Reset, bd_addr_to_str(bt_addr));
}

void ble_profile_init(void)
{
    log_info("--func=%s", __FUNCTION__);

    le_device_db_init();

    /* ble_sm_setup_init(IO_CAPABILITY_NO_INPUT_NO_OUTPUT, SM_AUTHREQ_BONDING, 7, TCFG_BLE_SECURITY_EN); */

    /* setup ATT server */
    att_server_init(NULL, NULL, NULL);

    // register for HCI events
    /* hci_event_callback_set(&packet_handler); */

    /* le_l2cap_register_packet_handler(&packet_handler); */
}

void input_key_handler(u8 key_status, u8 key_number)
{
    struct _switch press_switch;

    log_info("key_number=0x%x", key_number);

    if ((key_number == 2) && (key_status == KEY_EVENT_LONG)) {
        log_info("\n  <bt_mesh_reset> \n");
        bt_mesh_reset();
        return;
    }

    switch (key_status) {
    case KEY_EVENT_CLICK:
        log_info("  [KEY_EVENT_CLICK]  ");
        press_switch.sw_num = key_number;
        press_switch.onoff_state = 1;
        button_pressed_worker(&press_switch);
        break;
    case KEY_EVENT_LONG:
        log_info("  [KEY_EVENT_LONG]  ");
        press_switch.sw_num = key_number;
        press_switch.onoff_state = 0;
        button_pressed_worker(&press_switch);
        break;
    case KEY_EVENT_HOLD:
        log_info("  [KEY_EVENT_HOLD]  ");
        break;
    default :
        return;
    }
}

void bt_ble_init(void)
{
    bt_mac_addr_set();

    mesh_setup(mesh_init);
}

void bt_ble_adv_enable(u8 enable)
{
    if (0 == enable) {
        ble_user_cmd_prepare(BLE_CMD_ADV_ENABLE, 1, enable);
        ble_user_cmd_prepare(BLE_CMD_SCAN_ENABLE, 1, enable);
    }
}

void bt_ble_exit(void)
{
    bt_ble_adv_enable(0);
}

void ble_module_enable(u8 en)
{
    bt_ble_adv_enable(en);
}

#endif /* (TCFG_BLE_DEMO_SELECT == DEF_BLE_DEMO_MESH) */
