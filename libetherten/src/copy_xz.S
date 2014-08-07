1:	ld r0, X+
	st Z+, r0
.globl __copy_xz
__copy_xz:
	subi r23, 1
	brne 1b
	ret

