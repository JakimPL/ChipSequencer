    SEGMENT_CODE
adsr:
    movzx eax, byte [current_channel]
    movzx eax, byte [envelope_mode + eax]

    xor edx, edx
.prepare_interpolation_points:
    movzx ecx, byte [current_channel]
    lea esi, [envelope_timer + 4 * ecx]
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
    movzx ebx, byte [envelope_mode + eax]
    LOAD_OFFSET eax, envelope_offset
    movzx ecx, word [eax + ENVELOPE_ATTACK + 2 * ebx]
    mov eax, [magic_constant]
    cmp ecx, 0
    je .done
    div ecx

.increment_timer:
    movzx ebx, byte [current_channel]
    add dword [envelope_timer + 4 * ebx], eax
    mov eax, [envelope_timer + 4 * ebx]

    mov esi, dividend
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
    mov byte [envelope_mode + ecx], 0
    mov dword [envelope_timer + 4 * ecx], 0
    ret

set_release:
    movzx ecx, byte [current_channel]
    mov byte [envelope_mode + ecx], RELEASE
    ret

    SEGMENT_DATA
magic_constant:
; Let C = magic_constant. Each frame, an envelope timer increases by C / v.
; where v is proportional to time t belonging to the range [0, MAX_ENVELOPE_TIMER_LENGTH].
; v is 16-bit, so 0xFFFF corresponds to MAX_ENVELOPE_TIMER_LENGTH (10 s by default):
; v = 0xFFFF * t / MAX_ENVELOPE_TIMER_LENGTH
; The counter is being reset when it exceeds dividend := sample_rate << 14.
; Since there are t * sample_rate frames, the equation is:
; C / v * sample_rate * t = dividend
; which simplifies to:
    dd (0xFFFF << 14) / MAX_ENVELOPE_TIMER_LENGTH

phases:
    %ifdef ELF
    dd attack
    dd decay
    dd hold
    dd release
    dd note_cut
    %else
    dw attack
    dw decay
    dw hold
    dw release
    dw note_cut
    %endif
