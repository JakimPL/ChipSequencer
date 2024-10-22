    SEGMENT_CODE
oscillator:
    movzx eax, word [oscillator_offset]
    movzx eax, byte [OSCILLATOR_INDEX + eax]
    LOAD_FUNCTION oscillators_table, 4 * eax
    ret

increment_timer:
    movzx ecx, byte [current_channel]
    call check_fixed_frequency
    jne .load_pitch
    movzx ebx, word [channel_offset]
    mov ebx, [CHANNEL_TRANSPOSE + ebx]
    jmp .load_frequency
.load_pitch:
    movzx eax, byte [pitch + ecx]
    movzx edx, word [channel_offset]
    add al, [CHANNEL_TRANSPOSE + edx]
    shl ax, 2
    lea ebx, [frequencies]
    add ebx, eax
    mov ebx, [ebx]
.load_frequency:
    mov eax, [oscillator_timer + 4 * ecx]

; Increment index by frequency
    add eax, ebx
    mov esi, dividend
    call reduce
    mov [oscillator_timer + 4 * ecx], eax
    ret

reduce:
; If index > dividend, let index := index - dividend
    cmp eax, [si]
    jl .done
    sub eax, [si]
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

; Oscillators
    %include "SRC\OSC\SQUARE.ASM"
    %include "SRC\OSC\SAW.ASM"
    %include "SRC\OSC\SINE.ASM"

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
    dd square
    dd saw
    dd sine

    SEGMENT_BSS
    volume resw 1
    frequencies resd 128
