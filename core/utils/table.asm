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

load_item_16bit:
; Arguments:
; ECX - beginning offset (sequence)
; BL  - element index
    cmp bl, 0
    jz .done
.find_data_16bit:
    movzx eax, word [ecx]
    add ecx, eax
    dec bl
    jnz .find_data_16bit
.done:
    ret

reset:
; BX - loop size
; SI - function call
.reset_loop:
    cmp bx, 0
    je .done
    dec bx
    call esi
    jmp .reset_loop
.done:
    ret

load_table_16bit_item:
; EBX - table size
; ECX - divisor
; SI  - table address
; AX  - input timer/output
    call load_table_item
    jc .no_interpolation
.interpolation:
    call sample_interpolation_set
    mov cx, [esi + 2 * eax]
    call sample_interpolation_modulo
    mov bx, [esi + 2 * eax]
    call sample_interpolation_apply
    jmp .done
.no_interpolation:
    mov ax, [esi + 2 * eax]
.done:
    ret

load_table_8bit_item:
; EBX - table size
; ECX - divisor
; SI  - table address
; AX  - input timer/output
    cmp dl, 0x80
    jc .no_interpolation
.interpolation:
    call load_table_item
    call sample_interpolation_set
    movzx cx, byte [esi + eax]
    call sample_interpolation_modulo
    movzx bx, byte [esi + eax]
    call sample_interpolation_apply
    jmp .done
.no_interpolation:
    call load_table_item
    mov al, [esi + eax]
.done:
    ret

load_table_item:
    mul ebx
    div ecx
    ret

sample_interpolation_set:
    shr ecx, 16
    mov [value], ecx
    shr edx, 16
    mov [angle], edx
    xor edx, edx
    dec bx
    ret

sample_interpolation_modulo:
    cmp ax, bx
    je .modulo
    inc ax
    ret
.modulo:
    sub ax, bx
    ret

sample_interpolation_apply:
    lea esi, [angle]
    lea edi, [value]
    xchg bx, cx
    call interpolate
    movzx eax, ax
    ret
