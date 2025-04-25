import shutil
import subprocess
from distutils.dir_util import copy_tree

from compiler.compiler import Compiler


class LinuxCompiler(Compiler):
    def __call__(self) -> None:
        self.temp_dir.mkdir(exist_ok=True)
        self.bin_dir.mkdir(exist_ok=True)
        self.build_dir.mkdir(exist_ok=True)
        self.copy_source()

        # first pass
        self.compile()
        self.copy_executable()

    def copy_source(self):
        copy_tree("core", str(self.temp_dir / "core"))
        copy_tree("tools", str(self.temp_dir / "tools"))
        shutil.copy("compile.sh", self.temp_dir / "compile.sh")
        shutil.copy("player/main.hpp", self.temp_dir / "main.hpp")
        shutil.copy("player/main.cpp", self.temp_dir / "main.cpp")
        shutil.copy(self.song_dir / "header.asm", self.temp_dir / "core" / "song" / "header.asm")
        shutil.copy(self.song_dir / "data.asm", self.temp_dir / "core" / "song" / "data.asm")

    def compile(self):
        args = [
            "bash",
            "-c",
            "./compile.sh",
        ]

        subprocess.run(args, cwd=self.temp_dir)

    def copy_executable(self):
        self.compress = True  # not implemented yet
        source = "player" if self.compress else "main"
        shutil.copy(self.bin_dir / source, self.target_path)
