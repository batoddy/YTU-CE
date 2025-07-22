main:
    # x10 = s1 = 0xC7B52169
    lui   x10, 0xC7B52        # upper 20 bits = 0xC7B52000
    addi  x10, x10, 0x169     # x10 = 0xC7B52169

    # x11 = s2 = 1
    addi  x11, x0, 1

    # x12 = ctr = 0
    addi  x12, x0, 0

    # x13 = i = 0
    addi  x13, x0, 0

    # x14 = limit = 32
    addi  x14, x0, 8
    slli  x14, x14, 2         # x14 = 32

loop_cond:
    beq x13, x14, loop_exit

    and x15, x10, x11         # x15 = s1 & s2

    # if x15 != x11, skip increment (simulate bne)
    # temp = x15 - x11
    add  x16, x15, x31        # placeholder (x31 = x0)
    addi x16, x16, -1         # x16 = x15 - 1
    addi x17, x11, -1         # x17 = x11 - 1
    add  x16, x16, x17        # x16 = x15 - x11
    beq x16, x0, do_inc       # if (x15 - x11 == 0) → eşit

    beq x0, x0, skip_inc

do_inc:
    addi x12, x12, 1          # ctr++

skip_inc:
    slli x11, x11, 1          # s2 <<= 1
    addi x13, x13, 1          # i++
    beq  x0, x0, loop_cond

loop_exit:
    add  a0, x0, x12
    addi a7, x0, 10
    ecall
