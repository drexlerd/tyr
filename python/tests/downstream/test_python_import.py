import json
import os
import shutil
import subprocess
import sys
from pathlib import Path

import pytyr
import pytest


ROOT_DIR = Path(__file__).resolve().parents[3]
DOWNSTREAM_PACKAGE_DIR = Path(__file__).resolve().parent / "minimal_downstream_package"


def test_downstream_python_binding_imports_public_pytyr_api_and_links_tyr_core(tmp_path):
    cmake = shutil.which("cmake")
    if cmake is None:
        pytest.skip("cmake is required for the downstream binding smoke test")

    pytyr_prefix = Path(pytyr.native_prefix())
    tyr_library_dir = pytyr_prefix / "lib"
    tyr_core_libraries = list(tyr_library_dir.glob("libtyr_core.*"))
    if not tyr_core_libraries:
        pytest.skip("pytyr was not installed with a shared tyr::core library")

    tyr_include_dir = pytyr_prefix / "include"
    if not (tyr_include_dir / "tyr" / "common" / "config.hpp").exists():
        pytest.skip("pytyr was not installed with Tyr C++ public headers")

    tyr_cmake_dir = pytyr_prefix / "lib" / "cmake" / "tyr"
    if not (tyr_cmake_dir / "tyrConfig.cmake").exists():
        pytest.skip("pytyr was not installed with Tyr CMake package files")

    nanobind_cmake_dir = pytyr_prefix / "nanobind" / "cmake"
    if not nanobind_cmake_dir.exists():
        pytest.skip("nanobind CMake package was not found in the pytyr installation prefix")

    project_dir = tmp_path / "minimal_downstream_package"
    shutil.copytree(DOWNSTREAM_PACKAGE_DIR, project_dir)

    env = os.environ.copy()
    env["CMAKE_ARGS"] = " ".join(
        [
            f"-DCMAKE_PREFIX_PATH={pytyr_prefix}",
            f"-Dtyr_DIR={tyr_cmake_dir}",
            f"-DPython_EXECUTABLE={sys.executable}",
            f"-DDOWNSTREAM_TYR_RUNTIME_LIBRARY_DIR={tyr_library_dir}",
            env.get("CMAKE_ARGS", ""),
        ]
    ).strip()

    build_dir = tmp_path / "build"
    subprocess.run(
        [
            cmake,
            "-S",
            str(project_dir),
            "-B",
            str(build_dir),
            f"-DCMAKE_PREFIX_PATH={pytyr_prefix}",
            f"-Dtyr_DIR={tyr_cmake_dir}",
            f"-DPython_EXECUTABLE={sys.executable}",
            f"-DDOWNSTREAM_TYR_RUNTIME_LIBRARY_DIR={tyr_library_dir}",
        ],
        check=True,
        env=env,
    )
    subprocess.run([cmake, "--build", str(build_dir), "-j8"], check=True)

    env["PYTHONPATH"] = str(project_dir / "src") + os.pathsep + env.get("PYTHONPATH", "")

    result = subprocess.run(
        [
            sys.executable,
            "-c",
            (
                "import json, downstream_tyr_user; "
                "print(json.dumps({"
                "'imports': downstream_tyr_user.describe_pytyr_imports(), "
                "'product': downstream_tyr_user.multiply(6, 7)"
                "}))"
            ),
        ],
        check=True,
        capture_output=True,
        text=True,
        env=env,
    )

    payload = json.loads(result.stdout)
    assert payload == {
        "imports": {
            "execution_context": "ExecutionContext",
            "float_t_size": 8,
        },
        "product": 42,
    }
