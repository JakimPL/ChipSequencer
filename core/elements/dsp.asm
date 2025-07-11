    %ifdef USED_DSP
    SEGMENT_CODE
load_dsp:
.load_dsp:
    movzx ebx, byte [current_dsp]
    LOAD_VECTOR_ITEM CDECL(dsps), dsp_offset
.load_effect:
    movzx ebx, byte [DSP_EFFECT_INDEX + ecx]
    mov [current_effect], bl
.load_input:
    movzx ebx, byte [current_dsp]
    mov eax, [CDECL(dsp_input) + 4 * ebx]
    ret

reset_dsps:
    movzx bx, [CDECL(num_dsps)]
    mov esi, reset_dsp
    call CDECL(reset)
    ret

reset_dsp:
    movzx ecx, bl
    xor eax, eax
    mov [CDECL(dsp_timer) + 4 * ecx], eax
    ret

clear_dsps:
    movzx bx, [CDECL(num_dsps)]
    mov esi, clear_dsp
    call CDECL(reset)
    ret

clear_dsp:
    movzx ecx, bl
    xor eax, eax
    mov [CDECL(dsp_input) + 4 * ecx], eax
    ret

initialize_dsp_buffers:
    %ifdef TRACKER
    mov ebx, MAX_DSPS * MAX_DSP_BUFFER_SIZE
    %else
    mov ebx, DSPS * MAX_DSP_BUFFER_SIZE
    %endif
    mov esi, initialize_dsp_buffer
    call CDECL(reset)
    ret

initialize_dsp_buffer:
    mov ecx, ebx
    mov dword [CDECL(dsp_buffer) + 4 * ecx], __float32__(0.0)
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
    movzx edi, byte [DSP_TARGET + edi]
    mov edi, [CDECL(targets) + 4 * edi]
    ret

load_dsp_buffer:
    movzx ecx, byte [current_dsp]
    mov ebx, [CDECL(dsp_timer) + 4 * ecx]
    mov esi, ecx
    shl esi, LOG2_MAX_DSP_BUFFER_SIZE + 2
    lea esi, [esi + 4 * ebx]
    add esi, CDECL(dsp_buffer)
    ret

calculate_number_of_samples:
; EBX - input time value
    mov eax, [CDECL(sample_rate)]
    shr eax, 3
    imul eax, 5
    mul ebx
    shr eax, 12
    mov edx, eax
    ret

increment_dsp_timer:
    movzx ecx, byte [current_dsp]
    mov ebx, [CDECL(dsp_timer) + 4 * ecx]
    inc ebx
    cmp ebx, edx
    jne .done
    sub ebx, edx
.done:
    mov [CDECL(dsp_timer) + 4 * ecx], ebx
    ret

; Effects
    %include "core/dsp/gainer.asm"
    %include "core/dsp/distort.asm"
    %include "core/dsp/filter.asm"
    %include "core/dsp/delay.asm"

    SEGMENT_DATA
effects:
    dd gainer
    dd distortion
    dd filter
    dd delay

    SEGMENT_BSS
    current_dsp resb 1
    current_effect resb 1
    dsp_offset resd 1
    %endif
