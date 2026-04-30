# Feature Goal: Lifted Numeric FF

## Objective
Implement lifted numeric relaxed planning support for a metric-FF-style heuristic.

## Literature References
- [The FF Heuristic for Lifted Classical Planning](https://ojs.aaai.org/index.php/AAAI/article/view/21206)
  - Relevance: Lifted FF for classical planning.
- [The Metric-FF Planning System: Translating "Ignoring Delete Lists" to Numeric State Variables](https://www.jair.org/index.php/jair/article/view/10360/24783)
  - Relevance: Ground FF for numeric planning.

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

## Tasks
The agent should work on the highest unchecked task in this list. The agent must not mark tasks as complete.

- [ ] Extend Datalog Rule and GroundRule two support two head types: Atom and NumericEffectOperator.
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

