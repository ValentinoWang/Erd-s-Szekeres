#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

printf '== repository structure ==\n'
test -f README.md
test -f STATUS.md
test -f docs/PROOF_LEDGER.md
test -f paper/source/main.tex
test -f paper/releases/ES7_33_arXiv_source_slim.zip
test -f archives/ES7_33_complete_research_archive_2026-08-30.tar.gz

printf '== frozen hashes ==\n'
echo 'f0734053e5857720183643d2010cb8853a8b485f7bbf62ccd641b5715d00f9d8  paper/releases/ES7_33_arXiv_source_slim.zip' | sha256sum -c -
echo 'e308971434ee54bcb5177c225328bcd9ecd971a62397f697a32b18b15c8d4685  archives/ES7_33_complete_research_archive_2026-08-30.tar.gz' | sha256sum -c -

printf '== paper-facing exact reconstruction ==\n'
bash paper/source/anc/reproduce_local_dynamics.sh

printf '== arXiv source can be unpacked ==\n'
tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT
unzip -q paper/releases/ES7_33_arXiv_source_slim.zip -d "$tmp"
test -f "$tmp/main.tex"
test -f "$tmp/references.tex"

printf 'Archive verification passed.\n'
