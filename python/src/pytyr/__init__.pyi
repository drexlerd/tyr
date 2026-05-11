from pathlib import Path

from . import (
    common as common,
    formalism as formalism,
    planning as planning,
)

__version__: str

def native_prefix() -> Path: ...
