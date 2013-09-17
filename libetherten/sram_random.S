.section .noinit
.global sram_random_seed
sram_random_seed: .long 0

.section .init1
__sram_seed_random:
	eor r1, r1
	ldi r26, 0
	ldi r27, 1
	1:
	ld r0, X+
	eor r8, r0
	lsl r8
	rol r9
	rol r10
	rol r11
	adc r8, r1
	cpi r27, 9
	brne 1b
	sts sram_random_seed, r8
	sts sram_random_seed + 1, r9
	sts sram_random_seed + 2, r10
	sts sram_random_seed + 3, r11

