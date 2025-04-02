from enum import Enum


class Enumeration(Enum):
    def __str__(self):
        return super().__str__().split(".")[1]


class Generators(Enumeration):
    GENERATOR_SQUARE = 0
    GENERATOR_SAW = 1
    GENERATOR_SINE = 2
    GENERATOR_WAVETABLE = 3


class Effects(Enumeration):
    EFFECT_GAINER = 0
    EFFECT_DELAY = 1
    EFFECT_FILTER = 2
