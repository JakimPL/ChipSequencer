from pathlib import Path
from typing import List


def find_header_files(directory: Path) -> List[Path]:
    return list(directory.rglob("*.hpp"))


def check_pragma_once(file_path: Path) -> bool:
    try:
        with open(file_path, "r", encoding="utf-8") as file:
            first_line = file.readline().strip()
            return first_line == "#pragma once"
    except (IOError, OSError) as e:
        raise e


def check_header_files(tracker_directory: Path) -> None:
    if not tracker_directory.exists():
        raise FileNotFoundError(f"Tracker directory does not exist: {tracker_directory}")

    if not tracker_directory.is_dir():
        raise NotADirectoryError(f"Tracker path is not a directory: {tracker_directory}")

    hpp_files = find_header_files(tracker_directory)

    if not hpp_files:
        print(f"No .hpp files found in {tracker_directory}")
        return

    non_compliant_files = []

    for hpp_file in hpp_files:
        if not check_pragma_once(hpp_file):
            non_compliant_files.append(hpp_file)

    if non_compliant_files:
        error_message = 'The following .hpp files do not have "#pragma once" as the first line:\n'
        for file_path in non_compliant_files:
            error_message += f"  - {file_path}\n"
        raise ValueError(error_message.strip())

    print(f'All {len(hpp_files)} .hpp files in {tracker_directory} are compliant with "#pragma once" requirement.')


def main() -> None:
    tracker_dir = Path("tracker")
    check_header_files(tracker_dir)


if __name__ == "__main__":
    main()
