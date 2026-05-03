import os
import shutil
import subprocess
import sys
from pathlib import Path

from scikit_build_core import build as scikit_build


ROOT_DIR = Path(__file__).resolve().parent.parent
DEPENDENCY_BUILD_DIR = ROOT_DIR / "dependencies-build"
DEPENDENCY_INSTALL_DIR = ROOT_DIR / "dependencies-install"


def _build_type() -> str:
    return os.environ.get("TYR_BUILD_TYPE", "Release")


def _num_jobs() -> int:
    return int(os.environ.get("TYR_JOBS", "8"))


def _configure_and_install_dependencies() -> None:
    cmake = shutil.which("cmake")
    if cmake is None:
        raise RuntimeError("cmake is required to build pytyr dependencies")

    DEPENDENCY_BUILD_DIR.mkdir(parents=True, exist_ok=True)

    cmake_args = [
        cmake,
        "-S",
        str(ROOT_DIR / "dependencies"),
        "-B",
        str(DEPENDENCY_BUILD_DIR),
        f"-DCMAKE_BUILD_TYPE={_build_type()}",
        f"-DCMAKE_INSTALL_PREFIX={DEPENDENCY_INSTALL_DIR}",
        f"-DCMAKE_PREFIX_PATH={DEPENDENCY_INSTALL_DIR}",
        f"-DPython_EXECUTABLE={sys.executable}",
    ]

    subprocess.run(cmake_args, cwd=ROOT_DIR, check=True)
    subprocess.run([cmake, "--build", str(DEPENDENCY_BUILD_DIR), f"-j{_num_jobs()}"], cwd=ROOT_DIR, check=True)
    subprocess.run([cmake, "--install", str(DEPENDENCY_BUILD_DIR)], cwd=ROOT_DIR, check=True)


def _prepend_cmake_args(*args: str) -> None:
    existing = os.environ.get("CMAKE_ARGS", "")
    os.environ["CMAKE_ARGS"] = " ".join([*args, existing]).strip()


def _prepare_native_build() -> None:
    _configure_and_install_dependencies()
    _prepend_cmake_args(
        f"-DCMAKE_PREFIX_PATH={DEPENDENCY_INSTALL_DIR}",
        "-DBUILD_PYTYR=ON",
        "-DBUILD_TESTS=OFF",
        "-DBUILD_EXECUTABLES=OFF",
        "-DBUILD_PROFILING=OFF",
        "-DTYR_USE_LLD=OFF",
        f"-DTYR_HEADER_INSTANTIATION={os.environ.get('TYR_HEADER_INSTANTIATION', 'OFF')}",
    )


def get_requires_for_build_wheel(config_settings=None):
    return scikit_build.get_requires_for_build_wheel(config_settings)


def get_requires_for_build_editable(config_settings=None):
    return scikit_build.get_requires_for_build_editable(config_settings)


def prepare_metadata_for_build_wheel(metadata_directory, config_settings=None):
    return scikit_build.prepare_metadata_for_build_wheel(metadata_directory, config_settings)


def build_wheel(wheel_directory, config_settings=None, metadata_directory=None):
    _prepare_native_build()
    return scikit_build.build_wheel(wheel_directory, config_settings, metadata_directory)


def build_editable(wheel_directory, config_settings=None, metadata_directory=None):
    _prepare_native_build()
    return scikit_build.build_editable(wheel_directory, config_settings, metadata_directory)


def build_sdist(sdist_directory, config_settings=None):
    return scikit_build.build_sdist(sdist_directory, config_settings)
