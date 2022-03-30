#ifndef __MESH_CONFIG_H__
#define __MESH_CONFIG_H__

#include "adapter/include/common/sys_timer.h"

#if 0
#include "gpio.h"
#define     APP_IO_DEBUG_0(i,x)       {gpio_direction_output(PORT##i, BIT(x)); gpio_output_low(PORT##i, BIT(x));}
#define     APP_IO_DEBUG_1(i,x)       {gpio_direction_output(PORT##i, BIT(x)); gpio_output_high(PORT##i, BIT(x));}
#define     APP_IO_DEBUG_TOGGLE(i,x)  {gpio_direction_output(PORT##i, BIT(x)); gpio_output_toggle(PORT##i, BIT(x));}
#else
#define     APP_IO_DEBUG_0(i,x)
#define     APP_IO_DEBUG_1(i,x)
#define     APP_IO_DEBUG_TOGGLE(i,x)
#endif /*  */

#define CONFIG_BT_MESH_MODEL_KEY_COUNT      2
#define CONFIG_BT_MESH_MODEL_GROUP_COUNT    2

#define ADV_SCAN_UNIT(_ms) ((_ms) * 8 / 5)

#define BT_MESH_ADDR_IS_UNICAST(addr) ((addr) && (addr) < 0x8000)
#define BT_MESH_ADDR_IS_GROUP(addr) ((addr) >= 0xc000 && (addr) <= 0xff00)
#define BT_MESH_ADDR_IS_VIRTUAL(addr) ((addr) >= 0x8000 && (addr) < 0xc000)
#define BT_MESH_ADDR_IS_RFU(addr) ((addr) >= 0xff00 && (addr) <= 0xfffb)

#define __noinit

/**
 * @brief Check for macro definition in compiler-visible expressions
 *
 * This trick was pioneered in Linux as the config_enabled() macro.
 * The madness has the effect of taking a macro value that may be
 * defined to "1" (e.g. CONFIG_MYFEATURE), or may not be defined at
 * all and turning it into a literal expression that can be used at
 * "runtime".  That is, it works similarly to
 * "defined(CONFIG_MYFEATURE)" does except that it is an expansion
 * that can exist in a standard expression and be seen by the compiler
 * and optimizer.  Thus much ifdef usage can be replaced with cleaner
 * expressions like:
 *
 *     if (IS_ENABLED(CONFIG_MYFEATURE))
 *             myfeature_enable();
 *
 * INTERNAL
 * First pass just to expand any existing macros, we need the macro
 * value to be e.g. a literal "1" at expansion time in the next macro,
 * not "(1)", etc...  Standard recursive expansion does not work.
 */
#define IS_ENABLED(config_macro) _IS_ENABLED1(config_macro)

/* Now stick on a "_XXXX" prefix, it will now be "_XXXX1" if config_macro
 * is "1", or just "_XXXX" if it's undefined.
 *   ENABLED:   _IS_ENABLED2(_XXXX1)
 *   DISABLED   _IS_ENABLED2(_XXXX)
 */
#define _IS_ENABLED1(config_macro) _IS_ENABLED2(_XXXX##config_macro)

/* Here's the core trick, we map "_XXXX1" to "_YYYY," (i.e. a string
 * with a trailing comma), so it has the effect of making this a
 * two-argument tuple to the preprocessor only in the case where the
 * value is defined to "1"
 *   ENABLED:    _YYYY,    <--- note comma!
 *   DISABLED:   _XXXX
 *   DISABLED:   _XXXX0
 */
#define _XXXX1 _YYYY,
#define _XXXX0

/* Then we append an extra argument to fool the gcc preprocessor into
 * accepting it as a varargs macro.
 *                         arg1   arg2  arg3
 *   ENABLED:   _IS_ENABLED3(_YYYY,    1,    0)
 *   DISABLED   _IS_ENABLED3(_XXXX 1,  0)
 *   DISABLED   _IS_ENABLED3(_XXXX0 1,  0)
 */
