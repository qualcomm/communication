# Symbol Versioning and Transitive Dependencies Guide

## Understanding Symbol Versioning vs Symbol Inclusion

### Critical Distinction

**Symbol versioning scripts control VISIBILITY, NOT INCLUSION**

```
┌─────────────────────────────────────────────────────────┐
│                  Shared Library (.so)                    │
├─────────────────────────────────────────────────────────┤
│                                                           │
│  ┌─────────────────────────────────────────────┐        │
│  │  Symbols INCLUDED in library                │        │
│  │  (controlled by deps + alwayslink)          │        │
│  │                                              │        │
│  │  ┌────────────────────────────────┐         │        │
│  │  │ Symbols EXPORTED (visible)     │         │        │
│  │  │ (controlled by version script) │         │        │
│  │  │                                 │         │        │
│  │  │  - Public API symbols          │         │        │
│  │  │  - Marked as "global:"         │         │        │
│  │  └────────────────────────────────┘         │        │
│  │                                              │        │
│  │  ┌────────────────────────────────┐         │        │
│  │  │ Symbols HIDDEN (not visible)   │         │        │
│  │  │ (controlled by version script) │         │        │
│  │  │                                 │         │        │
│  │  │  - Internal implementation     │         │        │
│  │  │  - Marked as "local:"          │         │        │
│  │  └────────────────────────────────┘         │        │
│  └─────────────────────────────────────────────┘        │
└─────────────────────────────────────────────────────────┘
```

## How Symbol Inclusion Works

### 1. Symbol Inclusion (Getting symbols INTO the library)

Controlled by:
- **`cc_shared_library.deps`**: Direct dependencies to include
- **`alwayslink = True`**: Forces ALL object files from a library to be linked
- **Transitive dependencies**: Automatically included if referenced by direct deps
- **Linker behavior**: Dead code elimination may remove unreferenced symbols

### 2. Symbol Visibility (Making symbols VISIBLE from the library)

Controlled by:
- **Version script**: Defines which symbols are exported (global) vs hidden (local)
- **`-fvisibility=hidden`**: Compiler flag to hide symbols by default
- **`__attribute__((visibility("default")))`**: Explicit symbol export in code

## Version Script Format

```ld
SCORE_COM_1.0 {
    global:
        # Symbols visible to external users
        *Runtime*;           # Wildcard: all symbols containing "Runtime"
        *ProxyBase*;         # Wildcard: all symbols containing "ProxyBase"
        specific_function;   # Exact symbol name
        
    local:
        # All other symbols are hidden
        *;
};
```

### Wildcards in Version Scripts

- `*` matches any sequence of characters
- `?` matches any single character
- `[abc]` matches any character in the set
- C++ symbols are mangled, so wildcards are essential

Example mangled names:
```
_ZN5score2mw3com7Runtime4InitEv
_ZN5score2mw3com9ProxyBase10FindServiceEv
```

Both would match `*Runtime*` and `*ProxyBase*` respectively.

## QNX Platform Configuration

### QNX Linker Capabilities

QNX 7.x uses a GNU ld-compatible linker that supports:
- ✅ Version scripts (`--version-script`)
- ✅ Symbol visibility control
- ✅ RPATH (`-rpath`)
- ✅ SONAME (`-soname`)

### QNX-Specific Considerations

```python
"@platforms//os:qnx": [
    "-Wl,-soname,libscore_communication.so",           # Library name for runtime linker
    "-Wl,-rpath,'$$ORIGIN'",                           # Search for deps in same directory
    "-Wl,--version-script=$(location :generate_version_script)",  # Symbol versioning
    "-lslog2",                                          # QNX system logging (dynamic)
    "-Wl,--allow-multiple-definition",                 # Handle duplicate symbols
    "-static-libgcc",                                   # Static link GCC runtime
    "-static-libstdc++",                                # Static link C++ stdlib
],
```

**Key Points:**
1. **`-lslog2`**: QNX system library, must be dynamically linked
2. **RPATH**: `$$ORIGIN` allows library to find dependencies in same directory
3. **Version script**: Applied on QNX just like Linux

## Solution for Including Transitive Dependencies

### Problem

```python
cc_library(
    name = "com_dynamic",
    deps = [
        "//score/mw/com:runtime",  # Only direct dep
    ],
)

cc_shared_library(
    name = "score_communication",
    deps = [":com_dynamic"],
)
```

❌ **Result**: Only `runtime` symbols included, NOT its transitive dependencies

### Solution: Comprehensive Aggregation

```python
cc_library(
    name = "com_all_deps",
    alwayslink = True,  # Force inclusion of ALL object files
    deps = [
        # Explicitly list ALL libraries that should be included
        "//score/mw/com:runtime",
        "//score/mw/com:types",
        "//score/mw/com/impl:proxy_base",
        "//score/mw/com/impl:skeleton_base",
        "//score/mw/com/impl/plumbing",
        "//score/mw/com/impl/bindings/lola",
        # ... all other required libraries
    ],
)

cc_shared_library(
    name = "score_communication",
    deps = [":com_all_deps"],
)
```

