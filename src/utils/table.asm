    section .text
load_item:
; Arguments:
; ECX - beginning offset (sequence)
; BL  - element index
    cmp bl, 0
    jz .done
.find_data:
    movzx eax, byte [ecx]
    add ecx, eax
    inc ecx
    dec bl
    jnz .find_data
.done:
    ret

reset:
; BL - loop size
; SI - function call
.reset_loop:
    cmp bl, 0
    je .done
    dec bl
    call si
    jmp .reset_loop
.done:
    ret
