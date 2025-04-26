import os
import re
from collections import defaultdict
from typing import DefaultDict, Dict, List, Tuple, Union

from pyconf import (
    CONSTANTS_ASM_FILE,
    CONSTANTS_HPP_FILE,
    LINK_PY_FILE,
    OFFSETS_ASM_FILE,
    OFFSETS_HPP_FILE,
    SIZES_ASM_FILE,
    SIZES_HPP_FILE,
    TARGET_HPP_FILE,
    TYPE_HPP_FILE,
)
from utils import load_text, save_text


class ConstantManager:
    def __init__(
        self,
        constants_asm_file: Union[str, os.PathLike] = CONSTANTS_ASM_FILE,
        constants_hpp_file: Union[str, os.PathLike] = CONSTANTS_HPP_FILE,
        offsets_asm_file: Union[str, os.PathLike] = OFFSETS_ASM_FILE,
        offsets_hpp_file: Union[str, os.PathLike] = OFFSETS_HPP_FILE,
        sizes_asm_file: Union[str, os.PathLike] = SIZES_ASM_FILE,
        sizes_hpp_file: Union[str, os.PathLike] = SIZES_HPP_FILE,
        target_hpp_file: Union[str, os.PathLike] = TARGET_HPP_FILE,
        type_hpp_file: Union[str, os.PathLike] = TYPE_HPP_FILE,
        link_py_file: Union[str, os.PathLike] = LINK_PY_FILE,
    ):
        self.constants_asm_file = constants_asm_file
        self.constants_hpp_file = constants_hpp_file
        self.offsets_asm_file = offsets_asm_file
        self.offsets_hpp_file = offsets_hpp_file
        self.sizes_asm_file = sizes_asm_file
        self.sizes_hpp_file = sizes_hpp_file
        self.target_hpp_file = target_hpp_file
        self.type_hpp_file = type_hpp_file
        self.link_py_file = link_py_file

    def __call__(self):
        asm_constants = load_text(self.constants_asm_file)
        asm_offsets = load_text(self.offsets_asm_file)
        asm_sizes = load_text(self.sizes_asm_file)
        hpp_constants = self.convert_asm_to_cpp(asm_constants)
        hpp_offsets = self.convert_asm_to_cpp(asm_offsets)
        hpp_sizes = self.convert_asm_to_cpp(asm_sizes)
        save_text(hpp_constants, self.constants_hpp_file)
        save_text(hpp_sizes, self.sizes_hpp_file)

        target = load_text(self.target_hpp_file)
        link_type = load_text(self.type_hpp_file)
        link_py = self.compose_link_py_file(target, link_type)
        save_text(link_py, self.link_py_file)

        groups = self.extract_groups_from_targets(target)
        x16, x32 = self.parse_cpp_constants(hpp_offsets)
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
        return "#define TRACKER\n\n" + output

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
        lines = [line for line in cpp_constants.splitlines() if line.startswith("#") and line != "#define TRACKER"]
        x16 = defaultdict(dict)
        x32 = defaultdict(dict)
        elf = None
        for line in lines:
            if line == "#ifdef TRACKER":
                elf = True
                continue
            if line == "#else":
                elf = not elf
                continue
            if line == "#endif":
                elf = None
                continue

            split = line.split(" ")
            if len(split) != 3:
                continue

            _, key, value = split
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

    @staticmethod
    def compose_link_py_file(target: str, link_type: str) -> str:
        header = """# This file is auto-generated by ConstantManager. Do not edit it manually.

from dataclasses import dataclass
from enum import Enum
from typing import Dict


"""
        type_def_start = link_type.find("enum class ItemType")
        type_def_end = link_type.find("};", type_def_start) + 2

        type_def = link_type[type_def_start:type_def_end]
        type_def = (
            type_def.replace(r"enum class ItemType : uint8_t {", "class LinkType(Enum):")
            .replace(",", "")
            .replace("};", "")
        )

        target_def_start = target.find("enum class Target")
        target_def_end = target.find("};", target_def_start) + 2

        target_def = target[target_def_start:target_def_end]
        target_def = (
            target_def.replace(r"enum class Target : uint8_t {", "class LinkTarget(Enum):")
            .replace(",", "")
            .replace("};", "")
        )

        target_def_lines = target_def.splitlines()
        target_def = []
        target_items = []
        for line in target_def_lines:
            if not line.startswith("class"):
                name = line.strip()
                line += rf' = "{name.lower()}"'
                target_items.append(name)

            target_def.append(line)

        function_def = """

    @staticmethod
    def from_integer(integer: int) -> "LinkTarget":
        dictionary: Dict[int, LinkTarget] = {
"""

        for i, item in enumerate(target_items):
            function_def += f"            {i}: LinkTarget.{item},\n"
        function_def += """        }

        return dictionary[integer]
"""

        end = """

@dataclass
class Link:
    type: LinkType
    id: int
    target: LinkTarget
    index: int
    offset: int
"""
        return "".join(
            [
                header,
                type_def,
                "\n\n",
                "\n".join(target_def),
                function_def,
                end,
            ]
        )
