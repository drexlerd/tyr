"""
Parse and traverse planning formalism structures. 

This example demonstrates how to inspect the planning formalism and highlights
some high-level design choices. The provided type hints make it easy to
navigate and explore all structures.

Example usage (run from the repository root):

    python3 python/examples/formalism/planning/invariants.py \
        -d data/gripper/domain.pddl \
        -p data/gripper/test_problem.pddl

Author: Dominik Drexler (dominik.drexler@liu.se)
"""

import argparse

from pathlib import Path

from pytyr.formalism.planning import (
    ParserOptions, 
    Parser,
    Invariant,
    synthesize_invariants
)


def main():
    arg_parser = argparse.ArgumentParser(description="Parse and traverse planning formalism structures.")
    arg_parser.add_argument("-d", "--domain-filepath", type=Path, required=True, help="Path to a PDDL domain file.")
    arg_parser.add_argument("-p", "--task-filepath", type=Path, required=True, help="Path to PDDL task file.")
    args = arg_parser.parse_args()

    domain_filepath : Path = args.domain_filepath
    task_filepath : Path = args.task_filepath

    parser_options = ParserOptions()
    parser = Parser(domain_filepath, parser_options)

    planning_domain = parser.get_domain()

    invariants = synthesize_invariants(planning_domain.get_domain())

    print(invariants)

if __name__ == "__main__":
    main()