# Route B — Factorized Benders reachability and finite convergence

For guarded sources `s -> r_v`, guarded edges `r_u & g -> r_v`, and guarded sinks `r_v & t -> false`, the least unit-propagation closure of the reach variables is exactly the set of nodes reachable through active edges. Hence the factorized Horn block conflicts exactly when an active source-to-sink path exists. Eliminating reach variables yields the conjunction of all path clauses, but the circuit avoids explicit path explosion.

Merging harvested DAGs is sound even when it creates cross-prefix path combinations: every retained source, edge, and sink is an exact clause of the global extension graph, so every recombined path is itself a valid global obstruction.

The frozen extension universe contains 16,705 guarded sources, 189,718 single-guard edges, 18,906 binary-component half-edges, and 3,654 guarded sinks: 228,983 nontrivial guarded atoms. If each residual iteration inserts at least one atom absent from the shared circuit, the structural loop terminates after at most 228,983 additions, at which point it is the exact depth-bounded reachability circuit rather than an empirical cut family.
