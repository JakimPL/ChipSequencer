import argparse
import shutil
import subprocess
from distutils.dir_util import copy_tree
from pathlib import Path

if __name__ == "__main__":
    arg_parser = argparse.ArgumentParser()
    arg_parser.add_argument("directory", type=str)
    args = arg_parser.parse_args()

    temp_dir = Path(args.directory)
    temp_dir.mkdir(exist_ok=True)

    bin_dir = temp_dir / "bin"
    bin_dir.mkdir(exist_ok=True)

    copy_tree("src", str(temp_dir / "src"))
    shutil.copy("compile.bat", temp_dir / "compile.bat")
    shutil.copy("nasm.exe", temp_dir / "nasm.exe")
    shutil.copy("cwsdpmi.exe", temp_dir / "cwsdpmi.exe")
    shutil.copy("JWlinkd.exe", temp_dir / "jwlinkd.exe")
    shutil.copy("linker.lnk", temp_dir / "linker.lnk")
    shutil.copy(temp_dir / "song" / "song.asm", temp_dir / "src" / "song" / "data.asm")
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
            # "-c", "exit"
        ],
        cwd=temp_dir,
    )
