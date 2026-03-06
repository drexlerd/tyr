
# Tyr

Tyr is a C++20-based generalized planning library with Python bindings, designed for both grounded and lifted planning. At its core, it reimplements a subset of [Mimir](https://github.com/simon-stahlberg/mimir) with a simplified API and a parallelizable architecture.

Internally, Tyr is built around a weighted, annotated, and parallelizable Datalog solver with a grounder based on k-clique enumeration in k-partite graphs (KPKC). Core reasoning problems in planning, such as task grounding, lifted axiom evaluation, enumerating applicable actions, and computing relaxed planning graph heuristics, are compiled into Datalog programs. These programs are evaluated using a parallel bottom-up semi-naive solver.

# Key Features

- **Rich PDDL Support:**
  Tyr meets a wide range of Planning Domain Definition Language (PDDL) requirements:
  - `:action-costs`
  - `:conditional-effects`
  - `:derived-predicates`
  - `:disjunctive-preconditions`
  - `:equality`
  - `:existential-preconditions`
  - `:negative-preconditions`
  - `:numeric-fluents`
  - `:strips`
  - `:typing`
  - `:universal-preconditions`

- **Grounded and Lifted Planning:**
  Tyr supports the PDDL requirements for both grounded and lifted planning. In particular, lifted planning is critical for learning-based approaches, which often do not require access to grounded actions. This can result in significant speedups.

- **Collections of Problems:**
  Tyr supports parsing, translation, and in-memory manipulation for collections of problems.

- **Efficiency:**
  Tyr can also be included as a library in C++ projects, which is recommended for optimal performance.
  
# Getting Started

## 1. Installation

Instructions on how to build the library and executables is available [here](docs/BUILD.md).

## 2. PDDL Interface

The high level C++ planning interface is as follows:

## 2.1. Lifted Planning

We obtain a lifted task by parsing the PDDL. Then, we translate the lifted task into three datalog program: P1) ground task program, P2) action program, P3) axiom program. Program P1 encodes the delete free task to approximate the set of applicable ground actions and axioms in the task. Program P2 encodes the action preconditions to overapproximate the set of ground applicable actions in a state. Program P3 encodes the axiom evaluation in a state. Given these three programs, the API allows to retrieve the extended initial node (sparse state + metric value) using program P3. Given a node, compute the labeled successor nodes (ground action + node) using programs P2 and P3.

```cpp
#include <tyr/tyr.hpp>

auto parser = tyr::formalism::Parser("domain.pddl");
auto task = parser.parse_task("problem.pddl");
auto successor_generator = SuccessorGenerator<tyr::planning::LiftedTask>(task);

// Get the initial node (state + metric value)
auto initial_node = successor_generator.get_initial_node();

// Get the labeled successor nodes (sequence of ground action + node)
auto successor_nodes = successor_generator.get_labeled_successor_nodes(initial_node);

```

## 2.2. Grounded Planning

From the lifted task and using program P1, we can compute a ground task that overapproximates the delete-free reachable ground atoms, actions, and axioms. From those, we may derive mutex groups, enabling us to form a more compact finite-domain representation (FDR). The remaining interface remains identical, but uses FDR instead of a sparse state representation.

```cpp
#include <tyr/tyr.hpp>

auto parser = tyr::formalism::Parser("domain.pddl");
auto task = parser.parse_task("problem.pddl");

// Ground the task
auto ground_task = task.instantiate_ground_task();
auto successor_generator = SuccessorGenerator<tyr::planning::GroundTask>(ground_task);

// Get the initial node (state + metric value)
auto initial_node = successor_generator.get_initial_node();

// Get the labeled successor nodes (sequence of ground action + node)
auto successor_nodes = successor_generator.get_labeled_successor_nodes(initial_node);

```
