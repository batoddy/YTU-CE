;  MEAN SQUARED ERROR (MSE) HESAPLAMA

SSG SEGMENT PARA STACK 'STACK'
            DW 30 DUP(?)
SSG ENDS

DSG SEGMENT PARA 'DATA'
        SUM DD 0
        MSE DW 0
        ; VAR DB 0FBH
        D1  DW 10,  1, -3,  7,  0
        D2  DW 5,  5,  8, -6,  9
        N   DW 5
DSG ENDS

CSG SEGMENT PARA 'CODE'
                  ASSUME CS:CSG, DS:DSG, SS:SSG
START PROC FAR

                  PUSH   DS
                  XOR    AX, AX
                  PUSH   AX
                  MOV    AX, DSG
                  MOV    DS, AX

                  XOR    AX, AX
                  MOV    WORD PTR [SUM],   AX          ;
                  MOV    WORD PTR [SUM+2], AX          ;

                  LEA    SI, D1
                  LEA    DI, D2
                  MOV    CX, N

        CALC_LOOP:
                  MOV    AX, [SI]
                  MOV    BX, [DI]

                  SUB    AX, BX
                  IMUL   AX

                  ADD    WORD PTR [SUM],   AX          ;
                  ADD    WORD PTR [SUM+2], DX          ;

                  ADD    SI, 2
                  ADD    DI, 2
                  LOOP   CALC_LOOP

                  MOV    AX, WORD PTR [SUM]            ;
                  MOV    DX, WORD PTR [SUM+2]          ;
                  MOV    CX,N
                  DIV    CX
                  MOV    MSE, AX

                  RETF
START ENDP
CSG ENDS

END START
