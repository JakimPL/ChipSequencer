import math
import re
import shutil
import subprocess
import sys
from distutils.dir_util import copy_tree
from pathlib import Path
from typing import List, Optional

from compiler.compiler import Compiler

COMPRESSION_LEVEL = 1
PAGE_SIZE = 0x1000


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

        file_size = self.measure_file_size()
        shutil.copy(self.temp_dir / "core" / "platform" / "linux.asm.temp", path)
        self.substitute_values(path, message, sample_rate, output_channels, file_size)
        self.compile(flags, hide_output=hide_output)

        if self.compression:
            self.compress()

        self.copy_executable()

    def copy_source(self):
        compilation_script = self.app_dir / Path("shell") / "linux" / "compile.sh"
        copy_tree(self.app_dir / "core", str(self.temp_dir / "core"))
        copy_tree(self.app_dir / "tools", str(self.temp_dir / "tools"))
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

    def copy_executable(self):
        source = "player" if self.compression else "main"
        shutil.copy(self.bin_dir / source, self.target_path)

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

    def get_song_info(self):
        header_path = self.song_dir / "header.asm"
        with open(header_path, "r") as file:
            lines = file.readlines()

        message_pattern = r'^\s*%define MESSAGE "(.*)"'
        sample_rate_pattern = r"^\s*%define SAMPLE_RATE (\d+)"
        output_channels_pattern = r"^\s*%define OUTPUT_CHANNELS (\d+)"

        message = None
        sample_rate = None
        output_channels = None

        for line in lines:
            if match := re.match(message_pattern, line):
                message = match.group(1)
            elif match := re.match(sample_rate_pattern, line):
                sample_rate = int(match.group(1))
            elif match := re.match(output_channels_pattern, line):
                output_channels = int(match.group(1))

        if message is None or sample_rate is None or output_channels is None:
            raise ValueError("Failed to parse required values from header.asm")

        return message, sample_rate, output_channels

    @staticmethod
    def substitute_values(path: Path, message: str, sample_rate: int, output_channels: int, file_size: int = PAGE_SIZE):
        with open(path, "r") as file:
            code = file.read()
            code = code.format(
                message=message,
                output_channels=output_channels,
                sample_rate=sample_rate,
                file_size=file_size,
            )

        with open(path, "w") as file:
            file.write(code)

    def measure_file_size(self) -> int:
        main_path = self.bin_dir / "main"
        file_size = main_path.stat().st_size
        return math.ceil(file_size / PAGE_SIZE) * PAGE_SIZE
