from pathlib import Path

from . import (
    common as common,
    formalism as formalism,
    planning as planning,
)


def native_prefix() -> Path:
    return Path(__file__).resolve().parent.parent / "yggdrasil"
