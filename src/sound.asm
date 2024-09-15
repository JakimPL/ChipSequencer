play_sound:
    mov dx, 22ch
    mov al, 10h
    out dx, al

; Play sound
    mov al, [sound]
    mov dx, [port]
    out dx, al

; Send End of Interrupt (EOI) signal to PIC (Programmable Interrupt Controller)
    mov al, 0x20
    out 0x20, al
    ret

subroutine:
    mov eax, [index]
    mov ecx, [dividend]
    shr ecx, 1
    mov ebx, eax
    cmp eax, ecx

    jl .low_value
    jmp .high_value

.low_value:
    mov ax, 0x8000
    mov bx, [volume]
    add ax, bx
    jmp .save_sound

.high_value:
    mov ax, 0x8000
    mov bx, [volume]
    sub ax, bx

.save_sound:
; Cast to 8-bit
    mov al, ah
    mov [sound], al
    ret

increment_timer:
    xor eax, eax
    xor edx, edx
.load_frequency:
    lea ebx, [frequency_data]
    mov al, [pitch]
    shl al, 2
    add ebx, eax
    mov ebx, [ebx]
    mov eax, [index]

; Increment index by frequency
    add eax, ebx
    call reduce
    mov [index], eax
    ret

reduce:
; If index > dividend, let index := index - dividend
    cmp eax, [dividend]
    jl .done
    sub eax, [dividend]
    sbb ecx, 0
    stc
    ret
.done:
    clc
    ret

    section .data
port:
    dw 0x022C
pitch:
    db 0x14
dividend:
    dd 0x71AE0000
index:
    dd 0
sound:
    db 0
base_volume:
    dw 0x0FFF
volume:
    dw 0x0FFF

frequency_data:
    dd 0x0015D3A7
    dd 0x00171FE9
    dd 0x00187FED
    dd 0x0019F4E0
    dd 0x001B8000
    dd 0x001D229F
    dd 0x001EDE22
    dd 0x0020B405
    dd 0x0022A5D8
    dd 0x0024B546
    dd 0x0026E410
    dd 0x00293415
    dd 0x002BA74E
    dd 0x002E3FD2
    dd 0x0030FFDB
    dd 0x0033E9C0
    dd 0x00370000
    dd 0x003A453E
    dd 0x003DBC44
    dd 0x00416809
    dd 0x00454BB0
    dd 0x00496A8C
    dd 0x004DC821
    dd 0x0052682A
    dd 0x00574E9B
    dd 0x005C7FA5
    dd 0x0061FFB5
    dd 0x0067D380
    dd 0x006E0000
    dd 0x00748A7B
    dd 0x007B7888
    dd 0x0082D013
    dd 0x008A9760
    dd 0x0092D517
    dd 0x009B9041
    dd 0x00A4D054
    dd 0x00AE9D37
    dd 0x00B8FF49
    dd 0x00C3FF6A
    dd 0x00CFA700
    dd 0x00DC0000
    dd 0x00E914F6
    dd 0x00F6F110
    dd 0x0105A025
    dd 0x01152EC1
    dd 0x0125AA2E
    dd 0x01372082
    dd 0x0149A0A8
    dd 0x015D3A6D
    dd 0x0171FE92
    dd 0x0187FED5
    dd 0x019F4E01
    dd 0x01B80000
    dd 0x01D229EC
    dd 0x01EDE220
    dd 0x020B404A
    dd 0x022A5D82
    dd 0x024B545C
    dd 0x026E4104
    dd 0x0293414F
    dd 0x02BA74DB
    dd 0x02E3FD25
    dd 0x030FFDAA
    dd 0x033E9C01
    dd 0x03700000
    dd 0x03A453D9
    dd 0x03DBC440
    dd 0x04168094
    dd 0x0454BB04
    dd 0x0496A8B9
    dd 0x04DC8208
    dd 0x0526829E
    dd 0x0574E9B6
    dd 0x05C7FA4A
    dd 0x061FFB54
    dd 0x067D3803
    dd 0x06E00000
    dd 0x0748A7B1
    dd 0x07B78880
    dd 0x082D0128
    dd 0x08A97607
    dd 0x092D5172
    dd 0x09B90410
    dd 0x0A4D053D
    dd 0x0AE9D36B
    dd 0x0B8FF494
    dd 0x0C3FF6A7
    dd 0x0CFA7005
    dd 0x0DC00000
    dd 0x0E914F62
    dd 0x0F6F1100
    dd 0x105A0251
    dd 0x1152EC0E
    dd 0x125AA2E4
    dd 0x13720820
    dd 0x149A0A79
    dd 0x15D3A6D6
    dd 0x171FE928
    dd 0x187FED4E
    dd 0x19F4E00B
    dd 0x1B800000
    dd 0x1D229EC4
    dd 0x1EDE2200
    dd 0x20B404A2
    dd 0x22A5D81D
    dd 0x24B545C7
    dd 0x26E41040
    dd 0x293414F2
    dd 0x2BA74DAC
    dd 0x2E3FD250
    dd 0x30FFDA9D
    dd 0x33E9C015
    dd 0x37000000
    dd 0x3A453D89
    dd 0x3DBC4401
    dd 0x41680943
    dd 0x454BB03A
    dd 0x496A8B8F
    dd 0x4DC82080
    dd 0x526829E4
    dd 0x574E9B58
    dd 0x5C7FA49F
    dd 0x61FFB539
    dd 0x67D3802A
    dd 0x6E000000
    dd 0x748A7B12
    dd 0x7B788802
    dd 0x82D01286
