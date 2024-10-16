    %define SB_8BIT 1

    %define SB_BASE 0x0220
    %define SB_IRQ 7
    %define SB_BUFFER_SIZE 0x1000

    %if SB_8BIT
    %define SB_DMA 1
    %define SB_MASK_REG 0x0A
    %define SB_COUNT_REG 0x03
    %define SB_ADDR_REG 0x02
    %define SB_PAGE_REG 0x83
    %define SB_DMA_CHANNEL_1_DISABLE 0x05
    %define SB_AUTO_INIT_PLAYBACK_MODE 0x59
    %define SB_DMA_CHANNEL_COUNT SB_BUFFER_SIZE - 1
    %define SB_EXIT_AUTO_INIT_DMA_MODE 0xD5
    %define SB_ACKNOWLEDGE SB_BASE + 0x0E
    %else
    %define SB_DMA 5
    %define SB_MASK_REG 0xD4
    %define SB_COUNT_REG (0xC0 + (SB_DMA - 4) * 4 + 2)
    %define SB_ADDR_REG (0xC0 + (SB_DMA - 4) * 4)
    %define SB_PAGE_REG 0x8B
    %define SB_DMA_CHANNEL_1_DISABLE 0x04 + SB_DMA % 4
    %define SB_AUTO_INIT_PLAYBACK_MODE 0xB9
    %define SB_DMA_CHANNEL_COUNT SB_BUFFER_SIZE / 2 - 1
    %define SB_EXIT_AUTO_INIT_DMA_MODE 0xD9
    %define SB_ACKNOWLEDGE SB_BASE + 0x0F
    %endif

    %define SB_ISR_OFFSET 0x0F
    %define SB_MODE_REG 0x0B
    %define SB_CLEAR_FF 0xD8
    %define SB_DMA_MODE_REG 0xD6
    %define SB_DMA_MODE 0x58
    %define SB_FLIP_FLOP 0x0C
    %define SB_BLOCK_SIZE 0x48
    %define SB_BYTE_POINTER_FLIP_FLOP_CLEAR 0x00
    %define SB_DMA_CHANNEL_1_ENABLE 0x01
    %define SB_MONO_MODE 0x00

; DSP Commands
    %define SB_8BIT_DMA_MODE 0x1C
    %define SB_16BIT_DMA_MODE 0xB6
    %define SB_TURN_ON_SPEAKER 0xD1
    %define SB_TURN_OFF_SPEAKER 0xD3
    %define SB_TIME_CONSTANT 0x40

    %macro WRITE_PORT_BYTE 2
    mov dx, %1
    mov al, %2
    out dx, al
    %endmacro

    %macro WRITE_PORT_WORD 2
    mov dx, %1
    mov ax, %2
    out dx, al
    mov al, ah
    out dx, al
    %endmacro

    SEGMENT_CODE
initialize:
    call reset_sb_dsp
    call turn_speaker_on
    call install_isr
    call enable_irq
    call calculate_sound_buffer_page_offset
    call program_dma
    call set_sampling_rate
    call clear_buffer
    call start_playback
    ret

terminate:
    call exit_auto_init
    call disable_irq
    call uninstall_isr
    call turn_speaker_off
    ret

clear_buffer:
    mov bx, SB_BUFFER_SIZE
    mov si, clear_buffer_cell
    call reset
    ret

clear_buffer_cell:
    mov byte [buffer + bx], 0
    ret

calculate_sound_buffer_page_offset:
    mov ax, cs
    mov dx, ax
    shr dx, 12
    shl ax, 4
    add ax, buffer
    jnc .continue
    inc dx
.continue:
    mov cx, 0xFFFF
    sub cx, ax
    inc cx
    cmp cx, SB_BUFFER_SIZE
    jae .size_ok
.use_next_page:
    mov ax, 0
    inc dx
.size_ok:
    mov word [dma_page], dx
    mov word [dma_offset], ax
    ret

reset_sb_dsp:
    mov dx, SB_BASE
    add dl, 6

    mov al, 1
    out dx, al
    sub al, al
.delay:
    dec al
    jnz .delay
    out dx, al

    sub cx, cx
.empty:
    mov dx, SB_BASE
    add dl, 0x0E

    in al, dx
    or al, al
    jns .next_attempt

    sub dl, 4
    in al, dx
    cmp al, 0xAA
    je .reset_ok

.next_attempt:
    loop .empty

.reset_ok:
    ret

write_sb_dsp:
    mov dx, SB_BASE
    add dl, 0x0C
.busy:
    in al, dx
    or al, al
    js .busy

    mov al, bl
    out dx, al
    ret

read_sb_dsp:
    mov dx, SB_BASE
    add dl, 0x0E
.busy:
    in al, dx
    or al, al
    jns .busy

    sub dl, 4
    in al, dx
    ret

turn_speaker_on:
    mov bl, SB_TURN_ON_SPEAKER
    call write_sb_dsp
    ret

