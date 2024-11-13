from structures.channel import Channel
from structures.dsp import DSP
from structures.envelope import Envelope
from structures.order import Order
from structures.oscillator import Oscillator
from structures.sequence import Sequence
from structures.wavetable import Wavetable

CONFIG_YAML_FILE = "config.yaml"
CONFIG_ASM_FILE = "src/config.asm"
ASM_SONG_DATA_FILE = "src/song/data.asm"
ASM_SONG_HEADER_FILE = "src/song/header.asm"

PADDING = 30

NOTES = ["C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-"]
CLASSES = [Envelope, Sequence, Order, Oscillator, Wavetable, DSP, Channel]
