.arm

.global FB1BPPtoIndexed
FB1BPPtoIndexed:

    stmfd   sp!, {r4-r11}

    ldr     r10, =0x01010101

    mov     r11, #0xFF000000

0:

    ldr     r9,[r1], #4

    @ bits 0-7

    mov     r3, r9, lsl #24

    orr     r3, r3, r3, lsr #9

    orr     r3, r3, r3, lsr #18

    and     r4, r10, r3

    and     r3, r10, r3, lsr #4

    @ bits 8-15

    and     r5, r11, r9, lsl #16

    orr     r5, r5, r5, lsr #9

    orr     r5, r5, r5, lsr #18

    and     r6, r10, r5

    and     r5, r10, r5, lsr #4

    @ bits 16-23

    and     r7, r11, r9, lsl #8

    orr     r7, r7, r7, lsr #9

    orr     r7, r7, r7, lsr #18

    and     r8, r10, r7

    and     r7, r10, r7, lsr #4

    @ bits 24-31

    and     r9, r11, r9

    orr     r9, r9, r9, lsr #9

    orr     r9, r9, r9, lsr #18

    and     r12, r10, r9

    and     r9, r10, r9, lsr #4



    stmia   r0!, {r3-r9, r12}

    subs    r2, r2, #4

    bgt     0b

    ldmfd   sp!, {r4-r11}

    bx      lr
