1:	lpm r0, Z+
	st X+, r0
.globl __copy_const_zx
__copy_const_zx:
	subi r23, 1
	brne 1b
	ret

