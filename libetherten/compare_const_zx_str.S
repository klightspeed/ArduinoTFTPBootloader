1:	lpm r0, Z+
	ld r21, X+
	eor r21, r0
	brne 1f
	or r0, r0
	breq 1f
.globl __compare_const_zx_str
__compare_const_zx_str:	
	subi r23, 1
	brne 1b
1:	ret

