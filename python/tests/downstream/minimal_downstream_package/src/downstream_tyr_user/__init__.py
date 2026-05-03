from ._downstream_tyr import float_t_size, multiply
from pytyr.common import ExecutionContext


def describe_pytyr_imports():
    execution_context = ExecutionContext(1)
    return {
        "execution_context": type(execution_context).__name__,
        "float_t_size": float_t_size(),
    }
