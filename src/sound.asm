subroutine:
    mov dx, 22ch
    mov al, 10h
    out dx, al

    mov eax, [index]
    mov ecx, [dividend]
    shr ecx, 1
    mov ebx, eax
    cmp eax, ecx

; call print_hex
    jl .low_value
    jmp .high_value

.low_value:
    mov ax, 0x8000
    mov bx, [volume]
    add ax, bx
    jmp .output_sound

.high_value:
    mov ax, 0x8000
    mov bx, [volume]
    sub ax, bx

.output_sound:
    mov al, ah
    mov dx, [port]
    out dx, al
    ret

increment_timer:
; Initialize index and frequency
    xor edx, edx
    mov eax, [index]
    mov ebx, [frequency]

; Increment index by frequency
    add eax, ebx

; If index > dividend, let index = index - dividend
    cmp eax, [dividend]
    jl .done
    sub eax, [dividend]
    sbb ecx, 0
.done:
    mov [index], eax
    ret
