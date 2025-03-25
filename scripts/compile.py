import argparse
import shutil
import subprocess
from distutils.dir_util import copy_tree
from pathlib import Path


def copy_source(temp_dir: Path, song_dir: Path, apack_dir: Path) -> None:
    copy_tree("src", str(temp_dir / "src"))
    shutil.copy("compile.bat", temp_dir / "compile.bat")
    shutil.copy("nasm.exe", temp_dir / "nasm.exe")
    shutil.copy("cwsdpmi.exe", temp_dir / "cwsdpmi.exe")
    shutil.copy("JWlinkd.exe", temp_dir / "jwlinkd.exe")
    shutil.copy("linker.lnk", temp_dir / "linker.lnk")
    shutil.copy(song_dir / "song.asm", temp_dir / "src" / "song" / "data.asm")
    copy_tree(str(apack_dir), str(temp_dir / apack_dir.name))


def run_dosbox(temp_dir: Path) -> None:
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
        cwd=temp_dir,
    )


if __name__ == "__main__":
    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument("directory", type=str)
    args = arg_parser.parse_args()

    temp_dir = Path(args.directory)
    temp_dir.mkdir(exist_ok=True)

    apack_dir = Path("apack")

    bin_dir = temp_dir / "bin"
    bin_dir.mkdir(exist_ok=True)

    song_dir = temp_dir / "song"
    copy_source(temp_dir, song_dir, apack_dir)
    run_dosbox(temp_dir)

    shutil.copy(bin_dir / "PLAYER.EXE", song_dir / "player.exe")
