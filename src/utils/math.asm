    SEGMENT_CODE
interpolate:
; Interpolates linearly between two values: BX and CX into AX
; using the interpolation point from SI and the ratio from DI
    movzx eax, bx
    sub eax, ecx
    mov ecx, [si]

    imul ecx
    idiv dword [di]

    neg ax
    add ax, bx
    ret

generate_sine_table:
; Generates a sine table in the sine_table buffer
    fninit
    mov dword [angle], __float32__(0.0)
    mov ecx, TABLE_SIZE
    mov di, sine_table
    fld dword [angle_constant]
.generate:
    dec ecx
    fld dword [angle]
    fsin
    fmul dword [half_range]
    fadd dword [half_range]
    fistp word [di]
    add di, 2
    fld dword [angle]
    fadd dword [angle_constant]
    fstp dword [angle]
    cmp ecx, 0
    jne .generate
    fstp st0
    ret

generate_wavetables:
    mov cl, WAVETABLES
    mov si, wavetables
    mov di, wavetable_samples
.process_wavetable:
    cmp cl, 0
    je .done
    movzx bx, byte [si]
    inc si

.generate_wavetable:
    movzx eax, byte [si]
    shl eax, 8
    mov [di], eax
    add di, 2
    inc si
    dec bx
    jnz .generate_wavetable

    dec cl
    jne .process_wavetable
.done:
    ret

integer_to_float:
; EAX - input/output
    sub eax, BASE_VOLUME

    mov dword [value], eax
    fild dword [value]
    fld dword [half_range]
    fdiv
    fstp dword [value]
    mov eax, [value]
    ret

float_to_integer:
; EAX - input/output
    mov dword [value], eax
    fld dword [value]
    fld dword [half_range]
    fmul
    fistp dword [value]

    mov eax, [value]
    add eax, BASE_VOLUME
    ret

multiply_by_integer:
; EAX - input float, output
; EBX - input 16-bit integer
    mov [value], ebx
    fild dword [value]
    mov [value], eax
    fld dword [value]
    fmul
    fld dword [half_range]
    fdiv
    fstp dword [value]
    mov eax, [value]
    ret

add_floats:
; EAX - input/output
; EBX - input float
    mov dword [value], ebx
    fld dword [value]
    mov dword [value], eax
    fld dword [value]
    fadd
    fstp dword [value]
    fstp st0
    mov eax, dword [value]
    ret

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

    %if FFT
fft:
    cli
    fninit
    xor edi, edi

.outer_loop:
    fldz
    fldz

    push ecx
    mov edx, FFT_SIZE
    xor esi, esi
    mov ebx, edi

.inner_loop:
    push ecx
    xor edx, edx
    mov eax, edi
    mul esi
    mov ecx, FFT_SIZE
    div ecx
    mov ecx, edx

    fld dword [fft_input + 8 * esi]
    fld dword [fft_input + 8 * esi + 4]
    fld dword [fft_twiddles + 8 * ecx]
    fld dword [fft_twiddles + 8 * ecx + 4]

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
    sti
    ret
    %endif

    SEGMENT_DATA
angle_constant:
    dd __float32__(ANGLE_CONSTANT)
fft_angle_constant:
    dd __float32__(FFT_ANGLE_CONSTANT)
half_range:
    dd __float32__(32767.5)

    SEGMENT_BSS
    angle resd 1
    value resd 1
    sine_table resw TABLE_SIZE
    fft_twiddles resd FFT_SIZE * 2

    %if FFT
    fft_input resd FFT_SIZE * 2
    fft_output resd FFT_SIZE * 2
    %endif
