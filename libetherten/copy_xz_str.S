1:	ld r0, X+
	st Z+, r0
	or r0, r0
	breq 1f
.globl __copy_xz_str	
__copy_xz_str:	
	subi r23, 1
	brne 1b
1:	ret

