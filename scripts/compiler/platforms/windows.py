import shutil
import subprocess
from distutils.dir_util import copy_tree
from pathlib import Path
from typing import List, Optional

from compiler.compiler import Compiler


class WindowsCompiler(Compiler):
    def __call__(self, flags=None, hide_output=False):
        self.temp_dir.mkdir(exist_ok=True)
        self.bin_dir.mkdir(exist_ok=True)
        self.build_dir.mkdir(exist_ok=True)
        self.copy_source()

        self.song_dir.mkdir(exist_ok=True)

        message, sample_rate, output_channels = self.get_song_info()
        path = self.temp_dir / "core" / "platform" / "windows.asm"
        self.substitute_values(path, message, sample_rate, output_channels)
        self.compile(hide_output=hide_output)

        file_size = self.measure_file_size()
        shutil.copy(self.temp_dir / "core" / "platform" / "windows.asm.temp", path)
        self.substitute_values(path, message, sample_rate, output_channels, file_size)
        self.compile(flags, hide_output=hide_output)

        if self.compression:
            self.compress()

        self.copy_executable()

    def copy_source(self):
        compilation_script = self.app_dir / Path("shell") / "windows" / "compile.bat"
        copy_tree(self.app_dir / "core", str(self.temp_dir / "core"))
        copy_tree(
            self.app_dir / "tools" / "crinkler" / "crinkler23" / "Win32", str(self.temp_dir / "tools" / "crinkler")
        )
        shutil.copy(compilation_script, self.temp_dir / "compile.bat")
        shutil.copy(self.song_dir / "header.asm", self.temp_dir / "core" / "song" / "header.asm")
        shutil.copy(self.song_dir / "data.asm", self.temp_dir / "core" / "song" / "data.asm")
        shutil.copy(
            self.temp_dir / "core" / "platform" / "windows.asm",
            self.temp_dir / "core" / "platform" / "windows.asm.temp",
        )

    def compile(self, flags: Optional[List[str]] = None, hide_output: bool = False):
        flags = flags or []
        command = "compile.bat" + (" DEBUG" if self.debug else "")
        for flag in flags:
            command += f" --define={flag}"

        args = ["cmd", "/c", command]
        subprocess.run(
            args,
            stdout=subprocess.DEVNULL if hide_output else None,
            stderr=subprocess.DEVNULL if hide_output else None,
            cwd=self.temp_dir,
        )

    def compress(self):
        return
