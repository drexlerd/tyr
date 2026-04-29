#!/usr/bin/env python3
import argparse
import datetime as dt
import json
import os
import pathlib
import platform
import re
import subprocess
import sys
import time

from schema import validate_suite


def run_text(command: list[str], cwd: pathlib.Path | None = None):
    result = subprocess.run(command, cwd=cwd, text=True, stdout=subprocess.PIPE, stderr=subprocess.DEVNULL)
    if result.returncode != 0:
        return None
    return result.stdout.strip()


def find_cmake_cache(path: pathlib.Path):
    for candidate in [path, *path.parents]:
        cache = candidate / "CMakeCache.txt"
        if cache.exists():
            return cache
    return None


def read_cmake_cache(cache_path: pathlib.Path | None):
    if cache_path is None:
        return {}

    result = {}
    wanted = {
        "CMAKE_BUILD_TYPE",
        "CMAKE_CXX_COMPILER",
        "CMAKE_CXX_COMPILER_ID",
        "CMAKE_CXX_COMPILER_VERSION",
        "CMAKE_CXX_FLAGS",
    }

    for line in cache_path.read_text(errors="replace").splitlines():
        if line.startswith("//") or line.startswith("#") or "=" not in line:
            continue
        key_with_type, value = line.split("=", 1)
        key = key_with_type.split(":", 1)[0]
        if key in wanted:
            result[key] = value

    result["CMakeCache"] = str(cache_path)
    return result


def read_cmake_compiler_metadata(cache_path: pathlib.Path | None):
    if cache_path is None:
        return {}

    cmake_files = cache_path.parent / "CMakeFiles"
    compiler_files = list(cmake_files.glob("*/CMakeCXXCompiler.cmake"))
    if not compiler_files:
        return {}

    result = {}
    wanted = {
        "CMAKE_CXX_COMPILER_ID",
        "CMAKE_CXX_COMPILER_VERSION",
    }
    pattern = re.compile(r'set\((?P<key>[A-Za-z0-9_]+)\s+"(?P<value>.*)"\)')

    for line in compiler_files[0].read_text(errors="replace").splitlines():
        match = pattern.match(line)
        if match and match.group("key") in wanted:
            result[match.group("key")] = match.group("value")

    return result


def collect_metadata(args, executable: pathlib.Path, test_dir: pathlib.Path, output_dir: pathlib.Path):
    cache_path = find_cmake_cache(test_dir)
    return {
        "git": {
            "commit": run_text(["git", "rev-parse", "HEAD"]),
            "branch": run_text(["git", "rev-parse", "--abbrev-ref", "HEAD"]),
            "dirty": bool(run_text(["git", "status", "--porcelain"])),
        },
        "build": {
            "executable": str(executable),
            "test_dir": str(test_dir),
            **read_cmake_cache(cache_path),
            **read_cmake_compiler_metadata(cache_path),
        },
        "host": {
            "hostname": platform.node(),
            "platform": platform.platform(),
            "cpu_count": os.cpu_count(),
        },
        "runner": {
            "command": sys.argv,
            "suite_json": str(args.suite_json),
            "output_dir": str(output_dir),
            "benchmark_min_time": args.benchmark_min_time,
            "benchmark_repetitions": args.benchmark_repetitions,
            "benchmark_report_aggregates_only": args.benchmark_report_aggregates_only,
            "benchmark_timeout_seconds": args.benchmark_timeout,
        },
    }


def load_suite(suite_json: pathlib.Path):
    suite = json.loads(suite_json.read_text())
    validate_suite(suite)
    return suite


def normalize_stdout(stdout):
    if stdout is None:
        return ""
    if isinstance(stdout, bytes):
        return stdout.decode(errors="replace")
    return stdout


def load_cases(suite):
    cases = []

    for domain_name, domain_config in suite["domains"].items():
        for task_name, task_file in domain_config["tasks"].items():
            cases.append(
                {
                    "run_name": f"{domain_name}/{task_name}",
                    "domain": domain_name,
                    "task": task_name,
                    "domain_file": domain_config["domain_file"],
                    "problem_file": task_file,
                }
            )

    return cases


def group_cases(cases):
    groups = {"passed": [], "timed_out": [], "failed": [], "not_run": []}
    for case in cases:
        groups[case["status"]].append(case)
    return groups


def build_benchmark_command(
    benchmark_executable: pathlib.Path,
    run_name: str,
    min_time: str,
    result_file: pathlib.Path | None,
    repetitions: int | None,
    report_aggregates_only: bool,
):
    command = [
        str(benchmark_executable),
        f"--benchmark_filter=^{run_name}/.*",
        f"--benchmark_min_time={min_time}",
        "--benchmark_format=json",
    ]
    if result_file is not None:
        command.extend(
            [
                f"--benchmark_out={result_file}",
                "--benchmark_out_format=json",
            ]
        )
    if repetitions is not None:
        command.append(f"--benchmark_repetitions={repetitions}")
    if report_aggregates_only:
        command.append("--benchmark_report_aggregates_only=true")
    return command


def run_command(command: list[str], timeout: float):
    started = time.monotonic()
    try:
        result = subprocess.run(command, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, timeout=timeout)
        duration = time.monotonic() - started
        stdout = normalize_stdout(result.stdout)
        exit_code = result.returncode
        status = "passed" if result.returncode == 0 else "failed"
    except subprocess.TimeoutExpired as error:
        duration = time.monotonic() - started
        stdout = normalize_stdout(error.stdout)
        exit_code = None
        status = "timed_out"

    return {
        "command": command,
        "status": status,
        "exit_code": exit_code,
        "duration_seconds": duration,
        "stdout": stdout,
    }


