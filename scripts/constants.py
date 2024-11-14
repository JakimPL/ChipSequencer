import re

from pyconf import CONSTANTS_ASM_FILE, CONSTANTS_CPP_FILE


def convert_asm_to_cpp(data: str) -> str:
    output = data.replace(";", "//").replace("%", "#")
    output = re.sub(r"^[ \t]+", "", output, flags=re.MULTILINE)
    return "#define ELF\n\n" + output


if __name__ == "__main__":
    with open(CONSTANTS_ASM_FILE, "r") as file:
        asm_constants = file.read()

    cpp_constants = convert_asm_to_cpp(asm_constants)
    with open(CONSTANTS_CPP_FILE, "w") as file:
        file.write(cpp_constants)
