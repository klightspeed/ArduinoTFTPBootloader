#include "config.h"
.section .vectors,"ax",@progbits
.p2align 5
.global __copy_tftp_flash
__copy_tftp_flash:
	jmp copy_tftp_flash
.p2align 2
.global __copy_tcp_flash
__copy_tcp_flash:
#ifdef CONFIG_NOTCP
	jmp __reboot_application
#else
	jmp copy_tcp_flash
#endif
