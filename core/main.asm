    %ifdef BITS_16
    %include "core/platform/dos.asm"
    %else
    %include "core/platform/linux.asm"
    %endif
