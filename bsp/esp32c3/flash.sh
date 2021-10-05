# !/bin/sh
/usr/bin/python3 espressif_hal/components/esptool_py/esptool/esptool.py --chip esp32c3 elf2image --flash_mode dio --flash_freq 40m -o rtthread.bin rtthread.elf
/usr/bin/python3 espressif_hal/components/esptool_py/esptool/esptool.py --chip auto --baud 921600 --before default_reset --after hard_reset write_flash -u --flash_mode dio --flash_freq 40m --flash_size detect 0x0000 bootloader/bootloader.bin 0x8000 bootloader/partitions_singleapp.bin 0x10000 rtthread.bin
