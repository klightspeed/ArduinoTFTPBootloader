.globl __strlen_z
__strlen_z:
	push r30
	push r31
	rjmp 2f
1:	ld r0, Z+
	or r0, r0
	breq 1f
2:	subi r23, 1
	brne 1b
1:	mov r23, r30
	pop r31
	pop r30
	sub r23, r30
	ret
