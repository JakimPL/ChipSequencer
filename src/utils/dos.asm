    segment code
return_to_dos:
    %if PRECALCULATE
    call deallocate_memory
    %endif
    mov ah, DOS_TERMINATE
    int DOS_INTERRUPT

print_message:
; DX - message address
    mov ah, DOS_PRINT_STRING
    int DOS_INTERRUPT
    ret
