import math
import os
import re
import shutil
from pathlib import Path
from typing import Union

PAGE_SIZE = 0x1000


class Compiler:
    def __init__(
        self,
        app_dir: Union[str, os.PathLike],
        temp_dir: Union[str, os.PathLike],
        target_path: Union[str, os.PathLike],
        compression: bool = True,
        debug: bool = False,
    ):
        self.app_dir = Path(app_dir)
        self.temp_dir = Path(temp_dir)
        self.target_path = Path(target_path)
        self.compression = False if debug else compression
        self.debug = debug
        self.tools_dir = Path.cwd() / "tools"
        self.bin_dir = self.temp_dir / "bin"
        self.build_dir = self.temp_dir / "build"
        self.song_dir = self.temp_dir / "song"
        self.header_path = self.song_dir / "header.json"
        self.pattern = re.compile(r"^[0-9a-f]{4}:[0-9a-f]{4}")

    def __call__(self) -> None:
        raise NotImplementedError("This is an abstract class.")

    def copy_executable(self, extension: str = "") -> None:
        source = "player" + extension if self.compression else "main" + extension
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

    def measure_file_size(self, main_path: Path) -> int:
        file_size = main_path.stat().st_size
        return math.ceil(file_size / PAGE_SIZE) * PAGE_SIZE

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
