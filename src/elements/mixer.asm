    SEGMENT_CODE
mix:
    call clear_dsps
    mov eax, dword __float32__(0.0)
    mov [output], eax
.mix:
    mov cl, CHANNELS
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
    mov cl, DSPS
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
    fild word [normalizer]
    fdiv
    fstp dword [value]
    mov eax, [value]

    call float_to_integer
    mov [output], eax
    ret

store_output:
    mov edx, [di]

.check_mode:
    test cl, 0b01000000
    je .set_size
.set_mode:
    xor edx, edx
.set_size:
; TODO: check 32-bit
    test cl, 0b00010000
    jne .add_16_bit
    test cl, 0b00100000
    jne .add_8_bit
.add_float:
    mov [value], eax
    fld dword [value]
    fadd dword [di]
    fstp dword [di]
    fstp st0
    jmp .done
.add_32_bit:
    call float_to_integer
    call shift
    mov [di], edx
    add [di], eax
    jmp .done
.add_16_bit:
    call float_to_integer
    call shift
    mov [di], dx
    add [di], ax
    jmp .done
.add_8_bit:
    call float_to_integer
    call shift
    mov [di], dl
    add [di], al
.done:
    ret

shift:
    mov bl, cl
    and bl, 0b00000111
    shr eax, cl
    ret
