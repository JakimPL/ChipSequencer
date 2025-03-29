from structures.channel import Channel
from structures.dsp import DSP
from structures.envelope import Envelope
from structures.order import Order
from structures.oscillator import Oscillator
from structures.sequence import Sequence
from structures.wavetable import Wavetable

CONSTANTS_ASM_FILE = "src/common/const.asm"
CONSTANTS_CPP_FILE = "tracker/constants.hpp"
TARGET_HPP_FILE = "tracker/song/target.hpp"
X16_TO_X32_FILE = "tracker/maps/x16_to_x32.json"
X32_TO_X16_FILE = "tracker/maps/x32_to_x16.json"
CONFIG_YAML_FILE = "config.yaml"
CONFIG_ASM_FILE = "src/common/config.asm"
ASM_SONG_DATA_FILE = "src/song/data.asm"
ASM_SONG_HEADER_FILE = "src/song/header.asm"

PADDING = 30

NOTES = ["C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-"]
CLASSES = [Envelope, Sequence, Order, Oscillator, Wavetable, DSP, Channel]
