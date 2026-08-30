#!/usr/bin/env bash
set -euo pipefail
c++ -O3 -std=c++20 -DNDEBUG rebuild_local_dynamics.cpp -o rebuild_local_dynamics
./rebuild_local_dynamics rebuild_local_dynamics.reproduced.json
python3 - <<'PY'
import json
from pathlib import Path
expected=json.loads(Path('rebuild_local_dynamics_result.json').read_text())
actual=json.loads(Path('rebuild_local_dynamics.reproduced.json').read_text())
for d in (expected, actual):
    d.pop('seconds', None)
if expected != actual:
    raise SystemExit('reconstruction mismatch')
print('PASS: exact local dynamics reconstruction matches the archived record')
PY
rm -f rebuild_local_dynamics rebuild_local_dynamics.reproduced.json
