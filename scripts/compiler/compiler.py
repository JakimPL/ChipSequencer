import os
import re
from pathlib import Path
from typing import Union


class Compiler:
    def __init__(
        self,
        temp_dir: Union[str, os.PathLike],
        target_path: Union[str, os.PathLike],
        compress: bool = True,
        debug: bool = False,
    ):
        self.temp_dir = Path(temp_dir)
        self.target_path = Path(target_path)
        self.compress = False if debug else compress
        self.debug = debug
        self.tools_dir = Path.cwd() / "tools"
        self.bin_dir = self.temp_dir / "bin"
        self.build_dir = self.temp_dir / "build"
        self.song_dir = self.temp_dir / "song"
        self.header_path = self.song_dir / "header.json"
        self.pattern = re.compile(r"^[0-9a-f]{4}:[0-9a-f]{4}")

    def __call__(self) -> None:
        raise NotImplementedError("This is an abstract class.")