turn_speaker_off:
    mov bl, SB_TURN_OFF_SPEAKER
    call write_sb_dsp
    ret

enable_irq:
    in al, 0x21
    and al, 01111111b
    out 0x21, al
    ret

disable_irq:
    in al, 0x21
    or al, 10000000b
    out 0x21, al
    ret

exit_auto_init:
    mov bl, SB_EXIT_AUTO_INIT_DMA_MODE
    call write_sb_dsp
    ret

install_isr:
    cli
    xor ax, ax
    mov es, ax

    mov ax, [es:4 * SB_ISR_OFFSET]
    mov [old_int_offset], ax
    mov ax, [es:4 * SB_ISR_OFFSET + 2]
    mov [old_int_seg], ax

    mov word [es:4 * SB_ISR_OFFSET], isr
    mov word [es:4 * SB_ISR_OFFSET + 2], cs
    sti
    ret

uninstall_isr:
    cli
    mov ax, 0
    mov es, ax
    mov ax, [old_int_offset]
    mov [es:4 * SB_ISR_OFFSET], ax
    mov ax, [old_int_seg]
    mov [es:4 * SB_ISR_OFFSET + 2], ax
    sti
    ret

program_dma:
    WRITE_PORT_BYTE SB_MASK_REG, SB_DMA_CHANNEL_1_DISABLE ; Disable DMA channel 1
    WRITE_PORT_BYTE SB_CLEAR_FF, SB_DMA_CHANNEL_1_DISABLE ; Clear FF
    WRITE_PORT_BYTE SB_FLIP_FLOP, SB_BYTE_POINTER_FLIP_FLOP_CLEAR ; Clear byte pointer flip-flop
    WRITE_PORT_BYTE SB_MODE_REG, SB_AUTO_INIT_PLAYBACK_MODE ; Auto-init playback
    WRITE_PORT_WORD SB_COUNT_REG, SB_DMA_CHANNEL_COUNT ; Channel 1 count
    WRITE_PORT_WORD SB_ADDR_REG, [dma_offset] ; Channel 1 address
    WRITE_PORT_BYTE SB_PAGE_REG, [dma_page] ; Page register for 8-bit DMA channel 1
    WRITE_PORT_BYTE SB_MASK_REG, SB_DMA_CHANNEL_1_ENABLE ; Enable DMA channel 1
    WRITE_PORT_BYTE SB_DMA_MODE_REG, SB_DMA_MODE + SB_DMA % 4 ; Transfer mode
    ret

set_sampling_rate:
; 29102 Hz
; time constant = 65536 - (256 000 000 / 29102)
    mov bl, SB_TIME_CONSTANT
    call write_sb_dsp
    mov bl, 0xDE
    call write_sb_dsp
    ret

start_playback:
    %if SB_8BIT
    mov bl, SB_BLOCK_SIZE
    call write_sb_dsp
    mov bl, (SB_BUFFER_SIZE / 2 - 1) & 0xFF
    call write_sb_dsp
    mov bl, (SB_BUFFER_SIZE / 2 - 1) >> 8
    call write_sb_dsp
    mov bl, SB_8BIT_DMA_MODE
    call write_sb_dsp
    %else
    mov bl, SB_16BIT_DMA_MODE
    call write_sb_dsp
    mov bl, SB_MONO_MODE
    call write_sb_dsp
    mov bl, (SB_BUFFER_SIZE / 2 - 1) & 0xFF
    call write_sb_dsp
    mov bl, (SB_BUFFER_SIZE / 2 - 1) >> 8
    call write_sb_dsp
    %endif
    ret

sound_driver_step:
    cmp byte [calculate], 1
    jne .not_ready

    mov cx, SB_BUFFER_SIZE / 2
    mov di, buffer
    cmp byte [buffer_half], 0
    je .fill_buffer
    add di, SB_BUFFER_SIZE / 2
.fill_buffer:
    cmp cx, 0
    je .finish

    pusha
    %if PRECALCULATE & EXE
    call load_precalculated
    %else
    call mix
    %endif
    popa

    mov ax, [output]
    %if SB_8BIT
    mov al, ah               ; Cast to 8-bit
    mov [edi], al
    inc edi
    %else
    xchg al, ah              ; Swap high and low bytes
    mov [edi], ax
    add edi, 2
    %endif

    dec cx
    jmp .fill_buffer
.finish:
    mov byte [calculate], 0
    xor byte [buffer_half], 1
.not_ready:
    ret

play_sound:
    mov dx, SB_ACKNOWLEDGE
    in al, dx

; End Of Interrupt
    mov al, 0x20
    out 0x20, al
    ret

isr:
    pusha
    call play_sound
    mov byte [calculate], 1
    popa
    iret

    SEGMENT_DATA
dma_page:
    dw 0
dma_offset:
    dw 0
old_int_offset:
    dw 0
old_int_seg:
    dw 0
buffer_half:
    db 0

    SEGMENT_BSS
buffer:
    resb SB_BUFFER_SIZE
