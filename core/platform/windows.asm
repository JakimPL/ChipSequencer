    bits 32

    %define CDECL(x) _ %+ x

    %define BIN

    org 0x10000000

ehdr:
    db 'MZ'
    times 58 db 0
    dd phdr - ehdr

phdr:
    db 'PE', 0, 0
    dw 0x14C
    dw 1
    times 12 db 0
    dw opthdr_size
    dw 0x102

opthdr:
    dw 0x10B
    times 14 db 0
    dd _start - 0x10000000
    times 8 db 0
    dd 0x10000000
    dd 0x200
    dd 0x200
    dw 4, 0
    times 4 db 0
    dw 4, 0
    times 4 db 0
    dd actual_size
    dd 0x200
    times 4 db 0
    dw 3
    times 26 db 0

    opthdr_size equ $ - opthdr

    db '.text', 0, 0, 0
    dd actual_code_size
    dd 0x200
    dd actual_code_size
    dd 0x200
    times 12 db 0
    dd 0x60000020

    times 0x200 - ($ - ehdr) db 0

_start:
    ret

code_end:
    actual_code_size equ code_end - _start
    actual_size equ code_end - ehdr
