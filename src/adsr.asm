    %define ENVELOPE_SIZE 17 ; Define the size of each envelope

    section .text
adsr:
    call load_offsets
    mov eax, [envelope_offset]
    movzx eax, byte [eax]

; If mode == 4: exit
    cmp al, 4
    jge .done

    xor edx, edx
    call [phases + eax * 2]

.set_volume:
    mov word [volume], ax

.update_timer:
    xor edx, edx

; Load the divisor
    mov eax, [envelope_offset]
    movzx ebx, byte [eax]
    movzx ecx, word [eax + 9 + 2 * ebx]
    mov eax, [magic_constant]
    div ecx

.increment_timer:
    mov ecx, [envelope_offset]
    add dword [ecx + 1], eax

    mov eax, [ecx + 1]
    call reduce
    mov ecx, [envelope_offset]
    mov [ecx + 1], eax

    jc .increment_mode
    ret

.increment_mode:
    mov ecx, [envelope_offset]
    inc byte [ecx]

.done:
    ret

interpolate:
; Interpolates linearly between two values: BX and CX into AX
    movzx eax, bx
    sub eax, ecx
    mov ecx, [envelope_offset]
    mov ecx, [ecx + 1]

    imul ecx
    idiv dword [dividend]

    neg ax
    add ax, bx
    ret

attack:
    xor bx, bx
    mov ecx, [envelope_offset]
    mov cx, [ecx + 5]
    call interpolate
    ret
decay:
    mov ecx, [envelope_offset]
    mov bx, [ecx + 5]
    mov cx, [ecx + 7]
    call interpolate
    ret
hold:
    mov eax, [envelope_offset]
    mov ax, [eax + 7]
    ret
release:
    mov ebx, [envelope_offset]
    mov bx, [ebx + 7]
    xor cx, cx
    call interpolate
    ret

reset_envelope:
    mov ecx, [envelope_offset]
    mov byte [ecx], 0
    mov dword [ecx + 1], 0
    ret

    section .data
magic_constant:
    dd 0x3D09000

phases:
    dw attack
    dw decay
    dw hold
    dw release

envelopes:
; Envelope 1
    db 0                     ; mode
    dd 0                     ; global_timer
    dw 0x2FFF                ; base_volume
    dw 0x2FFF                ; sustain_level
    dw 25                    ; attack
    dw 25                    ; decay
    dw 500                   ; hold
    dw 1000                  ; release

; Envelope 2
    db 0                     ; mode
    dd 0                     ; global_timer
    dw 0x2FFF                ; base_volume
    dw 0x2FFF                ; sustain_level
    dw 250                   ; attack
    dw 250                   ; decay
    dw 250                   ; hold
    dw 250                   ; release
