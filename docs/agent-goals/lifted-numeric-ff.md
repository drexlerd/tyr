# Feature Goal: Lifted Numeric FF

## Objective
Implement lifted numeric relaxed planning support for a metric-FF-style heuristic.

## Literature References
- [The FF Heuristic for Lifted Classical Planning](https://ojs.aaai.org/index.php/AAAI/article/view/21206)
  - Relevance: Lifted FF for classical planning.
- [The Metric-FF Planning System: Translating "Ignoring Delete Lists" to Numeric State Variables](https://www.jair.org/index.php/jair/article/view/10360/24783)
  - Relevance: Ground FF for numeric planning.
- [Interval Based Relaxation Heuristics for Numeric Planning with Action Costs](https://gki.informatik.uni-freiburg.de/papers/aldinger-nebel-ki2017.pdf)
  - Relevance: The main idea that we want to implement but in the lifted setting

## Scope
- In scope:
  - Interval-based numeric expression evaluation.
  - Tests for numeric effects and mixed propositional/numeric applicability.
  - Supporting the general numeric planning fragment.
  - We will enforce monotonicity during rule evaluation by lower and upper bounds of numeric intervals
- Out of scope:
  - Matching the informativity of Metric-FF due to the generality.

## Success Criteria
- Core builds successfully.
- Numeric interval semantics are covered by unit tests.
- The heuristic handles at least one small numeric planning task end to end.
- The implementation documents its relaxation assumptions.

## Constraints
- Keep `main` stable.
- Reuse existing code where practical.

## Notes
- The relaxation intentionally widens lower/upper bounds and may be imprecise.
- Compile with 8 cores only.

## Tasks
The agent should work on the highest unchecked task in this list. The agent must not mark tasks as complete.

- [x] Extend Datalog Rule and GroundRule two support two head types: Atom and NumericEffectOperator.
  - Interesting files:
    - `docs/agent-goals/lifted-numeric-ff.md`
    - `include/tyr/formalism/planning/declarations.hpp`
    - `include/tyr/formalism/planning/numeric_effect_data.hpp`
    - `include/tyr/formalism/planning/ground_numeric_effect_data.hpp`
    - `include/tyr/planning/programs/`
    - `include/tyr/formalism/planning/`
  - Likely modification areas:
    - `include/tyr/formalism/datalog/rule.hpp`
    - `include/tyr/formalism/datalog/ground_rule.hpp`
  - Validation:
    - Build and test suite passes
  - Notes:
    - Represent numeric effects in the datalog component,i.e., it should mirror how it is done in the `formalism/planning` component.
    - We want Rule and GroundRule to have `cista::offset::variant` head.
    - We want the RPGProgram in `planning/programs/rpg.hpp` to also translate numeric effects into respective datalog rules

- [x] Patch semi-naive evaluation to support two head types: Atom and NumericEffectOperator.
  - Interesting files:
    - `docs/agent-goals/lifted-numeric-ff.md`
    - `include/tyr/planning/programs/`
    - `include/tyr/formalism/datalog/`
    - `include/tyr/datalog/bottom_up.hpp`
    - `src/datalog/bottom_up.cpp`
    - `include/tyr/datalog/workspaces/program.hpp`
    - `src/datalog/workspaces/program.cpp`
  - Likely modification areas:
    - `include/tyr/datalog/bottom_up.hpp`
    - `src/datalog/bottom_up.cpp`
    - `include/tyr/datalog/workspaces/program.hpp`
    - `src/datalog/workspaces/program.cpp`
  - Validation:
    - Build and test suite passes
  - Notes:
    - Patch CostBuckets to also store ClosedInterval<float_t> for ground function terms
    - Add is_valid_binding for numeric effects. If the function expression yields NaN the rule is inapplicable.

- [] Implement greedy local support selector/cost aggregation for witness rules with numeric effect head and numeric constraint goals.
  - Interesting files:
    - `docs/agent-goals/lifted-numeric-ff.md`
    - `include/tyr/planning/programs/`
    - `include/tyr/formalism/datalog/`
    - `include/tyr/datalog/bottom_up.hpp`
    - `src/datalog/bottom_up.cpp`
    - `include/tyr/datalog/policies/annotation_types.hpp`
    - `include/tyr/datalog/policies/annotation.hpp`
    - `src/datalog/policies/annotation.cpp`
    - `include/tyr/datalog/policies/termination.hpp`
    - `src/datalog/policies/termination.cpp`
  - Likely modification areas:
    - `include/tyr/datalog/policies/annotation_types.hpp`
    - `include/tyr/datalog/policies/annotation.hpp`
    - `src/datalog/policies/annotation.cpp`
    - `include/tyr/datalog/policies/termination.hpp`
    - `src/datalog/policies/termination.cpp`
  - Validation:
      - Build and test suite passes
      - `rpg_add` profiling suite still runs and reports `initial_h_value`, `cost`, `length`, `num_expanded`, `num_generated`, and `solved`.
    - Notes:
      - Replace the raw `using NumericIntervalAnnotations = std::vector<NumericIntervalAnnotation>` with a reusable flat class for storing RPG numeric nodes as `(function binding, interval,
  annotation/cost)` triples.
      - Keep all internal data structures flat and reusable across heuristic evaluations; avoid nested heap-allocated containers.
      - Add a shared numeric support selector used by both witness annotation and termination/goal-cost computation.
      - The selector should use the input state's numeric values as closest-to-initial reference values.
      - Numeric constraint support should use greedy local target/value selection rather than the current Cartesian-product search over all interval annotation combinations.
      - Numeric effect-head witness annotation must backchain from the produced output interval/target and only accept selected supports that still justify the required output.
      - The same selector/cost aggregation should be used for:
        - witness rules with numeric constraints in the body,
        - witness rules with numeric effect heads,
        - numeric constraints in goal termination cost.

