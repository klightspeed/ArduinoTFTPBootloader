.section .vectors,"ax",@progbits
.global __vectors
__vectors:
	jmp __init
.p2align 2
	jmp __vector_1
.p2align 2
	jmp __vector_2
.p2align 2
	jmp __vector_3
.p2align 2
	jmp __vector_4
.p2align 2
	jmp __vector_5
.p2align 2
	jmp __vector_6
.p2align 2
	jmp __vector_7
.p2align 2
	jmp __vector_8
.p2align 2
	jmp __vector_9
.p2align 2
	jmp __vector_10
.p2align 2
	jmp __vector_11
.p2align 2
	jmp __vector_12
.p2align 2
	jmp __vector_13
.p2align 2
	jmp __vector_14
.p2align 2
	jmp __vector_15
.p2align 2
	jmp __vector_16
.p2align 2
	jmp __vector_17
.p2align 2
	jmp __vector_18
.p2align 2
	jmp __vector_19
.p2align 2
	jmp __vector_20
.p2align 2
	jmp __vector_21
.p2align 2
	jmp __vector_22
.p2align 2
	jmp __vector_23
.p2align 2
	jmp __vector_24
.p2align 2
	jmp __vector_25

.p2align 5
.global __flash_write_page
__flash_write_page:
	jmp ___flash_write_page
.p2align 2
.global __reboot_application
__reboot_application:
	jmp ___reboot_application
.p2align 2
.global __reboot_bootloader
__reboot_bootloader:
	jmp ___reboot_bootloader
.p2align 2
.global __wdt_reset_mcu
__wdt_reset_mcu:
	jmp ___wdt_reset_mcu
.p2align 2

