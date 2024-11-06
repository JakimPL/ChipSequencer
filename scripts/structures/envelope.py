from dataclasses import dataclass
from typing import Union

from structures.types import Word


@dataclass
class Envelope:
    base_volume: Word
    sustain_level: Word
    bias: Word
    attack: Word
    decay: Word
    hold: Word
    release: Word

    def __init__(
        self,
        base_volume: Union[int, Word],
        sustain_level: Union[int, Word],
        bias: Union[int, Word],
        attack: Union[int, Word],
        decay: Union[int, Word],
        hold: Union[int, Word],
        release: Union[int, Word],
    ):
        self.base_volume = Word(base_volume)
        self.sustain_level = Word(sustain_level)
        self.bias = Word(bias)
        self.attack = Word(attack)
        self.decay = Word(decay)
        self.hold = Word(hold)
        self.release = Word(release)
