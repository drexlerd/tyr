# Tyr: Generalized Planning in C++20 and Python

Tyr is designed to address several challenges in modern planning systems:

1. **Unified grounded and lifted planning** within a type-safe API.

2. **Rapid prototyping** through Python bindings with type hints, backed by a high-performance C++ core.

3. **Support for expressive numeric planning formalisms** across both grounded and lifted reasoning paradigms (see [Supported PDDL Features](docs/PDDL_SUPPORT.md)).

4. **Integration of learning and reasoning** by supporting collections of planning tasks over a shared planning domain.

## Technical Overview

- **PDDL frontend**: Tyr uses [Loki](https://github.com/planning-and-learning/loki) to parse, normalize, and translate PDDL input. The parser is implemented with [Boost](https://www.boost.org/) and provides informative error messages for syntactically invalid input. The normalization pipeline largely follows the approach described in Section 4 of [*Concise finite-domain representations for PDDL planning tasks*](https://ai.dmi.unibas.ch/papers/helmert-aij2009.pdf).

- **Datalog engine**: Tyr implements a parallel semi-naive Datalog engine for lifted successor generation, axiom evaluation, relaxed planning graph heuristics, and task grounding. Its execution model is synchronous and supports both rule-level and grounding-level parallelism.

- **Ground planning**: For grounded tasks, Tyr uses data structures inspired by [*The Fast Downward Planning System*](https://jair.org/index.php/jair/article/view/10457) to efficiently identify applicable actions in a given state. Grounding often yields substantial performance improvements, although it is not always feasible for large tasks.

- **State representation**: Tyr statically analyzes domain and problem files and partitions predicates, functions, and related structures into strongly typed categories such as static, fluent, and derived atoms. This design prevents accidental mixing of conceptually different entities. To represent sequences compactly, Tyr uses tree databases of perfectly balanced binary trees, allowing common subsequences to be shared through shared subtrees. As a special case, Tyr synthesizes finite-domain variables for fluent atoms in grounded planning, largely following the method described in Section 5 of [*Concise finite-domain representations for PDDL planning tasks*](https://ai.dmi.unibas.ch/papers/helmert-aij2009.pdf), enabling more compact storage when grounding is feasible.

- **Memory model**: Tyr stores generated data in hierarchically structured, geometrically growing buffers. For variable-sized objects, it uses [Cista](https://github.com/felixguendling/cista) for serialization and zero-copy deserialization. This design allows derived buffers to inherit data from parent buffers without duplication. For example, multiple tasks can share a domain, and multiple workers can share task data.
  
# Getting Started

The library consists of a **formalism** and a **planning** component. The formalism component is responsible for representing PDDL entities. The planning component provides functionality for implementing search algorithms, as well as off-the-shelf implementations of eager A*, lazy GBFS, and heuristics such as blind, max, add, and FF. Below is a minimal overview of the Python and C++ APIs for implementing custom search algorithms.

## Python Interface

Pytyr is available at [PyPI](https://pypi.org/project/pytyr/) and can be installed with `pip install pytyr`. 

Detailed examples are available in the `python/examples` directory:

- [`structures.py`](python/examples/formalism/planning/structures.py) – Parse and traverse all planning formalism structures.
- [`builder.py`](python/examples/formalism/planning/builder.py) – Create new planning formalism structures.
- [`invariants.py`](python/examples/formalism/planning/invariants.py) – Synthesize invariants, access candidate variable bindings, and match atoms through unification.
- [`astar_eager.py`](python/examples/planning/astar_eager.py) – Use and customize off-the-shelf search algorithms.
- [`gbfs_lazy.py`](python/examples/planning/gbfs_lazy.py) – Implement a custom search algorithm from scratch.

The Python interface for implementing search algorithms is:

```py
# Recommended namespace aliases
from pytyr.common import ExecutionContext
import pytyr.formalism.planning as tfp
import pytyr.planning.lifted as tpl  # pytyr.planning.ground also exists

# Parse and translate a task over a domain.
parser = tfp.Parser("domain.pddl")
# Instantiate a lifted task.
task = tpl.Task(parser.parse_task("problem.pddl"))

# Instantiate a single-threaded execution environment.
execution_context = ExecutionContext(1)

# Instantiate the planning objects. Factories assign unique context indices so
# state views from different state repositories hash and compare correctly.
axiom_evaluator_factory = tpl.AxiomEvaluatorFactory()
state_repository_factory = tpl.StateRepositoryFactory()
successor_generator_factory = tpl.SuccessorGeneratorFactory()
axiom_evaluator = axiom_evaluator_factory.create(task, execution_context)
state_repository = state_repository_factory.create(task, axiom_evaluator)
successor_generator = successor_generator_factory.create(task, execution_context, state_repository)

# Get the initial node (state + metric value)
initial_node = successor_generator.get_initial_node()

# Get the labeled successor nodes (sequence of ground action + node)
labeled_successor_nodes = successor_generator.get_labeled_successor_nodes(initial_node)
```

## C++ Interface

The C++ interface for implementing search algorithms is:

```cpp
#include <tyr/tyr.hpp>

// Recommended namespace aliases.
namespace tfp = tyr::formalism::planning;
namespace tp = tyr::planning;

// Parse and translate a task over a domain.
auto parser = tfp::Parser("domain.pddl");
// Instantiate a lifted task.
auto task = tp::Task<tp::LiftedTag>::create(parser.parse_task("problem.pddl"));

// Instantiate a single-threaded execution environment
auto execution_context = tyr::ExecutionContext::create(1);

// Instantiate the planning objects. Factories assign unique context indices so
// state views from different state repositories hash and compare correctly.
auto axiom_evaluator_factory = tp::AxiomEvaluatorFactory<tp::LiftedTag>();
auto state_repository_factory = tp::StateRepositoryFactory<tp::LiftedTag>();
auto successor_generator_factory = tp::SuccessorGeneratorFactory<tp::LiftedTag>();

auto axiom_evaluator = axiom_evaluator_factory.create(task, execution_context);
auto state_repository = state_repository_factory.create(task, axiom_evaluator);
auto successor_generator = successor_generator_factory.create(task, execution_context, state_repository);

// Get the initial node (state + metric value).
auto initial_node = successor_generator->get_initial_node();

// Get the labeled successor nodes (sequence of ground action + node).
auto labeled_successor_nodes = successor_generator->get_labeled_successor_nodes(initial_node);

```

## Dependencies

Tyr consumes native dependencies from Python packages:

```console
uv pip install pyyggdrasil>=0.0.8 pypddl>=1.0.4
cmake -S . -B build \
  -DPython_EXECUTABLE="$(python -c 'import sys; print(sys.executable)')" \
  -DPython3_EXECUTABLE="$(python -c 'import sys; print(sys.executable)')" \
  -DCMAKE_PREFIX_PATH="$(python -c 'import pypddl, pyyggdrasil; print(f"{pypddl.native_prefix()};{pyyggdrasil.native_prefix()}")')"
```

For offline/local development, install the native providers from sibling source
checkouts first:

```console
cd ../yggdrasil
uv pip install --python ../tyr/.venv/bin/python .
cd ../loki
uv pip install --python ../tyr/.venv/bin/python .
```

## Build Instructions

```console
cmake -S . -B build \
  -DPython_EXECUTABLE="$(python -c 'import sys; print(sys.executable)')" \
  -DPython3_EXECUTABLE="$(python -c 'import sys; print(sys.executable)')" \
  -DCMAKE_PREFIX_PATH="$(python -c 'import pypddl, pyyggdrasil; print(f"{pypddl.native_prefix()};{pyyggdrasil.native_prefix()}")')" \
  -DTYR_BUILD_SHARED=ON \
  -DTYR_LINK_STATIC_DEPENDENCIES=OFF

cmake --build build -j16
cmake --install build --prefix=<path/to/installation-directory>
```

Optional targets are disabled by default and can be enabled during configure:

- `-DBUILD_TESTS=ON`
- `-DBUILD_EXECUTABLES=ON`
- `-DBUILD_PROFILING=ON`
- `-DBUILD_PYTYR=ON`

More detailed build instructions are available [here](docs/BUILD.md).

## Integration Instructions

The Python package `pytyr` installs Tyr's native headers, shared library, and
CMake package config under `pytyr.native_prefix()`. It depends on `pypddl` for
Loki and `pyyggdrasil` for third-party native dependencies:

```python
import pypddl
import pyyggdrasil
import pytyr

print(pytyr.native_prefix())
print(pypddl.native_prefix())
print(pyyggdrasil.native_prefix())
```

Downstream CMake projects can then use:

```console
cmake -S . -B build \
  -DCMAKE_PREFIX_PATH="$(python -c 'import pytyr, pypddl, pyyggdrasil; print(f"{pytyr.native_prefix()};{pypddl.native_prefix()};{pyyggdrasil.native_prefix()}")')"
```
