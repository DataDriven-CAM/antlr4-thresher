# AI Coding Agent Instructions

Purpose: Help an AI quickly contribute to the family of *thresher* C++ projects (antlr4-thresher, json-thresher, xml-thresher, cifio, cnpm) centered on fast data binding, grammar-driven parsing, and code generation utilities.

## 1. Architecture & Relationships
- Core concept: Each *-thresher* repo provides high-performance binding/parsing over a domain (ANTLR grammars, JSON, XML, CIF) using modern C++ (>= C++26 flags already present) with small focused components.
- `antlr4-thresher` produces a shared lib `libantlr4thresher.so` plus the generator tool `cantlrtool` that turns `.g4` grammars into C++ lexer/parser code placed under include/io/<domain>.
- `json-thresher` & `xml-thresher` depend on generated parsers and expose Binder / Path traversal APIs (pattern: create Binder, build Path, invoke Binder(path, lambda)).
- `cifio` layers additional domain parsing (CIF/MOL2/SMIRKS) and uses both json/xml capabilities and ANTLR-generated parsers.
- `cnpm` is a custom C++ package manager orchestrating builds across these repos; relies on `package.json` plus custom subcommands (`lib`, `test`, domain-specific targets like `urlcpp`, `antlr4`, etc.).

## 2. Key Directories (antlr4-thresher)
- `include/` public headers (namespaces like `sylvanmats::antlr4::parse`).
- `src/parse/` grammar reading (`G4Reader.cpp`).
- `src/dsl/` DSL transformations (`Morpher.cpp`).
- `tool/src/` entrypoint for `cantlrtool`.
- `grammars/` bundled ANTLR grammar sources (ANTLRv4). External grammars (e.g. xpath, xml) pulled from `grammars-v4` module when needed.
- `cpp_modules/` vendored dependencies (fmt, graph-v2, mio, CLI11, antlr4, etc.). Avoid adding system-wide assumptions—include paths wired manually in Makefile.

## 3. Build & Test Workflow
Preferred: use `cnpm` from the workspace root of each project.
```
cnpm install   # fetch/build dependencies (idempotent)
cnpm lib       # builds shared library & tools
cnpm test      # builds & runs doctest-based unit tests
```
Fallback (antlr4-thresher only): `make -j` (targets: default, clean). Output binaries root level.
Environment: GCC >= 13 (some repos mention 13.2.0+), C++26 features (`-std=c++26`). Shared libs use `-fPIC` and set rpath to `$ORIGIN` for the tool.

## 4. Code Generation Pattern
- Invoke `cantlrtool <Lexer.g4> <Parser.g4> -o <outputDir>` to emit C++ headers/sources under `include/io/<name>` (namespace default given in README). Multiple grammar pairs allowed.
- Generated code is then consumed by binder/parsing layers in sibling repos (e.g., xml-thresher requires XPath31 & XML grammar code before building full features).
- Do NOT commit transient generated objects under `build/`; commit stable generated headers if they are part of public API (follow existing precedent in repo—confirm before adding large generated sets).

## 5. API Usage Conventions
- Binder pattern (json/xml): construct `Path`, chain key / wildcard selectors, optional comparison (`==value`), then invoke binder with lambda capturing either key/value or value only.
- Lambda signatures vary: JSON example captures `(std::any& v)` or `(std::string_view& key, std::any& v)`; XML example uses UTF-16 strings and conversion utilities.
- Prefer `std::string_view` / `std::u16string_view` to avoid allocations. Avoid copying large buffers.
- Namespaces are nested under `sylvanmats::io::<domain>` or `sylvanmats::antlr4::parse`.

## 6. Dependencies & Modules
- Internal vendored modules under `cpp_modules/`; include paths set manually (see Makefile). When adding a dependency: place under `cpp_modules/<name>` and mirror existing include/style. No package manager manifests besides `package.json` + cnpm logic.
- External heavy-lifters: antlr4 runtime/tool, fmt (formatting), graph-v2 (graph containers), mio (memory mapped IO), CLI11 (CLI parsing), libgit2/openssl/zlib (in cnpm / network related).

## 7. Adding Features Safely
- Touch only necessary Makefile lines; keep optimization flags and rpath logic intact.
- Expose new public APIs through headers in `include/`; update tool or binders with minimal coupling.
- Prefer doctest for new tests: mimic existing test folder layout (`test/src`, `test/Makefile` when present) and integrate with `cnpm test`.

## 8. Common Pitfalls
- Forgetting to run `cantlrtool` before building downstream repos (xml-thresher needs XPath & XML grammar code present in target include paths).
- Breaking rpath: always retain `-Wl,-rpath,"$$ORIGIN"` when modifying tool linkage.
- Introducing non-view string parameters causing unnecessary copies in hot paths.
- Committing large build artifacts from `build/` or temporary grammar outputs.

## 9. Example Quick Flow (XML Path)
1. In `antlr4-thresher`: generate XPath & XML parsers with `cantlrtool ... -o ./include/io/xpath` & `... -o ./include/io/xml`.
2. Build xml-thresher: `cnpm install && cnpm lib`.
3. Use binder API as per README snippet to fetch node text.

## 10. When Unsure
- Check analogous implementation in sibling thresher repos.
- Read Makefile for include/link patterns before adding flags.
- Prefer minimal diffs; align naming with existing namespaces.

(End)