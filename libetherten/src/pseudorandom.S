.global pseudorandom_state
.comm pseudorandom_state, 4

.section .text
.global get_pseudorandom_bytes
get_pseudorandom_bytes:
	movw r26, r24
	movw r24, r22
	lds r20, pseudorandom_state
	lds r21, pseudorandom_state + 1
	lds r22, pseudorandom_state + 2
	lds r23, pseudorandom_state + 3
	adiw r24, 1
	rjmp 3f
1:	ldi r30, 32
	eor r31, r31
2:	eor r18, r18
	ldi r19, 0x80
	sbrc r20, 1
	eor r18, r19
	sbrc r20, 5
	eor r18, r19
	sbrc r20, 6
	eor r18, r19
	sbrc r23, 7
	eor r18, r19
	lsr r23
	ror r22
	ror r21
	ror r20
	or r23, r18
	eor r31, r20
	subi r30, 1
	brne 2b
	st X+, r31
3:	sbiw r24, 1
	brne 1b
	sts pseudorandom_state, r20
	sts pseudorandom_state + 1, r21
	sts pseudorandom_state + 2, r22
	sts pseudorandom_state + 3, r23
	ret

