    SEGMENT_CODE
mix:
    pusha
    call clear_dsps
    mov edi, output
    xor ecx, ecx
    mov cl, MAX_OUTPUT_CHANNELS
    mov eax, dword __float32__(0.0)
    rep stosd
.mix:
    xor cl, cl
.mix_loop:
    cmp cl, [num_channels]
    je .dsp

    mov [current_channel], cl

.process_channel:
    call load_offsets
    call step
    call increment_timer
    call play_channel
    call load_channel_target
    call integer_to_float
    call store_output

    mov cl, [current_channel]
    inc cl
    jmp .mix_loop

.dsp:
    xor eax, eax
    xor cl, cl
.dsp_loop:
    cmp cl, [num_dsps]
    je .normalize

    mov [current_dsp], cl
.process_dsp:
    call load_dsp
    call process_dsp
    call load_dsp_target
    call store_output

    mov cl, [current_dsp]
    inc cl
    jmp .dsp_loop

.normalize:
    fld dword [output]
    fld dword [normalizer]
    fmul

    fstp dword [value]
    mov eax, [value]

    %ifndef ELF
    call float_to_integer
    %endif
    mov [output], eax
    popa
    ret

store_output:
    cmp edx, 0
    je .store_single_output
    mov bl, 0
.store_multiple_outputs:
    push eax

.apply_weight:
    mov [value], eax
    fld dword [value]
    mov dword [value], 0
    mov byte [value], dl
    fimul dword [value]
    fidiv word [i_255]
    fstp dword [value]
    mov eax, [value]

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
    ret

store_single_output:
    MOV_FROM_DI edx

.check_mode:
    test cl, 0b01000000
    je .set_size
.set_mode:
    xor edx, edx
.set_size:
    test cl, 0b00110000
    je .add_or_multiply_float
    jpe .add_32_bit
    test cl, 0b00010000
    jne .add_8_bit
    test cl, 0b00100000
    jne .add_16_bit
.add_or_multiply_float:
    mov [value], eax
    fld dword [value]
    test cl, 0b10000000
    jz .add_float
.multiply_float:
    %ifdef ELF
    fmul dword [edi]
    %else
    fmul dword [di]
    %endif
    jmp .store_float
.add_float:
    %ifdef ELF
    fadd dword [edi]
    %else
    fadd dword [di]
    %endif
.store_float:
    %ifdef ELF
    fstp dword [edi]
    %else
    fstp dword [di]
    %endif
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
    and cl, 0b00001111
    shr eax, cl
    pop cx
    ret
