from dataclasses import dataclass
from typing import Union

from structures.output import Output
from structures.types import Byte, Dword, Word


@dataclass
class Channel:
    envelope_index: Byte
    order_index: Byte
    oscillator_index: Byte
    pitch: Dword
    output: Output
    output_flag: Byte

    def __init__(
        self,
        envelope_index: Union[int, Byte],
        order_index: Union[int, Byte],
        oscillator_index: Union[int, Byte],
        pitch: Union[int, Dword],
        output: Output,
        output_flag: Union[int, Byte],
    ):
        self.envelope_index = Byte(envelope_index)
        self.order_index = Byte(order_index)
        self.oscillator_index = Byte(oscillator_index)
        self.pitch = Dword(pitch)
        self.output = output
        self.output_flag = Byte(output_flag)
