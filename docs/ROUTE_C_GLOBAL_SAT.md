# Route C — global SAT and proof production

Route C keeps two exact decision interfaces.

## Full extension master

```text
variables  50,480
clauses   663,397
```

This is the direct exact signotope relaxation for a 32-prefix plus rightmost extension.

## Exact depth-three prefix formula

```text
variables   53,194
clauses  1,114,973
```

This asks for a legal 32-prefix that avoids every source-to-sink contradiction path of internal depth at most three.

## Proof policy

A global outcome is accepted only as:

- SAT plus a complete model independently checked against every clause; or
- UNSAT plus a complete DRAT/LRAT/FRAT proof accepted by an independent checker.

Timeouts, `UNKNOWN`, and incomplete proof prefixes remain engineering records only.
