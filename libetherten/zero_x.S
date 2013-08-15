1:	st X+, r1
.globl __zero_x	
__zero_x:	
	subi r23, 1
	brne 1b
	ret

