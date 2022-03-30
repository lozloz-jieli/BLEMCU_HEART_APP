#ifndef __HID_USER_H__
#define __HID_USER_H__

#include "typedef.h"
#include "bt_common.h"

enum {
    HID_USER_ERR_NONE = 0x0,
    HID_USER_ERR_DONE,
    HID_USER_ERR_SEND_FAIL,
};

void user_hid_init(void);
void user_hid_exit(void);
void user_hid_enable(u8 en);
int  user_hid_send_data(u8 *buf, u32 len);
void user_hid_disconnect(void);

#endif//__SPP_USER_H__
