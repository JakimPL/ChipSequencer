import struct
from dataclasses import dataclass, field
from typing import Union


@dataclass(frozen=True)
class Byte:
    value: int = field(init=False)

    def __init__(self, value: int):
        if isinstance(value, Byte):
            object.__setattr__(self, "value", value.value)
        else:
            object.__setattr__(self, "value", value)

    def __len__(self):
        return 1

    @property
    def type(self):
        return "db"

    @property
    def raw(self):
        return struct.pack("B", self.value)


@dataclass(frozen=True)
class Word:
    value: int = field(init=False)

    def __init__(self, value: int):
        if isinstance(value, Word):
            object.__setattr__(self, "value", value.value)
        else:
            object.__setattr__(self, "value", value)

    def __len__(self):
        return 2

    @property
    def type(self):
        return "dw"

    @property
    def raw(self):
        return struct.pack(">H", self.value)


@dataclass(frozen=True)
class Dword:
    value: int = field(init=False)

    def __init__(self, value: int):
        if isinstance(value, Dword):
            object.__setattr__(self, "value", value.value)
        else:
            object.__setattr__(self, "value", value)

    def __len__(self):
        return 4

    @property
    def type(self):
        return "dd"

    @property
    def raw(self):
        return struct.pack(">I", self.value)


@dataclass(frozen=True)
class Float:
    value: float = field(init=False)

    def __init__(self, value: float):
        if isinstance(value, Word):
            object.__setattr__(self, "value", value.value)
        else:
            object.__setattr__(self, "value", value)

    def __len__(self):
        return 4

    @property
    def type(self):
        return "dd"

    @property
    def raw(self):
        return struct.pack("f", self.value)


Data = Union[Byte, Word, Dword]


def convert(value: Data, fmt: str = "") -> str:
    if fmt == "x":
        hex_value = "{0:0{1}x}".format(value.value, 2 * len(value)).upper()
        return f"0x{hex_value}"
    elif fmt == "b":
        bin_value = "{0:0{1}b}".format(value.value, 8 * len(value))
        return f"0b{bin_value}"

    return str(value.value)
