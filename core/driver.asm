    %include "core/common.asm"

    %ifndef BIN
    extern CDECL(calculate)
    extern CDECL(frame)
    extern CDECL(output)
    extern CDECL(reset)
    %endif
