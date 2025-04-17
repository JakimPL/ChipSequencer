import re
from typing import Any, Dict, List, Tuple, Union

from pyconf import CLASSES, NOTES, PADDING
from structures.dsp import DSP
from structures.enumeration import Effects, Generators
from structures.output import Output
from structures.song import Song
from structures.types import Byte, Data, Word, convert


class Creator:
    def __init__(self, song: Song):
        self.song: Song = song
        self.lines = []
        self.references = self.map_objects()

    def __call__(self) -> Tuple[str, str]:
        self.lines.clear()
        self.define_header()
        self.define_tuning()
        self.define_wavetables()
        self.define_buffers()
        self.counts()
        header = "\n".join(self.lines)

        self.lines.clear()
        self.song_header()
        self.envelopes()
        self.sequences()
        self.orders()
        self.oscillators()
        self.wavetables()
        self.dsps()
        self.channels()
        self.buffer_offsets()
        data = "\n".join(self.lines)

        return header, data

    @staticmethod
    def get_class_name(klass):
        for base_class in CLASSES:
            if issubclass(klass, base_class):
                return base_class.__name__.lower()
        raise ValueError(f"unmatched class {klass}")

    @staticmethod
    def camel_to_pascal_case(name):
        first_pass = re.sub(r"(.)([A-Z][a-z]+)", r"\1_\2", name)
        return re.sub(r"([a-z0-9])([A-Z])", r"\1_\2", first_pass)

    def map_objects(self) -> Dict[int, Tuple[type, int, str]]:
        objects = [
            self.song.envelopes,
            self.song.sequences,
            self.song.orders,
            self.song.oscillators,
            self.song.wavetables,
            self.song.dsps,
            self.song.channels,
        ]
        obj_map = {}
        for collection in objects:
            for i, obj in enumerate(collection):
                obj_map.update({id(value): (type(obj), i, key) for key, value in obj.__dict__.items()})

        return obj_map

    def add(self, string: str):
        self.lines.append(string)

    def define(self, key: str, value: Any):
        self.add(f"    %define {key.upper()} {value}")

    def label(self, label: str):
        self.add(f"{label}:")

    @staticmethod
    def note(value: int) -> str:
        if value % 256 == 255:
            return "note off"
        name = NOTES[value % 12]
        octave = value // 12
        return f"{name}{octave}"

    @staticmethod
    def add_comment(line: str, comment: str = "") -> str:
        if comment:
            line += " " * max(1, PADDING - len(line) - 1)
            line += f"; {comment}"

        return line

    def value(
        self,
        value: Union[Data, List[Data], Tuple[Data, ...]],
        comment: str = "",
        fmt: str = "",
    ):
        if isinstance(value, (list, tuple)):
            assert len(value)
            string = ", ".join(convert(v, fmt) for v in value)
            value_type = value[0].type
        else:
            string = convert(value, fmt)
            value_type = value.type

        line = f"    {value_type} {string}"
        line = self.add_comment(line, comment)
        self.add(line)

    def reference(self, klass: type, reference: str, comment: str = ""):
        value_type = klass(0).type
        line = f"    {value_type} {reference}"
        line = self.add_comment(line, comment)
        self.add(line)

    def output(self, output: Output):
        if output.reference is None:
            return self.reference(Word, "output", "output")
        elif isinstance(output.reference, DSP):
            offset = 4 * self.song.dsps.index(output.reference)
            return self.reference(Word, f"dsp_input + {offset}", "output")
        else:
            klass, i, variable = self.references[id(output.reference)]
            class_name = self.get_class_name(klass)
            label = f"{class_name}s.{class_name}_{i}"

            class_name = self.camel_to_pascal_case(klass.__name__)
            variable_name = f"{class_name}_{variable}".upper()
            self.reference(Word, f"{label} + {variable_name}", "output")

        self.comment()

    def comment(self, comment: str = ""):
        if comment:
            self.add(f"; {comment}")
        else:
            self.add("")

    def define_header(self):
        self.define("CHANNELS", len(self.song.channels))
        self.define("DSPS", len(self.song.dsps))
        self.define("WAVETABLES", len(self.song.wavetables))
        self.comment()

        self.define("OUTPUT_CHANNELS", self.song.output_channels)
        self.define("SONG_LENGTH", self.song.song_length)
        self.define("SAMPLE_RATE", self.song.sample_rate)
        self.comment()

    def define_tuning(self):
        self.comment("Tuning")
        self.define("TUNING_FREQUENCY", self.song.reference_frequency)
        self.define("TUNING_NOTE_DIVISOR", self.song.note_divisor)
        self.comment()

    def define_wavetables(self):
        self.comment("Wavetables")
        wavetable_sizes = [len(wavetable) for wavetable in self.song.wavetables]
        total_size = 0
        for i, size in enumerate(wavetable_sizes):
            self.define(f"WAVETABLE_{i}_SIZE", size)
            total_size += size

        self.define("WAVETABLE_SIZE", total_size)
        self.comment()

    def define_buffers(self):
        self.comment("Buffers")
        buffer_sizes = [dsp.buffer for dsp in self.song.dsps]
        total_size = 0
        for i, size in enumerate(buffer_sizes):
            self.define(f"DSP_{i}_BUFFER_SIZE", size)
            total_size += size

        self.define("DSP_BUFFER_SIZE", total_size)
        self.comment()

    def counts(self):
        self.add("    extern num_channels")
        self.add("    extern num_dsps")
        self.add("    extern unit")
        self.comment()

    def song_header(self):
        self.add("    SEGMENT_DATA")
        self.label("bpm")
        self.value(self.song.bpm)
        self.label("unit")
        self.value(self.song.unit)
        self.label("normalizer")
        self.value(self.song.normalizer)
        self.comment()
        self.label("num_channels")
        self.reference(Byte, "CHANNELS")
        self.label("num_dsps")
        self.reference(Byte, "DSPS")
        self.comment()

    def envelopes(self):
        self.label("envelopes")
        for i, envelope in enumerate(self.song.envelopes):
            self.label(f".envelope_{i}")
            self.value(envelope.base_volume, "base_volume", fmt="x")
            self.value(envelope.sustain_level, "sustain_level", fmt="x")
            self.value(envelope.bias, "bias", fmt="x")
            self.value(envelope.attack, "attack")
            self.value(envelope.decay, "decay")
            self.value(envelope.hold, "hold")
            self.value(envelope.release, "release")

        self.comment()

    def sequences(self):
        self.label("sequences")
        for i, sequence in enumerate(self.song.sequences):
            self.label(f".sequence_{i}")
            self.value(Byte(len(sequence)), "data_size = 2 * note_count")
            self.comment("notes")
            for note, length in sequence.notes:
                self.value((note, length), self.note(note.value))

        self.comment()

    def orders(self):
        self.label("orders")
        for i, order in enumerate(self.song.orders):
            self.label(f".order_{i}")
            self.value(Byte(len(order)), "order_length")
            self.comment("sequences")
            self.value(order.sequences)

        self.comment()

    def oscillators(self):
        self.label("oscillators")
        for i, oscillator in enumerate(self.song.oscillators):
            self.label(f".oscillator_{i}")
            self.value(Byte(len(oscillator)), "oscillator_size")
            self.reference(Byte, str(oscillator.generator_index), "generator_index")
            match oscillator.generator_index:
                case Generators.GENERATOR_SQUARE:
                    self.value(oscillator.duty_cycle, "duty_cycle", fmt="x")
                case Generators.GENERATOR_WAVETABLE:
                    self.value(oscillator.wavetable_index, "wavetable_index")

        self.comment()

    def wavetables(self):
        self.label("wavetables")
        for i, wavetable in enumerate(self.song.wavetables):
            self.label(f".wavetable_{i}")
            self.reference(Byte, f"WAVETABLE_{i}_SIZE", "wavetable_size")
            self.value(wavetable.wave, fmt="x")

        self.comment()

    def dsps(self):
        self.label("dsps")
        for i, dsp in enumerate(self.song.dsps):
            self.label(f".dsp_{i}")
            self.value(Byte(len(dsp)), "dsp_size")
            self.reference(Byte, str(dsp.effect_index), "effect_index")
            self.value(dsp.output_flag, "output_flag", fmt="b")
            self.output(dsp.output)
            match dsp.effect_index:
                case Effects.EFFECT_GAINER:
                    self.value(dsp.volume, "volume", fmt="x")
                case Effects.EFFECT_DISTORTION:
                    self.value(dsp.level, "level", fmt="x")
                case Effects.EFFECT_FILTER:
                    self.value(dsp.frequency, "frequency")

        self.comment()

    def channels(self):
        self.label("channels")
        for i, channel in enumerate(self.song.channels):
            self.label(f".channel_{i}")
            self.value(channel.envelope_index, "envelope_index")
            self.value(channel.order_index, "order_index")
            self.value(channel.oscillator_index, "oscillator_index")
            self.value(channel.pitch, "pitch", fmt="x")
            self.value(channel.output_flag, "output_flag", fmt="b")
            self.output(channel.output)

    def buffer_offsets(self):
        self.label("buffer_offsets")
        offset = 0
        for i, dsp in enumerate(self.song.dsps):
            self.value(Word(offset), f"dsp_{i}_buffer_offset")
            offset += dsp.buffer

        self.comment()
