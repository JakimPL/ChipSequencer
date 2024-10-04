    %define SB_BASE 0x0220
    %define SB_IRQ 7
    %define SB_DMA 1
    %define SOUND_SIZE 0x2000

    section .text
initialize:
    call reset_dsp
    call turn_speaker_off
    call install_isr
    call enable_irq
    call calculate_sound_buffer_page_offset
    call program_dma
    call set_sampling_rate
    call start_playback
    ret

terminate:
    call exit_auto_init
    call disable_irq
    call uninstall_isr
    call turn_speaker_off
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
    mov cx, 0ffffh
    sub cx, ax
    inc cx
    cmp cx, SOUND_SIZE
    jae .size_ok
.use_next_page:
    mov ax, 0
    inc dx
.size_ok:
    mov word [dma_page], dx
    mov word [dma_offset], ax
    ret

reset_dsp:
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
    cmp al, 0aah
    je .reset_ok

.next_attempt:
    loop .empty

.reset_ok:
    ret

write_dsp:
    mov dx, SB_BASE
    add dl, 0x0C
.busy:
    in al, dx
    or al, al
    js .busy

    mov al, bl
    out dx, al
    ret

read_dsp:
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
    mov bl, 0xD1
    call write_dsp
    ret

turn_speaker_off:
    mov bl, 0xD3
    call write_dsp
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
    mov bl, 0xDA
    call write_dsp
    ret

install_isr:
    cli                      ; Clear interrupts while setting vector
    xor ax, ax               ; Segment for IVT (0000h)
    mov es, ax
    mov ax, [es:8*4]
    mov [old_int_offset], ax
    mov ax, [es:8*4+2]
    mov [old_int_seg], ax
    mov word [es:8*4], isr   ; Set ISR offset at INT 08h (IRQ0)
    mov word [es:8*4+2], cs  ; Set ISR segment
    sti                      ; Enable interrupts
    ret

uninstall_isr:
    cli
    mov ax, 0
    mov es, ax
    mov ax, [old_int_offset]
    mov [es:8*4], ax
    mov ax, [old_int_seg]
    mov [es:8*4+2], ax
    sti
    ret

program_dma:
    mov dx, 0x0A             ; write single mask register
    mov al, 0x05             ; disable DMA channel 1
    out dx, al

    mov dx, 0x0C             ; clear byte pointer flip flop
    mov al, 0                ; any value
    out dx, al

    mov dx, 0x0B             ; write mode register
    mov al, 0x59             ; auto-init playback
    out dx, al

    mov dx, 0x03             ; channel 1 count
    mov al, (SOUND_SIZE - 1) & 0xFF
    out dx, al               ; low byte
    mov al, (SOUND_SIZE - 1) >> 8
    out dx, al               ; high byte

    mov dx, 2                ; channel 1 address
    mov ax, [dma_offset]
    out dx, al               ; low byte
    mov al, ah
    out dx, al               ; high byte

    mov dx, 0x83             ; page register for 8-bit DMA channel 1
    mov al, [dma_page]
    out dx, al

    mov dx, 0x0A             ; write single mask register
    mov al, 1                ; enable DMA channel 1
    out dx, al

    ret

set_sampling_rate:
; 29102 Hz
; time constant = 65536 - (256 000 000 / 29102)
    mov bl, 0x40
    call write_dsp
    mov bl, 0xDE
    call write_dsp
    ret

start_playback:
    mov bl, 0x48             ; Set block size for 8-bit auto-init mode
    call write_dsp
    mov bl, (SOUND_SIZE / 2 - 1) & 0xFF
    call write_dsp
    mov bl, (SOUND_SIZE / 2 - 1) >> 8 ; high byte
    call write_dsp

    mov bl, 0x1C             ; Start auto-init 8-bit DMA transfer
    call write_dsp
    ret

sound_driver_step:
    cmp byte [calculate], 1
    jne .not_ready

    mov di, buffer
    mov cx, SOUND_SIZE
.fill_buffer:
    cmp cx, 0
    je .fill_done

    pusha
    call mix
    popa
    mov ax, [sound]
    mov al, ah               ; Cast to 8-bit

    mov [di], al
    inc di
    dec cx
    jmp .fill_buffer
.fill_done:
    mov byte [calculate], 0
.not_ready:
    ret

play_sound:
    mov dx, SB_BASE + 0x0E
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

    section .data

dma_page:
    dw 0
dma_offset:
    dw 0
old_int_offset:
    dw 0
old_int_seg:
    dw 0

    section .bss
buffer:
    resb SOUND_SIZE
