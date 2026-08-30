# Complete compact research archive

The complete compact archive is stored as Base64 text chunks so it can be committed through the GitHub connector without binary corruption.

## Reassemble

```bash
cat complete-research-archive/part-* > ES7_33_complete_research_archive_2026-08-30.tar.gz.b64
base64 -d ES7_33_complete_research_archive_2026-08-30.tar.gz.b64 > ES7_33_complete_research_archive_2026-08-30.tar.gz
sha256sum ES7_33_complete_research_archive_2026-08-30.tar.gz
```

Expected SHA-256:

```text
e308971434ee54bcb5177c225328bcd9ecd971a62397f697a32b18b15c8d4685
```

The archive contains the theorem-first arXiv source, submission metadata, round reports and validations, key audits, direct checkers, and 162 deduplicated historical Python/C/C++/shell programs extracted from the proof-harness snapshots.

Large raw CNF, DIMACS, LRAT, and cumulative ZIP objects are indexed by SHA-256 inside the archive rather than duplicated in normal Git history.