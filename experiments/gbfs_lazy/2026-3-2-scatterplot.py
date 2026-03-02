#! /usr/bin/env python

import json
from pathlib import Path

from lab.experiment import Experiment
from downward.reports.scatter import ScatterPlotReport
from lab.reports import Attribute

### All data points
exp = Experiment("plot-search-time-ms-per-expanded")
exp.add_fetcher("results-combined/2026-1-8-gbfs_lazy-tyr-pl-fd-eval", name="fetch")
exp.add_report(
    ScatterPlotReport(
        attributes=[Attribute("search_time_ms_per_expanded", digits=3)],
        filter_algorithm=["powerlifted-gbfs-lazy-hff-pref-ff", "gbfs-lazy-hff-pref-ff-1"],
        format="png",  # Use "tex" for pgfplots output.
    ),
    name="scatterplot-search-time-ms-per-expanded-png",
)
exp.add_report(
    ScatterPlotReport(
        attributes=[Attribute("search_time_ms_per_expanded", digits=3)],
        filter_algorithm=["powerlifted-gbfs-lazy-hff-pref-ff", "gbfs-lazy-hff-pref-ff-1"],
        format="tex",  # Use "tex" for pgfplots output.
    ),
    name="scatterplot-search-time-ms-per-expanded-tex",
)
exp.run_steps()


TYR_DIR = "results_raw/tyr-2026-1-8-gbfs_lazy-combined-eval"
PL_DIR  = "results_raw/pl-2026-1-9-lazy-gbfs-ff-pref-ff-eval"
TYR_ALGO = "gbfs-lazy-hff-pref-ff-1"

def df(run) -> float:
    total_ms = run["total_time_ns"] / 1_000_000.0
    if total_ms <= 0:
        return 0.0
    ax = float(run.get("axiom_prog_t_tot_ms", 0.0))
    sg = float(run.get("succgen_prog_t_tot_ms", 0.0))
    ff = float(run.get("ff_prog_t_tot_ms", 0.0))
    return (ax + sg + ff) / total_ms

def task_key(run):
    return tuple(run["id"][1:])  # (domain, problem, ...)

def allowed_tasks_from_tyr(predicate) -> set:
    props = json.load(open(Path(TYR_DIR) / "properties"))
    allowed = set()
    for run in props.values():
        if run.get("algorithm") != TYR_ALGO:
            continue
        if run["coverage"] == 0:
            allowed.add(task_key(run))
            continue
        if predicate(df(run)):
            allowed.add(task_key(run))
            continue
    return allowed

def make_task_filter(allowed):
    def _f(run):
        return task_key(run) in allowed
    return _f

# ---- choose one bucket here ----
allowed_lt_05 = allowed_tasks_from_tyr(lambda x: x < 0.5)
allowed_ge_05 = allowed_tasks_from_tyr(lambda x: x >= 0.5)


### df < 0.5
allowed = allowed_lt_05

exp = Experiment("plot-search-time-ms-per-expanded-df-lt-0-5")
task_filter = make_task_filter(allowed)

exp.add_fetcher(TYR_DIR, name="fetch-tyr", filter=task_filter)
exp.add_fetcher(PL_DIR,  name="fetch-pl",  filter=task_filter)

exp.add_report(
    ScatterPlotReport(
        attributes=[Attribute("search_time_ms_per_expanded", digits=3)],
        filter_algorithm=["powerlifted-gbfs-lazy-hff-pref-ff", "gbfs-lazy-hff-pref-ff-1"],
        format="png",
    ),
    name="scatterplot-png",
)
exp.add_report(
    ScatterPlotReport(
        attributes=[Attribute("search_time_ms_per_expanded", digits=3)],
        filter_algorithm=["powerlifted-gbfs-lazy-hff-pref-ff", "gbfs-lazy-hff-pref-ff-1"],
        format="tex",
    ),
    name="scatterplot-tex",
)

exp.run_steps()

### df >= 0.5
allowed = allowed_ge_05

exp = Experiment("plot-search-time-ms-per-expanded-df-ge-0-5")
task_filter = make_task_filter(allowed)

exp.add_fetcher(TYR_DIR, name="fetch-tyr", filter=task_filter)
exp.add_fetcher(PL_DIR,  name="fetch-pl",  filter=task_filter)

exp.add_report(
    ScatterPlotReport(
        attributes=[Attribute("search_time_ms_per_expanded", digits=3)],
        filter_algorithm=["powerlifted-gbfs-lazy-hff-pref-ff", "gbfs-lazy-hff-pref-ff-1"],
        format="png",
    ),
    name="scatterplot-png",
)
exp.add_report(
    ScatterPlotReport(
        attributes=[Attribute("search_time_ms_per_expanded", digits=3)],
        filter_algorithm=["powerlifted-gbfs-lazy-hff-pref-ff", "gbfs-lazy-hff-pref-ff-1"],
        format="tex",
    ),
    name="scatterplot-tex",
)

exp.run_steps()
