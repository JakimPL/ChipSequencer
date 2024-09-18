    section .text

adsr:
; If mode == 4: exit
    cmp byte [envelopes], 4
    jge .done

    xor edx, edx
    movzx eax, byte [envelopes]
    call [phases + eax * 2]

.set_volume:
    mov word [volume], ax

.update_timer:
    xor edx, edx

; Load the divisor
    movzx ebx, byte [envelopes]
    movzx ecx, word [envelopes + 9 + 2 * ebx]
    mov eax, [magic_constant]
    div ecx

.increment_timer:
    add dword [envelopes + 1], eax

    mov eax, [envelopes + 1]
    call reduce
    mov [envelopes + 1], eax

    jc .increment_mode
    ret

.increment_mode:
    inc byte [envelopes]

.done:
    ret

interpolate:
; Interpolates linearly between two values: BX and CX into AX
    movzx eax, bx
    sub ax, cx
    mov ecx, [envelopes + 1]
    mul ecx
    div dword [dividend]

    neg ax
    add ax, bx
    ret

attack:
    mov bx, 0x0000
    mov cx, [envelopes + 5]
    call interpolate
    ret
decay:
    mov bx, [envelopes + 5]
    mov cx, [envelopes + 7]
    call interpolate
    ret
hold:
    mov ax, [envelopes + 7]
    ret
release:
    mov bx, [envelopes + 7]
    xor cx, cx
    call interpolate
    ret

    section .data
magic_constant:
    dd 0x3D09000

phases:
    dw attack
    dw decay
    dw hold
    dw release

envelopes:
    db 0                     ; mode
    dd 0                     ; global_timer
    dw 0x7FFF                ; base_volume
    dw 0x0FFF                ; sustain_level
    dw 25                    ; attack
    dw 475                   ; decay
    dw 500                   ; hold
    dw 1000                  ; release
