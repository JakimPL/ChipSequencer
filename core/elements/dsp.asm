    %ifdef USED_DSP
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
    mov dword [dsp_timer + 4 * ecx], 0
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
    %ifdef TRACKER
    mov ebx, MAX_DSPS * MAX_DSP_BUFFER_SIZE
    %else
    mov ebx, DSPS * MAX_DSP_BUFFER_SIZE
    %endif
    mov esi, initialize_dsp_buffer
    call reset
    ret

initialize_dsp_buffer:
    mov ecx, ebx
    mov dword [dsp_buffer + 4 * ecx], __float32__(0.0)
    ret

process_dsp:
    movzx ebx, byte [current_effect]
    LOAD_FUNCTION effects, ebx
    ret

load_dsp_target:
    LOAD_OFFSET edi, dsp_offset
    mov edx, [DSP_SPLITTER + edi]
    mov cl, [DSP_OUTPUT_FLAG + edi]
    mov ch, [DSP_FLAG + edi]
    mov edi, [DSP_OUTPUT + edi]
    ret

load_dsp_buffer:
    movzx ecx, byte [current_dsp]
    mov esi, [dsp_timer + 4 * ecx]
    shl esi, 2
    add esi, [buffer_offsets + 4 * ecx]
    add esi, dsp_buffer
    ret

calculate_number_of_samples:
; EBX - input time value
    mov eax, [sample_rate]
    shr eax, 3
    imul eax, 5
    mul ebx
    shr eax, 12
    mov edx, eax
    ret

increment_dsp_timer:
    movzx ecx, byte [current_dsp]
    mov ebx, [dsp_timer + 4 * ecx]
    inc ebx
    cmp ebx, edx
    jne .done
    sub ebx, edx
.done:
    mov [dsp_timer + 4 * ecx], ebx
    ret

; Effects
    %include "core/dsp/gainer.asm"
    %include "core/dsp/distort.asm"
    %include "core/dsp/filter.asm"
    %include "core/dsp/delay.asm"

    SEGMENT_DATA
effects:
    %ifndef BITS_16
    dd gainer
    dd distortion
    dd filter
    dd delay
    %else
    dw gainer
    dw distortion
    dw filter
    dw delay
    %endif

    SEGMENT_BSS
    current_dsp resb 1
    current_effect resb 1
    %ifndef BITS_16
    dsp_offset resd 1
    %else
    dsp_offset resw 1
    %endif

    %endif
