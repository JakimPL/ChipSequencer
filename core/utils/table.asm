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
    shr ecx, 16
    mov [value], ecx
    shr edx, 16
    mov [angle], edx
    xor edx, edx
    dec bx
    mov cx, [esi + 2 * eax]
    cmp ax, bx
    jne .load_items
.modulo:
    sub ax, bx
.load_items:
    mov bx, [esi + 2 * eax + 2]
    lea esi, [angle]
    lea edi, [value]
    call interpolate
    movzx eax, ax
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
