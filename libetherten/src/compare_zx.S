1:	ld r0, Z+
	ld r21, X+
	eor r21, r0
	brne 1f
.globl __compare_zx
__compare_zx:
	subi r23, 1
	brne 1b
1:	ret

