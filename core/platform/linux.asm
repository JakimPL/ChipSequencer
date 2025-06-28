    bits 32

    %ifdef WIN32
    %define CDECL(x) _ %+ x
    %else
    %define CDECL(x) x
    %endif

    %define BIN

    %ifdef DEBUG
    global _start
    %else
    global CDECL(output)
    %define PHDR_FILE_SIZE {file_size}
    %define PHDR_MEMORY_SIZE 0xD00000
    %define PHDR_PERMISSION 0x7
    %define PHDR_ALIGN 0x1000
    %endif

    %define SYS_EXIT 0x01
    %define SYS_FORK 0x02
    %define SYS_READ 0x03
    %define SYS_WRITE 0x04
    %define SYS_CLOSE 0x06
    %define SYS_EXECVE 0x0B
    %define SYS_PIPE 0x2A
    %define SYS_IOCTL 0x36
    %define SYS_DUP2 0x3F

    %define TCSETS_CMD 0x5402

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
    dd PHDR_FILE_SIZE
    dd PHDR_MEMORY_SIZE
    dd PHDR_PERMISSION
    dd PHDR_ALIGN
    phdr_size equ $ - phdr
    %endif

message:
    db "{message}", 10
    message_len equ $ - message
cmd_aplay:
    db "/bin/aplay", 0
cmd_dash_r:
    db "-r", 0
cmd_sample_rate:
    db "{sample_rate}", 0
cmd_channels:
    db "{output_channels}", 0
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

_start:
.pipe:
    mov al, SYS_PIPE
    mov ebx, pipe_fds
    int 0x80

.fork:
    mov al, SYS_FORK
    int 0x80

    test al, al
    jz child

    mov al, SYS_CLOSE
    mov ebx, [pipe_fds]
    int 0x80

.parent:
    pusha
    call print_message
    call initialize
    popa

.termios:
    xor eax, eax
    mov edi, termios
    mov ecx, 4
.clear_termios_loop:
    stosd
    loop .clear_termios_loop

.sys_ioctl:
    mov al, SYS_IOCTL
    mov ebx, 0
    mov ecx, TCSETS_CMD
    lea edx, [termios]
    int 0x80

main_loop:
    pusha

.read:
    mov al, SYS_READ
    xor ebx, ebx
    mov ecx, key
    mov edx, 1
    int 0x80

    cmp al, 1
    jne .mix

    call exit

.mix:
    call CDECL(frame)

.write_stream:
    mov eax, SYS_WRITE
    mov ebx, [pipe_fds + 4]
    mov ecx, CDECL(output)
    mov edx, 4 * {output_channels}
    int 0x80

    popa
    jmp main_loop

exit:
    mov eax, SYS_CLOSE
    mov ebx, [pipe_fds + 4]
    int 0x80

    mov al, SYS_EXIT
    xor ebx, ebx
    int 0x80

child:
.child_close_write:
    mov eax, SYS_CLOSE
    mov ebx, [pipe_fds + 4]
    int 0x80

.dup2:
    mov al, SYS_DUP2
    mov ebx, [pipe_fds]
    mov ecx, 0
    int 0x80

.child_close_read:
    mov al, SYS_CLOSE
    mov ebx, [pipe_fds]
    int 0x80

.execve:
    mov al, SYS_EXECVE
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

    section .bss
    pipe_fds resd 2
    termios resb 32
    key resb 1
