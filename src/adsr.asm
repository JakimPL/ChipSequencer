    section .text

adsr:
; If mode == 4: exit
    cmp byte [mode], 4
    jge .done

    xor edx, edx
    movzx eax, byte [mode]
    call [phases + eax * 2]

.set_volume:
    mov word [volume], ax

.update_timer:
    xor edx, edx

; Load the divisor
    movzx ebx, byte [mode]
    movzx ecx, word [timers + 2 * ebx]
    mov eax, [magic_constant]
    div ecx

.increment_timer:
    add dword [global_timer], eax

    mov eax, [global_timer]
    call reduce
    mov [global_timer], eax

    jc .increment_mode
    ret

.increment_mode:
    inc byte [mode]

.done:
    ret

interpolate:
; Interpolates linearly between two values: BX and CX into AX
    movzx eax, bx
    sub ax, cx
    mov ecx, [global_timer]
    mul ecx
    div dword [dividend]

    neg ax
    add ax, bx
    ret

attack:
    mov bx, 0x0000
    mov cx, [base_volume]
    call interpolate
    ret
decay:
    mov bx, [base_volume]
    mov cx, [sustain_level]
    call interpolate
    ret
hold:
    mov ax, [sustain_level]
    ret
release:
    mov bx, [sustain_level]
    xor cx, cx
    call interpolate
    ret

    section .data
magic_constant:
    dd 0x3D09000
global_timer:
    dd 0
mode:
    db 0

phases:
    dw attack
    dw decay
    dw hold
    dw release

base_volume:
    dw 0x7FFF                ; range 0x0000 - 0x7FFF
sustain_level:
    dw 0x0FFF
timers:
    dw 25                    ; attack
    dw 475                   ; decay
    dw 500                   ; hold
    dw 1000                  ; release
