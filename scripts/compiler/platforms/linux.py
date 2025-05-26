import re
import shutil
import subprocess
from distutils.dir_util import copy_tree

from compiler.compiler import Compiler

COMPRESSION_LEVEL = 1


class LinuxCompiler(Compiler):
    def __call__(self) -> None:
        self.temp_dir.mkdir(exist_ok=True)
        self.bin_dir.mkdir(exist_ok=True)
        self.build_dir.mkdir(exist_ok=True)
        self.copy_source()

        self.song_dir.mkdir(exist_ok=True)

        message, sample_rate, output_channels = self.get_song_info()
        self.substitute_values(message, sample_rate, output_channels)

        self.compile()
        self.copy_executable()

    def copy_source(self):
        copy_tree("core", str(self.temp_dir / "core"))
        copy_tree("tools", str(self.temp_dir / "tools"))
        shutil.copy("compile.sh", self.temp_dir / "compile.sh")
        shutil.copy(self.song_dir / "header.asm", self.temp_dir / "core" / "song" / "header.asm")
        shutil.copy(self.song_dir / "data.asm", self.temp_dir / "core" / "song" / "data.asm")

    def compile(self):
        args = ["bash", "-c", "./compile.sh" + (" DEBUG" if not self.compress else "")]

        subprocess.run(args, cwd=self.temp_dir)

        if self.compress:
            args = [
                "python",
                "onekpaq.py",
                "1",
                str(COMPRESSION_LEVEL),
                self.bin_dir / "main",
                self.bin_dir / "player",
            ]

            subprocess.run(args, cwd=self.temp_dir / "tools" / "oneKpaq")

    def copy_executable(self):
        source = "player" if self.compress else "main"
        shutil.copy(self.bin_dir / source, self.target_path)

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

    def substitute_values(self, message: str, sample_rate: int, output_channels: int):
        path = self.temp_dir / "core" / "platform" / "linux.asm"
        with open(path, "r") as file:
            code = file.read()
            code = code.format(
                message=message,
                output_channels=output_channels,
                sample_rate=sample_rate,
            )

        with open(path, "w") as file:
            file.write(code)
