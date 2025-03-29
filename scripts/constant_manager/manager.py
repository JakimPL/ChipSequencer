import os
import re
from collections import defaultdict
from typing import DefaultDict, Dict, List, Tuple, Union

from pyconf import CONSTANTS_ASM_FILE, CONSTANTS_CPP_FILE, TARGET_HPP_FILE, X16_TO_X32_FILE, X32_TO_X16_FILE

from utils import load_text, save_json, save_text


class ConstantManager:
    def __init__(
        self,
        constants_asm_file: Union[str, os.PathLike] = CONSTANTS_ASM_FILE,
        constants_cpp_file: Union[str, os.PathLike] = CONSTANTS_CPP_FILE,
        target_hpp_file: Union[str, os.PathLike] = TARGET_HPP_FILE,
        x16_to_x32_file: Union[str, os.PathLike] = X16_TO_X32_FILE,
        x32_to_x16_file: Union[str, os.PathLike] = X32_TO_X16_FILE,
    ):
        self.constants_asm_file = constants_asm_file
        self.constants_cpp_file = constants_cpp_file
        self.target_hpp_file = target_hpp_file
        self.x16_to_x32_file = x16_to_x32_file
        self.x32_to_x16_file = x32_to_x16_file

    def __call__(self):
        asm_constants = load_text(self.constants_asm_file)
        cpp_constants = self.convert_asm_to_cpp(asm_constants)
        save_text(cpp_constants, self.constants_cpp_file)

        target = load_text(self.target_hpp_file)
        groups = self.extract_groups_from_targets(target)
        x16, x32 = self.parse_cpp_constants(cpp_constants)
        x16_to_x32 = self.get_offset_map(x16, x32, groups)
        x32_to_x16 = self.get_offset_map(x32, x16, groups)
        save_json(x16_to_x32, self.x16_to_x32_file)
        save_json(x32_to_x16, self.x32_to_x16_file)

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
    def parse(string: str) -> int:
        if string.startswith("0x"):
            return int(string, 16)
        if string.startswith("0b"):
            return int(string, 2)

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
