#1. Specify SDK & Project workspace directory
SDK_DIR = ../..

PROJECT_DIR = .

#2. config
-include $(PROJECT_DIR)/.config

#3. Sources/includes/defines/library
include $(PROJECT_DIR)/linker_script/linker_script.mk


PROJECT_SRCS = $(PROJECT_DIR)/main/main.c \
               $(PROJECT_DIR)/main/evt_task.c \
               $(PROJECT_DIR)/main/app_adv.c \
               $(PROJECT_DIR)/main/app_sec.c \
               $(PROJECT_DIR)/main/system.c \
               $(PROJECT_DIR)/../ble_common/om_ancs/ancs_client.c \
               $(PROJECT_DIR)/../ble_common/om_bms/om_bms.c \
               $(PROJECT_DIR)/../ble_common/om_bms/service_om_bms.c \
               $(PROJECT_DIR)/../ble_common/om_cgms/om_cgms.c \
               $(PROJECT_DIR)/../ble_common/om_cgms/service_om_cgms.c \
               $(PROJECT_DIR)/../ble_common/om_dfu/om_dfu.c \
               $(PROJECT_DIR)/../ble_common/om_dfu/om_dfu_nvds.c \
               $(PROJECT_DIR)/../ble_common/om_dfu/service_om_dfu.c \
               $(PROJECT_DIR)/../ble_common/om_gap_dis_batt/service_common.c \
               $(PROJECT_DIR)/../ble_common/om_hid_media/app_hid_media.c \
               $(PROJECT_DIR)/../ble_common/om_tspp/service_tspp.c \
               $(PROJECT_DIR)/../ble_common/om_wechat_lite/app_wechat_lite.c \

PROJECT_INCS = $(PROJECT_DIR)/config/ \
               $(PROJECT_DIR)/../ble_common/om_ancs/ \
               $(PROJECT_DIR)/../ble_common/om_bms/ \
               $(PROJECT_DIR)/../ble_common/om_cgms/ \
               $(PROJECT_DIR)/../ble_common/om_dfu/ \
               $(PROJECT_DIR)/../ble_common/om_gap_dis_batt/ \
               $(PROJECT_DIR)/../ble_common/om_hid_media/ \
               $(PROJECT_DIR)/../ble_common/om_tspp/ \
               $(PROJECT_DIR)/../ble_common/om_wechat_lite/ \

PROJECT_DEFS =

PROJECT_LIBS =

#3. rules.mk
include $(SDK_DIR)/tools/build/rules.mk
