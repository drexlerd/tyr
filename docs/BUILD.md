# Building Tyr

Tyr consumes native third-party dependencies from Python packages:

- `pyyggdrasil==0.0.5` provides shared libraries, headers, and CMake packages for common native dependencies.
- `pypddl==1.0.2` provides Loki's PDDL parser library, headers, and CMake package.

## Requirements

1. CMake 3.21 or newer.
2. Python >= 3.9 with development headers.
3. A C++20 compiler.

## Dependency Setup

Create a virtual environment and install the native dependency providers:

```console
uv venv
uv pip install pyyggdrasil==0.0.5 pypddl==1.0.2
```

For plain `pip`, use:

```console
python -m venv .venv
.venv/bin/python -m pip install --upgrade pip
.venv/bin/python -m pip install pyyggdrasil==0.0.5 pypddl==1.0.2
```

## C++ Build

Configure Tyr with the native prefixes from the installed Python packages:

```console
cmake -S . -B build \
  -DPython_EXECUTABLE=${PWD}/.venv/bin/python \
  -DPython3_EXECUTABLE=${PWD}/.venv/bin/python \
  -DCMAKE_PREFIX_PATH="$(.venv/bin/python -c 'import pypddl, pyyggdrasil; print(f"{pypddl.native_prefix()};{pyyggdrasil.native_prefix()}")')" \
  -DTYR_BUILD_SHARED=ON \
  -DTYR_LINK_STATIC_DEPENDENCIES=OFF

cmake --build build -j$(nproc)
```

Enable optional targets as needed:

```console
cmake -S . -B build \
  -DPython_EXECUTABLE=${PWD}/.venv/bin/python \
  -DPython3_EXECUTABLE=${PWD}/.venv/bin/python \
  -DCMAKE_PREFIX_PATH="$(.venv/bin/python -c 'import pypddl, pyyggdrasil; print(f"{pypddl.native_prefix()};{pyyggdrasil.native_prefix()}")')" \
  -DTYR_BUILD_SHARED=ON \
  -DTYR_LINK_STATIC_DEPENDENCIES=OFF \
  -DBUILD_TESTS=ON \
  -DBUILD_EXECUTABLES=ON \
  -DBUILD_PROFILING=ON
```

Install Tyr from a configured build directory with:

```console
cmake --install build --prefix=<path/to/installation-directory>
```

## Python Build

The Python wheel build uses the same dependency providers through `pyproject.toml`:

```console
uv pip install .[test]
python -m pytest python/tests
```

The installed `pytyr` package exposes its native CMake prefix:

```python
import pytyr
print(pytyr.native_prefix())
```
