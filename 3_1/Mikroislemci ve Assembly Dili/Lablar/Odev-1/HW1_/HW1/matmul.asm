.586
.model flat, C
.code
PUBLIC matmul_asm
matmul_asm proc
    push ebp
    mov ebp, esp
    
    push ebx
    push esi
    push edi
    
    sub esp, 12
    
    mov edi, [ebp+16]
    mov eax, [ebp+20]
    imul eax, [ebp+28]
    shl eax, 2
    mov ecx, eax
    xor eax, eax
    shr ecx, 2
    rep stosd
    
    mov dword ptr [ebp-4], 0
    
outer_loop:
    mov eax, [ebp-4]
    cmp eax, [ebp+20]
    jge outer_end
    
    mov dword ptr [ebp-8], 0
    
middle_loop:
    mov eax, [ebp-8]
    cmp eax, [ebp+28]
    jge middle_end
    
    mov dword ptr [ebp-12], 0
    
inner_loop:
    mov eax, [ebp-12]
    cmp eax, [ebp+24]
    jge inner_end
    
    mov eax, [ebp-4]
    imul eax, [ebp+24]
    add eax, [ebp-12]
    mov esi, [ebp+8]
    mov ebx, [esi+eax*4]
    
    mov eax, [ebp-12]
    imul eax, [ebp+28]
    add eax, [ebp-8]
    mov esi, [ebp+12]
    mov ecx, [esi+eax*4]
    
    imul ebx, ecx
    
    mov eax, [ebp-4]
    imul eax, [ebp+28]
    add eax, [ebp-8]
    mov esi, [ebp+16]
    add [esi+eax*4], ebx
    
    inc dword ptr [ebp-12]
    jmp inner_loop
    
inner_end:
    inc dword ptr [ebp-8]
    jmp middle_loop
    
middle_end:
    inc dword ptr [ebp-4]
    jmp outer_loop
    
outer_end:
    add esp, 12
    pop edi
    pop esi
    pop ebx
    pop ebp
    ret
    
matmul_asm endp
end