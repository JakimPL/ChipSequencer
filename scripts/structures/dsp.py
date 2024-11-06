from dataclasses import dataclass
from typing import Union

from structures.enumeration import Effects
from structures.output import Output
from structures.types import Byte, Word


@dataclass
class DSP:
    output: Output
    output_flag: Byte
    effect_index: Effects

    def __init__(self, output: Output, output_flag: Union[int, Byte], effect_index: Effects):
        self.output = output
        self.output_flag = Byte(output_flag)
        self.effect_index = effect_index

    @property
    def buffer(self) -> int:
        raise NotImplementedError


class DSPDelay(DSP):
    dry: Word
    wet: Word
    feedback: Word
    delay_time: Word

    def __init__(
        self,
        output: Output,
        output_flag: Union[int, Byte],
        dry: Union[int, Word],
        wet: Union[int, Word],
        feedback: Union[int, Word],
        delay_time: Union[int, Word],
    ):
        super().__init__(output, output_flag, Effects.EFFECT_DELAY)
        self.dry = Word(dry)
        self.wet = Word(wet)
        self.feedback = Word(feedback)
        self.delay_time = Word(delay_time)

    def __len__(self) -> int:
        return 12

    @property
    def buffer(self) -> int:
        return self.delay_time.value


class DSPGainer(DSP):
    volume: Word

    def __init__(self, output: Output, output_flag: Union[int, Byte], volume: Union[int, Word]):
        super().__init__(output, output_flag, Effects.EFFECT_GAINER)
        self.volume = Word(volume)

    def __len__(self) -> int:
        return 6

    @property
    def buffer(self) -> int:
        return 0


class DSPFilter(DSP):
    frequency: Word

    def __init__(self, output: Output, output_flag: Union[int, Byte], frequency: Union[int, Word]):
        super().__init__(output, output_flag, Effects.EFFECT_FILTER)
        self.frequency = Word(frequency)

    def __len__(self) -> int:
        return 6

    @property
    def buffer(self) -> int:
        return 0
