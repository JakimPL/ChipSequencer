    SEGMENT_CODE
mix:
    pusha
    call clear_dsps
    mov eax, dword __float32__(0.0)
    mov [output], eax
.mix:
    mov cl, [num_channels]
.mix_loop:
    cmp cl, 0
    je .dsp

    dec cl
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
    jmp .mix_loop

.dsp:
    xor eax, eax
    mov cl, [num_dsps]
.dsp_loop:
    cmp cl, 0
    je .normalize

    dec cl
    mov [current_dsp], cl
.process_dsp:
    call load_dsp
    call process_dsp
    call load_dsp_target
    call store_output

    mov cl, [current_dsp]
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
    MOV_FROM_DI edx

.check_mode:
    test cl, 0b01000000
    je .set_size
.set_mode:
    xor edx, edx
.set_size:
    test cl, 0b00110000
    je .add_float
    jpe .add_32_bit
    test cl, 0b00010000
    jne .add_16_bit
    test cl, 0b00100000
    jne .add_8_bit
.add_float:
    mov [value], eax
    fld dword [value]
    %ifdef ELF
    fadd dword [edi]
    fstp dword [edi]
    %else
    fadd dword [di]
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
