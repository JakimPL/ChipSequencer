import os
import re
import shutil
import subprocess
from distutils.dir_util import copy_tree
from pathlib import Path
from typing import Dict, Tuple, Union


class Compiler:
    def __init__(self, temp_dir: Union[str, os.PathLike]):
        self.temp_dir = Path(temp_dir)
        self.apack_dir = Path("apack")
        self.bin_dir = self.temp_dir / "bin"
        self.song_dir = self.temp_dir / "song"
        self.pattern = re.compile(r"^[0-9a-f]{4}:[0-9a-f]{4}")

    def __call__(self) -> None:
        self.temp_dir.mkdir(exist_ok=True)
        self.bin_dir.mkdir(exist_ok=True)
        self.copy_source()
        self.run_dosbox()
        references = self.get_references()
        self.copy_executable()

    def copy_source(self) -> None:
        copy_tree("src", str(self.temp_dir / "src"))
        shutil.copy("compile.bat", self.temp_dir / "compile.bat")
        shutil.copy("nasm.exe", self.temp_dir / "nasm.exe")
        shutil.copy("cwsdpmi.exe", self.temp_dir / "cwsdpmi.exe")
        shutil.copy("JWlinkd.exe", self.temp_dir / "jwlinkd.exe")
        shutil.copy("linker.lnk", self.temp_dir / "linker.lnk")
        shutil.copy(self.song_dir / "song.asm", self.temp_dir / "src" / "song" / "data.asm")
        copy_tree(str(self.apack_dir), str(self.temp_dir / self.apack_dir.name))

    def copy_executable(self):
        shutil.copy(self.bin_dir / "PLAYER.EXE", self.song_dir / "player.exe")

    def run_dosbox(self) -> None:
        subprocess.run(
            [
                "dosbox",
                "-noautoexec",
                "-c",
                "mount c: .",
                "-c",
                "c:",
                "-c",
                "call compile.bat",
                "-c",
                "cd apack",
                "-c",
                "apack ../bin/MAIN.EXE ../bin/player.exe",
                "-c",
                "exit",
            ],
            cwd=self.temp_dir,
        )

    def get_references(self) -> Dict[str, Tuple[int, ...]]:
        map_path = self.bin_dir / "PLAYER.MAP"
        with open(map_path, "r") as file:
            addresses = {}
            for line in file.readlines():
                match = self.pattern.findall(line)
                line = line.strip()
                if match:
                    name = line.split(" ")[-1]
                    addresses[name] = tuple(map(lambda x: int(x, 16), match[0].split(":")))

            return addresses
