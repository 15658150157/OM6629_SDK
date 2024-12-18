BSP_DIR = $(SDK_DIR)/bsp

BSP_INCS = $(BSP_DIR)/include
BSP_DEFS =

ifeq ($(CONFIG_BOARD_EVB), y)
BSP_SRCS = $(BSP_DIR)/board_evb/board_evb.c                                    \
           $(BSP_DIR)/source/glcd_gc9c01.c                                     \
           $(BSP_DIR)/source/glcd_gc9b71.c
else
BSP_SRCS =
endif
