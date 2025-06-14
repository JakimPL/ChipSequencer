import shutil
import tempfile
from distutils.dir_util import copy_tree
from pathlib import Path
from typing import Dict, List, Optional, Tuple

from pyconf import COMPONENTS_SIZES_HPP_FILE
from tqdm import tqdm

TEMP_BASE = Path(tempfile.gettempdir()) / "chipsequencer"
CATEGORIES = ["DSP", "Oscillator", "Command"]
NAME_EXCEPTIONS = {
    "Set bpm": "Set BPM",
}

DEPENDENCIES = {
    "USED_COMMAND_PORTAMENTO_UP": ["USED_COMMAND_PORTAMENTO"],
    "USED_COMMAND_PORTAMENTO_DOWN": ["USED_COMMAND_PORTAMENTO"],
    "USED_COMMAND_CHANGE_FLOAT_VALUE": ["USED_COMMAND_CHANGE_32BIT_VALUE"],
    "USED_COMMAND_CHANGE_BYTE_VALUE": ["USED_COMMAND_LOAD_TARGET"],
    "USED_COMMAND_CHANGE_WORD_VALUE": ["USED_COMMAND_LOAD_TARGET"],
    "USED_COMMAND_CHANGE_DWORD_VALUE": ["USED_COMMAND_CHANGE_32BIT_VALUE"],
    "USED_COMMAND_ADD_FLOAT_VALUE": ["USED_COMMAND_ADD_32BIT_VALUE"],
    "USED_COMMAND_ADD_BYTE_VALUE": ["USED_COMMAND_LOAD_TARGET"],
    "USED_COMMAND_ADD_WORD_VALUE": ["USED_COMMAND_LOAD_TARGET"],
    "USED_COMMAND_ADD_DWORD_VALUE": ["USED_COMMAND_ADD_32BIT_VALUE"],
}


class SizeChecker:
    def __init__(self, platform: str):
        self.platform = platform
        self.temp_dir = TEMP_BASE / platform
        self.song_dir = self.temp_dir / "song"

        if platform == "linux":
            from compiler.platforms.linux import LinuxCompiler

            self.target_path = self.temp_dir / "main"
            self.prepare_directory()
            self.compiler = LinuxCompiler(self.temp_dir, self.target_path, False, False)
        else:
            raise ValueError(f"Unknown platform: {platform}, only 'linux' is supported.")

    def __call__(self) -> None:
        flags = self.gather_flags()
        module_sizes, component_sizes = self.calculate_sizes(flags)
        self.write_hpp_file(module_sizes, component_sizes, COMPONENTS_SIZES_HPP_FILE)

    def prepare_directory(self):
        self.temp_dir.mkdir(parents=True, exist_ok=True)
        self.song_dir.mkdir(parents=True, exist_ok=True)
        copy_tree("core", str(self.temp_dir / "core"))
        shutil.copy(Path("core") / "song" / "data.asm", self.song_dir / "data.asm")
        shutil.copy(Path("core") / "song" / "header.asm", self.song_dir / "header.asm")

    def check_size(self, flags: Optional[List[str]] = None) -> int:
        try:
            self.compiler(flags, hide_output=True)
        except Exception as exception:
            print(f"Error during compilation of: {flags}")
            raise exception

        return self.target_path.stat().st_size

    def calculate_sizes(self, flags: Dict[str, List[str]]) -> Tuple[Dict[str, int], Dict[str, Dict[str, int]]]:
        core_size = self.check_size()
        component_sizes = {f"{category}s": {} for category in CATEGORIES}
        module_sizes = {"Core": core_size}

        total_flags = sum(len(f_list) for f_list in flags.values())
        with tqdm(total=total_flags, desc="Calculating sizes") as pbar:
            for category, category_flags in flags.items():
                base_flag = f"USED_{category.upper()}"
                base_size = self.check_size([base_flag])
                for flag in category_flags:
                    size = self.check_size([base_flag, flag])
                    category_name = f"{category}s"
                    if flag != base_flag:
                        name = flag.replace(f"{base_flag}_", "").replace("_", " ").capitalize()
                        component_sizes[category_name][name] = size - base_size
                        if flag in DEPENDENCIES:
                            for dependency in DEPENDENCIES[flag]:
                                dep_size = self.check_size([base_flag, dependency])
                                component_sizes[category_name][name] -= dep_size - base_size
                    else:
                        module_sizes[category_name] = base_size - core_size
                    pbar.update(1)

        return module_sizes, component_sizes

    def gather_flags(self) -> Dict[str, List[str]]:
        path = self.temp_dir / "core" / "common.asm"
        with open(path, "r") as file:
            lines = file.readlines()

        flags = [line.split("%define")[-1].strip() for line in lines if "%define USED_" in line]
        return {
            category: [flag for flag in flags if flag.startswith(f"USED_{category.upper()}")] for category in CATEGORIES
        }

    @staticmethod
    def write_hpp_file(module_sizes: dict, component_sizes: dict, output_path: str) -> None:
        with open(output_path, "w") as file:
            file.write("#pragma once\n\n")
            file.write("#include <string>\n")
            file.write("#include <unordered_map>\n\n")

            # Write module_sizes
            file.write("std::unordered_map<const char *, size_t> module_sizes = {\n")
            for module, size in module_sizes.items():
                file.write(f'    {{"{module}", {size}}},\n')
            file.write("};\n\n")

            file.write("std::unordered_map<std::string, std::unordered_map<std::string, size_t>> component_sizes = {\n")
            for category, components in component_sizes.items():
                file.write("    {\n")
                file.write(f'        "{category}",\n')
                file.write("        {\n")
                for component, size in components.items():
                    component = NAME_EXCEPTIONS.get(component, component)
                    file.write(f'            {{"{component}", {size}}},\n')
                file.write("        },\n")
                file.write("    },\n")
            file.write("};\n\n")

            file.write("#endif // MAPS_SIZES_HPP\n")
