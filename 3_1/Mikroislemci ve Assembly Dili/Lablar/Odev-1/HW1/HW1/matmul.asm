.386
.model flat, C
.code
PUBLIC matmul_asm
matmul_asm proc
    ; Create stack frame
    push ebp
    mov ebp, esp
    
    ; Save registers
    push eax
    push ebx
    push ecx
    push edx
    push esi
    push edi
    
    ; local vars
    ; [ebp-4]  -> i
    ; [ebp-8]  -> j
    ; [ebp-12] -> k
    sub esp, 12
    
    ; Paremetreler
    ; [ebp+8]  -> A matrix address
    ; [ebp+12] -> B matrix address
    ; [ebp+16] -> C matrix address
    ; [ebp+20] -> A_rows
    ; [ebp+24] -> A_cols
    ; [ebp+28] -> B_cols
    
    ; C sifirla
    mov edi, [ebp+16]           ; C matrix address
    mov eax, [ebp+20]           ; A_rows
    imul eax, [ebp+28]          ; A_rows * B_cols
    shl eax, 1                 
    shl eax, 1                  ; *4 (int = 4 bytes)
    mov ecx, eax
    xor eax, eax
    rep stosb                   ; Clear C
    
    ; i = 0
    mov dword ptr [ebp-4], 0
    
outer_loop:
    mov eax, [ebp-4]
    cmp eax, [ebp+20]           ; i < A_rows ?
    jge outer_end
    
    ; j = 0 
    mov dword ptr [ebp-8], 0
    
middle_loop:
    mov eax, [ebp-8]
    cmp eax, [ebp+28]           ; j < B_cols ?
    jge middle_end
    
    ; k = 0
    mov dword ptr [ebp-12], 0
    
inner_loop:
    mov eax, [ebp-12]
    cmp eax, [ebp+24]           ; k < A_cols ?
    jge inner_end
    
    ; A[i * A_cols + k]
    mov eax, [ebp-4]            ; i
    imul eax, [ebp+24]          ; i * A_cols
    add eax, [ebp-12]           ; + k
    shl eax, 1                  
    shl eax, 1                  
    mov esi, [ebp+8]            ; A
    mov ebx, [esi+eax]          ; EBX = A[i * A_cols + k]
    
    ;  B[k * B_cols + j]
    mov eax, [ebp-12]           ; k
    imul eax, [ebp+28]          ; k * B_cols
    add eax, [ebp-8]            ; + j
    shl eax, 1                   
    shl eax, 1                   
    mov esi, [ebp+12]           ; B
    mov ecx, [esi+eax]          ; ECX = B[k * B_cols + j]
    
    ; A[i,k] * B[k,j]
    imul ebx, ecx               ; EBX = A[i,k] * B[k,j]
    
    ; C[i * B_cols + j] and add
    mov eax, [ebp-4]            ; i
    imul eax, [ebp+28]          ; i * B_cols
    add eax, [ebp-8]            ; + j
    shl eax, 1                 
    shl eax, 1                 
    mov esi, [ebp+16]           ; C
    add [esi+eax], ebx          ; C[i * B_cols + j] += A[i,k] * B[k,j]
    
    ; k++
    inc dword ptr [ebp-12]
    jmp inner_loop
    
inner_end:
    ; j++
    inc dword ptr [ebp-8]
    jmp middle_loop
    
middle_end:
    ; i++
    inc dword ptr [ebp-4]
    jmp outer_loop
    
outer_end:
    add esp, 12
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    pop ebp
    ret
    
matmul_asm endp
end