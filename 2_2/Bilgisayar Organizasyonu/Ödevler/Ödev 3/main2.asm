main:
    # s1 = 0xC7B52169 
    lui   x10, 0xC7B52          # upper 20 bits
    addi  x10, x10, 0x169       # s1: x10 = 0x99999999

    # s2 = 1
    addi  x11, x0, 1             # s2 başlangıcı

    # ctr = 0
    addi  x12, x0, 0             # sayaç

    # i = 0 (loop counter)
    addi  x13, x0, 0

    # sabit 32 değeri (limit)
    addi  x14, x0, 8
    slli  x14, x14, 2            # 8 << 2 = 32

loop_cond:
    beq   x13, x14, loop_exit    # i == 32 → çık

    # x15 = s1 & s2
    and   x15, x10, x11
    bne   x15, x11, no_inc       # eşit değilse atlansın

    # ctr++
    addi  x12, x12, 1

no_inc:
    # s2 <<= 1
    slli  x11, x11, 1

    # i++
    addi  x13, x13, 1

    # tekrar koşula dön
    beq x0, x0, loop_cond

loop_exit:
    # sonucu a0'a yaz
    addi  a0, x12, 0

    # çıkış
    addi  a7, x0, 10
    ecall
