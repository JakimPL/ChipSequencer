import shutil
import struct
import subprocess
from distutils.dir_util import copy_tree
from typing import Dict, List, Tuple

from compiler.compiler import Compiler
from compiler.link import Link, LinkTarget, LinkType
from utils import load_binary, save_binary

DSP_OUTPUT = 3
CHANNEL_OUTPUT = 8


class DOSCompiler(Compiler):
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
        self.compile(pack=self.compress)

        self.copy_executable()

    def copy_source(self) -> None:
        copy_tree("core", str(self.temp_dir / "core"))
        copy_tree("tools", str(self.temp_dir / "tools"))
        shutil.copy("compile.bat", self.temp_dir / "compile.bat")
        shutil.copy("linker.lnk", self.temp_dir / "linker.lnk")
        shutil.copy(self.song_dir / "header.asm", self.temp_dir / "core" / "song" / "header.asm")
        shutil.copy(self.song_dir / "data.asm", self.temp_dir / "core" / "song" / "data.asm")

    def copy_executable(self):
        source = "PLAYER.EXE" if self.compress else "MAIN.EXE"
        shutil.copy(self.bin_dir / source, self.target_path)

    def compile(self, pack: bool = False, terminate: bool = True) -> None:
        args = ["dosbox", "-noautoexec", "-c", "mount c: .", "-c", "c:", "-c", "call compile.bat"]

        if pack:
            args += [
                "-c",
                "cd tools",
                "-c",
                "apack ../bin/MAIN.EXE ../bin/PLAYER.EXE",
            ]

        if terminate:
            args += ["-c", "exit"]

        subprocess.run(args, cwd=self.temp_dir)

    def get_links(self) -> List[Link]:
        links_path = self.song_dir / "links.bin"
        binary_data = load_binary(links_path)

        links = []
        format_string = "<BBBIH"
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
                filename = self.song_dir / "chans" / f"chan_{i}.bin"
                target_slice = slice(CHANNEL_OUTPUT, CHANNEL_OUTPUT + 2)
            elif link.type == LinkType.DSP:
                filename = self.song_dir / "dsps" / f"dsp_{i}.bin"
                target_slice = slice(DSP_OUTPUT, DSP_OUTPUT + 2)
            else:
                raise ValueError(f"Invalid type: {link.type}")

            if target == LinkTarget.SPLITTER_OUTPUT or target == LinkTarget.DIRECT_OUTPUT:
                reference = references["output"][1]
            elif target == LinkTarget.SPLITTER_DSP or target == LinkTarget.DIRECT_DSP:
                reference = references["dsp_input"][1]
            else:
                reference = references[f"{target.value}s.{target.value}_{link.index}"][1]
            reference += link.offset

            binary_data = bytearray(load_binary(filename))
            binary_data[target_slice] = struct.pack("<H", reference)
            save_binary(bytes(binary_data), filename)
