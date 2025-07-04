    SEGMENT_CODE
adsr:
    movzx eax, byte [current_channel]
    movzx eax, byte [CDECL(envelope_mode) + eax]

    xor edx, edx
.prepare_interpolation_points:
    movzx ecx, byte [current_channel]
    lea esi, [CDECL(envelope_timer) + 4 * ecx]
    mov edi, dividend
.phase:
    LOAD_FUNCTION phases, eax
.add_bias:
    LOAD_OFFSET ecx, envelope_offset
    add ax, [ENVELOPE_BIAS + ecx]
.set_volume:
    mov word [volume], ax

.update_timer:
    xor edx, edx

; Load the divisor
    movzx eax, byte [current_channel]
    movzx ebx, byte [CDECL(envelope_mode) + eax]
    LOAD_OFFSET eax, envelope_offset
    movzx ecx, word [eax + ENVELOPE_ATTACK + 2 * ebx]
    mov eax, MAGIC_CONSTANT
    cmp ecx, 0
    je .done
    div ecx

.increment_timer:
    movzx ebx, byte [current_channel]
    add dword [CDECL(envelope_timer) + 4 * ebx], eax
    mov eax, [CDECL(envelope_timer) + 4 * ebx]

    mov esi, dividend
    call reduce
    mov [CDECL(envelope_timer) + 4 * ebx], eax

    jc .increment_mode
    ret

.increment_mode:
    inc byte [CDECL(envelope_mode) + ebx]

.done:
    ret

attack:
    xor bx, bx
    LOAD_OFFSET ecx, envelope_offset
    mov cx, [ENVELOPE_BASE_VOLUME + ecx]
    call interpolate
    ret

decay:
    LOAD_OFFSET ecx, envelope_offset
    mov bx, [ENVELOPE_BASE_VOLUME + ecx]
    mov cx, [ENVELOPE_SUSTAIN_LEVEL + ecx]
    call interpolate
    ret

hold:
    LOAD_OFFSET eax, envelope_offset
    mov ax, [ENVELOPE_SUSTAIN_LEVEL + eax]
    ret

release:
.check_if_release:
    LOAD_OFFSET ebx, envelope_offset
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
    xor eax, eax
    mov [CDECL(envelope_mode) + ecx], al
    mov [CDECL(envelope_timer) + 4 * ecx], eax
    ret

set_phase:
    movzx ecx, byte [current_channel]
    mov byte [CDECL(envelope_mode) + ecx], al
    ret

    SEGMENT_DATA
phases:
    dd attack
    dd decay
    dd hold
    dd release
    dd note_cut
