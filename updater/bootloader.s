.section ".text.data"
.global bootloader_data
.type bootloader_data, @object
bootloader_data:
.incbin "../bootloader/bootloader.bin"
.global bootloader_data_end
bootloader_data_end:
