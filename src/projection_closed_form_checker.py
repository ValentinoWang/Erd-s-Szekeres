#!/usr/bin/env python3
"""Exact low-width projection checker for the ES(7)=33 right-extension fiber.

This script uses only Python's standard library.  It reconstructs the variable
numbering of the 32-point tight-path prefix, derives the exact K<=3 projection
of the guarded-Horn right-extension formula, counts the factorized K=4 pair-edge
family, writes the complete K=2 DIMACS projection, and verifies two concrete
K=4/K=5 unit-propagation witnesses.

Scope: canonical rank-3 signotope relaxation with point 0 as the left anchor and
point 32 added as the new rightmost point.  The script does NOT prove that the
32-point prefix master is UNSAT; it certifies exact structural subresults.
"""
from __future__ import annotations

import argparse
import hashlib
import itertools
import json
import math
from collections import Counter, defaultdict, deque
from pathlib import Path
from typing import Dict, Iterable, Iterator, List, Sequence, Tuple

N = 32
SIGNS = (1, -1)
LENGTHS = range(3, 8)
O_MAX = math.comb(N, 3)
PREFIX_VARS_EXPECTED = 49_920
EXTENSION_OFFSET = PREFIX_VARS_EXPECTED

Pair = Tuple[int, int]
Clause = Tuple[int, ...]


def feasible_r(length: int, a: int, i: int, j: int) -> bool:
    return a < i < j and i - a >= length - 2


def feasible_p(length: int, a: int, g: int) -> bool:
    return a < g and g - a >= length - 1


def allocate_prefix_vars() -> tuple[dict[tuple[int, int, int], int], dict[tuple[int, int, int, int, int], int], dict[tuple[int, int, int, int], int], int]:
    o = {triple: idx + 1 for idx, triple in enumerate(itertools.combinations(range(N), 3))}
    r: dict[tuple[int, int, int, int, int], int] = {}
    p: dict[tuple[int, int, int, int], int] = {}
    v = O_MAX + 1
    for sign in SIGNS:
        for length in LENGTHS:
            for a, i, j in itertools.combinations(range(N), 3):
                if feasible_r(length, a, i, j):
                    r[(sign, length, a, i, j)] = v
                    v += 1
    for sign in SIGNS:
        for length in LENGTHS:
            for a, g in itertools.combinations(range(N), 2):
                if feasible_p(length, a, g):
                    p[(sign, length, a, g)] = v
                    v += 1
    return o, r, p, v - 1


def pair_local_id(pair: Pair) -> int:
    # combinations(range(32),2) ordering, 1-based
    i, j = pair
    if not (0 <= i < j < N):
        raise ValueError(pair)
    before = i * (2 * N - i - 1) // 2
    return before + (j - i)


def x_global(pair: Pair) -> int:
    return EXTENSION_OFFSET + pair_local_id(pair)


def source_groups(edge: Pair, o: dict, r: dict) -> list[tuple[int, str]]:
    i, j = edge
    if i == 0:
        return []  # anchored extension variable, eliminated as true
    out: list[tuple[int, str]] = [(o[(0, i, j)], "orientation")]
    if i >= 3:
        out.append((-r[(-1, 5, 0, i, j)], "R-5-a0"))
    for a in range(0, i - 3):
        out.append((-r[(-1, 6, a, i, j)], "R-6"))
    return out


def sink_groups(edge: Pair, r: dict) -> list[tuple[int, str]]:
    i, j = edge
    if i == 0:
        return []
    return [(-r[(1, 6, a, i, j)], "R+6") for a in range(0, i - 3)]


def s(i: int) -> int:
    return 1 + int(i >= 3) + max(i - 3, 0)


def t(i: int) -> int:
    return max(i - 3, 0)


def canonical_clause(lits: Iterable[int]) -> Clause:
    vals = sorted(set(lits), key=lambda z: (abs(z), z < 0))
    if any(-x in vals for x in vals):
        raise ValueError(f"tautology: {vals}")
    return tuple(vals)


def generate_k2(o: dict, r: dict) -> tuple[list[Clause], Counter]:
    clauses: set[Clause] = set()
    classes: Counter = Counter()
    for i in range(1, N - 1):
        for j in range(i + 1, N):
            edge = (i, j)
            for sg, sk in source_groups(edge, o, r):
                for tg, tk in sink_groups(edge, r):
                    clause = canonical_clause((sg, tg))
                    clauses.add(clause)
                    classes[f"{sk}__{tk}"] += 1
    return sorted(clauses), classes


