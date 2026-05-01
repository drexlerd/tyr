import datetime as dt
import json
import pathlib
import sys

from schema import AttributeType, normalize_attribute_value


def load_json(path: pathlib.Path):
    return json.loads(path.read_text())


def group_cases(cases):
    groups = {"passed": [], "timed_out": [], "failed": [], "not_run": []}
    for case in cases:
        groups[case["status"]].append(case)
    return groups


def load_result_benchmarks(result_file):
    if not result_file:
        return []

    path = pathlib.Path(result_file)
    if not path.exists():
        return []

    return load_json(path).get("benchmarks", [])


def select_case_benchmark(run_name, benchmarks):
    prefix = f"{run_name}/"
    for benchmark in benchmarks:
        if benchmark.get("name", "").startswith(prefix) and benchmark.get("run_type", "iteration") == "iteration":
            return benchmark
    for benchmark in benchmarks:
        if benchmark.get("name", "").startswith(prefix):
            return benchmark
    return None


def is_numeric_attribute(config):
    try:
        attribute_type = AttributeType(config["type"])
    except (KeyError, ValueError):
        return False
    return attribute_type in {AttributeType.FLOAT, AttributeType.INT}


def summarize_domains(cases, attributes):
    summaries = {}
    numeric_attributes = {name: config for name, config in attributes.items() if is_numeric_attribute(config)}

    for case in cases:
        domain = case["domain"]
        summary = summaries.setdefault(
            domain,
            {
                "total": 0,
                "passed": 0,
                "attributes": {name: {"count": 0, "sum": 0} for name in numeric_attributes},
            },
        )
        summary["total"] += 1

        if case.get("status") != "passed":
            continue

        summary["passed"] += 1

        benchmark = select_case_benchmark(case["run_name"], load_result_benchmarks(case.get("benchmark_result_file")))
        if benchmark is None:
            continue

        for attribute_name, config in numeric_attributes.items():
            value = normalize_attribute_value(attribute_name, config, benchmark.get(attribute_name))
            if value is None:
                continue

            attribute_summary = summary["attributes"][attribute_name]
            attribute_summary["count"] += 1
            attribute_summary["sum"] += value

    return dict(sorted(summaries.items()))


def summarize_attributes(domain_summaries, attributes):
    numeric_attributes = {name: config for name, config in attributes.items() if is_numeric_attribute(config)}
    summaries = {name: {"count": 0, "sum": 0} for name in numeric_attributes}

    for domain_summary in domain_summaries.values():
        for attribute_name in numeric_attributes:
            attribute_summary = domain_summary["attributes"][attribute_name]
            summaries[attribute_name]["count"] += attribute_summary["count"]
            summaries[attribute_name]["sum"] += attribute_summary["sum"]

    return summaries


def build_summary(suite, cases, metadata, benchmark_results, benchmark_failures):
    groups = group_cases(cases)
    exit_code = 1 if groups["failed"] else 0
    attributes = suite.get("attributes", {})
    domain_summaries = summarize_domains(cases, attributes)

    return {
        "generated": dt.datetime.now().astimezone().isoformat(timespec="seconds"),
        "metadata": metadata,
        "attributes": attributes,
        "exit_code": exit_code,
        "cases": cases,
        "passed": groups["passed"],
        "timed_out": groups["timed_out"],
        "failed": groups["failed"],
        "not_run": groups["not_run"],
        "benchmark_results": benchmark_results,
        "benchmark_failures": benchmark_failures,
        "attribute_summaries": summarize_attributes(domain_summaries, attributes),
        "domain_summaries": domain_summaries,
        "counts": {
            "passed": len(groups["passed"]),
            "timed_out": len(groups["timed_out"]),
            "failed": len(groups["failed"]),
            "not_run": len(groups["not_run"]),
            "total": len(cases),
        },
    }


def format_summary_value(value):
    if isinstance(value, float):
        return f"{value:.6g}"
    return str(value)


def print_summary(summary, file=None):
    if file is None:
        file = sys.stdout

    attributes = summary.get("attributes", {})
    attribute_names = list(attributes)
    columns = ["domain", "passed/total", *attribute_names]
    rows = []

    for domain, domain_summary in summary.get("domain_summaries", {}).items():
        attribute_summaries = domain_summary.get("attributes", {})
        rows.append(
            [
                domain,
                f"{domain_summary.get('passed', 0)}/{domain_summary.get('total', 0)}",
                *[
                    format_summary_value(attribute_summaries.get(attribute_name, {}).get("sum", 0))
                    for attribute_name in attribute_names
                ],
            ]
        )

    totals = summary.get("attribute_summaries", {})
    counts = summary.get("counts", {})
    rows.append(
        [
            "TOTAL",
            f"{counts.get('passed', 0)}/{counts.get('total', 0)}",
            *[format_summary_value(totals.get(attribute_name, {}).get("sum", 0)) for attribute_name in attribute_names],
        ]
    )

    widths = [len(column) for column in columns]
    for row in rows:
        widths = [max(width, len(cell)) for width, cell in zip(widths, row)]

    def render_row(row):
        return "  ".join(cell.ljust(width) for cell, width in zip(row, widths))

    print(render_row(columns), file=file)
    print(render_row(["-" * width for width in widths]), file=file)
    for row in rows:
        print(render_row(row), file=file)
