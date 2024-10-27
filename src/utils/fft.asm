    SEGMENT_CODE
generate_fft_twiddles:
    mov cx, FFT_SIZE
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

fft:
    fninit
    pusha
    xor edi, edi

.outer_loop:
    fldz
    fldz

    mov ecx, FFT_SIZE
    xor esi, esi
    mov ebx, edi

.inner_loop:
    xor edx, edx
    mov eax, edi
    mul esi
    div ecx
    mov ebx, edx

    fld dword [fft_input + 8 * esi]
    fld dword [fft_input + 8 * esi + 4]
    fld dword [fft_twiddles + 8 * ebx]
    fld dword [fft_twiddles + 8 * ebx + 4]

; Real part
    fld st3
    fmul st0, st2
    fld st3
    fmul st0, st2
    fsubp st1, st0
    faddp st6, st0

; Imaginary part
    fld st3
    fmul st0, st1
    fld st3
    fmul st0, st3
    faddp st1, st0
    faddp st5, st0

; Unload stack
    fstp st0
    fstp st0
    fstp st0
    fstp st0

; Increment
    inc esi
    cmp esi, FFT_SIZE
    jnz .inner_loop

; Store output
    fstp dword [fft_output + 8 * edi + 4]
    fstp dword [fft_output + 8 * edi]

    inc edi
    cmp edi, FFT_SIZE
    jnz .outer_loop
    popa
    ret

    SEGMENT_BSS
    fft_twiddles resq FFT_SIZE
    fft_input resq FFT_SIZE
    fft_output resq FFT_SIZE
