from dataclasses import dataclass
from typing import Union

from structures.enumeration import Effects
from structures.output import Output
from structures.types import Byte, Dword, Word


@dataclass
class DSP:
    effect_index: Effects
    output_flag: Byte
    output: Output
    splitter: Dword

    def __init__(
        self, output_flag: Union[int, Byte], output: Output, effect_index: Effects, splitter: Union[int, Dword]
    ):
        self.effect_index = effect_index
        self.output_flag = Byte(output_flag)
        self.output = output
        self.splitter = Dword(splitter)

    @property
    def buffer(self) -> int:
        raise NotImplementedError


class DSPGainer(DSP):
    volume: Word

    def __init__(
        self,
        output_flag: Union[int, Byte],
        output: Output,
        splitter: Union[int, Dword],
        volume: Union[int, Word],
    ):
        super().__init__(output_flag, output, Effects.EFFECT_GAINER, splitter)
        self.volume = Word(volume)

    def __len__(self) -> int:
        return 11

    @property
    def buffer(self) -> int:
        return 0


class DSPDistortion(DSP):
    level: Word

    def __init__(
        self,
        output_flag: Union[int, Byte],
        output: Output,
        splitter: Union[int, Dword],
        level: Union[int, Word],
    ):
        super().__init__(output_flag, output, Effects.EFFECT_DISTORTION, splitter)
        self.level = Word(level)

    def __len__(self) -> int:
        return 11

    @property
    def buffer(self) -> int:
        return 0


class DSPFilter(DSP):
    frequency: Word

    def __init__(
        self,
        output_flag: Union[int, Byte],
        output: Output,
        splitter: Union[int, Dword],
        frequency: Union[int, Word],
        mode: Union[int, Byte] = 0,
    ):
        super().__init__(output_flag, output, Effects.EFFECT_FILTER, splitter)
        self.frequency = Word(frequency)
        self.mode = Byte(mode)

    def __len__(self) -> int:
        return 12

    @property
    def buffer(self) -> int:
        return 1


class DSPDelay(DSP):
    dry: Word
    wet: Word
    feedback: Word
    delay_time: Word

    def __init__(
        self,
        output_flag: Union[int, Byte],
        output: Output,
        splitter: Union[int, Dword],
        dry: Union[int, Byte],
        wet: Union[int, Byte],
        feedback: Union[int, Byte],
        delay_time: Union[int, Word],
    ):
        super().__init__(output_flag, output, Effects.EFFECT_DELAY, splitter)
        self.dry = Byte(dry)
        self.wet = Byte(wet)
        self.feedback = Byte(feedback)
        self.delay_time = Word(delay_time)

    def __len__(self) -> int:
        return 14

    @property
    def buffer(self) -> int:
        return self.delay_time.value
