PUBLIC IS_SORTED, BINARY_SEARCH
CSG2 SEGMENT PARA 'CODE2'
                   ASSUME CS:CSG2

    ;-----------------------------------------
    ; IS_SORTED Procedure
    ; Stack Layout (FAR call):
    ;   [BP+10] -> result (return value slot)
    ;   [BP+8]  -> array base address
    ;   [BP+6]  -> array count
    ;   [BP+4]  -> return CS
    ;   [BP+2]  -> return IP
    ;   [BP+0]  -> old BP (if we push BP)
    ;-----------------------------------------
IS_SORTED PROC FAR
                   PUSH   BP
                   MOV    BP, SP
                  
                   MOV    CX, [BP+6]          ; count
                   MOV    SI, [BP+8]          ; array base
                   DEC    CX                  ; n number needs n-1 comparison
                                 
    CHK_SORT_LOOP: 
                   MOV    AX, [SI]
                   CMP    AX, [SI+2]
                   JA     IS_NOT_SORTED       ; if n > n+1, not sorted
                   ADD    SI, 2
                   LOOP   CHK_SORT_LOOP
                  
    IS_SORTED_TRUE:
                   MOV    AX, 1
                   JMP    IS_SORTED_DONE
                  
    IS_NOT_SORTED: 
                   MOV    AX, 0
                  
    IS_SORTED_DONE:
                   MOV    [BP+10], AX         ; store result in return slot
                   POP    BP
                   RETF   4
IS_SORTED ENDP

    ;-----------------------------------------
    ; BINARY_SEARCH Procedure (Recursive)
    ; Stack Layout:
    ;   [BP+14] -> result
    ;   [BP+12] -> key
    ;   [BP+10] -> high
    ;   [BP+8]  -> low
    ;   [BP+6]  -> arr base address
    ;   [BP+4]  -> CS
    ;   [BP+2]  -> Ofset
    ;   [BP+0]  -> old BP
    ;-----------------------------------------
BINARY_SEARCH PROC FAR
                   PUSH   BP
                   MOV    BP, SP
                  
    ; Load parameters
                   MOV    SI, [BP+6]          ; base address
                   MOV    BX, [BP+8]          ; low
                   MOV    DX, [BP+10]         ; high
                   MOV    AX, [BP+12]         ; key
                  
    ; Base case: low > high => not found
                   CMP    BX, DX
                   JA     BS_NOT_FOUND
                  
    ; Calculate mid = (low + high) / 2
                   MOV    CX, BX
                   ADD    CX, DX
                   SHR    CX, 1               ; CX = mid index
                  
    ; Access arr[mid]
                   PUSH   BX                  ; save low
                   MOV    BX, CX
                   SHL    BX, 1               ; mid * 2 (word size)
                   MOV    BX, [SI+BX]         ; BX = arr[mid]
                  
    ; Compare key with arr[mid]
                   CMP    AX, BX
                   POP    BX                  ; restore low
                   JE     BS_FOUND
                   JB     BS_GO_LEFT
                  
    ;-----------------------------
    ; Search right half: low = mid + 1
    ;-----------------------------
    BS_GO_RIGHT:   
                   INC    CX                  ; new_low = mid + 1
                  
                   SUB    SP, 2               ; allocate result slot
                   PUSH   WORD PTR [BP+12]    ; key
                   PUSH   WORD PTR [BP+10]    ; high
                   PUSH   CX                  ; low = mid + 1
                   PUSH   WORD PTR [BP+6]     ; base
                   CALL   BINARY_SEARCH
                   POP    AX                  ; get result from stack
                  
                   JMP    BS_EXIT
                  
    ;-----------------------------
    ; Search left half: high = mid - 1
    ;-----------------------------
    BS_GO_LEFT:    
                   DEC    CX                  ; new_high = mid - 1
                  
                   SUB    SP, 2               ; allocate result slot
                   PUSH   WORD PTR [BP+12]    ; key
                   PUSH   CX                  ; high = mid - 1
                   PUSH   WORD PTR [BP+8]     ; low
                   PUSH   WORD PTR [BP+6]     ; base
                   CALL   BINARY_SEARCH
                   POP    AX                  ; get result from stack
                  
                   JMP    BS_EXIT
                  
    BS_FOUND:      
                   MOV    AX, CX              ; return mid index
                   JMP    BS_EXIT
                  
    BS_NOT_FOUND:  
                   MOV    AX, -1              ; return -1
                  
    BS_EXIT:       
                   MOV    [BP+14], AX         ; store result in return slot
                   POP    BP
                   RETF   8                   ; clean 4
BINARY_SEARCH ENDP

CSG2 ENDS
END