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
    mov dword [fft_output], 0
    mov dword [fft_output + 4], 0
    mov dword [fft_output + 8], 0
    mov dword [fft_output + 12], 0
    mov dword [fft_output + 16], 0
    mov dword [fft_output + 20], 0
    mov dword [fft_output + 24], 0
    mov dword [fft_output + 28], 0

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
