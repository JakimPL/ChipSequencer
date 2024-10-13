    section .text
load_dsp:
.load_dsp:
    movzx bx, byte [current_dsp]
    lea ecx, [dsps]
    call load_item
    mov dword [dsp_offset], ecx
.load_effect:
    mov bl, [DSP_EFFECT_INDEX + ecx]
    mov [current_effect], bl
.load_input:
    movzx bx, byte [current_dsp]
    mov eax, [dsp_input + 4 * ebx]
    ret

clear_dsps:
    mov bl, DSPS
    mov si, clear_dsp
    call reset
    ret

clear_dsp:
    movzx ecx, bl
    mov dword [dsp_input + 4 * ecx], __float32__(0.0)
    ret

process_dsp:
    movzx ebx, byte [current_effect]
    call [effects + 2 * ebx]
    ret

load_dsp_target:
    mov ecx, [dsp_offset]
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
    current_dsp resb 1
    dsp_offset resd 1
    current_effect resb 1
