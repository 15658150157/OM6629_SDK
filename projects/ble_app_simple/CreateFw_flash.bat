python ..\..\tools\imgutils\win\gen_mbr.py

..\..\tools\imgutils\win\CreateFwTool.exe pack ^
    .\ble_app_simple.bin APP 0  .\mbr.bin MBR_USR1 @0x108

pause

