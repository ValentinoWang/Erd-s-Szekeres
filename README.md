# Erdős–Szekeres ES(7)=33 Research Archive

> An auditable archive of the MathArc Research investigation of the planar Erdős–Szekeres problem at the open case `ES(7)=33`.

This repository collects the research process, proof-search workflows, programs, intermediate theorems, exact computations, adversarial audits, and the current arXiv manuscript developed in the accompanying research program.

## Claim boundary

The repository **does not claim a proof of `ES(7)=33`**. The global 33-point SAT/signotope decision remains unresolved. Results marked as theorems are scoped local or extension-structural statements with replayable evidence. Solver timeouts, `UNKNOWN`, partial DRAT traces, and empirical progress percentages are not treated as proofs.

## Main mathematical results currently archived

1. **Tight-path propagation and convexity structure.** Monochromatic consecutive triple orientations force all triples on the path to have the same sign, yielding complementary-path characterizations and fork, ladder, bridge, and Horn obstructions.
2. **Complete seven-element implication layer through width six.** The archive classifies `393,988` width-six prime implicates and gives layered RUP/LRAT-style closure from the four-, five-, and six-element layers.
3. **Propagation-complete local OBDD closure.** A six-clause ITE encoding of a reduced OBDD propagates every local implicate under input partial assignments.
4. **Exact eight- and nine-element dynamics.** There are `1,221,504` convex-seven-free eight-element states and `108,068,130` nine-element models. The exact overlap digraph has `59,441,346` edges, is strongly connected, and contains a directed 2-cycle.
5. **Exact right-extension interface.** For a fixed 32-element prefix, adjoining a rightmost point becomes a guarded-Horn reachability problem / fixed-prefix 2-SAT instance.
6. **Low-width obstruction spectrum.** The exact width-two projection contains `58,464` RUP-certified clauses; higher-width path obstruction families and factorized Benders circuits are archived with independent audits.
7. **Finite structural convergence.** The frozen extension universe contains `228,983` nontrivial guarded atoms, giving a finite monotone completion target for the shared Benders circuit.

## Repository map

- [`paper/`](paper/) — current arXiv manuscript source and submission metadata.
- [`docs/`](docs/) — theorem summaries, route descriptions, claim ledger, process timeline, and audit notes.
- [`rounds/`](rounds/) — round-by-round checkpoint summaries and validation records.
- [`src/`](src/) — directly browsable reconstruction/checking programs.
- [`historical-code/`](historical-code/) — deduplicated programs extracted from the historical harness bundles.
- [`artifacts/manifests/`](artifacts/manifests/) — SHA-256 inventory for all generated ES7 artifacts, including large objects not stored directly in Git.

## Reproduce the paper-facing local dynamics

```bash
cd paper/source/anc
bash reproduce_local_dynamics.sh
```

The paper-facing source is intentionally small enough for arXiv. Large CNFs, explicit cut families, and historical harness bundles are represented by names, sizes, and SHA-256 hashes in the artifact inventory.

## Current manuscript

**Tight Paths, Prime Closure, and Extension Obstructions in Convex-Seven-Free Rank-3 Signotopes**

See [`paper/source/`](paper/source/) and [`paper/submission/SUBMISSION_INFO.zh-CN.md`](paper/submission/SUBMISSION_INFO.zh-CN.md).

## Research status

```text
P0 contract and scoped subfamily goal       COMPLETE
Local seven-element theorem/certificate     COMPLETE
Eight/nine-element exact dynamics           COMPLETE
Fixed-prefix right-extension interface      COMPLETE
Global 33-point SAT/UNSAT decision           OPEN
Complete global DRAT/LRAT/FRAT              ABSENT
```

## Authorship and tool disclosure

Human authors are responsible for every mathematical statement released as a theorem. Generative AI was used for exploratory decomposition, code scaffolding, adversarial hypothesis generation, and manuscript editing. Reported theorems were retained only after deterministic reconstruction, exhaustive enumeration, certificate checking, or explicit human-readable proof.

## License

No blanket license has been selected for the repository. Third-party components retain their original licenses. Add an explicit project license before third-party redistribution or reuse beyond GitHub viewing/forking defaults.
