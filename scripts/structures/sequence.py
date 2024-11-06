from dataclasses import dataclass
from typing import List, Tuple, Union

from structures.types import Byte


@dataclass
class Sequence:
    notes: List[Tuple[Byte, Byte]]

    def __init__(self, notes: List[Tuple[Union[int, Byte], Union[int, Byte]]]):
        self.notes = [(Byte(note), Byte(length)) for note, length in notes]

    def __len__(self):
        return 2 * len(self.notes)
