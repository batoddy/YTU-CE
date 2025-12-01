EXTRN IS_SORTED:FAR, BINARY_SEARCH:FAR

SSG SEGMENT PARA STACK 'STACK'
        DW 128 DUP(?)
SSG ENDS

DSG SEGMENT PARA 'DATA'
    is_sorted_flag DW  0
    number_list    DW  3, 7, 10, 15, 21, 25, 30, 42, 57, 60
    number_count   DW  10
    key            DW  ?
    result_index   DW  ?
    
    CR             EQU 13
    LF             EQU 10
    msg_array      DB  CR, LF, 'Dizi: ', 0
    msg_sorted     DB  CR, LF, 'Dizi sirali.', CR, LF, 0
    msg_not_sorted DB  CR, LF, 'Dizi sirali degil!', CR, LF, 0
    msg_enter_key  DB  CR, LF, 'Aranacak deger (q=cikis): ', 0
    msg_found      DB  CR, LF, 'Bulundu, index: ', 0
    msg_not_found  DB  CR, LF, 'Bulunamadi (-1)', CR, LF, 0
    
DSG ENDS

CSG SEGMENT PARA 'CODE'
                    ASSUME SS:SSG, DS:DSG, CS:CSG

MAIN PROC FAR
                    PUSH   DS
                    XOR    AX, AX
                    PUSH   AX
               
                    MOV    AX, DSG
                    MOV    DS, AX

                    MOV    AX, OFFSET msg_array
                    CALL   PUT_STR                      ; "Dizi: "
                    CALL   PRINT_ARRAY                  ; number_list içeriğini yaz
                    
    ; check is sotred
                    LEA    SI, number_list
                    MOV    CX, number_count
               
                    SUB    SP, 2
                    PUSH   SI                           ; base addr
                    PUSH   CX                           ; count
                    CALL   IS_SORTED
                    POP    AX
               
                    MOV    is_sorted_flag, AX
               
                    CMP    AX, 1
                    JE     PRINT_SORTED
               
                    MOV    AX, OFFSET msg_not_sorted
                    CALL   PUT_STR
                    JMP    EXIT_PROGRAM
               
    PRINT_SORTED:   
                    MOV    AX, OFFSET msg_sorted
                    CALL   PUT_STR
               
    ; serch ask loop
    SEARCH_LOOP:    
                    MOV    AX, OFFSET msg_enter_key
                    CALL   PUT_STR
               
                    CALL   GETN
             
                    CMP    AX, -999
                    JE     EXIT_PROGRAM
               
                    MOV    key, AX
               
    ; binary search
                    LEA    SI, number_list
                    MOV    BX, 0
                    MOV    DX, number_count
                    DEC    DX
                    MOV    CX, key
               
                    SUB    SP, 2
                    PUSH   CX                           ; key
                    PUSH   DX                           ; max
                    PUSH   BX                           ; min
                    PUSH   SI                           ; array base
                    CALL   BINARY_SEARCH
                    POP    AX
               
                    MOV    result_index, AX
               
                    CMP    AX, -1
                    JE     PRINT_NOT_FOUND
               
                    MOV    AX, OFFSET msg_found
                    CALL   PUT_STR
                    MOV    AX, result_index
                    CALL   PUTN
               
                    JMP    SEARCH_LOOP
               
    PRINT_NOT_FOUND:
                    MOV    AX, OFFSET msg_not_found
                    CALL   PUT_STR
                    JMP    SEARCH_LOOP
               
    EXIT_PROGRAM:   
                    MOV    AL, CR
                    CALL   PUTC
                    MOV    AL, LF
                    CALL   PUTC
                    RETF
MAIN ENDP

    ;-----------------------------------------
GETC PROC NEAR
                    MOV    AH, 1h
                    INT    21H
                    RET
GETC ENDP

    ;-----------------------------------------
PUTC PROC NEAR
                    PUSH   AX
                    PUSH   DX
                    MOV    DL, AL
                    MOV    AH, 2
                    INT    21H
                    POP    DX
                    POP    AX
                    RET
PUTC ENDP

    ;-----------------------------------------
GETN PROC NEAR
                    PUSH   BX
                    PUSH   CX
                    PUSH   DX
               
    GETN_START:     
                    MOV    DX, 1
                    XOR    BX, BX
                    XOR    CX, CX
               
    NEW:            CALL   GETC
                    CMP    AL, CR
                    JE     FIN_READ
                    CMP    AL, 'q'
                    JE     QUIT_SIGNAL
                    CMP    AL, 'Q'
                    JE     QUIT_SIGNAL
                    CMP    AL, '-'
                    JNE    CTRL_NUM
               
    NEGATIVE:       MOV    DX, -1
                    JMP    NEW
               
    CTRL_NUM:       CMP    AL, '0'
                    JB     GETN_START
                    CMP    AL, '9'
                    JA     GETN_START
               
                    SUB    AL, '0'
                    MOV    BL, AL
                    MOV    AX, 10
                    PUSH   DX
                    MUL    CX
                    POP    DX
                    MOV    CX, AX
                    ADD    CX, BX
                    JMP    NEW
               
    QUIT_SIGNAL:    
                    MOV    AX, -999
                    JMP    FIN_GETN
               
    FIN_READ:       MOV    AX, CX
                    CMP    DX, 1
                    JE     FIN_GETN
                    NEG    AX
               
    FIN_GETN:       POP    DX
                    POP    CX
                    POP    BX
                    RET
GETN ENDP

    ;-----------------------------------------
PUTN PROC NEAR
                    PUSH   CX
                    PUSH   DX
                    PUSH   BX
               
                    CMP    AX, 0
                    JGE    PU_DIV
               
                    PUSH   AX
                    MOV    DL, '-'
                    MOV    AH, 02h
                    INT    21h
                    POP    AX
                    NEG    AX
               
    PU_DIV:         XOR    CX, CX
               
    PU_LOOP:        XOR    DX, DX
                    MOV    BX, 10
                    DIV    BX
                    ADD    DL, '0'
                    PUSH   DX
                    INC    CX
                    TEST   AX, AX
                    JNZ    PU_LOOP
               
    PU_OUT:         POP    DX
                    MOV    AH, 02h
                    INT    21h
                    LOOP   PU_OUT
               
                    POP    BX
                    POP    DX
                    POP    CX
                    RET
PUTN ENDP

    ;-----------------------------------------
PUT_STR PROC NEAR
                    PUSH   BX
                    MOV    BX, AX
                    MOV    AL, BYTE PTR [BX]
               
    PUT_LOOP:       CMP    AL, 0
                    JE     PUT_FIN
                    CALL   PUTC
                    INC    BX
                    MOV    AL, BYTE PTR [BX]
                    JMP    PUT_LOOP
               
    PUT_FIN:        POP    BX
                    RET
PUT_STR ENDP

    ;-----------------------------------------
PRINT_ARRAY PROC NEAR
                    PUSH   AX
                    PUSH   CX
                    PUSH   SI
               
                    LEA    SI, number_list
                    MOV    CX, number_count
               
    PA_LOOP:        MOV    AX, [SI]
                    CALL   PUTN
               
                    CMP    CX, 1
                    JE     PA_DONE
               
                    MOV    AL, ' '
                    CALL   PUTC
               
                    ADD    SI, 2
                    LOOP   PA_LOOP
               
    PA_DONE:        MOV    AL, CR
                    CALL   PUTC
                    MOV    AL, LF
                    CALL   PUTC
               
                    POP    SI
                    POP    CX
                    POP    AX
                    RET
PRINT_ARRAY ENDP

CSG ENDS
END MAIN