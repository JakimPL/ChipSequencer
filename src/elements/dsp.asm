    SEGMENT_CODE
load_dsp:
.load_dsp:
    movzx bx, byte [current_dsp]
    lea cx, [dsps]
    call load_item
    mov [dsp_offset], cx
.load_effect:
    mov bl, [DSP_EFFECT_INDEX + ecx]
    mov [current_effect], bl
.load_input:
    movzx bx, byte [current_dsp]
    mov eax, [dsp_input + 4 * ebx]
    ret

reset_dsps:
    mov bx, DSPS
    mov si, reset_dsp
    call reset
    ret

reset_dsp:
    movzx ecx, bl
    mov dword [dsp_timer + 4 * ecx], 0
    ret

clear_dsps:
    mov bx, DSPS
    mov si, clear_dsp
    call reset
    ret

clear_dsp:
    movzx ecx, bl
    mov dword [dsp_input + 4 * ecx], __float32__(0.0)
    ret

initialize_dsp_buffers:
    mov bx, DSP_BUFFER_SIZE
    mov si, initialize_dsp_buffer
    call reset
    ret

initialize_dsp_buffer:
    movzx ecx, bx
    mov dword [dsp_buffer + 4 * ecx], __float32__(0.0)
    ret

process_dsp:
    movzx ebx, byte [current_effect]
    LOAD_FUNCTION effects, 2 * ebx
    ret

load_dsp_target:
    movzx ecx, word [dsp_offset]
    mov di, [DSP_OUTPUT + ecx]
    mov cl, [DSP_SHIFT + ecx]
    ret

; Effects
    %include "SRC\DSP\GAINER.ASM"
    %include "SRC\DSP\DELAY.ASM"
    %include "SRC\DSP\FILTER.ASM"

    SEGMENT_DATA
effects:
    dw gainer
    dw delay

    SEGMENT_BSS
    current_dsp resb 1
    current_effect resb 1
    dsp_offset resw 1
