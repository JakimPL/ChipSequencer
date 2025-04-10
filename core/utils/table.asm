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
    %ifdef ELF
    call esi
    %else
    call si
    %endif
    jmp .reset_loop
.done:
    ret

load_table_16bit_item:
; EBX - table size
; ECX - divisor
; SI  - table address
; AX  - input timer/output
    mul ebx
    div ecx
    mov ax, [esi + 2 * eax]
    ret

load_table_8bit_item:
; EBX - table size
; ECX - divisor
; SI  - table address
; AX  - input timer/output
    mul ebx
    div ecx
    mov al, [esi + eax]
    ret
