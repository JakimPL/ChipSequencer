    SEGMENT_CODE
noise:
    %ifdef USED_OSCILLATOR_NOISE
    movzx ebx, byte [current_channel]
    lea esi, [CDECL(oscillator_timer) + 4 * ebx]
    mov ax, [noise_seed + 2 * ebx]
    add ax, [esi]
    mov cx, ax
    shr ax, 1
    and cx, 1
    jz .no_xor
    xor ax, NOISE_GALOIS_GENERATOR
.no_xor:
    mov [noise_seed + 2 * ebx], ax
.apply_volume:
    call apply_volume
    ret

initialize_seeds:
    movzx ecx, byte [CDECL(num_channels)]
    mov edi, noise_seed
    mov ax, NOISE_SEED
    rep stosw
    %endif
    ret

    %ifdef USED_OSCILLATOR_NOISE
    SEGMENT_BSS
noise_seed:
    %ifdef TRACKER
    resw MAX_CHANNELS
    %else
    resw CHANNELS
    %endif
    %endif
