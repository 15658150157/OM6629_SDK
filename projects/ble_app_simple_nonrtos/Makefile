#1. Specify SDK & Project workspace directory
SDK_DIR = ../../
PROJECT_DIR = .
PROJECT_BLE_DIR = ..

#2. config
include $(PROJECT_DIR)/.config

#3. Sources/includes/defines/library
include $(PROJECT_DIR)/config/config.mk
include $(PROJECT_DIR)/linker_script/linker_script.mk
include $(PROJECT_DIR)/main/main.mk

PROJECT_BLE_SRCS = $(PROJECT_BLE_DIR)/ble_common/om_ancs/ancs_client.c					\
				   $(PROJECT_BLE_DIR)/ble_common/om_bms/om_bms.c						\
				   $(PROJECT_BLE_DIR)/ble_common/om_bms/service_om_bms.c				\
				   $(PROJECT_BLE_DIR)/ble_common/om_cgms/om_cgms.c						\
				   $(PROJECT_BLE_DIR)/ble_common/om_cgms/service_om_cgms.c				\
				   $(PROJECT_BLE_DIR)/ble_common/om_dfu/om_dfu.c					\
				   $(PROJECT_BLE_DIR)/ble_common/om_dfu/om_dfu_nvds.c				\
				   $(PROJECT_BLE_DIR)/ble_common/om_dfu/service_om_dfu.c 			\
				   $(PROJECT_BLE_DIR)/ble_common/om_gap_dis_batt/service_common.c		\
				   $(PROJECT_BLE_DIR)/ble_common/om_hid_media/app_hid_media.c			\
				   $(PROJECT_BLE_DIR)/ble_common/om_tspp/service_tspp.c					\
				   $(PROJECT_BLE_DIR)/ble_common/om_wechat_lite/app_wechat_lite.c

PROJECT_BLE_INCS = $(PROJECT_BLE_DIR)/ble_common/om_ancs								\
				   $(PROJECT_BLE_DIR)/ble_common/om_bms									\
				   $(PROJECT_BLE_DIR)/ble_common/om_cgms								\
				   $(PROJECT_BLE_DIR)/ble_common/om_dfu									\
				   $(PROJECT_BLE_DIR)/ble_common/om_gap_dis_batt						\
				   $(PROJECT_BLE_DIR)/ble_common/om_hid_media							\
				   $(PROJECT_BLE_DIR)/ble_common/om_tspp								\
				   $(PROJECT_BLE_DIR)/ble_common/om_wechat_lite

PROJECT_SRCS = $(PROJECT_CONFIG_SRCS)                                          	\
               $(PROJECT_MAIN_SRCS)                                            	\
               $(PROJECT_APP_SRCS)                                         	   	\
			   $(PROJECT_BLE_SRCS)

PROJECT_INCS = $(PROJECT_CONFIG_INCS)                                          	\
               $(PROJECT_MAIN_INCS)                                            	\
               $(PROJECT_APP_INCS)                                             	\
			   $(PROJECT_BLE_INCS)

PROJECT_DEFS = $(PROJECT_CONFIG_DEFS)                                          	\
               $(PROJECT_MAIN_DEFS)                                            	\
               $(PROJECT_APP_DEFS)                                             	\

PROJECT_LIBS =

#4. rules.mk
include $(SDK_DIR)/tools/build/rules.mk
