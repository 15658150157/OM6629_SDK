# Onmicro IAR PACK安装说明

## 介绍
Onmicro_EWARM_PACK目录存放的是Onmicro的IAR PACK, 其中包括文件夹:
```bash
Onmicro_EWARM_PACK\
    --debugger\Onmicro
    --devices\Onmicro
```

## 使用

### 安装
IAR软件装好之后会有一个arm\config的文件夹,文件夹部分目录如下：
```bash
IAR Systems\Embedded Workbench 9.0\arm\config
    --debugger\
    --devices\
```
1. 将Onmicro_EWARM_PACK\xxx\Onmicro下的文件分别拷贝至IAR安装目录下对应的同名文件夹IAR Systems\Embedded Workbench 9.0\arm\config\xxx\Onmicro中（其中xxx即为debugger、devices、linker或flashloader）。如果安装目录中没有Onmicro文件夹,则在上述四个路径下分别新建Onmicro文件夹，并进行拷贝即可。
2. 打开IAR工程,选择芯片型号Onmicro对应的芯片即可