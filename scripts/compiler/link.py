from dataclasses import dataclass
from enum import Enum
from typing import Dict


class LinkType(Enum):
    CHANNEL = 0
    DSP = 1


class LinkTarget(Enum):
    OUTPUT = "output"
    DSP_INPUT = "dsp_input"
    ENVELOPE = "envelope"
    SEQUENCE = "sequence"
    ORDER = "order"
    OSCILLATOR = "oscillator"
    WAVETABLE = "wavetable"
    DSP = "dsp"
    CHANNEL = "channel"

    @staticmethod
    def from_integer(integer: int) -> "LinkTarget":
        dictionary: Dict[int, LinkTarget] = {
            0: LinkTarget.OUTPUT,
            1: LinkTarget.DSP_INPUT,
            2: LinkTarget.ENVELOPE,
            3: LinkTarget.SEQUENCE,
            4: LinkTarget.ORDER,
            5: LinkTarget.OSCILLATOR,
            6: LinkTarget.WAVETABLE,
            7: LinkTarget.DSP,
            8: LinkTarget.CHANNEL,
        }

        return dictionary[integer]


@dataclass
class Link:
    type: LinkType
    id: int
    target: LinkTarget
    index: int
    offset: int
