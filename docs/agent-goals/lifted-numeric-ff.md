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

- [ ] Datalog Rule and GroundRule with numeric head.
  - Interesting files:
    - `docs/agent-goals/lifted-numeric-ff.md`
    - `include/tyr/formalism/planning/declarations.hpp`
    - `include/tyr/formalism/planning/numeric_effect_data.hpp`
    - `include/tyr/formalism/planning/ground_numeric_effect_data.hpp`
  - Likely modification areas:
    - Documentation or design notes for the feature branch.
  - Validation:
    - No build required for documentation-only changes.
  - Notes:
    - Specify interval expression rules, effect widening rules, duplicate effect policy, infinity/epsilon behavior, and applicability interpretation.

- [ ] Add unit tests for interval numeric expression and effect semantics.
  - Interesting files:
    - `tests/unit/formalism/planning/`
    - `include/tyr/formalism/planning/`
    - `src/formalism/planning/`
  - Likely modification areas:
    - New or existing numeric interval/effect test files.
  - Validation:
    - `cmake --build build --target core -j24`
    - Run the targeted numeric unit test executable.
  - Notes:
    - Cover all numeric operators, division by zero-crossing intervals, and monotone bound widening.
