JLink版本7.64及以上 添加算法步骤:

1. 将本目录下的JLinkDevices.xml文件中的内容拷贝至C:\Users\xxx\AppData\Roaming\SEGGER\JlinkDevices\中。
如果没有该目录，手动创建即可
注：如果目的目录中JLinkDevices.xml已存在，源文件中<DataBase>和</DataBase>标签不用拷贝

2. 将本目录下的elf文件拷贝至C:\Users\xxx\AppData\Roaming\SEGGER\JlinkDevices\Devices\Onmicro\中。
如果没有该目录，手动创建即可

3. 打开J-Link GDB Server， Target device一栏选择目标chip。

4. 检查.gdbinit中的下载命令, 若有 mon device，则需要将device后的chip 修改成上文中对应的chip name



JLink版本7.64以下 添加算法步骤:

1. 将本目录下的JLinkDevices.xml文件中的内容拷贝至segger安装路径的根目录中。
注：如果目的目录中JLinkDevices.xml已存在，源文件中<DataBase>和</DataBase>标签不用拷贝

2. 将本目录下的elf文件拷贝至Devices\Onmicro\中。
如果没有该目录，手动创建即可

3. 打开J-Link GDB Server， Target device一栏选择目标chip。

4. 检查.gdbinit中的下载命令, 若有 mon device，则需要将device后的chip 修改成上文中对应的chip name