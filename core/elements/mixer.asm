    SEGMENT_CODE
mix:
    pusha
    %ifdef USED_DSP
    call clear_dsps
    %endif
    mov edi, CDECL(output)
    xor ecx, ecx
    mov cl, MAX_OUTPUT_CHANNELS
    mov eax, dword __float32__(0.0)
    rep stosd
.mix:
    xor cl, cl
.channel_loop:
    cmp cl, [CDECL(num_channels)]
    %ifdef USED_DSP
    je .dsp
    %else
    je .normalize
    %endif

    mov [current_channel], cl

.process_channel:
    call load_channel
    call step
    call increment_timer
    call play_channel
    call load_channel_target
.check_channel_bypass:
    test ch, FLAG_BYPASS
    jnz .channel_done

    call integer_to_float
    call store_output
.channel_done:
    mov cl, [current_channel]
    inc cl
    jmp .channel_loop

    %ifdef USED_DSP
.dsp:
    xor eax, eax
    xor cl, cl
.dsp_loop:
    cmp cl, [CDECL(num_dsps)]
    je .normalize

    mov [current_dsp], cl
.process_dsp:
    call load_dsp
.check_dsp_bypass:
    LOAD_OFFSET edi, dsp_offset
    mov ch, [DSP_FLAG + edi]
    test ch, FLAG_BYPASS
    jnz .load_dsp_target

    call process_dsp
.load_dsp_target:
    call load_dsp_target
    call store_output

.dsp_done:
    mov cl, [current_dsp]
    inc cl
    jmp .dsp_loop
    %endif

.normalize:
    xor ecx, ecx
    mov cl, MAX_OUTPUT_CHANNELS
.normalize_loop:
    dec cl

    fld dword [CDECL(output) + 4 * ecx]
    fld dword [CDECL(normalizer)]
    fmul

    call save_eax_from_fpu
    mov [CDECL(output) + 4 * ecx], eax

    cmp cl, 0
    jne .normalize_loop
    popa
    ret

store_output:
.splitter:
    test ch, FLAG_SPLITTER
    jz .store_single_output
    mov bl, 0
.store_multiple_outputs:
    push eax

.apply_weight:
    call multiply_by_byte_integer_to_eax

.store:
    push edx
    call store_single_output
    pop edx

.increment:
    shr edx, 8
    add edi, 4
    inc bl
    cmp bl, MAX_OUTPUT_CHANNELS
    pop eax
    jnge .store_multiple_outputs
    ret
.store_single_output:
    call store_single_output

.done:
    ret

store_single_output:
    MOV_FROM_DI edx

.check_mode:
    test cl, MASK_OPERATION_ADD
    je .set_size
.set_mode:
    xor edx, edx
.set_size:
    test cl, MASK_VARIABLE_TYPE
    je .add_or_multiply_float
    jpe .add_32_bit
    test cl, MASK_VARIABLE_BYTE
    jne .add_8_bit
    test cl, MASK_VARIABLE_WORD
    jne .add_16_bit
.add_or_multiply_float:
    call load_eax_to_fpu
    test cl, MASK_OPERATION_MULTIPLY
    jz .add_float
.multiply_float:
    fmul dword [edi]
    jmp .store_float
.add_float:
    fadd dword [edi]
.store_float:
    fstp dword [edi]
    fstp st0
    jmp .done
.add_32_bit:
    call float_to_integer
    shl eax, 16
    call shift
    MOV_TO_DI edx
    ADD_TO_DI eax
    jmp .done
.add_16_bit:
    call float_to_integer
    call shift
    MOV_TO_DI dx
    ADD_TO_DI ax
    jmp .done
.add_8_bit:
    call float_to_integer
    call shift
    MOV_TO_DI dl
    ADD_TO_DI al
.done:
    ret

shift:
    push cx
    and cl, MASK_SHIFT
    shr eax, cl
    pop cx
    ret
