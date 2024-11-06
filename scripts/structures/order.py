from dataclasses import dataclass
from typing import List, Union

from structures.types import Byte


@dataclass
class Order:
    patterns: List[Byte]

    def __init__(self, patterns: List[Union[int, Byte]]):
        self.sequences = [Byte(pattern) for pattern in patterns]

    def __len__(self):
        return len(self.sequences)
