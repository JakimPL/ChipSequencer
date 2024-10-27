    %define SAMPLE_RATE 0x71AE

    %define PRECALCULATE 0
    %define DIRECT_MODE 1
    %define FFT 1

; SoundBlaster16 configuration
    %ifn DIRECT_MODE
    %define SB_BUFFER_SIZE 0x0400
    %define SB_8BIT 0
    %define SB_EXACT_SAMPLE_RATE 0
    %define SB_BASE 0x0220
    %define SB_IRQ 7
    %define SB_16BIT (1 - SB_8BIT)
    %endif

    %define TABLE_SIZE 0x0800
    %define ANGLE_CONSTANT 0.006135923151542565 ; 2 * PI / TABLE_SIZE

    %if FFT
    %define FFT_SIZE 0x10
    %define FFT_ANGLE_CONSTANT 0.39269908169872414 ; 2 * PI / FFT_SIZE
    %endif
