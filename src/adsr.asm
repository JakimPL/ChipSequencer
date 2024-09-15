    section .text

adsr:
; If mode == 0: attack
    cmp byte [mode], 0
    je .attack
    cmp byte [mode], 1
    je .decay
.hold:
    mov ax, [sustain_level]
    jmp .set_volume
.attack:
; volume = timer / attack_time
    mov eax, [global_timer]
    mov ecx, [dividend]
    shr ecx, 16

    xor edx, edx
    div ecx
    shr eax, 1
    jmp .set_volume
.decay:
; If mode == 0: attack
    cmp byte [mode], 1
    jne .hold
; volume = 0x8000 + global_timer * (sustain_level - 0x8000) / dividend
    xor eax, eax
    mov ax, 0x8000
    sub ax, [sustain_level]
    mov ecx, [global_timer]
    mul ecx

    mov ecx, [dividend]
    div ecx

    neg ax
    add ax, 0x8000

.set_volume:
    mov word [volume], ax

.update_timer:
    xor ecx, ecx
    xor edx, edx
    mov eax, [magic_constant]

    cmp byte [mode], 0
    je .update_attack_timer
    cmp byte [mode], 1
    je .update_decay_timer
    jmp .done
.update_attack_timer:
    mov cx, [attack_time]
    div ecx
    jmp .increment_timer
.update_decay_timer:
    mov cx, [decay_time]
    div ecx
    jmp .increment_timer
.increment_timer:
    add dword [global_timer], eax

    mov eax, [global_timer]
    call reduce
    mov [global_timer], eax

    jc .increment_mode
    ret

.increment_mode:
    inc byte [mode]

.done:
    ret

    section .data
magic_constant:
    dd 0x3D09000
attack_time:
    dw 15                    ; 0.015 seconds
decay_time:
    dw 150                   ; 0.15 seconds
sustain_level:
    dw 0x0FFF                ; maximum volume
release_time:
    dw 1500                  ; 1.5 seconds
global_timer:
    dd 0
mode:
    db 0
