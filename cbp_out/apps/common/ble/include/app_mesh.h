#ifndef __APP_MESH_H__
#define __APP_MESH_H__

typedef unsigned char   bit1, uint8_t, u8_t;
typedef char            s8_t;
typedef unsigned short  uint16_t, u16_t;
typedef unsigned int    tu8, tu16, tbool, tu32, uint32_t, u32_t;
typedef signed int      s32_t;
typedef unsigned long long u64_t;
typedef signed long long s64_t;

// -- output terminal color
#define RedBold             "\033[31;1;7m" // 红色加粗
#define RedBoldBlink        "\033[31;1;5m" // 红色加粗、闪烁
#define BlueBold            "\033[34;1;7m" // 蓝色加粗
#define BlueBoldBlink       "\033[34;1;5m" // 蓝色加粗、闪烁
#define PurpleBold          "\033[35;1m"   // 紫色加粗
#define PurpleBoldBlink     "\033[35;1;5m" // 紫色加粗、闪烁
#define Reset               "\033[0;25m"   // 颜色复位

#include "mesh/mesh_config.h"
#include "mesh/main.h"
#include "mesh/cfg_srv.h"
#include "mesh/cfg_cli.h"

#include "device/vm.h"

extern s32 vm_open(u16 index);
extern VM_ERR vm_close(vm_hdl hdl);
extern s32 vm_write(vm_hdl hdl, u8 *data_buf, u16 len);
extern s32 vm_read(vm_hdl hdl, u8 *data_buf, u16 len);

extern uint32_t btctler_get_rand_from_assign_range(uint32_t rand, uint32_t min, uint32_t max);
extern void pseudo_random_genrate(uint8_t *dest, unsigned size);
extern void mesh_setup(void (*init_cb)(void));
extern char *bd_addr_to_str(u8 addr[6]);

#endif /* __APP_MESH_H__ */
