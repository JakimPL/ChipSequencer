    SEGMENT_CODE
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
; BX - loop size
; SI - function call
.reset_loop:
    cmp bx, 0
    je .done
    dec bx
    call si
    jmp .reset_loop
.done:
    ret

load_table_item:
; EBX - table size
; SI  - table address
; AX  - input timer/output
    mul ebx
    div ecx
    movzx esi, si
    mov ax, word [esi + 2 * eax]
    ret
