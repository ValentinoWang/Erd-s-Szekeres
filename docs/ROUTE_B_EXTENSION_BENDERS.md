# Route B — exact extension obstructions and Logic-Benders

For a fixed 32-label prefix, variables `x_ij = chi(i,j,32)` describe a rightmost extension. The extension clauses reduce to guarded Horn implications and fixed-prefix 2-SAT.

## Exact criterion

A prefix is extendable exactly when the activated implication graph contains no path from a forced-true source to a forced-false sink.

## Consequences

- explicit path blockers are sound;
- reachability states factorize exponentially many path clauses;
- DAGs harvested from different prefixes can be safely merged because every retained source, edge, and sink belongs to the frozen global extension graph;
- the structural universe is finite, so a monotone completion process terminates after at most `228,983` nontrivial guarded-atom additions.

The explicit cut family is useful as a certificate ledger, but it is not treated as globally complete until the shared structural circuit itself reaches exact coverage.
