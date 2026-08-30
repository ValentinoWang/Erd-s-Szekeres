# Reproducibility

## Paper-facing rebuild

```bash
cd paper/source/anc
bash reproduce_local_dynamics.sh
```

The C++ reconstruction regenerates the exact local state and transition counts used in the manuscript.

## Historical process code

Deduplicated historical programs extracted from the round harnesses are stored under `historical-code/`.

## Artifact integrity

`artifacts/manifests/full-artifact-inventory.json` records the relative path, byte size, and SHA-256 hash for every ES7 project artifact available in the working archive at freeze time. Large CNFs and historical ZIP bundles are represented by this manifest when they are unsuitable for direct Git storage.

## Trusted computing base

Central claims use one or more of:

- human proof;
- independent exhaustive implementation;
- deterministic exact enumeration;
- RUP/LRAT-style replay;
- SHA-256 frozen artifact identity.
