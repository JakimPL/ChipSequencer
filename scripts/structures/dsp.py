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


class DSPDelay(DSP):
    dry: Word
    wet: Word
    feedback: Word
    delay_time: Word

    def __init__(
        self,
        output_flag: Union[int, Byte],
        output: Output,
        dry: Union[int, Word],
        wet: Union[int, Word],
        feedback: Union[int, Word],
        delay_time: Union[int, Word],
    ):
        super().__init__(output_flag, output, Effects.EFFECT_DELAY)
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

    def __init__(self, output_flag: Union[int, Byte], output: Output, volume: Union[int, Word]):
        super().__init__(output_flag, output, Effects.EFFECT_GAINER)
        self.volume = Word(volume)

    def __len__(self) -> int:
        return 6

    @property
    def buffer(self) -> int:
        return 0


class DSPFilter(DSP):
    frequency: Word

    def __init__(self, output_flag: Union[int, Byte], output: Output, frequency: Union[int, Word]):
        super().__init__(output_flag, output, Effects.EFFECT_FILTER)
        self.frequency = Word(frequency)

    def __len__(self) -> int:
        return 6

    @property
    def buffer(self) -> int:
        return 0
