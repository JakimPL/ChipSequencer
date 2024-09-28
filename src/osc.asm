    section .text
oscillator:
    mov eax, [oscillator_offset]
    movzx eax, byte [OSCILLATOR_INDEX + eax]
    call [oscillators_table + 2 * eax]
    ret

increment_timer:
; Load the frequency
    movzx ecx, byte [current_channel]
    movzx eax, byte [pitch + ecx]
    mov edx, [channel_offset]
    add al, [edx + CHANNEL_TRANSPOSE]
    shl al, 2
    lea ebx, [frequencies]
    add ebx, eax
    mov ebx, [ebx]
    mov eax, [oscillator_timer + 4 * ecx]

; Increment index by frequency
    add eax, ebx
    call reduce
    mov [oscillator_timer + 4 * ecx], eax
    ret

reduce:
; If index > dividend, let index := index - dividend
    cmp eax, [dividend]
    jl .done
    sub eax, [dividend]
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
    cmp ebx, 0
    jl .next_note
    mov [frequencies + 4 * ebx], eax
    shr eax, 1
    sub ebx, 12
    jmp .fill_loop
.next_note:
    cmp cl, 0
    jnz .loop
    ret

; Oscillators
    %include "SRC\OSC\SQUARE.ASM"
    %include "SRC\OSC\SAW.ASM"

    section .data
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
    dw square
    dw saw

    section .bss
    volume resw 1
    frequencies resd 128
