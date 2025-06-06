bits 32
global main         ; Make main visible to linker
extern printf       ; External C library functions
extern exit
extern malloc       ; For 'new' operator if you have one

section .text

    ; TAC: FUNCTION_BEGIN main

main:
    ; Function prologue
    push ebp
    mov ebp, esp
    sub esp, 48
    ; TAC: grade = 'A'
    mov eax, 65
    mov BYTE [ebp-1], al
    ; TAC: isActive = true
    mov eax, 1
    mov DWORD [ebp-4], eax
    ; TAC: _t0 = 20 * 2
    mov eax, 20
    mov ebx, 2
    imul eax, ebx
    mov DWORD [ebp-8], eax
    ; TAC: _t1 = _t0 / 5
    mov eax, DWORD [ebp-8]
    mov ebx, 5
    cdq
    idiv ebx
    mov DWORD [ebp-12], eax
    ; TAC: _t2 = 10 + _t1
    mov eax, 10
    mov ebx, DWORD [ebp-12]
    add eax, ebx
    mov DWORD [ebp-16], eax
    ; TAC: _t3 = _t2 - 3
    mov eax, DWORD [ebp-16]
    mov ebx, 3
    sub eax, ebx
    mov DWORD [ebp-20], eax
    ; TAC: num = _t3
    mov eax, DWORD [ebp-20]
    mov DWORD [ebp-24], eax
    ; TAC: _t4 = num > 15
    mov eax, DWORD [ebp-24]
    mov ebx, 15
    cmp eax, ebx
    setg al
    movzx eax, al
    mov DWORD [ebp-28], eax
    ; ERROR: Unhandled TAC Opcode: 17 (_t5 = _t4 op isActive)
    ; TAC: IF_NOT _t5 GOTO L0
    mov eax, DWORD [ebp-32]
    cmp eax, 0
    je L0
    ; TAC: PRINT num
    mov eax, DWORD [ebp-24]
    push eax
    push DWORD _int_fmt
    call printf
    add esp, 8
    ; TAC: GOTO L1
    jmp L1
    ; TAC: LABEL L0:
L0:
    ; TAC: _t6 = num <= 10
    mov eax, DWORD [ebp-24]
    mov ebx, 10
    cmp eax, ebx
    setle al
    movzx eax, al
    mov DWORD [ebp-36], eax
    ; TAC: IF_NOT _t6 GOTO L2
    mov eax, DWORD [ebp-36]
    cmp eax, 0
    je L2
    ; TAC: PRINT 0
    mov eax, 0
    push eax
    push DWORD _int_fmt
    call printf
    add esp, 8
    ; TAC: GOTO L3
    jmp L3
    ; TAC: LABEL L2:
L2:
    ; TAC: _t7 = num + 1
    mov eax, DWORD [ebp-24]
    mov ebx, 1
    add eax, ebx
    mov DWORD [ebp-40], eax
    ; TAC: PRINT _t7
    mov eax, DWORD [ebp-40]
    push eax
    push DWORD _int_fmt
    call printf
    add esp, 8
    ; TAC: LABEL L3:
L3:
    ; TAC: LABEL L1:
L1:
    ; TAC: LABEL L4:
L4:
    ; TAC: _t8 = num > 0
    mov eax, DWORD [ebp-24]
    mov ebx, 0
    cmp eax, ebx
    setg al
    movzx eax, al
    mov DWORD [ebp-44], eax
    ; TAC: IF_NOT _t8 GOTO L5
    mov eax, DWORD [ebp-44]
    cmp eax, 0
    je L5
    ; TAC: _t9 = num - 1
    mov eax, DWORD [ebp-24]
    mov ebx, 1
    sub eax, ebx
    mov DWORD [ebp-48], eax
    ; TAC: num = _t9
    mov eax, DWORD [ebp-48]
    mov DWORD [ebp-24], eax
    ; TAC: PRINT num
    mov eax, DWORD [ebp-24]
    push eax
    push DWORD _int_fmt
    call printf
    add esp, 8
    ; TAC: GOTO L4
    jmp L4
    ; TAC: LABEL L5:
L5:
    ; TAC: FUNCTION_END main

.epilogue_main:
    ; Function epilogue
    mov esp, ebp
    pop ebp
    ret

section .data
    _int_fmt db "%d", 10, 0
    _char_fmt db "%c", 10, 0
    _true_str db "true", 10, 0
    _false_str db "false", 10, 0
    _str_nl_fmt db "%s", 10, 0
