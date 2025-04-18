import os
import re
from collections import defaultdict
from typing import DefaultDict, Dict, List, Tuple, Union

from pyconf import CONSTANTS_ASM_FILE, CONSTANTS_CPP_FILE, OFFSETS_HPP_FILE, TARGET_HPP_FILE
from utils import load_text, save_text


class ConstantManager:
    def __init__(
        self,
        constants_asm_file: Union[str, os.PathLike] = CONSTANTS_ASM_FILE,
        constants_cpp_file: Union[str, os.PathLike] = CONSTANTS_CPP_FILE,
        target_hpp_file: Union[str, os.PathLike] = TARGET_HPP_FILE,
        offsets_hpp_file: Union[str, os.PathLike] = OFFSETS_HPP_FILE,
    ):
        self.constants_asm_file = constants_asm_file
        self.constants_cpp_file = constants_cpp_file
        self.target_hpp_file = target_hpp_file
        self.offsets_hpp_file = offsets_hpp_file

    def __call__(self):
        asm_constants = load_text(self.constants_asm_file)
        cpp_constants = self.convert_asm_to_cpp(asm_constants)
        save_text(cpp_constants, self.constants_cpp_file)

        target = load_text(self.target_hpp_file)
        groups = self.extract_groups_from_targets(target)
        x16, x32 = self.parse_cpp_constants(cpp_constants)
        x16_to_x32 = self.get_offset_map(x16, x32, groups)
        x32_to_x16 = self.get_offset_map(x32, x16, groups)
        offsets = self.compose_offset_hpp_file(x16_to_x32, x32_to_x16)
        save_text(offsets, self.offsets_hpp_file)

    @staticmethod
    def extract_groups_from_targets(target: str) -> List[str]:
        return [
            key.strip().lower().replace(",", "")
            for key in target[target.find("{") + 1 : target.find("}")].splitlines()
            if key
        ]

    @staticmethod
    def convert_asm_to_cpp(data: str) -> str:
        output = data.replace(";", "//").replace("%", "#")
        output = re.sub(r"^[ \t]+", "", output, flags=re.MULTILINE)
        return "#define ELF\n\n" + output

    @staticmethod
    def parse(string: str) -> Union[float, int]:
        if string.startswith("0x"):
            return int(string, 16)
        if string.startswith("0b"):
            return int(string, 2)
        if "." in string:
            return float(string)

        return int(string)

    @staticmethod
    def parse_cpp_constants(cpp_constants: str) -> Tuple[DefaultDict[str, Dict[str, int]], ...]:
        lines = [line for line in cpp_constants.splitlines() if line.startswith("#") and line != "#define ELF"]
        x16 = defaultdict(dict)
        x32 = defaultdict(dict)
        elf = None
        for line in lines:
            if line == "#ifdef ELF":
                elf = True
                continue
            if line == "#else":
                elf = not elf
                continue
            if line == "#endif":
                elf = None
                continue

            _, key, value = line.split(" ")
            group = key.split("_")[0].lower()
            if '"' in value:
                continue

            value = ConstantManager.parse(value)
            if elf is not True:
                x16[group][key] = value
            if elf is not False:
                x32[group][key] = value

        return x16, x32

    @staticmethod
    def get_offset_map(
        source: DefaultDict[str, Dict[str, int]], target: DefaultDict[str, Dict[str, int]], groups: List[str]
    ) -> List[Dict[int, int]]:
        return [{source[group][key]: target[group][key] for key in source[group]} for group in groups]

    @staticmethod
    def compose_definition(source: List[Dict[int, int]], name: str) -> str:
        lines = [f"{name} = {{"]
        for mapping in source:
            if not mapping:
                lines += ["    {},"]
            else:
                lines += ["    {"]
                for key, value in mapping.items():
                    lines += [f"        {{{key}, {value}}},"]
                lines += ["    },"]
        lines += ["};", ""]
        return "\n".join(lines)

    @staticmethod
    def compose_offset_hpp_file(x16_to_x32: List[Dict[int, int]], x32_to_x16: List[Dict[int, int]]) -> str:
        declaration = "const std::vector<std::map<uint16_t, uint16_t>> "

        header = """#ifndef MAPS_OFFSETS_HPP
#define MAPS_OFFSETS_HPP

#include <cstdint>
#include <map>
#include <vector>

"""
        end = """
#endif // MAPS_OFFSETS_HPP
"""

        return "".join(
            [
                header,
                declaration,
                ConstantManager.compose_definition(x16_to_x32, "x16_to_x32"),
                declaration,
                ConstantManager.compose_definition(x32_to_x16, "x32_to_x16"),
                end,
            ]
        )
