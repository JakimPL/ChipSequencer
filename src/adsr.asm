    section .text

adsr:
; If mode == 0: attack
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
; Interpolates linearly between two values:
; upper part of EBX and the lower part of EBX
    movzx eax, bx
    mov ecx, ebx
    shl ecx, 16
    sub ax, cx
    mov ecx, [global_timer]
    mul ecx
    div dword [dividend]

    neg ax
    add ax, bx
    ret

attack:
; volume = timer / attack_time
    mov eax, [global_timer]
    mov ecx, [dividend]
    shr ecx, 16
    div ecx
    shr eax, 1
    ret
decay:
; volume = 0x8000 + global_timer * (sustain_level - 0x8000) / dividend
    mov bx, [sustain_level]
    shl bx, 16
    add bx, 0x8000
    call interpolate
    ret
hold:
    mov ax, [sustain_level]
    ret
release:
; volume = sustain_level - global_timer / dividend
    xor bx, bx
    shl bx, 16
    add bx, [sustain_level]
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

sustain_level:
    dw 0x1000
timers:
    dw 15                    ; attack:  0.015 seconds
    dw 150                   ; decay:   0.15 seconds
    dw 1500                  ; hold:   1.5 seconds
    dw 500                   ; release: 0.5 seconds
