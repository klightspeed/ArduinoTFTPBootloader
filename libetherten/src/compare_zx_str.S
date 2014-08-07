1:	ld r0, Z+
	ld r23, X+
	eor r23, r0
	brne 1f
	or r0, r0
	breq 1f
.globl __compare_zx_str
__compare_zx_str:
	subi r23, 1
	brne 1b
1:	ret

