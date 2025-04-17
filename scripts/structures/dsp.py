from dataclasses import dataclass
from typing import Union

from structures.enumeration import Effects
from structures.output import Output
from structures.types import Byte, Word


@dataclass
class DSP:
    output_flag: Byte
    output: Output
    effect_index: Effects

    def __init__(self, output_flag: Union[int, Byte], output: Output, effect_index: Effects):
        self.output_flag = Byte(output_flag)
        self.output = output
        self.effect_index = effect_index

    @property
    def buffer(self) -> int:
        raise NotImplementedError


class DSPGainer(DSP):
    volume: Word

    def __init__(self, output_flag: Union[int, Byte], output: Output, volume: Union[int, Word]):
        super().__init__(output_flag, output, Effects.EFFECT_GAINER)
        self.volume = Word(volume)

    def __len__(self) -> int:
        return 7

    @property
    def buffer(self) -> int:
        return 0


class DSPDistortion(DSP):
    level: Word

    def __init__(
        self,
        output_flag: Union[int, Byte],
        output: Output,
        level: Union[int, Word],
    ):
        super().__init__(output_flag, output, Effects.EFFECT_DISTORTION)
        self.level = Word(level)

    def __len__(self) -> int:
        return 7

    @property
    def buffer(self) -> int:
        return 0


class DSPFilter(DSP):
    frequency: Word

    def __init__(
        self, output_flag: Union[int, Byte], output: Output, frequency: Union[int, Word], mode: Union[int, Byte] = 0
    ):
        super().__init__(output_flag, output, Effects.EFFECT_FILTER)
        self.frequency = Word(frequency)
        self.mode = Byte(mode)

    def __len__(self) -> int:
        return 8

    @property
    def buffer(self) -> int:
        return 1