def implication_groups(o: dict, r: dict) -> Iterator[tuple[int, str, list[tuple[Pair, Pair]]]]:
    # Orientation groups.  Each list is already transitively closed.
    for a, b, c in itertools.combinations(range(1, N), 3):
        B, C, D = (a, b), (a, c), (b, c)
        # Prefix orientation +: D -> C -> B.
        yield -o[(a, b, c)], "O+", [(D, C), (C, B), (D, B)]
        # Prefix orientation -: B -> C -> D.
        yield o[(a, b, c)], "O-", [(B, C), (C, D), (B, D)]
    # Mixed path groups with one prefix state.
    for a in range(1, N):
        for h in range(a + 3, N):
            for u in range(h + 1, N):
                target = (h, u)
                sources = [(a, z) for z in range(a + 1, N)]
                yield -r[(-1, 5, a, h, u)], "R-5", [(src, target) for src in sources]
                yield -r[(1, 5, a, h, u)], "R+5", [(target, dst) for dst in sources]


def k3_metrics(o: dict, r: dict) -> dict:
    by_group = Counter()
    by_source = Counter()
    by_sign_pattern = Counter()
    explicit_edges = 0
    closure_pairs = 0
    group_count = 0
    total = 0
    for label, kind, relation in implication_groups(o, r):
        group_count += 1
        closure_pairs += len(relation)
        explicit_edges += len(relation) - (1 if kind in ("O+", "O-") else 0)
        label_positive = label > 0
        for src, dst in relation:
            src_groups = source_groups(src, o, r)
            dst_groups = sink_groups(dst, r)
            contribution = len(src_groups) * len(dst_groups)
            total += contribution
            by_group[kind] += contribution
            for sg, sk in src_groups:
                by_source[sk] += len(dst_groups)
                positives = int(sg > 0) + int(label_positive)  # sink guards are always negative
                by_sign_pattern[f"positive_literals_{positives}"] += len(dst_groups)
    return {
        "singleton_implication_groups": group_count,
        "singleton_implication_explicit_edges": explicit_edges,
        "singleton_implication_transitive_pairs": closure_pairs,
        "k3_total": total,
        "by_implication_group": dict(sorted(by_group.items())),
        "by_source_group": dict(sorted(by_source.items())),
        "by_sign_pattern": dict(sorted(by_sign_pattern.items())),
    }


def k4_pair_type_a_metrics(o: dict, r: dict) -> dict:
    states: dict[tuple[int, int], list[Pair]] = defaultdict(list)
    for a in range(N):
        for h in range(a + 1, N):
            for u in range(h + 1, N):
                for length in (3, 4):
                    if feasible_r(length, a, h, u):
                        states[(length, a)].append((h, u))
    raw = taut = groups = 0
    total = 0
    by_sign = Counter()
    for a in range(N):
        for plus_len, minus_len in ((3, 4), (4, 3)):
            for src in states[(plus_len, a)]:
                for dst in states[(minus_len, a)]:
                    raw += 1
                    if src == dst:
                        taut += 1
                        continue
                    groups += 1
                    sg = source_groups(src, o, r)
                    tg = sink_groups(dst, r)
                    total += len(sg) * len(tg)
                    for lit, _ in sg:
                        by_sign["one_positive" if lit > 0 else "all_negative"] += len(tg)
    return {
        "raw_pair_state_products": raw,
        "tautological_same_edge_pairs": taut,
        "pair_guard_implication_groups": groups,
        "k4_type_a_total": total,
        "by_sign_pattern": dict(sorted(by_sign.items())),
        "scope_note": "This is the complete one-pair-guard-edge K4 family; K4 paths using two distinct singleton labels are a separate family.",
    }


def unit_propagate(clauses: Sequence[Sequence[int]], assumptions: Sequence[int]) -> dict:
    assignment: dict[int, bool] = {}
    trail: list[int] = []
    q: deque[int] = deque(assumptions)
    while q:
        lit = q.popleft()
        var, val = abs(lit), lit > 0
        if var in assignment:
            if assignment[var] != val:
                return {"conflict": True, "trail": trail, "reason": "opposite assumptions/units"}
            continue
        assignment[var] = val
        trail.append(lit)
        changed = True
        while changed:
            changed = False
            for clause in clauses:
                sat = False
                unassigned: list[int] = []
                for x in clause:
                    v = assignment.get(abs(x))
                    if v is None:
                        unassigned.append(x)
                    elif v == (x > 0):
                        sat = True
                        break
                if sat:
                    continue
                if not unassigned:
                    return {"conflict": True, "trail": trail, "conflict_clause": list(clause)}
                if len(unassigned) == 1:
                    u = unassigned[0]
                    uv, uval = abs(u), u > 0
                    if uv in assignment:
                        if assignment[uv] != uval:
                            return {"conflict": True, "trail": trail, "conflict_clause": list(clause)}
                    else:
                        assignment[uv] = uval
                        trail.append(u)
                        changed = True
    return {"conflict": False, "trail": trail}


def witness_metrics() -> dict:
    # N32-01: source -> pair-guard implication -> sink.
    x451 = EXTENSION_OFFSET + 451
    x494 = EXTENSION_OFFSET + 494
    k4_formula = [
        (-39604, x451),
        (-12599, -28339, -x451, x494),
        (-18851, -x494),
    ]
    k4_projection = (-39604, -12599, -28339, -18851)
    k4_assumptions = tuple(-x for x in k4_projection)
    k4 = unit_propagate(k4_formula, k4_assumptions)

    # N32-02: orientation source -> singleton R-5 edge -> pair edge -> sink.
    x32 = EXTENSION_OFFSET + 32
    x484 = EXTENSION_OFFSET + 484
    x430 = EXTENSION_OFFSET + 430
    k5_formula = [
        (1, x32),
        (-35632, -x32, x484),
        (-14183, -30013, -x484, x430),
        (-18787, -x430),
    ]
    k5_projection = (1, -35632, -14183, -30013, -18787)
    k5_assumptions = tuple(-x for x in k5_projection)
    k5 = unit_propagate(k5_formula, k5_assumptions)

    if not k4["conflict"] or not k5["conflict"]:
        raise AssertionError("witness UP failure")
    return {
        "N32-01-K4": {
            "projected_clause": list(k4_projection),
            "negated_clause_assumptions": list(k4_assumptions),
            "tiny_formula": [list(c) for c in k4_formula],
            "up_result": k4,
            "semantics": [
                "R[-,6,1,21,31] forces x_(21,31)",
                "R[+,4,7,21,31] and R[-,3,7,29,30] activate x_(21,31) -> x_(29,30)",
                "R[+,6,0,29,30] forces not x_(29,30)",
            ],
        },
        "N32-02-K5": {
            "projected_clause": list(k5_projection),
            "negated_clause_assumptions": list(k5_assumptions),
            "tiny_formula": [list(c) for c in k5_formula],
            "up_result": k5,
            "semantics": [
                "orientation O(0,1,2)=- and the anchor force x_(1,2)",
                "R[-,5,1,26,29] activates x_(1,2) -> x_(26,29)",
                "R[+,4,18,26,29] and R[-,3,18,19,31] activate x_(26,29) -> x_(19,31)",
                "R[+,6,0,19,31] forces not x_(19,31)",
            ],
        },
    }


def write_dimacs(path: Path, nvars: int, clauses: Sequence[Clause]) -> str:
    path.parent.mkdir(parents=True, exist_ok=True)
    h = hashlib.sha256()
    with path.open("wb") as f:
        header = f"c Exact K=2 prefix projection of the ES7 right-extension guarded-Horn formula\n"
        header += f"p cnf {nvars} {len(clauses)}\n"
        data = header.encode("ascii")
        f.write(data); h.update(data)
        for clause in clauses:
            line = (" ".join(map(str, clause)) + " 0\n").encode("ascii")
            f.write(line); h.update(line)
    return h.hexdigest()


