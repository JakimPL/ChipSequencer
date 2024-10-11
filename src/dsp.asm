    section .text
load_dsp:
    movzx bx, byte [current_effect]
    mov ax, word [dsp_input + bx]
    ret

process_dsp:
    movzx bx, byte [current_effect]
    mov ax, word [effects + bx]
    call ax
    ret

load_dsp_target:
    mov ecx, [effect_offset]
    mov di, word [CHANNEL_OUTPUT + si]
    mov cl, [CHANNEL_SHIFT + ecx]
    ret

; Effects
    %include "SRC\DSP\GAINER.ASM"
    %include "SRC\DSP\DELAY.ASM"

    section .data
effects:
    dw gainer
    dw delay

    section .bss
    current_effect resb 1
    effect_offset resd 1
