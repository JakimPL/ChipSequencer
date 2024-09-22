    section .data
bpm:
    dw 240

envelopes:
; Envelope 1
    dw 0x7FFF                ; base_volume
    dw 0x2FFF                ; sustain_level
    dw 125                   ; attack
    dw 25                    ; decay
    dw 500                   ; hold
    dw 1000                  ; release

; Envelope 2
    dw 0x2FFF                ; base_volume
    dw 0x2FFF                ; sustain_level
    dw 250                   ; attack
    dw 250                   ; decay
    dw 250                   ; hold
    dw 250                   ; release

instruments:
; Instrument 1
    db 1                     ; envelope_index
    db 0                     ; envelope_mode
    dd 0                     ; envelope_timer
    db 1                     ; sequence_index
    dd 0                     ; sequence_remaining_ticks
    db 0                     ; sequence_current_note
    db 0                     ; oscillator_index
    dd 0                     ; oscillator_timer
    db 0                     ; oscillator_pitch

sequences:
; Sequence 1
    db 6                     ; note_count
; notes
    db 0x30, 1
    db 0x32, 1
    db 0x34, 2
    db 0x37, 1
    db 0x33, 1
    db 0x32, 2
; Sequence 2
    db 8                     ; note_count
; notes
    db 0x30, 3
    db 0x33, 1
    db 0x30, 2
    db 0x33, 1
    db 0x30, 1
    db 0x33, 1
    db 0x36, 1
    db 0x2F, 6
