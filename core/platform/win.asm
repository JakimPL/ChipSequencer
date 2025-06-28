    bits 32

    %define CDECL(x) _ %+ x

    %ifdef DEBUG
    global _start
    %else
    global CDECL(output)
    %endif

; Windows API functions
    extern _GetStdHandle@4
    extern _WriteConsoleA@20
    extern _ExitProcess@4
    extern _waveOutOpen@24
    extern _waveOutPrepareHeader@12
    extern _waveOutWrite@12
    extern _waveOutUnprepareHeader@12
    extern _waveOutClose@4
    extern _Sleep@4

; Constants
    %define STD_OUTPUT_HANDLE -11
    %define WAVE_FORMAT_IEEE_FLOAT 0x0003
    %define WAVE_MAPPER -1
    %define WHDR_DONE 0x00000001

; Wave format structure
    struc WAVEFORMATEX
    .wFormatTag resw 1
    .nChannels resw 1
    .nSamplesPerSec resd 1
    .nAvgBytesPerSec resd 1
    .nBlockAlign resw 1
    .wBitsPerSample resw 1
    .cbSize resw 1
    endstruc

; Wave header structure
    struc WAVEHDR
    .lpData resd 1
    .dwBufferLength resd 1
    .dwBytesRecorded resd 1
    .dwUser resd 1
    .dwFlags resd 1
    .dwLoops resd 1
    .lpNext resd 1
    .reserved resd 1
    endstruc

    section .data
message:
    db "{message}", 13, 10
    message_len equ $ - message

; Wave format for float audio
wave_format:
    istruc WAVEFORMATEX
    at WAVEFORMATEX.wFormatTag, dw WAVE_FORMAT_IEEE_FLOAT
    at WAVEFORMATEX.nChannels, dw {output_channels}
    at WAVEFORMATEX.nSamplesPerSec, dd {sample_rate}
    at WAVEFORMATEX.nAvgBytesPerSec, dd {sample_rate} * {output_channels} * 4
    at WAVEFORMATEX.nBlockAlign, dw {output_channels} * 4
    at WAVEFORMATEX.wBitsPerSample, dw 32
    at WAVEFORMATEX.cbSize, dw 0
    iend

; Double buffering
wave_hdr1:
    istruc WAVEHDR
    at WAVEHDR.lpData, dd audio_buffer1
    at WAVEHDR.dwBufferLength, dd 4 * {output_channels}
    at WAVEHDR.dwBytesRecorded, dd 0
    at WAVEHDR.dwUser, dd 0
    at WAVEHDR.dwFlags, dd 0
    at WAVEHDR.dwLoops, dd 0
    at WAVEHDR.lpNext, dd 0
    at WAVEHDR.reserved, dd 0
    iend

wave_hdr2:
    istruc WAVEHDR
    at WAVEHDR.lpData, dd audio_buffer2
    at WAVEHDR.dwBufferLength, dd 4 * {output_channels}
    at WAVEHDR.dwBytesRecorded, dd 0
    at WAVEHDR.dwUser, dd 0
    at WAVEHDR.dwFlags, dd 0
    at WAVEHDR.dwLoops, dd 0
    at WAVEHDR.lpNext, dd 0
    at WAVEHDR.reserved, dd 0
    iend

    section .bss
    wave_out_handle resd 1
    console_handle resd 1
    bytes_written resd 1
    current_buffer resd 1
    audio_buffer1 resd {output_channels}
    audio_buffer2 resd {output_channels}

    section .text
print_message:
    push STD_OUTPUT_HANDLE
    call _GetStdHandle@4
    mov [console_handle], eax

    push 0
    push bytes_written
    push message_len
    push message
    push dword [console_handle]
    call _WriteConsoleA@20
    ret

initialize_audio:
; Open wave output device
    push 0                   ; callback data
    push 0                   ; callback
    push 0                   ; flags
    push wave_format         ; format
    push WAVE_MAPPER         ; device id
    push wave_out_handle     ; handle
    call _waveOutOpen@24

; Prepare both headers
    push 32                  ; size of WAVEHDR
    push wave_hdr1
    push dword [wave_out_handle]
    call _waveOutPrepareHeader@12

    push 32
    push wave_hdr2
    push dword [wave_out_handle]
    call _waveOutPrepareHeader@12

    mov dword [current_buffer], 0
    ret

_start:
    call print_message
    call initialize_audio
    call initialize

main_loop:
; Generate audio frame
    call CDECL(frame)

; Determine which buffer to use
    mov eax, [current_buffer]
    test eax, eax
    jz .use_buffer1

.use_buffer2:
; Wait for buffer2 to be done
.wait_buffer2:
    mov eax, [wave_hdr2 + WAVEHDR.dwFlags]
    test eax, WHDR_DONE
    jz .wait_buffer2

; Copy output to buffer2
    mov esi, CDECL(output)
    mov edi, audio_buffer2
    mov ecx, {output_channels}
    rep movsd

; Play buffer2
    push 32
    push wave_hdr2
    push dword [wave_out_handle]
    call _waveOutWrite@12

    mov dword [current_buffer], 0
    jmp .continue

.use_buffer1:
; Wait for buffer1 to be done
.wait_buffer1:
    mov eax, [wave_hdr1 + WAVEHDR.dwFlags]
    test eax, WHDR_DONE
    jz .wait_buffer1

; Copy output to buffer1
    mov esi, CDECL(output)
    mov edi, audio_buffer1
    mov ecx, {output_channels}
    rep movsd

; Play buffer1
    push 32
    push wave_hdr1
    push dword [wave_out_handle]
    call _waveOutWrite@12

    mov dword [current_buffer], 1

.continue:
; Small delay to prevent 100% CPU usage
    push 1
    call _Sleep@4

    jmp main_loop

cleanup:
; Unprepare headers
    push 32
    push wave_hdr1
    push dword [wave_out_handle]
    call _waveOutUnprepareHeader@12

    push 32
    push wave_hdr2
    push dword [wave_out_handle]
    call _waveOutUnprepareHeader@12

; Close wave output
    push dword [wave_out_handle]
    call _waveOutClose@4

    push 0
    call _ExitProcess@4

    %include "core/player.asm"
    %include "core/song/data.asm"
