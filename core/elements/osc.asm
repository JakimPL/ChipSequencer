    %ifdef TRACKER
    extern reference_frequency
    extern note_divisor
    %endif

    SEGMENT_CODE
oscillator:
    LOAD_OFFSET eax, oscillator_offset
    movzx eax, byte [OSCILLATOR_INDEX + eax]
    LOAD_FUNCTION oscillators_table, eax
    ret

increment_timer:
    movzx ecx, byte [current_channel]
    call load_order_and_check_constant_pitch
    jz .load_pitch
    LOAD_OFFSET eax, channel_offset
    mov eax, [CHANNEL_PITCH + eax]
    jmp .increment_timer
.load_pitch:
    mov ebx, [frequency + 4 * ecx]
.detune:
    LOAD_OFFSET eax, channel_offset
    mov eax, [CHANNEL_PITCH + eax]
    mul ebx
    shrd eax, edx, 57
.increment_timer:
    shr eax, 2
    add eax, dword [oscillator_timer + 4 * ecx]
    mov esi, dividend
    call reduce
    mov [oscillator_timer + 4 * ecx], eax
    ret

initialize_frequencies:
    mov ecx, NOTES
    lea edi, [frequencies + 4 * ecx]
    fild qword [reference_frequency]
    fdiv dword [f_65536]

.loop:
    sub edi, 4
    fld st0
    fld dword [f_65536]
    fmulp st1, st0
    fistp dword [edi]
    fdiv qword [note_divisor]
    loop .loop
    ret

load_timer:
    movzx eax, byte [current_channel]
    mov eax, [oscillator_timer + 4 * eax]
    mov ecx, [dividend]
    ret

apply_volume:
    movzx ebx, word [volume]
    imul ebx
    shr eax, 15
    sub ax, [volume]
    add ax, BASE_VOLUME
    ret

; Oscillators
    %include "core/osc/square.asm"
    %include "core/osc/saw.asm"
    %include "core/osc/sine.asm"
    %include "core/osc/wave.asm"
    %include "core/osc/noise.asm"

    SEGMENT_DATA
    %ifndef TRACKER
reference_frequency:
    dq TUNING_FREQUENCY
note_divisor:
    dq __float64__(TUNING_NOTE_DIVISOR)
    %endif

oscillators_table:
    dd square
    dd saw
    dd sine
    dd wavetable
    dd noise

    SEGMENT_BSS
    volume resw 1
    frequencies resd NOTES
