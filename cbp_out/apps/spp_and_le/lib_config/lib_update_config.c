
#include "app_config.h"
#include "system/includes.h"
#include "update_loader_download.h"

const int config_update_mode = UPDATE_BT_LMP_EN | UPDATE_BLE_TEST_EN | UPDATE_RCSP_BLE_EN;

//是否采用双备份升级方案:0-单备份;1-双备份
const int support_dual_bank_update_en = 0;

//是否支持升级之后保留vm数据
const int support_vm_data_keep = 0;

const int support_norflash_update_en = 0;

const char log_tag_const_v_UPDATE AT(.LOG_TAG_CONST) = LIB_DEBUG &  FALSE;
const char log_tag_const_i_UPDATE AT(.LOG_TAG_CONST) = LIB_DEBUG &  TRUE;
const char log_tag_const_d_UPDATE AT(.LOG_TAG_CONST) = LIB_DEBUG &  FALSE;
const char log_tag_const_w_UPDATE AT(.LOG_TAG_CONST) = LIB_DEBUG &  TRUE;
const char log_tag_const_e_UPDATE AT(.LOG_TAG_CONST) = LIB_DEBUG &  TRUE;
