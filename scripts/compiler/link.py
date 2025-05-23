# This file is auto-generated by ConstantManager. Do not edit it manually.

from dataclasses import dataclass
from enum import Enum
from typing import Dict


class LinkType(Enum):
    CHANNEL = 0
    DSP = 1


class LinkTarget(Enum):
    SPLITTER_OUTPUT = "splitter_output"
    SPLITTER_DSP = "splitter_dsp"
    DIRECT_OUTPUT = "direct_output"
    DIRECT_DSP = "direct_dsp"
    ENVELOPE = "envelope"
    SEQUENCE = "sequence"
    ORDER = "order"
    OSCILLATOR = "oscillator"
    WAVETABLE = "wavetable"
    DSP = "dsp"
    CHANNEL = "channel"
    UNUSED = "unused"

    @staticmethod
    def from_integer(integer: int) -> "LinkTarget":
        dictionary: Dict[int, LinkTarget] = {
            0: LinkTarget.SPLITTER_OUTPUT,
            1: LinkTarget.SPLITTER_DSP,
            2: LinkTarget.DIRECT_OUTPUT,
            3: LinkTarget.DIRECT_DSP,
            4: LinkTarget.ENVELOPE,
            5: LinkTarget.SEQUENCE,
            6: LinkTarget.ORDER,
            7: LinkTarget.OSCILLATOR,
            8: LinkTarget.WAVETABLE,
            9: LinkTarget.DSP,
            10: LinkTarget.CHANNEL,
            11: LinkTarget.UNUSED,
        }

        return dictionary[integer]


@dataclass
class Link:
    type: LinkType
    id: int
    target: LinkTarget
    index: int
    offset: int
