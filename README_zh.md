# README for the SDK

## 项目简介

* 支持的芯片：见 tools/build/supported_devices.txt
* 支持的编译器：gcc, keil, iar

本SDK的构建系统支持makefile和scons，使用者可以根据需求通过Kconfig按组件进行配置、裁剪工程。

支持的工程介绍：
1. ble_app_mesh: ble mesh工程， 用于mesh协议栈的验证测试和二次开发， 具体使用请参考工程下的README.md
2. ble_app_multilink: 多连接参考工程，用于验证多连接协议栈验证及测试
3. ble_app_simple: 基于cmsis-rtos的单链接simple server的参考，用于协议栈验证及测试
4. ble_app_simple_nonrtos: 基于事件调度的simple server的参考，用于协议栈验证及测试
5. ble_hci: 基于H4的controller工程， 可用于蓝牙射频测试，认证等
6. ble_om24g_app_simple: 私有2.4G和ble共存的参考
7. om24g_app_simple: 私有2.4G工程
8. drv_example：常用外设的参考示例，具体使用参考工程下README.md


## 安装指南

### python脚本

* 安装python及pip，推荐python3.9
* 安装依赖包列表

```shell
pip install -r tools/python/requirements.txt
```

### GCC编译

安装 arm-gcc 编译器，推荐gcc-arm-none-eabi-10.3-2021.10及以上版本，不使用gcc编译器，此步可省略。

### Jlink

安装jlink驱动，推荐JLink_V7.58及以上版本， jlink 烧录算法见 tools/plugins/jlink

### Keil

安装keil pack，推荐Keil_v5.30及以上版本， 不使用keil编译器，此步可省略。keil pack 见tools/plugins/keil

### IAR

安装IAR Embedded Workbench for ARM，推荐IAR Embedded Workbench for ARM V8.50.6及以上版本， 不使用此步可省略。

----

## 快速开始

### 使用Make编译工程

进入到工程目录下， 执行make命令， 即可编译工程。如下：

```bash
# make clean: 清除目标文件; make distclean: 清除目标和中间等文件
make clean
make
```

编译完成后，会在当前目录下生成相应的 $(project).elf文件。

### 使用scons编译工程

进入到工程目录下， 执行scons命令， 即可编译工程。如下：

```shell
# scons --clean 或 scons -c 清除目标和中间文件
scons --clean
scons
```

编译完成后，会在当前目录下生成相应的 $(project).elf文件。

### 使用Keil & IAR编译工程

进入到keil或者IAR目录， 打开相应的工程，即可编译对应的工程。

----
