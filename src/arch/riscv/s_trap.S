.section .text
.align 2
.globl supervisor_trap_entry
supervisor_trap_entry:
    # Read sstatus first so we can check SPP
    csrr t0, sstatus

    # If SPP == 0, trap came from U-mode
    andi t1, t0, 0x100
    beqz t1, .from_user

.from_supervisor:
    # We are already on a supervisor stack.
    # Save original supervisor sp in t2.
    mv t2, sp
    j .build_frame

.from_user:
    # Swap user sp with supervisor trap stack from sscratch.
    csrrw sp, sscratch, sp

    # After swap:
    #   sp = supervisor trap stack
    #   sscratch = user sp
    csrr t2, sscratch       # t2 = saved user sp

.build_frame:
    addi sp, sp, -288

    sd ra,   0(sp)
    sd t2,   8(sp)          # saved original sp (user or supervisor)
    sd gp,  16(sp)
    sd tp,  24(sp)
    sd t0,  32(sp)          # save original sstatus value read at entry
    sd t1,  40(sp)
    sd t2,  48(sp)
    sd s0,  56(sp)
    sd s1,  64(sp)
    sd a0,  72(sp)
    sd a1,  80(sp)
    sd a2,  88(sp)
    sd a3,  96(sp)
    sd a4, 104(sp)
    sd a5, 112(sp)
    sd a6, 120(sp)
    sd a7, 128(sp)
    sd s2, 136(sp)
    sd s3, 144(sp)
    sd s4, 152(sp)
    sd s5, 160(sp)
    sd s6, 168(sp)
    sd s7, 176(sp)
    sd s8, 184(sp)
    sd s9, 192(sp)
    sd s10, 200(sp)
    sd s11, 208(sp)
    sd t3, 216(sp)
    sd t4, 224(sp)
    sd t5, 232(sp)
    sd t6, 240(sp)

    csrr t3, sepc
    sd   t3, 248(sp)

    # status already read into t0 at entry, but read again for accuracy
    csrr t3, sstatus
    sd   t3, 256(sp)

    csrr t3, scause
    sd   t3, 264(sp)

    csrr t3, stval
    sd   t3, 272(sp)

    mv a0, sp
    call supervisor_trap_handler

    # Restore CSRs
    ld t0, 248(sp)
    csrw sepc, t0

    ld t0, 256(sp)
    csrw sstatus, t0

    # Reload saved status so we know whether this was from U or S
    ld t0, 256(sp)
    andi t1, t0, 0x100      # SPP bit

    # Restore GPRs except sp for now
    ld ra,   0(sp)
    ld gp,  16(sp)
    ld tp,  24(sp)
    ld t0,  32(sp)
    ld t1,  40(sp)
    ld t2,  48(sp)
    ld s0,  56(sp)
    ld s1,  64(sp)
    ld a0,  72(sp)
    ld a1,  80(sp)
    ld a2,  88(sp)
    ld a3,  96(sp)
    ld a4, 104(sp)
    ld a5, 112(sp)
    ld a6, 120(sp)
    ld a7, 128(sp)
    ld s2, 136(sp)
    ld s3, 144(sp)
    ld s4, 152(sp)
    ld s5, 160(sp)
    ld s6, 168(sp)
    ld s7, 176(sp)
    ld s8, 184(sp)
    ld s9, 192(sp)
    ld s10, 200(sp)
    ld s11, 208(sp)
    ld t3, 216(sp)
    ld t4, 224(sp)
    ld t5, 232(sp)
    ld t6, 240(sp)

    # Load saved original sp
    ld t2, 8(sp)

    # Pop frame
    addi sp, sp, 288

    # If SPP == 0, return to U-mode and restore sscratch to trap stack
    andi t1, t0, 0x100
    beqz t1, .return_to_user

.return_to_supervisor:
    mv sp, t2
    sret

.return_to_user:
    csrw sscratch, sp       # put supervisor trap stack pointer back
    mv   sp, t2             # restore user sp
    sret