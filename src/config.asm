    %define SAMPLE_RATE 0x71AE
    %define TABLE_SIZE 0x0800

    %define PRECALCULATE 0
    %define DIRECT_MODE 0

; SoundBlaster16 configuration
    %ifn DIRECT_MODE
    %define SB_BUFFER_SIZE 0x0200
    %define SB_8BIT 0
    %define SB_EXACT_SAMPLE_RATE 0
    %define SB_BASE 0x0220
    %define SB_IRQ 7
    %define SB_16BIT (1 - SB_8BIT)
    %endif
