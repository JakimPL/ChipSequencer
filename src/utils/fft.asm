    SEGMENT_CODE
generate_fft_twiddles:
.load_fft_size:
    mov cx, FFT_SIZE
    mov [fft_size], cx
.load_angle_constant:
    mov di, fft_twiddles
    fld dword [fft_angle_constant]
    fldz

.generate_twiddle:
    fld st0
    fsincos
    fstp dword [di]
    mov eax, dword [di]
    fstp dword [di + 4]
    add edi, 8

    fadd st0, st1            ; angle += fft_angle_constant

    dec cx
    jnz .generate_twiddle

    fstp st0
    fstp st0
    ret

_fft:
    mov cl, [fft_inverse]
    fninit
    pusha
    xor edi, edi

.outer_loop:
    fldz
    fldz

    movzx ecx, word [fft_size]
    xor esi, esi
    mov ebx, edi

.inner_loop:
    xor edx, edx
    mov eax, edi
    mul esi
    cmp byte [fft_inverse], 1
    jne .get_twiddle_index
.inverse:
    neg eax
.get_twiddle_index:
    div ecx
    mov ebx, edx

    mov edx, [fft_input_pointer]
    fld dword [edx + 8 * esi]
    fld dword [edx + 8 * esi + 4]
    fld dword [fft_twiddles + 8 * ebx]
    fld dword [fft_twiddles + 8 * ebx + 4]

.real_part:
    fld st3
    fmul st0, st2
    fld st3
    fmul st0, st2
    fsubp st1, st0
    faddp st6, st0

.imaginary_part:
    fld st3
    fmul st0, st1
    fld st3
    fmul st0, st3
    faddp st1, st0
    faddp st5, st0

.unload_stack:
    fstp st0
    fstp st0
    fstp st0
    fstp st0

.increment:
    inc si
    cmp si, [fft_size]
    jnz .inner_loop

; Store output
    cmp byte [fft_inverse], 1
    jne .store_output
.normalize:
    fild word [fft_size]
    fdiv st1, st0
    fdiv st2, st0
    fstp st0
.store_output:
    mov edx, [fft_output_pointer]
    fstp dword [edx + 8 * edi + 4]
    fstp dword [edx + 8 * edi]

    inc di
    cmp di, [fft_size]
    jnz .outer_loop
    popa
    ret

fft:
    mov dx, fft_output
    mov [fft_output_pointer], dx
    mov dx, fft_input
    mov [fft_input_pointer], dx
    mov byte [fft_inverse], 0
    call _fft
    ret

ifft:
    mov dx, fft_input
    mov [fft_output_pointer], dx
    mov dx, fft_output
    mov [fft_input_pointer], dx
    mov byte [fft_inverse], 1
    call _fft
    ret

    SEGMENT_BSS
    fft_twiddles resq FFT_SIZE
    fft_input resq FFT_SIZE
    fft_output resq FFT_SIZE
    fft_size resw 1
    fft_inverse resb 1

    fft_input_pointer resw 1
    fft_output_pointer resw 1
