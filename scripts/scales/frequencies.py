from typing import Dict

import numpy as np
from scales.constants import A4_FREQUENCY


class FrequencyTable:
    def __init__(self, scale: Dict[int, str], notes: int, a4_frequency: float = A4_FREQUENCY):
        self.scale = scale
        self.edo = len(scale)
        self.notes = notes
        self.a4_index = None
        self.a4_frequency = a4_frequency
        self.note_divisor = self.get_note_divisor()
        self.values = self.get_frequency_table()
        self.note_names = self.assign_note_names()

    def get_note_divisor(self) -> float:
        return 2 ** (1 / self.edo)

    def get_frequency_table(self):
        range_mean = np.sqrt(20 * 20000)
        self.a4_index = self.notes // 2 - round(self.edo * np.log2(range_mean / self.a4_frequency))
        frequency = self.a4_frequency * self.note_divisor ** (self.notes - self.a4_index - 1)

        values = []
        for i in range(self.notes):
            values.append(frequency)
            frequency /= self.note_divisor

        return list(reversed(values))

    def assign_note_names(self):
        a_index = [index for index, key in self.scale.items() if key == "A"][0]
        notes = []
        for i in range(self.notes):
            difference = i - self.a4_index
            key = self.scale[(difference + a_index) % self.edo]
            octave = 4 + (difference + a_index) // self.edo
            notes.append((key, octave))

        return notes
