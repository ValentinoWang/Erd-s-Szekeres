# arXiv source archive

The current theorem-first slim source is stored as Base64 text chunks under `paper/arxiv-source-b64/`.

```bash
cat paper/arxiv-source-b64/part-* > ES7_33_arXiv_source.zip.b64
base64 -d ES7_33_arXiv_source.zip.b64 > ES7_33_arXiv_source.zip
sha256sum ES7_33_arXiv_source.zip
unzip ES7_33_arXiv_source.zip
pdflatex -interaction=nonstopmode -halt-on-error main.tex
pdflatex -interaction=nonstopmode -halt-on-error main.tex
pdflatex -interaction=nonstopmode -halt-on-error main.tex
```

Expected SHA-256:

```text
f0734053e5857720183643d2010cb8853a8b485f7bbf62ccd641b5715d00f9d8
```

The upload ZIP is the slim arXiv version: no nested certificate archives, no compressed CNFs, and no dependency on ancillary files during compilation.