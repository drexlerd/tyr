from pathlib import Path
from importlib.metadata import PackageNotFoundError, version

from . import (
    common as common,
    formalism as formalism,
    planning as planning,
)


def _source_version() -> str:
    for parent in Path(__file__).resolve().parents:
        pyproject = parent / "pyproject.toml"
        if not pyproject.exists():
            continue

        for line in pyproject.read_text(encoding="utf-8").splitlines():
            if line.startswith("version"):
                return line.split("=", maxsplit=1)[1].strip().strip("\"")

    return "0.0.0"


try:
    __version__ = version("pytyr")
except PackageNotFoundError:
    __version__ = _source_version()


def native_prefix() -> Path:
    return Path(__file__).resolve().parent / "native"
