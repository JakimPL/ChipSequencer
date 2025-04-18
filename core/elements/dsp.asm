    SEGMENT_CODE
load_dsp:
.load_dsp:
    movzx ebx, byte [current_dsp]
    LOAD_VECTOR_ITEM dsps, dsp_offset
.load_effect:
    movzx ebx, byte [DSP_EFFECT_INDEX + ecx]
    mov [current_effect], bl
.load_input:
    movzx ebx, byte [current_dsp]
    mov eax, [dsp_input + 4 * ebx]
    ret

reset_dsps:
    movzx bx, [num_dsps]
    mov esi, reset_dsp
    call reset
    ret

reset_dsp:
    movzx ecx, bl
    mov dword [dsp_timer + 2 * ecx], 0
    ret

clear_dsps:
    movzx bx, [num_dsps]
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
    LOAD_FUNCTION effects, ebx
    ret

load_dsp_target:
    LOAD_OFFSET ecx, dsp_offset
    mov edi, [DSP_OUTPUT + ecx]
    mov cl, [DSP_OUTPUT_FLAG + ecx]
    ret

load_dsp_buffer:
    movzx ecx, byte [current_dsp]
    movzx esi, word [dsp_timer + 2 * ecx]
    shl si, 2
    add si, [buffer_offsets + 2 * ecx]
    add esi, dsp_buffer
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
    %include "core/dsp/gainer.asm"
    %include "core/dsp/delay.asm"
    %include "core/dsp/filter.asm"

    SEGMENT_DATA
effects:
    %ifdef ELF
    dd gainer
    dd delay
    dd filter
    %else
    dw gainer
    dw delay
    dw filter
    %endif

    SEGMENT_BSS
    current_dsp resb 1
    current_effect resb 1
    %ifdef ELF
    dsp_offset resd 1
    %else
    dsp_offset resw 1
    %endif
