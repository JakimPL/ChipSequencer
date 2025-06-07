import shutil
import tempfile
from distutils.dir_util import copy_tree
from pathlib import Path
from typing import List

TEMP_BASE = Path(tempfile.gettempdir()) / "chipsequencer"


class SizeChecker:
    def __init__(self, platform: str):
        self.platform = platform
        self.temp_dir = TEMP_BASE / platform
        self.song_dir = self.temp_dir / "song"
        self.flags = self.gather_flags()
        print(self.flags)

        if platform == "linux":
            from compiler.platforms.linux import LinuxCompiler

            self.target_path = self.temp_dir / "main"
            self.prepare_directory()
            self.compiler = LinuxCompiler(self.temp_dir, self.target_path, False, False)
        else:
            raise ValueError(f"Unknown platform: {platform}, only 'linux' is supported.")

    def prepare_directory(self):
        self.temp_dir.mkdir(parents=True, exist_ok=True)
        self.song_dir.mkdir(parents=True, exist_ok=True)
        copy_tree("core", str(self.temp_dir / "core"))
        shutil.copy(Path("core") / "song" / "data.asm", self.song_dir / "data.asm")
        shutil.copy(Path("core") / "song" / "header.asm", self.song_dir / "header.asm")

    def check_size(self) -> int:
        self.compiler()
        file_size = self.target_path.stat().st_size
        return file_size

    def gather_flags(self) -> List[str]:
        path = self.temp_dir / "core" / "common.asm"
        with open(path, "r") as file:
            lines = file.readlines()

        return [line.split("%define")[-1].strip() for line in lines if "%define USED_" in line]
