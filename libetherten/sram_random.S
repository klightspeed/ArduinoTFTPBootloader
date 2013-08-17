.section .noinit
__rand: .long 0

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
    sts __rand, r8
    sts __rand + 1, r9
    sts __rand + 2, r10
    sts __rand + 3, r11

.section .text
.global sram_get_random_bytes
sram_get_random_bytes:
    movw r26, r24
    movw r24, r22
    lds r20, __rand
    lds r21, __rand + 1
    lds r22, __rand + 2
    lds r23, __rand + 3
    adiw r24, 1
    rjmp 2f
    1:
    eor r18, r18
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
    st X+, r20
    2:
    sbiw r24, 1
    brne 1b
    sts __rand, r20
    sts __rand + 1, r21
    sts __rand + 2, r22
    sts __rand + 3, r23
    ret

