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
         LEA    SI, ARR_C                 ; Celcius Array
         LEA    DI, ARR_F                 ; Fahrenheit Array
         MOV    CX, N                     ; N
         XOR    BX, BX                    ; used for sum to calc average
    L1:  
         MOV    AX, [SI]                  ; AX is used for calculation operations
    ; (AX * 18/10) + 32
         PUSH   BX                        ; Pushing BX to stack for not losing sum
         MOV    BX, 18

         IMUL   BX
         MOV    BX, 10
         
         CWD

         IDIV   BX
         ADD    AX, 32
         MOV    [DI], AX
         POP    BX                        ; Popping BX for adding to sum
         ADD    BX, AX
         ADD    SI, 2
         ADD    DI, 2
         LOOP   L1                        ; Celcius to Fahrenheit Loop

         MOV    AX, BX
         MOV    CX, N
         CWD                              ; Reseting DX depends on AX's sign
         IDIV   CX
         MOV    AVG, AX
         MOV    BX, AX                    ; BX has AVG
         

         LEA    SI, ARR_F
         XOR    DX, DX                    ; DX stores below avg count
    L2:  
         MOV    AX, [SI]
                
         CMP    AX, BX                    ; comparing
         JNGE   J1                        ; if not great/equal jump
         INC    DX
    
    J1:  ADD    SI, 2
         LOOP   L2                        ; count below average loop
        
         MOV    COUNT, DX
          
    ;
         RETF
MAIN ENDP
CSG ENDS

END MAIN