    bits 32

    %define BIN

    %ifdef DEBUG
    global _start
    %else
    global output
    %define FILE_SIZE 0x1000
    %define MEMORY_SIZE 0xD00000
    %endif

    %define SYS_EXIT 0x01
    %define SYS_FORK 0x02
    %define SYS_WRITE 0x04
    %define SYS_CLOSE 0x06
    %define SYS_EXECVE 0x0B
    %define SYS_PIPE 0x2A
    %define SYS_DUP2 0x3F

    %ifndef DEBUG
    org 0x10000
program_start:
    db $7F, "ELF", 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0
    dw 2
    dw 3
    dd 1
    dd _start
    dd phdr - program_start
    dd 0
    dd 0
    dw ehdr_size
    dw phdr_size
    dw 1
    dw 0
    dw 0
    dw 0
    ehdr_size equ $ - program_start

phdr:
    dd 1
    dd 0
    dd program_start
    dd program_start
    dd FILE_SIZE
    dd MEMORY_SIZE
    dd 7
    dd 4096
    phdr_size equ $ - phdr
    %endif

message:
    db "Chip Sequencer by Jakim, 2025", 10
    message_len equ $ - message
cmd_aplay:
    db "/bin/aplay", 0
cmd_dash_r:
    db "-r", 0
cmd_sample_rate:
    db "44100", 0
cmd_channels:
    db "2", 0
cmd_dash_c:
    db "-c", 0
cmd_format:
    db "FLOAT_LE", 0
cmd_dash_f:
    db "-f", 0

print_message:
    mov eax, SYS_WRITE
    mov ebx, 1
    mov ecx, message
    mov edx, message_len
    int 0x80
    ret

sound_driver_initialize:
    ret
sound_driver_terminate:
    ret

_start:
.pipe:
    mov al, SYS_PIPE
    mov ebx, esp
    int 0x80
    lea edx, [ebx + 12]
    mov ebp, _start
.fork:
    mov al, SYS_FORK
    int 0x80
    dec eax
    js child

    pusha
    call print_message
    call initialize
    popa

main_loop:
    pusha

    call mix

    mov eax, SYS_WRITE
    mov ebx, [esp + 36]
    mov ecx, output
    mov edx, 4 * 2
    int 0x80

    popa
    jmp main_loop

child:
.child_close_write:
    mov eax, SYS_CLOSE
    mov ebx, [esp + 4]
    int 0x80

.dup2:
    mov eax, SYS_DUP2
    mov ebx, [esp]
    mov ecx, 0
    int 0x80

.child_close_read:
    mov eax, SYS_CLOSE
    mov ebx, [esp]
    int 0x80

.execve:
    mov eax, SYS_EXECVE
    mov ebx, cmd_aplay

    push 0
    push cmd_sample_rate
    push cmd_dash_r
    push cmd_channels
    push cmd_dash_c
    push cmd_format
    push cmd_dash_f
    push ebx

    mov ecx, esp
    push 0
    mov edx, esp

    int 0x80
    mov eax, SYS_EXIT
    mov ebx, 127
    int 0x80

    %include "core/player.asm"
    %include "core/song/data.asm"
