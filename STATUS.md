# Research status

## Global question

For the planar Erdős–Szekeres number `ES(7)`, the lower bound `ES(7) >= 33` is classical. The unresolved task is the matching upper bound `ES(7) <= 33`.

## Current state

`BLOCKED_EXACT`

This means the scoped computations and theorem packages in this repository pass their stated checkers, while the exact global 33-point decision remains open.

## Accepted evidence

- complete human proof;
- exhaustive finite enumeration with an independent implementation;
- checked RUP/LRAT-style certificate;
- exact SAT model with independent clause verification;
- complete UNSAT certificate checked by an independent proof checker.

## Excluded evidence

- timeout or `UNKNOWN`;
- partial proof traces;
- unverified solver output;
- finite sampling presented as global coverage;
- engineering progress percentages presented as mathematical probability.

## Decisive remaining outcomes

1. `UNSAT` for the full exact master, with independently checked DRAT/LRAT/FRAT;
2. a full exact SAT signotope model, followed by realizability analysis;
3. a certified prefix with minimum obstruction depth at least four, refuting the current depth-three endpoint while preserving the exact full master.
