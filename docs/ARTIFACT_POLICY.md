# Artifact policy

GitHub is used for browsable source, reports, paper files, compact audits, and historical programs.

Several historical objects exceed practical Git repository limits, including 100-200 MB CNFs and large cumulative harness archives. They are therefore recorded in the complete SHA-256 inventory rather than silently omitted. The inventory gives an exact identity for every locally frozen object and enables later publication as GitHub Release or Zenodo assets without changing the paper-facing source.

No missing large object is required to compile the paper. The paper-facing ancillary code reconstructs the principal local counts and transition results independently.
