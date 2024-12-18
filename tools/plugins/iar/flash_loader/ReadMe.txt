1. 将OM6627X.board, OM6627X.flash, OM6627X.mac和OM6627X.out四个文件拷贝到IAR安装目录Embedded Workbench 9.0\arm\config\flashloader\Onmicro中，如果没有Onmicro目录，手动创建后拷入即可

2. 安装有SEGGER应用程序的，会关联IAR烧录算法。

IAR自带JLINK版本低于V7.62的，下载算法文件存放在SEGGER安装目录下SEGGER\JLink\Devices\Onmicro\OM6627X.elf中，没有的话请参考SEGGER算法ReadMe说明操作；

IAR自带JLINK版本高于V7.62的，下载算法文件存放在C:\Users\xxx\AppData\Roaming\SEGGER\JlinkDevices\Devices\Onmicro，没有的话请参考SEGGER算法ReadMe说明操作拷入即可，JLinkDevices.xml存放在C:\Users\xxx\AppData\Roaming\SEGGER\JlinkDevices下。目录结构如下所示：

JlinkDevices
----Devices
--------Onmicro
------------OM6627X.elf
----JlinkDevices.xml