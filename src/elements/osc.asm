    SEGMENT_CODE
oscillator:
    LOAD_OFFSET eax, oscillator_offset
    movzx eax, byte [OSCILLATOR_INDEX + eax]
    LOAD_FUNCTION oscillators_table, eax
    ret

increment_timer:
    movzx ecx, byte [current_channel]
    call check_fixed_frequency
    jne .load_pitch
    LOAD_OFFSET eax, channel_offset
    mov eax, [CHANNEL_PITCH + eax]
    jmp .increment_timer
.load_pitch:
    movzx eax, byte [pitch + ecx]
    shl ax, 2
    lea ebx, [frequencies]
    add ebx, eax
    mov ebx, [ebx]
.detune:
    LOAD_OFFSET eax, channel_offset
    mov eax, [CHANNEL_PITCH + eax]
    mul ebx
    shrd eax, edx, 57
.increment_timer:
    add eax, dword [oscillator_timer + 4 * ecx]
    mov esi, dividend
    call reduce
    mov [oscillator_timer + 4 * ecx], eax
    ret

reduce:
; If index > dividend, let index := index - dividend
    cmp eax, [esi]
    jl .done
    sub eax, [esi]
    sbb ecx, 0
    stc
    ret
.done:
    clc
    ret

initialize_frequencies:
    mov ecx, 128
    lea edi, [frequencies + 4 * ecx]

    fild dword [ref_frequency]
    fdiv dword [f_65536]

.loop:
    sub edi, 4
    fld st0
    fld dword [f_65536]
    fmulp st1, st0
    fistp dword [edi]

    fdiv dword [note_divisor]

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
    %include "src/osc/square.asm"
    %include "src/osc/saw.asm"
    %include "src/osc/sine.asm"
    %include "src/osc/wave.asm"

    SEGMENT_DATA
ref_frequency:
    dd 0x67D40000
note_divisor:
    dd 1.0594630943592953
f_65536:
    dd 65536.0

oscillators_table:
    %ifdef ELF
    dd square
    dd saw
    dd sine
    dd wavetable
    %else
    dw square
    dw saw
    dw sine
    dw wavetable
    %endif

    SEGMENT_BSS
    volume resw 1
    frequencies resd 128
