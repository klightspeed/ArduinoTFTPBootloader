.section .application,"x",@nobits
.global __app_start
__app_start:

.section .bootloader,"x",@nobits
.global __bootloader_start
__bootloader_start:

.section .optiboot,"x",@nobits
.global __optiboot_start
__optiboot_start:

.section .eeprom.boot,"",@nobits
.global __eeprom_boot_data
__eeprom_boot_data:

