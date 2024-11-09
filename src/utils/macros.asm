    %macro LOAD_FUNCTION 2
    call [ds:%1 + %2]
    %endmacro

    %macro PRINT_STRING 1
    mov dx, %1
    call print_message
    %endmacro
