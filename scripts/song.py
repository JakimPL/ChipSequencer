from pathlib import Path

from pyconf import ASM_SONG_DATA_FILE, ASM_SONG_HEADER_FILE
from structures.channel import Channel
from structures.creator import Creator
from structures.dsp import DSPDelay, DSPFilter, DSPGainer
from structures.envelope import Envelope
from structures.order import Order
from structures.oscillator import OscillatorSaw, OscillatorSine, OscillatorSquare, OscillatorWavetable
from structures.output import Output
from structures.sequence import Sequence
from structures.song import Song
from structures.types import Word
from structures.wavetable import Wavetable


def save_asm(asm: str, file: Path):
    with open(file, "w") as file:
        file.write(asm)


def main():
    bpm = Word(300)
    normalizer = Word(6)
    output_channels = 2
    song_length = 186253

    envelopes = [
        Envelope(
            base_volume=0x3F00,
            sustain_level=0x1F00,
            bias=0x4000,
            attack=25,
            decay=250,
            hold=250,
            release=0,
        ),
        Envelope(
            base_volume=0x7FFF,
            sustain_level=0x5FFF,
            bias=0x0000,
            attack=125,
            decay=250,
            hold=250,
            release=250,
        ),
        Envelope(
            base_volume=0x2FFF,
            sustain_level=0x1FFF,
            bias=0x0000,
            attack=1500,
            decay=1500,
            hold=1250,
            release=1250,
        ),
        Envelope(
            base_volume=0x0180,
            sustain_level=0x0180,
            bias=0x0000,
            attack=500,
            decay=1,
            hold=1,
            release=0,
        ),
        Envelope(
            base_volume=0x5FFF,
            sustain_level=0x5FFF,
            bias=0x0014,
            attack=1,
            decay=1,
            hold=1,
            release=0,
        ),
    ]

    sequences = [
        Sequence(
            [
                (0x3C, 3),
                (0x35, 3),
                (0x33, 3),
                (0x32, 3),
                (0x30, 1),
                (-0x01, 1),
                (0x30, 1),
                (0x2E, 1),
            ]
        ),
        Sequence(
            [
                (0x2D, 2),
                (0x37, 1),
                (0x35, 13),
            ]
        ),
        Sequence(
            [
                (0x18, 1),
                (-0x01, 1),
            ]
        ),
        Sequence(
            [
                (0x11, 1),
                (-0x01, 1),
            ]
        ),
        Sequence(
            [
                (0x30, 16),
                (0x30, 16),
            ]
        ),
        Sequence(
            [
                (0x33, 16),
                (0x33, 16),
            ]
        ),
        Sequence(
            [
                (0x37, 16),
                (0x35, 16),
            ]
        ),
        Sequence(
            [
                (0x3A, 16),
                (0x39, 16),
            ]
        ),
    ]

    orders = [
        Order([0, 1, 0, 1]),
        Order([2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3]),
        Order([4]),
        Order([5]),
        Order([6]),
        Order([7]),
    ]

    oscillator_square = OscillatorSquare(duty_cycle=0x20)
    oscillator_saw = OscillatorSaw()
    oscillator_sine = OscillatorSine()
    oscillator_wavetable = OscillatorWavetable(wavetable_index=1)

    oscillators = [
        oscillator_square,
        oscillator_saw,
        oscillator_sine,
        oscillator_wavetable,
    ]

    wavetables = [
        Wavetable(
            [
                0x00,
                0x1F,
                0x3F,
                0x5F,
                0x7F,
                0x9F,
                0xBF,
                0xDF,
                0xFF,
                0xDF,
                0xBF,
                0x9F,
                0x7F,
                0x5F,
                0x3F,
                0x1F,
            ]
        ),
        Wavetable([0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70]),
    ]

    effect_delay = DSPDelay(
        output=Output(),
        output_flag=0,
        dry=0x7FFF,
        wet=0x6FFF,
        feedback=0x5FFF,
        delay_time=0x0100,
    )

    effect_gainer = DSPGainer(output=Output(), output_flag=0, volume=0x9FFF)

    effect_filter = DSPFilter(output=Output(), output_flag=0, frequency=0x0150)

    dsps = [effect_delay, effect_gainer, effect_filter]

    osc1 = Channel(
        envelope_index=1,
        order_index=0,
        oscillator_index=3,
        pitch=0x02000000,
        output=Output(effect_filter),
        output_flag=0,
    )

    osc2 = Channel(
        envelope_index=1,
        order_index=1,
        oscillator_index=1,
        pitch=0x02000000,
        output=Output(effect_filter),
        output_flag=0,
    )

    osc3 = Channel(
        envelope_index=2,
        order_index=2,
        oscillator_index=0,
        pitch=0x02000000,
        output=Output(effect_gainer),
        output_flag=0,
    )

    osc4 = Channel(
        envelope_index=2,
        order_index=3,
        oscillator_index=0,
        pitch=0x02000000,
        output=Output(effect_gainer),
        output_flag=0,
    )

    osc5 = Channel(
        envelope_index=2,
        order_index=4,
        oscillator_index=0,
        pitch=0x02000000,
        output=Output(effect_gainer),
        output_flag=0,
    )

    osc6 = Channel(
        envelope_index=2,
        order_index=5,
        oscillator_index=0,
        pitch=0x02000000,
        output=Output(effect_gainer),
        output_flag=0,
    )

    mod1 = Channel(
        envelope_index=4,
        order_index=-1,
        oscillator_index=1,
        pitch=0x5000,
        output=Output(effect_filter.frequency),
        output_flag=0b01010111,
    )

    mod2 = Channel(
        envelope_index=3,
        order_index=-1,
        oscillator_index=2,
        pitch=0x00080000,
        output=Output(osc1.pitch),
        output_flag=0b01110110,
    )

    mod3 = Channel(
        envelope_index=0,
        order_index=-1,
        oscillator_index=2,
        pitch=0x00014800,
        output=Output(oscillator_square.duty_cycle),
        output_flag=0b01101000,
    )

    channels = [osc1, osc2, osc3, osc4, osc5, osc6, mod1, mod2, mod3]

    song = Song(
        bpm=bpm,
        normalizer=normalizer,
        output_channels=output_channels,
        song_length=song_length,
        envelopes=envelopes,
        sequences=sequences,
        orders=orders,
        oscillators=oscillators,
        wavetables=wavetables,
        dsps=dsps,
        channels=channels,
    )

    creator = Creator(song)
    asm_song_header, asm_song_data = creator()
    save_asm(asm_song_header, ASM_SONG_HEADER_FILE)
    save_asm(asm_song_data, ASM_SONG_DATA_FILE)


if __name__ == "__main__":
    main()
