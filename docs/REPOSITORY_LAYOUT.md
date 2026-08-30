# Repository layout

```text
.
├── README.md
├── STATUS.md
├── docs/
├── paper/
│   ├── source/
│   └── submission/
├── rounds/
├── src/
├── historical-code/
└── artifacts/manifests/
```

`rounds/` contains human-readable checkpoints. `historical-code/` preserves deduplicated programs from the round harnesses. `artifacts/manifests/` is the authoritative index for objects too large or too duplicative for direct Git storage.
