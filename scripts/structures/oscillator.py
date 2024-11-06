from dataclasses import dataclass
from typing import Union

from structures.enumeration import Oscillators
from structures.types import Byte


@dataclass
class Oscillator:
    generator_index: Oscillators

    def __init__(self, generator_index: Oscillators):
        self.generator_index = generator_index


class OscillatorSquare(Oscillator):
    duty_cycle: Byte

    def __init__(self, duty_cycle: int):
        super().__init__(Oscillators.OSCILLATOR_SQUARE)
        self.duty_cycle = Byte(duty_cycle)

    def __len__(self) -> int:
        return 2


class OscillatorSaw(Oscillator):
    def __init__(self):
        super().__init__(Oscillators.OSCILLATOR_SAW)

    def __len__(self) -> int:
        return 1


class OscillatorSine(Oscillator):
    def __init__(self):
        super().__init__(Oscillators.OSCILLATOR_SINE)

    def __len__(self) -> int:
        return 1


class OscillatorWavetable(Oscillator):
    wavetable_index: Byte

    def __init__(self, wavetable_index: Union[int, Byte]):
        super().__init__(Oscillators.OSCILLATOR_WAVETABLE)
        self.wavetable_index = Byte(wavetable_index)

    def __len__(self) -> int:
        return 2