def write_log_entry(log_file, run_name: str, phase: str, result):
    log_file.write(f"===== {run_name} ({phase}: {result['status']}) =====\n")
    log_file.write(result["stdout"])
    if result["stdout"] and not result["stdout"].endswith("\n"):
        log_file.write("\n")


def run_benchmarks(
    benchmark_executable: pathlib.Path,
    output_dir: pathlib.Path,
    cases,
    min_time: str,
    repetitions: int | None,
    report_aggregates_only: bool,
    timeout: float,
):
    results = []
    failures = []
    benchmark_output_dir = output_dir / "benchmark-results"
    benchmark_log_file = output_dir / "benchmark.log"

    with benchmark_log_file.open("w") as benchmark_log:
        for case in cases:
            run_name = case["run_name"]
            result_file = benchmark_output_dir / f"{run_name}.json"
            temp_result_file = result_file.with_name(f"{result_file.name}.tmp")
            result_file.parent.mkdir(parents=True, exist_ok=True)
            result_file.unlink(missing_ok=True)
            temp_result_file.unlink(missing_ok=True)

            command = build_benchmark_command(
                benchmark_executable,
                run_name,
                min_time,
                temp_result_file,
                repetitions,
                report_aggregates_only,
            )
            result = run_command(command, timeout)
            write_log_entry(benchmark_log, run_name, "benchmark", result)

            if result["status"] == "passed":
                temp_result_file.replace(result_file)
                results.append(
                    {
                        "run_name": run_name,
                        "command": command,
                        "status": result["status"],
                        "exit_code": result["exit_code"],
                        "result_file": str(result_file),
                        "duration_seconds": result["duration_seconds"],
                    }
                )
            else:
                temp_result_file.unlink(missing_ok=True)
                reason = "benchmark_timed_out" if result["status"] == "timed_out" else "benchmark_failed"
                failure = {
                    "run_name": run_name,
                    "command": command,
                    "status": result["status"],
                    "exit_code": result["exit_code"],
                    "duration_seconds": result["duration_seconds"],
                    "reason": reason,
                }
                if result["status"] == "timed_out":
                    failure["timeout_seconds"] = timeout
                failures.append(failure)

            case["status"] = result["status"]
            case["benchmark_status"] = result["status"]
            case["benchmark_duration_seconds"] = result["duration_seconds"]
            case["benchmark_exit_code"] = result["exit_code"]
            if result["status"] == "passed":
                case["benchmark_result_file"] = str(result_file)
            else:
                case["benchmark_failure_reason"] = failure["reason"]
                if result["status"] == "timed_out":
                    case["benchmark_timeout_seconds"] = timeout

    return results, failures


def main():
    parser = argparse.ArgumentParser(description="Run each profiling benchmark case with a hard wall-clock timeout.")
    parser.add_argument("--executable", type=pathlib.Path, required=True, help="Profiling benchmark executable to run.")
    parser.add_argument(
        "--output-dir",
        required=True,
        help="Directory for benchmark.log, summary.json, and benchmark result JSON files.",
    )
    parser.add_argument(
        "--suite-json",
        type=pathlib.Path,
        required=True,
        help="Profiling suite JSON with domains, domain files, and task files.",
    )
    parser.add_argument("--benchmark-min-time", default="0.1s", help="Google Benchmark --benchmark_min_time value.")
    parser.add_argument("--benchmark-repetitions", type=int, help="Google Benchmark --benchmark_repetitions value.")
    parser.add_argument(
        "--benchmark-timeout",
        type=float,
        default=60.0,
        help="Hard wall-clock timeout in seconds for each per-case Google Benchmark subprocess.",
    )
    parser.add_argument(
        "--benchmark-report-aggregates-only",
        action="store_true",
        help="Forward --benchmark_report_aggregates_only=true to Google Benchmark.",
    )
    args = parser.parse_args()
    if args.benchmark_timeout <= 0:
        parser.error("--benchmark-timeout must be greater than 0.")

    output_dir = pathlib.Path(args.output_dir)
    output_dir.mkdir(parents=True, exist_ok=True)

    executable = args.executable
    test_dir = executable.parent
    summary_file = output_dir / "summary.json"

    suite = load_suite(args.suite_json)
    cases = load_cases(suite)
    metadata = collect_metadata(args, executable, test_dir, output_dir)

    benchmark_results, benchmark_failures = run_benchmarks(
        executable,
        output_dir,
        cases,
        args.benchmark_min_time,
        args.benchmark_repetitions,
        args.benchmark_report_aggregates_only,
        args.benchmark_timeout,
    )
    groups = group_cases(cases)
    exit_code = 1 if groups["failed"] else 0

    summary = {
        "generated": dt.datetime.now().astimezone().isoformat(timespec="seconds"),
        "metadata": metadata,
        "attributes": suite.get("attributes", {}),
        "exit_code": exit_code,
        "cases": cases,
        "passed": groups["passed"],
        "timed_out": groups["timed_out"],
        "failed": groups["failed"],
        "not_run": groups["not_run"],
        "benchmark_results": benchmark_results,
        "benchmark_failures": benchmark_failures,
        "counts": {
            "passed": len(groups["passed"]),
            "timed_out": len(groups["timed_out"]),
            "failed": len(groups["failed"]),
            "not_run": len(groups["not_run"]),
            "total": len(cases),
        },
    }

    rendered_summary = json.dumps(summary, indent=2)
    summary_file.write_text(rendered_summary + "\n")
    print(rendered_summary)

    return exit_code


if __name__ == "__main__":
    sys.exit(main())
