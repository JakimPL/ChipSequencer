import shutil
import tempfile
from distutils.dir_util import copy_tree
from pathlib import Path
from typing import Dict, List, Optional, Tuple

from utils import save_json

TEMP_BASE = Path(tempfile.gettempdir()) / "chipsequencer"
CATEGORIES = ["DSP", "Oscillator", "Command"]


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
        module_sizes, sizes = self.calculate_sizes(flags)
        print(module_sizes)
        print(sizes)

    def prepare_directory(self):
        self.temp_dir.mkdir(parents=True, exist_ok=True)
        self.song_dir.mkdir(parents=True, exist_ok=True)
        copy_tree("core", str(self.temp_dir / "core"))
        shutil.copy(Path("core") / "song" / "data.asm", self.song_dir / "data.asm")
        shutil.copy(Path("core") / "song" / "header.asm", self.song_dir / "header.asm")

    def check_size(self, flags: Optional[List[str]] = None) -> int:
        self.compiler(flags)
        return self.target_path.stat().st_size

    def calculate_sizes(self, flags: Dict[str, List[str]]) -> Tuple[Dict[str, int], Dict[str, Dict[str, int]]]:
        core_size = self.check_size()
        sizes = {f"{category}s": {} for category in CATEGORIES}
        module_sizes = {"Core": core_size}
        for category, flags in flags.items():
            base_flag = f"USED_{category.upper()}"
            base_size = self.check_size([base_flag])
            for flag in flags:
                size = self.check_size([base_flag, flag])
                category_name = f"{category}s"
                if flag != base_flag:
                    name = flag.replace(f"{base_flag}_", "").replace("_", " ").capitalize()
                    sizes[category_name][name] = size - base_size
                else:
                    module_sizes[category_name] = base_size - core_size

        return module_sizes, sizes

    def gather_flags(self) -> Dict[str, List[str]]:
        path = self.temp_dir / "core" / "common.asm"
        with open(path, "r") as file:
            lines = file.readlines()

        flags = [line.split("%define")[-1].strip() for line in lines if "%define USED_" in line]
        return {
            category: [flag for flag in flags if flag.startswith(f"USED_{category.upper()}")] for category in CATEGORIES
        }
