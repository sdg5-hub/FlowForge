# Contributing

## Quick Start
1. Configure/build:
   - `cmake -S . -B build -DFLOWFORGE_BUILD_TESTS=ON -DFLOWFORGE_BUILD_BENCH=ON`
   - `cmake --build build -j`
2. Run tests:
   - `ctest --test-dir build --output-on-failure`
3. Run benchmark:
   - `./build/flowforge_bench`

## Dev Notes
- Keep simulation deterministic (fixed dt, seedable RNG).
- Prefer SoA and contiguous data for hot loops.
- Keep render/UI out of sim core.
- Add tests for new pathing or neighbor-query behavior.
- Run `clang-format` using repository `.clang-format`.

## PR Checklist
- [ ] Builds on your platform
- [ ] Tests pass
- [ ] Benchmark impact reviewed
- [ ] README/controls updated when behavior changes
