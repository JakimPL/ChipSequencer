    SEGMENT_CODE
load_dsp:
.load_dsp:
    movzx bx, byte [current_dsp]
    lea ecx, [dsps]
    call load_item
    mov [dsp_offset], ecx
.load_effect:
    mov bl, [DSP_EFFECT_INDEX + ecx]
    mov [current_effect], bl
.load_input:
    movzx ebx, byte [current_dsp]
    mov eax, [dsp_input + 4 * ebx]
    ret

reset_dsps:
    mov bx, DSPS
    mov esi, reset_dsp
    call reset
    ret

reset_dsp:
    movzx ecx, bl
    mov dword [dsp_timer + 2 * ecx], 0
    ret

clear_dsps:
    mov bx, DSPS
    mov esi, clear_dsp
    call reset
    ret

clear_dsp:
    movzx ecx, bl
    mov dword [dsp_input + 4 * ecx], __float32__(0.0)
    ret

initialize_dsp_buffers:
    mov bx, DSP_BUFFER_SIZE
    mov esi, initialize_dsp_buffer
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
    LOAD_OFFSET ecx, dsp_offset
    mov edi, [DSP_OUTPUT + ecx]
    mov cl, [DSP_SHIFT + ecx]
    ret

load_dsp_buffer:
    movzx cx, byte [current_dsp]
    movzx esi, word [dsp_timer + 2 * ecx]
    shl si, 2
    add si, [buffer_offsets + 2 * ecx]
    add si, dsp_buffer
    ret

increment_dsp_timer:
    movzx ecx, byte [current_dsp]
    mov bx, [dsp_timer + 2 * ecx]
    inc bx
    cmp bx, dx
    jne .done
    sub bx, dx
.done:
    mov [dsp_timer + 2 * ecx], bx
    ret

; Effects
    %include "src/dsp/gainer.asm"
    %include "src/dsp/delay.asm"
    %include "src/dsp/filter.asm"

    SEGMENT_DATA
effects:
    dw gainer
    dw delay
    dw filter

    SEGMENT_BSS
    current_dsp resb 1
    current_effect resb 1
    %ifdef ELF
    dsp_offset resd 1
    %else
    dsp_offset resw 1
    %endif