#define _IS_ENABLED2(one_or_two_args) _IS_ENABLED3(one_or_two_args true, false)

/* And our second argument is thus now cooked to be 1 in the case
 * where the value is defined to 1, and 0 if not:
 */
#define _IS_ENABLED3(ignore_this, val, ...) val

/** @def NET_BUF_SIMPLE_DEFINE
 *  @brief Define a net_buf_simple stack variable.
 *
 *  This is a helper macro which is used to define a net_buf_simple object
 *  on the stack.
 *
 *  @param _name Name of the net_buf_simple object.
 *  @param _size Maximum data storage for the buffer.
 */
#define NET_BUF_SIMPLE_DEFINE(_name, _size)     \
	u8_t net_buf_data_##_name[_size];       \
	struct net_buf_simple _name = {         \
		.data   = net_buf_data_##_name, \
		.len    = 0,                    \
		.size   = _size,                \
		.__buf  = net_buf_data_##_name, \
	}

/** @def NET_BUF_SIMPLE_DEFINE_STATIC
 *  @brief Define a static net_buf_simple variable.
 *
 *  This is a helper macro which is used to define a static net_buf_simple
 *  object.
 *
 *  @param _name Name of the net_buf_simple object.
 *  @param _size Maximum data storage for the buffer.
 */
#define NET_BUF_SIMPLE_DEFINE_STATIC(_name, _size)        \
	static __noinit u8_t net_buf_data_##_name[_size]; \
	static struct net_buf_simple _name = {            \
		.data   = net_buf_data_##_name,           \
		.len    = 0,                              \
		.size   = _size,                          \
		.__buf  = net_buf_data_##_name,           \
	}

/** @brief Simple network buffer representation.
 *
 *  This is a simpler variant of the net_buf object (in fact net_buf uses
 *  net_buf_simple internally). It doesn't provide any kind of reference
 *  counting, user data, dynamic allocation, or in general the ability to
 *  pass through kernel objects such as FIFOs.
 *
 *  The main use of this is for scenarios where the meta-data of the normal
 *  net_buf isn't needed and causes too much overhead. This could be e.g.
 *  when the buffer only needs to be allocated on the stack or when the
 *  access to and lifetime of the buffer is well controlled and constrained.
 *
 */
struct net_buf_simple {
    /** Pointer to the start of data in the buffer. */
    u8_t *data;

    /** Length of the data behind the data pointer. */
    u16_t len;

    /** Amount of data that this buffer can store. */
    u16_t size;

    /** Start of the data storage. Not to be accessed directly
     *  (the data pointer should be used instead).
     */
    u8_t *__buf;
};

struct k_work {
    sys_timer   systimer;
    void        *callback;
};

struct k_delayed_work {
    struct k_work work;
};

/********************************************************************************/
/*
 *                  Function Declaration
 */

/**
 * @brief Add a unsigned char variable at the tail of the buffer.
 *
 * @param buf Target buffer head address.
 *
 * @param val The variable will set.
 *
 * @return The tail address of the buffer.
 */
u8 *buffer_add_u8_at_tail(void *buf, u8 val);

/**
 * @brief Get the unsigned char variable from the buffer head address.
 *
 * @param buf Target buffer head address.
 *
 * @return Target variable.
 */
u8 buffer_pull_u8_from_head(void *buf);

/**
 * @brief Memcpy a array at the tail of the buffer.
 *
 * @param buf Target buffer head address.
 *
 * @param mem The source memory address.
 *
 * @param len The copy length.
 *
 * @return The result of the process : 0 is succ.
 */
void *buffer_memcpy(void *buf, const void *mem, u32 len);

/**
 * @brief Memset at the tail of the buffer.
 *
 * @param buf Target buffer head address.
 *
 * @param mem The set value.
 *
 * @param len The set length.
 *
 * @return The result of the process : 0 is succ.
 */
void *buffer_memset(struct net_buf_simple *buf, u8 val, u32 len);

/**
 * @brief Loading the node info from storage (such as flash and so on).
 */
