ifeq ($(CONFIG_SHELL), y)
SHELL_DIR = $(COMPONENTS_DIR)/shell
COMPONENTS_INCS += $(SHELL_DIR)
COMPONENTS_SRCS += $(SHELL_DIR)/shell_common.c          \
                   $(SHELL_DIR)/shell_port_rtos.c

endif
