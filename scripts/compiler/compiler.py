import json
import os
import re
import shutil
import struct
import subprocess
from distutils.dir_util import copy_tree
from pathlib import Path
from typing import Any, Dict, List, Tuple, Union

from compiler.link import Link, LinkTarget, LinkType


class Compiler:
    def __init__(self, temp_dir: Union[str, os.PathLike]):
        self.temp_dir = Path(temp_dir)
        self.tools_dir = Path.cwd() / "tools"
        self.bin_dir = self.temp_dir / "bin"
        self.build_dir = self.temp_dir / "build"
        self.song_dir = self.temp_dir / "song"
        self.header_path = self.song_dir / "header.json"
        self.pattern = re.compile(r"^[0-9a-f]{4}:[0-9a-f]{4}")

    def __call__(self) -> None:
        self.temp_dir.mkdir(exist_ok=True)
        self.bin_dir.mkdir(exist_ok=True)
        self.build_dir.mkdir(exist_ok=True)
        self.copy_source()

        # first pass
        self.compile()

        links = self.get_links()
        references = self.get_references()
        self.inject_references(links, references)

        # second pass
        self.compile(pack=True)

        self.copy_executable()

    def copy_source(self) -> None:
        copy_tree("src", str(self.temp_dir / "src"))
        shutil.copy("compile.bat", self.temp_dir / "compile.bat")
        shutil.copy(self.tools_dir / "nasm.exe", self.temp_dir / "nasm.exe")
        shutil.copy(self.tools_dir / "cwsdpmi.exe", self.temp_dir / "cwsdpmi.exe")
        shutil.copy(self.tools_dir / "JWlinkd.exe", self.temp_dir / "jwlinkd.exe")
        shutil.copy(self.tools_dir / "apack.exe", self.temp_dir / "apack.exe")
        shutil.copy("linker.lnk", self.temp_dir / "linker.lnk")
        shutil.copy(self.song_dir / "data.asm", self.temp_dir / "src" / "song" / "data.asm")

    def copy_executable(self):
        shutil.copy(self.bin_dir / "PLAYER.EXE", self.song_dir / "player.exe")

    def compile(self, pack: bool = False) -> None:
        args = ["dosbox", "-noautoexec", "-c", "mount c: .", "-c", "c:", "-c", "call compile.bat"]

        if pack:
            args += [
                "-c",
                "apack ../bin/MAIN.EXE ../bin/player.exe",
            ]

        args += ["-c", "exit"]

        subprocess.run(args, cwd=self.temp_dir)

    def get_links(self) -> List[Link]:
        links_path = self.song_dir / "links.bin"
        binary_data = self.load_binary(links_path)

        links = []
        format_string = "<BBBBH"
        struct_size = struct.calcsize(format_string)

        for i in range(0, len(binary_data), struct_size):
            chunk = binary_data[i : i + struct_size]
            type_id, item_id, target, index, offset = struct.unpack(format_string, chunk)
            links.append(Link(LinkType(type_id), item_id, LinkTarget.from_integer(target), index, offset))

        return links

    def get_references(self) -> Dict[str, Tuple[int, ...]]:
        map_path = self.build_dir / "PLAYER.MAP"
        with open(map_path, "r") as file:
            addresses = {}
            for line in file.readlines():
                match = self.pattern.findall(line)
                line = line.strip()
                if match:
                    name = line.split(" ")[-1]
                    addresses[name] = tuple(map(lambda x: int(x, 16), match[0].split(":")))

            return addresses

    def inject_references(self, links: List[Link], references: Dict[str, Tuple[int, ...]]) -> None:
        for link in links:
            target = link.target
            i = link.id
            if link.type == LinkType.CHANNEL:
                file = self.song_dir / "chans" / f"chan_{i}.bin"
                target_slice = slice(7, 9)
            elif link.type == LinkType.DSP:
                file = self.song_dir / "dsps" / f"dsp_{i}.bin"
                target_slice = slice(2, 4)
            else:
                raise ValueError(f"Invalid type: {link.type}")

            if target == LinkTarget.OUTPUT:
                reference = references["output"][1]
            elif target == LinkTarget.DSP_INPUT:
                reference = references["dsp_input"][1]
            else:
                reference = references[f"{target.value}s.{target.value}_{link.index}"][1]
            reference += link.offset

            binary_data = bytearray(self.load_binary(file))
            binary_data[target_slice] = struct.pack("<H", reference)
            self.save_binary(file, bytes(binary_data))

    @staticmethod
    def load_json(path: Union[str, os.PathLike]) -> Dict[Any, Any]:
        with open(path, "r") as file:
            return json.load(file)

    @staticmethod
    def load_binary(path: Union[str, os.PathLike]) -> bytes:
        with open(path, "rb") as file:
            return file.read()

    @staticmethod
    def save_binary(path: Union[str, os.PathLike], data: bytes) -> None:
        with open(path, "wb") as file:
            file.write(data)
