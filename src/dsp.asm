    section .text
load_dsp:
    ret

; Effects
    %include "SRC\DSP\DELAY.ASM"

    section .bss
    current_effect resb 1
