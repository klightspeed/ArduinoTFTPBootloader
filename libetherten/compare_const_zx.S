1:	lpm r0, Z+
	ld r21, X+
	eor r21, r0
	brne 1f
.globl __compare_const_zx
__compare_const_zx:
	subi r23, 1
	brne 1b
	eor r1, r1
1:	ret

