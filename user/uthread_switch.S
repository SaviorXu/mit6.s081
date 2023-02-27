	.text

	/*
         * save the old thread's registers,
         * restore the new thread's registers.
         */

	.globl thread_switch
thread_switch:
	/* YOUR CODE HERE */
		
        /*sd ra, 0(a0)
        sd sp, 0(a1)
        sd s0, 8(a1)
        sd s1, 16(a1)
        sd s2, 24(a1)
        sd s3, 32(a1)
        sd s4, 40(a1)
        sd s5, 48(a1)
        sd s6, 56(a1)
        sd s7, 64(a1)
        sd s8, 72(a1)
        sd s9, 80(a1)
        sd s10, 88(a1)
        sd s11, 96(a1)

        ld ra, 0(a2)
        ld sp, 0(a3)
        ld s0, 8(a3)
        ld s1, 16(a3)
        ld s2, 24(a3)
        ld s3, 32(a3)
        ld s4, 40(a3)
        ld s5, 48(a3)
        ld s6, 56(a3)
        ld s7, 64(a3)
        ld s8, 72(a3)
        ld s9, 80(a3)
        ld s10, 88(a3)
        ld s11, 96(a3)*/

        sd ra, 0(a0)
        sd sp, 8(a0)
        sd s0, 16(a0)
        sd s1, 24(a0)
        sd s2, 32(a0)
        sd s3, 40(a0)
        sd s4, 48(a0)
        sd s5, 56(a0)
        sd s6, 64(a0)
        sd s7, 72(a0)
        sd s8, 80(a0)
        sd s9, 88(a0)
        sd s10, 96(a0)
        sd s11, 104(a0)

        ld ra, 0(a1)
        ld sp, 8(a1)
        ld s0, 16(a1)
        ld s1, 24(a1)
        ld s2, 32(a1)
        ld s3, 40(a1)
        ld s4, 48(a1)
        ld s5, 56(a1)
        ld s6, 64(a1)
        ld s7, 72(a1)
        ld s8, 80(a1)
        ld s9, 88(a1)
        ld s10, 96(a1)
        ld s11, 104(a1)

	ret    /* return to ra */

