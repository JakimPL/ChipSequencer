
    section .text
adsr:
    call load_offsets
    mov eax, [instrument_offset]
    movzx eax, byte [eax + INSTRUMENT_ENVELOPE_MODE]

; If mode == 4: exit
    cmp al, 4
    jge .done

    xor edx, edx
    call [phases + eax * 2]

.set_volume:
    mov word [volume], ax

.update_timer:
    xor edx, edx

; Load the divisor
    mov eax, [instrument_offset]
    movzx ebx, byte [eax + INSTRUMENT_ENVELOPE_MODE]
    mov eax, [envelope_offset]
    movzx ecx, word [eax + ENVELOPE_ATTACK + 2 * ebx]
    mov eax, [magic_constant]
    div ecx

.increment_timer:
    mov ebx, [instrument_offset]
    add dword [ebx + INSTRUMENT_ENVELOPE_TIMER], eax
    mov eax, [ebx + INSTRUMENT_ENVELOPE_TIMER]

    call reduce
    mov [ebx + INSTRUMENT_ENVELOPE_TIMER], eax

    jc .increment_mode
    ret

.increment_mode:
    mov ecx, [instrument_offset]
    inc byte [ecx + INSTRUMENT_ENVELOPE_MODE]

.done:
    ret

interpolate:
; Interpolates linearly between two values: BX and CX into AX
    movzx eax, bx
    sub eax, ecx
    mov ecx, [instrument_offset]
    mov ecx, [ecx + INSTRUMENT_ENVELOPE_TIMER]

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

reset_envelope:
    mov ecx, [instrument_offset]
    mov byte [ecx + INSTRUMENT_ENVELOPE_MODE], 0
    mov dword [ecx + INSTRUMENT_ENVELOPE_TIMER], 0
    ret

    section .data
magic_constant:
    dd 0x3D09000

phases:
    dw attack
    dw decay
    dw hold
    dw release
