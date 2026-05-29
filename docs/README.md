# Documentation

This directory contains the Doxygen template used to generate the API docs for
VIPA.

To build the docs locally:

```bash
cmake -S . -B build -DBUILD_DOCS=ON
cmake --build build --target docs
```

The HTML output will be in `build/docs/html`.
