    %include "src/config.asm"
    %include "src/const.asm"
    %include "src/utils/macros.asm"

    %ifn DIRECT_MODE
    segment bss
    align 2
    buffer resb SB_BUFFER_SIZE * (1 + SB_16BIT)
    %endif

    global player

    segment code
player:
.initialize:
    PRINT_STRING message

    call initialize_frequencies
    call calculate_ticks_per_beat

    call initialize_dsp_buffers
    call reset_channels
    call reset_dsps

    call generate_sine_table
    call generate_wavetables

    %if PRECALCULATE
.precalculate:
    call precalculate
    %endif

    call initialize

main_loop:
.check_esc:
    mov ah, BIOS_KEYBOARD_CHECK
    int BIOS_KEYBOARD_INTERRUPT
    jz .no_key

    mov ah, BIOS_KEYBOARD_READ
    int BIOS_KEYBOARD_INTERRUPT
    cmp al, ESC_KEY
    jmp exit

.no_key:
    cmp byte [calculate], 1
    jnz main_loop
    call sound_driver_step
    jmp main_loop

exit:
    call terminate
    call return_to_dos

    %include "src/song.asm"
    %include "src/vars.asm"
    %include "src/utils.asm"
    %include "src/elements.asm"

    %if DIRECT_MODE
    %include "src/driver/direct.asm"
    %else
    %include "src/driver/sb16.asm"
    %endif

    segment data
calculate:
    db 1
dividend:
    dd SAMPLE_RATE << 16
message:
    db 'Chip Sequencer by Jakim, 2024', 13, 10, '$'

    segment bss
    output resd OUTPUT_CHANNELS
    dsp_buffer resd DSP_BUFFER_SIZE

    %ifndef ELF
    group dgroup bss data
    %endif
