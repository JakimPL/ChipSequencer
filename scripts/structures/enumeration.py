from enum import Enum


class Enumeration(Enum):
    def __str__(self):
        return super().__str__().split(".")[1]


class Oscillators(Enumeration):
    OSCILLATOR_SQUARE = 0
    OSCILLATOR_SAW = 1
    OSCILLATOR_SINE = 2
    OSCILLATOR_WAVETABLE = 3


class Effects(Enumeration):
    EFFECT_GAINER = 0
    EFFECT_DELAY = 1
    EFFECT_FILTER = 2
