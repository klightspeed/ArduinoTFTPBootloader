1:	ld r0, Z+
	st X+, r0
.globl __copy_zx
__copy_zx:
	subi r23, 1
	brne 1b
	ret

