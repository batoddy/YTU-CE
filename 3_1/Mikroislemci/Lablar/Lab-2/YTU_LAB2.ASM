; CELCIUS TO FAHRENHEIT CONVERTER

SSG SEGMENT PARA STACK 'STACK'
        DW 30 DUP(?)
SSG ENDS

DSG SEGMENT PARA 'DATA'
    AVG   DW 0
    COUNT DW 0

    ARR_F DW 7 DUP(?)
    ARR_C DW 0, 11, -273, 72, 100, 27, -33
    N     DW 7


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
         ADD    BX, AX
         ADD    SI, 2
         ADD    DI, 2
         LOOP   L1

         MOV    AX, BX
         MOV    CX, N
         CWD
         IDIV   CX
         MOV    AVG, AX
         MOV    BX, AX
         

         LEA    SI, ARR_F
         XOR    DX, DX
    L2:  
         MOV    AX, [SI]
                
         CMP    AX, BX
         JNGE   J1
         INC    DX

    J1:  ADD    SI, 2
         LOOP   L2
        
         MOV    COUNT, DX
          
    ;
         RETF
MAIN ENDP
CSG ENDS

END MAIN