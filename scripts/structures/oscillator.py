from dataclasses import dataclass
from typing import Union

from structures.enumeration import Generators
from structures.types import Byte


@dataclass
class Oscillator:
    generator_index: Generators

    def __init__(self, generator_index: Generators):
        self.generator_index = generator_index


class OscillatorSquare(Oscillator):
    duty_cycle: Byte

    def __init__(self, duty_cycle: int):
        super().__init__(Generators.GENERATOR_SQUARE)
        self.duty_cycle = Byte(duty_cycle)

    def __len__(self) -> int:
        return 2


class OscillatorSaw(Oscillator):
    def __init__(self, reverse: bool = False):
        super().__init__(Generators.GENERATOR_SAW)
        self.reverse = reverse

    def __len__(self) -> int:
        return 2


class OscillatorSine(Oscillator):
    def __init__(self):
        super().__init__(Generators.GENERATOR_SINE)

    def __len__(self) -> int:
        return 1


class OscillatorWavetable(Oscillator):
    wavetable_index: Byte

    def __init__(self, wavetable_index: Union[int, Byte], interpolate: bool = False):
        super().__init__(Generators.GENERATOR_WAVETABLE)
        self.wavetable_index = Byte(wavetable_index)
        self.interpolate = interpolate

    def __len__(self) -> int:
        return 3


class OscillatorNoise(Oscillator):
    def __init__(self):
        super().__init__(Generators.GENERATOR_NOISE)

    def __len__(self) -> int:
        return 1
