    %include "core/common.asm"

    %ifndef BIN
    extern calculate
    extern frame
    extern output
    extern reset

    global sound_driver_initialize
    global sound_driver_terminate
    global sound_driver_step
    %endif
