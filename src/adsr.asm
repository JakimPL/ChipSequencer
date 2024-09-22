    %define ENVELOPE_SIZE 17
    %define ENVELOPE_MODE 0
    %define ENVELOPE_GLOBAL_TIMER 1
    %define ENVELOPE_BASE_VOLUME 5
    %define ENVELOPE_SUSTAIN_LEVEL 7
    %define ENVELOPE_ATTACK 9

    section .text
adsr:
    call load_offsets
    mov eax, [envelope_offset]
    movzx eax, byte [eax + ENVELOPE_MODE]

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
    movzx ebx, byte [eax + ENVELOPE_MODE]
    movzx ecx, word [eax + ENVELOPE_ATTACK + 2 * ebx]
    mov eax, [magic_constant]
    div ecx

.increment_timer:
    mov ecx, [envelope_offset]
    add dword [ecx + ENVELOPE_GLOBAL_TIMER], eax

    mov eax, [ecx + ENVELOPE_GLOBAL_TIMER]
    call reduce
    mov ecx, [envelope_offset]
    mov [ecx + ENVELOPE_GLOBAL_TIMER], eax

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
    mov ecx, [ecx + ENVELOPE_GLOBAL_TIMER]

    imul ecx
    idiv dword [dividend]

    neg ax
    add ax, bx
    ret

attack:
    xor bx, bx
    mov ecx, [envelope_offset]
    mov cx, [ecx + ENVELOPE_BASE_VOLUME]
    call interpolate
    ret
decay:
    mov ecx, [envelope_offset]
    mov bx, [ecx + ENVELOPE_BASE_VOLUME]
    mov cx, [ecx + ENVELOPE_SUSTAIN_LEVEL]
    call interpolate
    ret
hold:
    mov eax, [envelope_offset]
    mov ax, [eax + ENVELOPE_SUSTAIN_LEVEL]
    ret
release:
    mov ebx, [envelope_offset]
    mov bx, [ebx + ENVELOPE_SUSTAIN_LEVEL]
    xor cx, cx
    call interpolate
    ret

reset_envelope:
    mov ecx, [envelope_offset]
    mov byte [ecx + ENVELOPE_MODE], 0
    mov dword [ecx + ENVELOPE_GLOBAL_TIMER], 0
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
    dw 0x7FFF                ; base_volume
    dw 0x2FFF                ; sustain_level
    dw 125                   ; attack
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
