from structures.channel import Channel
from structures.dsp import DSP
from structures.envelope import Envelope
from structures.order import Order
from structures.oscillator import Oscillator
from structures.sequence import Sequence
from structures.wavetable import Wavetable

CONSTANTS_ASM_FILE = "core/common/const.asm"
CONSTANTS_CPP_FILE = "tracker/constants.hpp"
TARGET_HPP_FILE = "tracker/song/links/target.hpp"
TYPE_HPP_FILE = "tracker/song/links/type.hpp"
OFFSETS_HPP_FILE = "tracker/maps/offsets.hpp"
CONFIG_YAML_FILE = "config.yaml"
CONFIG_ASM_FILE = "core/common/config.asm"
ASM_SONG_DATA_FILE = "core/song/data.asm"
ASM_SONG_HEADER_FILE = "core/song/header.asm"
LINK_PY_FILE = "scripts/compiler/link.py"

PADDING = 30

NOTES = ["C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-"]
CLASSES = [Envelope, Sequence, Order, Oscillator, Wavetable, DSP, Channel]