✅ **Result**: All listed libraries AND their transitive dependencies are included

## Why `alwayslink = True` is Critical

### Without `alwayslink`

```
Linker behavior:
1. Scan object files for undefined symbols
2. Include only object files that resolve undefined symbols
3. Discard unreferenced object files (dead code elimination)
```

**Problem**: If no one directly references a symbol, it won't be included, even if it's needed at runtime (e.g., factory registration, static initializers).

### With `alwayslink = True`

```
Linker behavior:
1. Include ALL object files from libraries marked with alwayslink
2. No dead code elimination for these libraries
3. All symbols are available in the final binary
```

**Benefit**: Ensures all implementation code is present, even if not directly referenced.

## Verification Commands

### Check Included Symbols

```bash
# Build the library
bazel build --config=qnx //score/mw/com/deploy:score_communication

# List all symbols (including hidden ones)
nm bazel-bin/score/mw/com/deploy/libscore_communication.so

# List only exported (visible) symbols
nm -D bazel-bin/score/mw/com/deploy/libscore_communication.so

# Search for specific symbol
nm -D bazel-bin/score/mw/com/deploy/libscore_communication.so | grep Runtime

# Detailed symbol information
objdump -T bazel-bin/score/mw/com/deploy/libscore_communication.so | grep Runtime
```

### Check Dependencies

```bash
# List dynamic dependencies
ldd bazel-bin/score/mw/com/deploy/libscore_communication.so

# On QNX, use:
objdump -p bazel-bin/score/mw/com/deploy/libscore_communication.so | grep NEEDED
```

### Verify Version Script Application

```bash
# Check if version script was applied
readelf -V bazel-bin/score/mw/com/deploy/libscore_communication.so

# Should show version definitions like:
# Version definition section '.gnu.version_d' contains 2 entries:
#   0x0001: Rev: 1  Flags: BASE   Index: 1  Cnt: 1  Name: libscore_communication.so
#   0x0002: Rev: 1  Flags: none   Index: 2  Cnt: 1  Name: SCORE_COM_1.0
```

## Best Practices

### 1. Explicit Dependency Listing

✅ **DO**: List all required libraries explicitly in `com_all_deps`
```python
deps = [
    "//score/mw/com:runtime",
    "//score/mw/com/impl:proxy_base",
    "//score/mw/com/impl/plumbing",
]
```

❌ **DON'T**: Rely on transitive dependency magic
```python
deps = [
    "//score/mw/com:runtime",  # Hoping transitive deps are included
]
```

### 2. Use `alwayslink = True` for Aggregation Libraries

```python
cc_library(
    name = "com_all_deps",
    alwayslink = True,  # Critical for ensuring all symbols are included
    deps = [...],
)
```

### 3. Maintain Version Script

Keep version script in sync with public API:
- Add new public symbols to `global:` section
- Keep implementation details in `local:` section
- Use wildcards for C++ mangled names

### 4. Test Symbol Visibility

```bash
# Verify public API symbols are exported
nm -D libscore_communication.so | grep -E "Runtime|Proxy|Skeleton"

# Verify internal symbols are hidden
nm -D libscore_communication.so | grep -E "impl|internal" || echo "Good: internal symbols hidden"
```

## Common Issues and Solutions

### Issue 1: Missing Symbols at Runtime

**Symptom**: `undefined symbol` error when loading library

**Cause**: Symbol not included in library (not in deps)

**Solution**: Add missing library to `com_all_deps.deps`

### Issue 2: Symbol Conflicts

**Symptom**: Multiple definition errors during linking

**Cause**: Same symbol defined in multiple libraries

**Solution**: Use `-Wl,--allow-multiple-definition` (already in config)

### Issue 3: Symbols Not Exported

**Symptom**: Symbol exists in library but not visible to users

**Cause**: Symbol marked as `local:` in version script

**Solution**: Add symbol pattern to `global:` section

### Issue 4: Library Too Large

**Symptom**: Shared library size is very large

**Cause**: Too many symbols included with `alwayslink = True`

**Solution**: 
1. Review `com_all_deps` - remove unnecessary dependencies
2. Consider dynamic linking for large baselibs
3. Use link-time optimization (LTO) to remove dead code

## Summary

| Aspect | Controlled By | Purpose |
|--------|---------------|---------|
| **Symbol Inclusion** | `cc_shared_library.deps` + `alwayslink` | Get symbols INTO the library |
| **Symbol Visibility** | Version script (`--version-script`) | Control which symbols are EXPORTED |
| **Transitive Deps** | Explicit listing in aggregation library | Ensure all needed code is included |
| **QNX Support** | GNU ld-compatible linker | Same as Linux, with QNX-specific libs |

**Key Takeaway**: Version scripts control visibility, not inclusion. To include transitive dependencies, explicitly list them in an aggregation library with `alwayslink = True`.