void settings_load(void);

/**
 * @brief Free the buffer malloc by OS.
 */
void bt_mesh_adv_buf_free(void);
void bt_mesh_friend_buf_free(void);

/* Features */
#define BT_MESH_FEAT_RELAY                  BIT(0)
#define BT_MESH_FEAT_PROXY                  BIT(1)
#define BT_MESH_FEAT_FRIEND                 BIT(2)
#define BT_MESH_FEAT_LOW_POWER              BIT(3)
#define BT_MESH_FEAT_SUPPORTED_TEMP         ( \
                                                BT_MESH_FEAT_RELAY | \
                                                BT_MESH_FEAT_PROXY | \
                                                0 \
                                            )

#define BT_MESH_FEAT_SUPPORTED              BT_MESH_FEAT_SUPPORTED_TEMP

#define BT_MESH_FEATURES_GET(x)             (!!(BT_MESH_FEAT_SUPPORTED & x))

#if BT_MESH_FEATURES_GET(BT_MESH_FEAT_FRIEND) & BT_MESH_FEATURES_GET(BT_MESH_FEAT_LOW_POWER)
#error " error: Friend and LPN feature can't exist in the same node !!!"
#endif

#if BT_MESH_FEATURES_GET(BT_MESH_FEAT_LOW_POWER) & BT_MESH_FEATURES_GET(BT_MESH_FEAT_RELAY)
#undef  BT_MESH_FEAT_SUPPORTED
#define BT_MESH_FEAT_SUPPORTED              (BT_MESH_FEAT_SUPPORTED_TEMP & (~BT_MESH_FEAT_RELAY))
#endif

#define BT_MESH_FEATURES_IS_SUPPORT(x)              (config_bt_mesh_features & (x))

#define BT_MESH_FEATURES_IS_SUPPORT_OPTIMIZE(x)     if (BT_MESH_FEATURES_IS_SUPPORT(x) == 0x0) return

/**
 * @brief Config current node features(Relay/Proxy/Friend/Low Power)
 */
/*-----------------------------------------------------------*/
extern const int config_bt_mesh_features;

/**
 * @brief Config adv bearer hardware param when node send messages
 */
/*-----------------------------------------------------------*/
extern const u16 config_bt_mesh_node_msg_adv_interval;
extern const u16 config_bt_mesh_node_msg_adv_duration;

/**
 * @brief Config proxy connectable adv hardware param
 */
/*-----------------------------------------------------------*/
extern const u16 config_bt_mesh_proxy_unprovision_adv_interval;
extern const u16 config_bt_mesh_proxy_node_adv_interval;
extern const u16 config_bt_mesh_proxy_pre_node_adv_interval;

/**
 * @brief Config lpn node character
 */
/*-----------------------------------------------------------*/
extern const u8 config_bt_mesh_lpn_auto_timeout;
extern const u8 config_bt_mesh_lpn_retry_timeout;
extern const int config_bt_mesh_lpn_scan_latency;
extern const u32 config_bt_mesh_lpn_init_poll_timeout;
extern const u8 config_bt_mesh_lpn_powerup_add_sub_list;
extern const u8 config_bt_mesh_lpn_recv_delay;
extern const u32 config_bt_mesh_lpn_poll_timeout;
extern const u8 config_bt_mesh_lpn_rssi_factor;
extern const u8 config_bt_mesh_lpn_recv_win_factor;
extern const u8 config_bt_mesh_lpn_min_queue_size;

/**
 * @brief Config friend node character
 */
/*-----------------------------------------------------------*/
extern const u8 config_bt_mesh_friend_lpn_count;
extern const u8 config_bt_mesh_friend_recv_win;
extern const u8 config_bt_mesh_friend_sub_list_size;
extern const u8 config_bt_mesh_friend_queue_size;

/**
 * @brief Config cache buffer
 */
/*-----------------------------------------------------------*/
extern const u8 config_bt_mesh_adv_buf_count;

#endif /* __MESH_CONFIG_H__ */
