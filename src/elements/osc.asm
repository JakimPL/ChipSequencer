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
    mov ecx, 12
.loop:
    dec cl
    mov eax, [frequency_data + 4 * ecx]
    mov ebx, 116
    add bl, cl
.fill_loop:
    cmp bl, 0
    jl .next_note
    mov [frequencies + 4 * ebx], eax
    shr eax, 1
    sub bl, 12
    jmp .fill_loop
.next_note:
    cmp cl, 0
    jnz .loop
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
frequency_data:
    dd 0x454BB03A
    dd 0x496A8B8F
    dd 0x4DC82080
    dd 0x526829E4
    dd 0x574E9B58
    dd 0x5C7FA49F
    dd 0x61FFB539
    dd 0x67D3802A
    dd 0x6E000000
    dd 0x748A7B12
    dd 0x7B788802
    dd 0x82D01286

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
