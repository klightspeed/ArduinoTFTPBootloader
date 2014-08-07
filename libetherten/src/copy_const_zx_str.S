1:	lpm r0, Z+
	st X+, r0
	or r0, r0
	breq 1f
.globl __copy_const_zx_str
__copy_const_zx_str:
	subi r23, 1
	brne 1b
1:
	ret

