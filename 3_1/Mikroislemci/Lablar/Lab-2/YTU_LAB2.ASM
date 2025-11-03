; CELCIUS TO FAHRENHEIT CONVERTER

SSG SEGMENT PARA STACK 'STACK'
        DW 30 DUP(?)
SSG ENDS

DSG SEGMENT PARA 'DATA'
    ARR_F DW 7 DUP(?)
    ARR_C DW 0, 11, -273, 72, 100, 27, -33
    N     DW 7
    AVG   DW 0
    COUNT DW 0

DSG ENDS

CSG SEGMENT PARA 'CODE'
         ASSUME CS:CSG, DS:DSG, SS:SSG

MAIN PROC FAR
         PUSH   DS
         XOR    AX,AX
         PUSH   AX

         MOV    AX, DSG
         MOV    DS, AX

    ;
         LEA    SI, ARR_C
         LEA    DI, ARR_F
         MOV    CX, N
         XOR    BX, BX
    L1:  
         MOV    AX, [SI]
    ; (AX * 18/10) + 32
         PUSH   BX
         MOV    BX, 18

         IMUL   BX
         MOV    BX, 10
         
         CWD
         
         IDIV   BX
         ADD    AX, 32
         MOV    [DI], AX
         POP    BX
         ADD    AX, BX
         ADD    SI, 2
         ADD    DI, 2
         LOOP   L1
    
         MOV    AX, BX
         MOV    CX, N
         IDIV   CX
         MOV    AVG, AX


          
    ;
         RETF
MAIN ENDP
CSG ENDS

END MAIN