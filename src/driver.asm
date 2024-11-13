    %include "src/common.asm"

    extern calculate
    extern mix
    extern output
    extern reset

    global sound_driver_initialize
    global sound_driver_terminate
    global sound_driver_step

    %if DIRECT_MODE
    %include "src/driver/direct.asm"
    %else
    extern buffer
    %include "src/driver/sb16.asm"
    %endif
