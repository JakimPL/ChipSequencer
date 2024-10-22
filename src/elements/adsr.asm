    SEGMENT_CODE
adsr:
    movzx eax, byte [current_channel]
    movzx eax, byte [envelope_mode + eax]

    xor edx, edx
.prepare_interpolation_points:
    movzx ecx, byte [current_channel]
    lea esi, [envelope_timer + 4 * ecx]
    lea edi, [dividend]
.phase:
    LOAD_FUNCTION phases, 4 * eax

.set_volume:
    mov word [volume], ax

.update_timer:
    xor edx, edx

; Load the divisor
    movzx eax, byte [current_channel]
    movzx ebx, byte [envelope_mode + eax]
    movzx eax, word [envelope_offset]
    movzx ecx, word [eax + ENVELOPE_ATTACK + 2 * ebx]
    mov eax, [magic_constant]
    cmp ecx, 0
    je .done
    div ecx

.increment_timer:
    movzx ebx, byte [current_channel]
    add dword [envelope_timer + 4 * ebx], eax
    mov eax, [envelope_timer + 4 * ebx]

    lea esi, [dividend]
    call reduce
    mov [envelope_timer + 4 * ebx], eax

    jc .increment_mode
    ret

.increment_mode:
    inc byte [envelope_mode + ebx]

.done:
    ret

attack:
    xor bx, bx
    movzx ecx, word [envelope_offset]
    mov cx, [ENVELOPE_BASE_VOLUME + ecx]
    call interpolate
    ret
decay:
    movzx ecx, word [envelope_offset]
    mov bx, [ENVELOPE_BASE_VOLUME + ecx]
    mov cx, [ENVELOPE_SUSTAIN_LEVEL + ecx]
    call interpolate
    ret
hold:
    movzx eax, word [envelope_offset]
    mov ax, [ENVELOPE_SUSTAIN_LEVEL + eax]
    ret
release:
.check_if_release:
    movzx ebx, word [envelope_offset]
    mov bx, [ENVELOPE_SUSTAIN_LEVEL + ebx]
    cmp bx, 0
    jne .release
    jmp hold
.release:
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

    SEGMENT_DATA
magic_constant:
    dd 0x3D09000

phases:
    dd attack
    dd decay
    dd hold
    dd release
    dd note_cut