def build_metrics(write_k2: Path | None = None) -> dict:
    o, r, p, last = allocate_prefix_vars()
    if last != PREFIX_VARS_EXPECTED:
        raise AssertionError((last, PREFIX_VARS_EXPECTED))

    k2_clauses, k2_classes = generate_k2(o, r)
    k3 = k3_metrics(o, r)
    k4 = k4_pair_type_a_metrics(o, r)
    witnesses = witness_metrics()

    source_total = sum(len(source_groups((i, j), o, r)) for i in range(1, N - 1) for j in range(i + 1, N))
    sink_total = sum(len(sink_groups((i, j), r)) for i in range(1, N - 1) for j in range(i + 1, N))

    expected = {
        "prefix_variables": 49_920,
        "source_guard_groups": 4_525,
        "sink_guard_groups": 3_654,
        "k2_total": 58_464,
        "singleton_implication_groups": 16_298,
        "singleton_implication_explicit_edges": 189_718,
        "singleton_implication_transitive_pairs": 198_708,
        "k3_total": 15_736_941,
        "k4_pair_guard_groups": 2_623_282,
        "k4_type_a_total": 399_999_204,
    }
    actual = {
        "prefix_variables": last,
        "source_guard_groups": source_total,
        "sink_guard_groups": sink_total,
        "k2_total": len(k2_clauses),
        "singleton_implication_groups": k3["singleton_implication_groups"],
        "singleton_implication_explicit_edges": k3["singleton_implication_explicit_edges"],
        "singleton_implication_transitive_pairs": k3["singleton_implication_transitive_pairs"],
        "k3_total": k3["k3_total"],
        "k4_pair_guard_groups": k4["pair_guard_implication_groups"],
        "k4_type_a_total": k4["k4_type_a_total"],
    }
    if actual != expected:
        raise AssertionError({"expected": expected, "actual": actual})

    k2_sha = None
    if write_k2 is not None:
        k2_sha = write_dimacs(write_k2, last, k2_clauses)

    prior_templates = 365_066 + 526_332
    metrics = {
        "schema_version": 1,
        "status": "PASS",
        "scope": "Exact low-width projection of the canonical 32-prefix/rightmost-point guarded-Horn extension formula; not a full ES(7)=33 proof.",
        "n_prefix": N,
        "new_point": 32,
        "prefix_variables": last,
        "extension_variables_before_projection": math.comb(N, 2),
        "free_extension_variables_after_anchor": math.comb(N - 1, 2),
        "guarded_horn_factorization": {
            "source_guard_groups": source_total,
            "sink_guard_groups": sink_total,
            **{k: k3[k] for k in ("singleton_implication_groups", "singleton_implication_explicit_edges", "singleton_implication_transitive_pairs")},
            "pair_guard_implication_groups": k4["pair_guard_implication_groups"],
        },
        "exact_projection": {
            "K1": {"minimal_clauses": 0, "complete": True},
            "K2": {"minimal_clauses": len(k2_clauses), "complete": True, "classes": dict(sorted(k2_classes.items())), "dimacs_sha256": k2_sha},
            "K3": {
                "minimal_clauses": k3["k3_total"],
                "complete": True,
                "by_implication_group": k3["by_implication_group"],
                "by_source_group": k3["by_source_group"],
                "by_sign_pattern": k3["by_sign_pattern"],
                "previous_flat_template_clauses": prior_templates,
                "previous_template_coverage_fraction": prior_templates / k3["k3_total"],
                "previous_template_coverage_percent": 100 * prior_templates / k3["k3_total"],
                "missing_from_previous_templates": k3["k3_total"] - prior_templates,
            },
            "K4_type_A": k4,
            "K4_complete": False,
            "K4_incomplete_reason": "Type B paths using two distinct singleton implication labels have not been globally flattened or minimized.",
        },
        "known_prefix_minimum_width_checkpoint": {
            "eleven_round6_prefixes": {"K2": 4, "K3": 5, "K4": 1, "K5": 1},
            "one_hundred_round7_cegar_prefixes_empirical": {"K4": 84, "K5": 14, "K6": 1, "K7": 1},
            "warning": "The 100-prefix distribution is empirical and is not an omission-free theorem about all prefixes.",
        },
        "local_witnesses": witnesses,
        "theorem_closed": False,
        "remaining_critical_obligation": "Prove that every valid convex-seven-free 32-point prefix activates some source-to-sink guarded-Horn path, or produce a standard UNSAT proof for the equivalent prefix master.",
    }
    return metrics


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--write-k2", type=Path)
    ap.add_argument("--json", type=Path)
    args = ap.parse_args()
    metrics = build_metrics(args.write_k2)
    text = json.dumps(metrics, ensure_ascii=False, indent=2, sort_keys=True) + "\n"
    if args.json:
        args.json.parent.mkdir(parents=True, exist_ok=True)
        args.json.write_text(text, encoding="utf-8")
    print(text, end="")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
