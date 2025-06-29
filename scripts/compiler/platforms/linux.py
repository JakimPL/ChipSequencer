import shutil
import subprocess
import sys
from distutils.dir_util import copy_tree
from pathlib import Path
from typing import List, Optional

from compiler.compiler import Compiler

COMPRESSION_LEVEL = 1


class LinuxCompiler(Compiler):
    def __call__(self, flags: Optional[List[str]] = None, hide_output: bool = False) -> None:
        self.temp_dir.mkdir(exist_ok=True)
        self.bin_dir.mkdir(exist_ok=True)
        self.build_dir.mkdir(exist_ok=True)
        self.copy_source()

        self.song_dir.mkdir(exist_ok=True)

        message, sample_rate, output_channels = self.get_song_info()
        path = self.temp_dir / "core" / "platform" / "linux.asm"
        self.substitute_values(path, message, sample_rate, output_channels)
        self.compile(hide_output=hide_output)

        main_path = self.bin_dir / "main"
        file_size = self.measure_file_size(main_path)
        shutil.copy(self.temp_dir / "core" / "platform" / "linux.asm.temp", path)
        self.substitute_values(path, message, sample_rate, output_channels, file_size)
        self.compile(flags, hide_output=hide_output)

        if self.compression:
            self.compress()

        self.copy_executable()

    def copy_source(self):
        compilation_script = self.app_dir / Path("shell") / "linux" / "compile.sh"
        copy_tree(self.app_dir / "core", str(self.temp_dir / "core"))
        copy_tree(self.app_dir / "tools" / "oneKpaq", str(self.temp_dir / "tools" / "oneKpaq"))
        shutil.copy(compilation_script, self.temp_dir / "compile.sh")
        shutil.copy(self.song_dir / "header.asm", self.temp_dir / "core" / "song" / "header.asm")
        shutil.copy(self.song_dir / "data.asm", self.temp_dir / "core" / "song" / "data.asm")
        shutil.copy(
            self.temp_dir / "core" / "platform" / "linux.asm", self.temp_dir / "core" / "platform" / "linux.asm.temp"
        )

    def compile(self, flags: Optional[List[str]] = None, hide_output: bool = False):
        flags = flags or []
        command = "./compile.sh" + (" DEBUG" if self.debug else "")
        for flag in flags:
            command += f" --define={flag}"

        args = ["bash", "-c", command]
        subprocess.run(
            args,
            stdout=subprocess.DEVNULL if hide_output else None,
            stderr=subprocess.DEVNULL if hide_output else None,
            cwd=self.temp_dir,
        )

    def compress(self):
        args = [
            sys.executable,
            "onekpaq.py",
            "1",
            str(COMPRESSION_LEVEL),
            self.bin_dir / "main",
            self.bin_dir / "player",
        ]

        subprocess.run(args, cwd=self.temp_dir / "tools" / "oneKpaq")
