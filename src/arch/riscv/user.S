.section .text
.align 2
.globl user_enter

user_enter:
    # a0 = UserContext*

    # Load supervisor CSRs for return into user
    ld t0, 248(a0)          # epc
    csrw sepc, t0

    ld t0, 256(a0)          # status
    csrw sstatus, t0

    # Restore registers from context.
    # IMPORTANT: restore a0 last because a0 holds the context pointer.

    ld ra,   0(a0)
    ld sp,   8(a0)
    ld gp,  16(a0)
    ld tp,  24(a0)
    ld t0,  32(a0)
    ld t1,  40(a0)
    ld t2,  48(a0)
    ld s0,  56(a0)
    ld s1,  64(a0)

    ld a1,  80(a0)
    ld a2,  88(a0)
    ld a3,  96(a0)
    ld a4, 104(a0)
    ld a5, 112(a0)
    ld a6, 120(a0)
    ld a7, 128(a0)

    ld s2, 136(a0)
    ld s3, 144(a0)
    ld s4, 152(a0)
    ld s5, 160(a0)
    ld s6, 168(a0)
    ld s7, 176(a0)
    ld s8, 184(a0)
    ld s9, 192(a0)
    ld s10, 200(a0)
    ld s11, 208(a0)

    ld t3, 216(a0)
    ld t4, 224(a0)
    ld t5, 232(a0)
    ld t6, 240(a0)

    ld a0,  72(a0)

    sret