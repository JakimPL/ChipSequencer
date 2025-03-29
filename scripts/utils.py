import json
import os
from typing import Any, Dict, Union


def load_json(path: Union[str, os.PathLike]) -> Dict[Any, Any]:
    with open(path, "r") as file:
        return json.load(file)


def load_binary(path: Union[str, os.PathLike]) -> bytes:
    with open(path, "rb") as file:
        return file.read()


def load_text(path: Union[str, os.PathLike]) -> str:
    with open(path, "r") as file:
        return file.read()


def save_json(data: Dict[Any, Any], path: Union[str, os.PathLike]) -> None:
    with open(path, "w") as file:
        json.dump(data, file, indent=4)


def save_binary(data: bytes, path: Union[str, os.PathLike]) -> None:
    with open(path, "wb") as file:
        file.write(data)


def save_text(data: str, path: Union[str, os.PathLike]) -> None:
    with open(path, "w") as file:
        file.write(data)
