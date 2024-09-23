    section .text
adsr:
    movzx eax, byte [current_channel]
    movzx eax, byte [envelope_mode + eax]

    xor edx, edx
    call [phases + eax * 2]

.set_volume:
    mov word [volume], ax

.update_timer:
    xor edx, edx

; Load the divisor
    movzx eax, byte [current_channel]
    movzx ebx, byte [envelope_mode + eax]
    mov eax, [envelope_offset]
    movzx ecx, word [eax + ENVELOPE_ATTACK + 2 * ebx]
    mov eax, [magic_constant]
    div ecx

.increment_timer:
    movzx ebx, byte [current_channel]
    add dword [envelope_timer + 4 * ebx], eax
    mov eax, [envelope_timer + 4 * ebx]

    call reduce
    mov [envelope_timer + 4 * ebx], eax

    jc .increment_mode
    ret

.increment_mode:
    inc byte [envelope_mode + ebx]

.done:
    ret

interpolate:
; Interpolates linearly between two values: BX and CX into AX
    movzx eax, bx
    sub eax, ecx
    movzx ecx, byte [current_channel]
    mov ecx, [envelope_timer + 4 * ecx]

    imul ecx
    idiv dword [dividend]

    neg ax
    add ax, bx
    ret

attack:
    xor bx, bx
    mov ecx, [envelope_offset]
    mov cx, [ecx + ENVELOPE_BASE_VOLUME]
    call interpolate
    ret
decay:
    mov ecx, [envelope_offset]
    mov bx, [ecx + ENVELOPE_BASE_VOLUME]
    mov cx, [ecx + ENVELOPE_SUSTAIN_LEVEL]
    call interpolate
    ret
hold:
    mov eax, [envelope_offset]
    mov ax, [eax + ENVELOPE_SUSTAIN_LEVEL]
    ret
release:
    mov ebx, [envelope_offset]
    mov bx, [ebx + ENVELOPE_SUSTAIN_LEVEL]
    xor cx, cx
    call interpolate
    ret
note_cut:
    mov ax, 0
    ret

reset_envelope:
    movzx ecx, byte [current_channel]
    mov byte [envelope_mode + ecx], 0
    mov dword [envelope_timer + 4 * ecx], 0
    ret

set_release:
    movzx ecx, byte [current_channel]
    mov byte [envelope_mode + ecx], RELEASE
    ret

    section .data
magic_constant:
    dd 0x3D09000

phases:
    dw attack
    dw decay
    dw hold
    dw release
    dw note_cut
