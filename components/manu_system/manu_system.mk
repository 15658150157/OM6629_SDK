MANU_SYSTEM_DIR = $(COMPONENTS_DIR)/manu_system

ifeq ($(CONFIG_FAULT_HANDLE), y)
COMPONENTS_INCS += $(MANU_SYSTEM_DIR)/fault_handle
COMPONENTS_SRCS += $(MANU_SYSTEM_DIR)/fault_handle/fault_handle.c
endif
