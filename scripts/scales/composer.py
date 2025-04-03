from fractions import Fraction
from typing import List, Optional

import numpy as np
from scales.constants import INTERVALS, NAMES, SYMBOLS


class ScaleComposer:

    def __init__(
        self,
        edo: int,
        names: Optional[List[str]] = None,
        intervals: Optional[List[Fraction]] = None,
        symbols: Optional[List[str]] = None,
    ):
        assert edo >= 7, "at least 7 notes are needed"
        self.names = names or NAMES
        self.intervals = intervals or INTERVALS
        self.symbols = symbols or SYMBOLS

        self.n = edo
        self.centers, self.limits = self.get_note_centers_and_limits()

    def get_note_centers_and_limits(self):
        centers = []
        limits = []
        for i, interval in enumerate(self.intervals[:-1]):
            x = np.log2(float(interval))
            y = np.log2(float(self.intervals[i + 1]))
            centers.append(x)
            limits.append(0.5 * (x + y))

        return centers, limits

    def render(self, name: str, offset: int):
        symbol = self.symbols[offset // abs(offset) if offset != 0 else 0]
        return f"{name}{symbol * abs(offset)}"

    def compose(self):
        scale = {i: None for i in range(self.n)}
        start = self.limits[-1] - 1
        for note, (center, end) in enumerate(zip(self.centers, self.limits)):
            x = start * self.n
            y = end * self.n
            center = round(center * self.n)
            indices = range(int(np.ceil(x)), int(np.floor(y) + 1))
            index = indices.index(center)

            for j, i in enumerate(indices):
                scale[i % self.n] = self.render(self.names[note], j - index)
            start = end

        return scale
