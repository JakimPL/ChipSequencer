from dataclasses import dataclass
from typing import List

from structures.types import Byte


@dataclass
class Wavetable:
    wave: List[Byte]

    def __init__(self, wave: List[int]):
        self.wave = [Byte(w) for w in wave]

    def __len__(self):
        return len(self.wave)
