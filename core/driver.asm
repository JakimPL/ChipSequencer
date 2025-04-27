    %include "core/common.asm"

    %ifndef BIN
    extern calculate
    extern mix
    extern output
    extern reset

    global sound_driver_initialize
    global sound_driver_terminate
    global sound_driver_step
    %endif

    %if DIRECT_MODE
    %include "core/driver/direct.asm"
    %else
    extern buffer
    %include "core/driver/sb16.asm"
    %endif
