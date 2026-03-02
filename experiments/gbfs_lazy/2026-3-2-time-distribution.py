#! /usr/bin/env python

import matplotlib.pyplot as plt
import json

from collections import defaultdict

import matplotlib.pyplot as plt
import numpy as np

MAX_MEMORY_MB = 20_000_000_000
MAX_AVG_NUM_STATE_VARIABLES = 200_000_000

def main():
    with open("results-combined/2026-1-8-gbfs_lazy-htg-tyr-pl-fd-eval/properties", 'r') as file:
    # with open("results-combined/2026-1-8-gbfs_lazy-tyr-pl-fd-eval/properties", 'r') as file:
    # with open("data/2026-1-8-gbfs_lazy-profiling-classical-1-eval/properties", 'r') as file:
        data = json.load(file)

        task_to_runs = defaultdict(list)
        for task, run in data.items():
            task_to_runs[tuple(run["id"][1:])].append(run)

        sum_runs = 0
        sum_total = 0
        sum_preprocessing = 0
        sum_axiom_t_tot = 0
        sum_succgen_t_tot = 0
        sum_ff_t_tot = 0

        df = []

        for task, runs in task_to_runs.items():
            for run in runs:
                if run["coverage"] == 0:
                    continue
                if run["algorithm"] != "gbfs-lazy-hff-pref-ff-1":
                    continue         

                total_time_ms = run["total_time_ns"] / 1_000_000
                preprocessing_time_ms = run["preprocessing_time_s"] * 1000
                axiom_prog_t_tot_ms = run["axiom_prog_t_tot_ms"]
                succgen_prog_t_tot_ms = run["succgen_prog_t_tot_ms"]
                ff_prog_t_tot_ms = run["ff_prog_t_tot_ms"]

                if total_time_ms < 6000:
                    continue

                sum_runs += 1
                sum_total += total_time_ms
                sum_preprocessing += preprocessing_time_ms
                sum_axiom_t_tot += axiom_prog_t_tot_ms
                sum_succgen_t_tot += succgen_prog_t_tot_ms
                sum_ff_t_tot += ff_prog_t_tot_ms

                df.append((axiom_prog_t_tot_ms + succgen_prog_t_tot_ms + ff_prog_t_tot_ms) / total_time_ms)

        sum_other_t_tot = sum_total - (sum_preprocessing + sum_axiom_t_tot + sum_succgen_t_tot + sum_ff_t_tot)

        print("sum_runs:", sum_runs)
        print("sum_total:", sum_total)
        print("sum_preprocessing:", sum_preprocessing, "fraction:", sum_preprocessing / sum_total)
        print("sum_axiom_t_tot:", sum_axiom_t_tot, "fraction:", sum_axiom_t_tot / sum_total)
        print("sum_succgen_t_tot:", sum_succgen_t_tot, "fraction:", sum_succgen_t_tot / sum_total)
        print("sum_ff_t_tot:", sum_ff_t_tot, "fraction:", sum_ff_t_tot / sum_total)
        print("sum_other_t_tot:", sum_other_t_tot, "fraction:", sum_other_t_tot / sum_total)

        bins = np.linspace(0, 1, 21)
        plt.figure()
        plt.hist(df, bins=bins, edgecolor="black")
        plt.xlabel("Parallelizable fraction p")
        plt.ylabel("Number of runs")
        plt.title("Distribution of parallelization potential")
        plt.savefig("parallel_fraction_histogram.png", dpi=300, bbox_inches="tight")
  
        print("DF:")
        for x in df:
            print(x)

        return

        count_compression_ratio_below_1 = 0

        X = []
        Y = []
        for task, runs in task_to_runs.items():
            list_peak_mem = None
            tree_peak_mem = None
            avg_num_state_variables = None
            for run in runs:
                if "average_num_state_variables" in run and avg_num_state_variables is None:
                    avg_num_state_variables = run["average_num_state_variables"] 

                if run["algorithm"] == "list-lifted-astar-eager-blind":
                    if "state_peak_memory_usage_in_mib" in run:
                        list_peak_mem = run["state_peak_memory_usage_in_mib"]
                    else:
                        list_peak_mem = MAX_MEMORY_MB
                elif run["algorithm"] == "dtdb-s-lifted-astar-eager-blind":
                    if "state_peak_memory_usage_in_mib" in run:
                        tree_peak_mem = run["state_peak_memory_usage_in_mib"]
                    else:
                        tree_peak_mem = MAX_MEMORY_MB

            if list_peak_mem == MAX_MEMORY_MB or tree_peak_mem == MAX_MEMORY_MB:
                continue

            assert avg_num_state_variables is not None
        
            X.append(avg_num_state_variables)
            Y.append(list_peak_mem / tree_peak_mem)

            if list_peak_mem > tree_peak_mem:
                count_compression_ratio_below_1 += 1

        print(" ".join([f"({x:.4f}, {y:.4f})" for x, y in zip(X, Y)]))
        print(count_compression_ratio_below_1)

        plt.scatter(X, Y)
        plt.xlabel("Avg. #state atoms")
        plt.ylabel("Compression ratio")
        plt.title("Scatterplot of Structural vs Memory Efficiency")
        plt.grid(True)
        plt.savefig("scatterplot.png", dpi=300, bbox_inches='tight')

if __name__ == "__main__":
    main()