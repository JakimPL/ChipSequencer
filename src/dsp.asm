    section .text
load_dsp:
.load_effect:
    movzx bx, byte [current_effect]
    lea ecx, [dsps]
    call load_item
    ret

process_dsp:
    movzx bx, byte [current_effect]
    mov ax, word [effects + bx]
    call ax
    ret

load_dsp_target:
    mov ecx, [effect_offset]
    mov di, word [DSP_OUTPUT + ecx]
    mov cl, [DSP_SHIFT + ecx]
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
