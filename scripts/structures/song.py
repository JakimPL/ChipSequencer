from dataclasses import dataclass
from typing import List

from structures.channel import Channel
from structures.dsp import DSP
from structures.envelope import Envelope
from structures.order import Order
from structures.oscillator import Oscillator
from structures.sequence import Sequence
from structures.types import Byte, Word
from structures.wavetable import Wavetable


@dataclass
class Song:
    bpm: Word
    normalizer: Byte
    output_channels: int
    song_length: int
    envelopes: List[Envelope]
    sequences: List[Sequence]
    orders: List[Order]
    oscillators: List[Oscillator]
    wavetables: List[Wavetable]
    dsps: List[DSP]
    channels: List[Channel]
