    SEGMENT_CODE
filter:
    call load_dsp_buffer
    mov [si], eax

    movzx ebx, byte [current_dsp]
    mov bx, [buffer_offsets + 2 * ebx]
    add bx, dsp_buffer
    mov [fft_input_pointer], bx

    xor ecx, ecx
    mov edx, fft_input
.copy_input:
    mov eax, [ebx + 4 * ecx]
    mov [edx + 8 * ecx], eax
    mov dword [edx + 8 * ecx + 4], 0
    inc cx
    cmp cx, [fft_size]
    jne .copy_input

.fft:
    call fft

.filter:
    movzx ecx, word [dsp_offset]
    movzx ecx, word [DSP_FILTER_VALUE + ecx]
    mov esi, ecx
    and si, 0b0111111111111111
    and cx, 0b1000000000000000
.clear:
    mov dword [fft_output + 8 * esi], 0
    mov dword [fft_output + 8 * esi + 4], 0

    cmp cx, 0
    je .clear_increment
    dec si
    cmp si, 0
    jne .clear
    jmp .ifft
.clear_increment:
    inc si
    cmp si, [fft_size]
    jne .clear

.ifft:
    call ifft

.increment_timer:
    mov dx, [fft_size]
    call increment_dsp_timer

.load_output:
    movzx ebx, byte [current_dsp]
    mov bx, [dsp_timer + 2 * ebx]
    mov eax, [fft_input + 8 * ebx]
    ret
