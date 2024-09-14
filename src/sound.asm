play_sound:
    mov dx, 22ch
    mov al, 10h
    out dx, al

; Play sound
    mov al, [sound]
    mov dx, [port]
    out dx, al

; Send End of Interrupt (EOI) signal to PIC (Programmable Interrupt Controller)
    mov al, 0x20
    out 0x20, al
    ret

subroutine:
    mov eax, [index]
    mov ecx, [dividend]
    shr ecx, 1
    mov ebx, eax
    cmp eax, ecx

    jl .low_value
    jmp .high_value

.low_value:
    mov ax, 0x8000
    mov bx, [volume]
    add ax, bx
    jmp .save_sound

.high_value:
    mov ax, 0x8000
    mov bx, [volume]
    sub ax, bx

.save_sound:
; Cast to 8-bit
    mov al, ah
    mov [sound], al
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
