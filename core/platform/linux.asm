    bits 32

    %define CDECL(x) x

    %define BIN

    %ifdef DEBUG
    global _start
    %else
    global CDECL(output)
    %define PHDR_FILE_SIZE {file_size}
    %define PHDR_MEMORY_SIZE 0x4000
    %define PHDR_PERMISSION 0x7
    %define PHDR_ALIGN 0x1000

    %define BASE_ADDRESS 0x8048000
    %define BSS_ADDRESS (BASE_ADDRESS + PHDR_MEMORY_SIZE)
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
    org 0x8048000
    db 0x7F, "ELF"           ; e_ident[EI_MAG0-3]
    db 1                     ; e_ident[EI_CLASS]
    db 1                     ; e_ident[EI_DATA]
    db 1                     ; e_ident[EI_VERSION]
    db 0                     ; e_ident[EI_OSABI]
    db 0                     ; e_ident[EI_ABIVERSION]
    times 7 db 0             ; e_ident[EI_PAD]

    dw 2                     ; e_type
    dw 3                     ; e_machine
    dd 1                     ; e_version
    dd _start                ; e_entry
    dd phdr - $$             ; e_phoff
    dd 0                     ; e_shoff
    dd 0                     ; e_flags
    dw 52                    ; e_ehsize
    dw 32                    ; e_phentsize
    dw 2                     ; e_phnum
    dw 0                     ; e_shentsize
    dw 0                     ; e_shnum
    dw 0                     ; e_shstrndx

phdr:
    dd 1                     ; p_type
    dd 0                     ; p_offset
    dd 0x8048000             ; p_vaddr
    dd 0x8048000             ; p_paddr
    dd {file_size}           ; p_filesz
    dd {file_size}           ; p_memsz
    dd 7                     ; p_flags
    dd 0x1000                ; p_align

phdr_bss:
    dd 1                     ; p_type
    dd 0                     ; p_offset
    dd 0x8048000+0xD000000   ; p_vaddr
    dd 0x8048000+0xD000000   ; p_paddr
    dd 0                     ; p_filesz
    dd 0x1000000             ; p_memsz
    dd 6                     ; p_flags
    dd 0x1000                ; p_align

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

    SEGMENT_BSS
    pipe_fds resd 2
    termios resb 32
    key resb 1
