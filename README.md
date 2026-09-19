# LIGHT_SOR_SOLVER

A lightweight, **header-only** C++17 solver for sparse linear systems based on the
**SOR (Successive Over-Relaxation)** iterative method, with **CSR (Compressed Sparse Row)**
storage. Designed for CFD and other sparse linear problems where a full linear-algebra
library is overkill.

## Features

- **Single header, zero dependency** — drop `LSS/light_sor_solver.hpp` into your project and include it.
- **CSR sparse storage** — memory-efficient for large sparse systems; diagonal entries are indexed for O(1) access during iteration.
- **Built-in robustness checks** — NaN/Inf validation, diagonal-element checks, parameter validation, and a complete error-code system.
- **Divergence protection** — periodic residual monitoring with relative/absolute divergence detection (never silently returns garbage).
- **Templated on floating-point types** — works with `float`, `double`, etc. (compile-time `static_assert` enforced).
- **Convenient assembly APIs** — row-by-row (`AddRow`), batch (`AddMultiRow`), or overwrite-style full matrix (`AddMtx`) input; scalar or vector right-hand side.

## Directory Layout

```
LIGHT_SOR_SOLVER/
├── LSS/
│   └── light_sor_solver.hpp   # the whole library (header-only)
├── example/
│   └── example1.cpp           # 10×10 sparse system demo
└── README.md
```

## Quick Start

```cpp
#include <iostream>
#include <vector>
#include "LSS/light_sor_solver.hpp"

using namespace LSS;

int main()
{
    SparseMatrix<double> mtx;

    // 1. Set the matrix size (square, > 2)
    mtx.Reset(3);

    // 2. Assemble the matrix (diagonal entries must be non-zero)
    size_t errRow = 0;
    mtx.AddMtx({
        {4.0, 1.0, 0.0},
        {1.0, 3.0, 1.0},
        {0.0, 1.0, 4.0},
    }, errRow);

    // 3. Set the right-hand side b
    mtx.SetRhs({1.0, 2.0, 3.0});

    // 4. Solve Ax = b with SOR (w = 1.2, max 100 iterations)
    std::vector<double> x(3, 0.0);
    unsigned short ret = mtx.SOR_Solve(x, 100, 1.2);

    if (ret == LSS_OK)
    {
        // x = [0.15, 0.4, 0.65]
        for (double v : x) std::cout << v << "\n";
    }
    return 0;
}
```

Compile with any C++17 compiler:

```bash
g++ -std=c++17 -O2 example/example1.cpp -o example1
```

## API Overview

| Function | Description |
|---|---|
| `Reset(colSize)` | Reset as a `colSize × colSize` square matrix, reserve storage |
| `Clear()` | Clear everything, back to uninitialized state |
| `ClearMtx()` | Clear matrix coefficients only (keep size & RHS, ready to refill) |
| `ClearRhs()` | Zero the right-hand side (keep matrix structure) |
| `AddRow(vec)` | Add one dense row (compressed into CSR internally) |
| `AddMultiRow(mtx, errRow)` | Append rows; on failure, `errRow` points to the bad row |
| `AddMtx(mtx, errRow)` | Overwrite-style batch assembly; size pre-checked |
| `AddRhs(vec)` / `AddRhs(val)` | Accumulate RHS: `rhs += vec` / `rhs[i] += val` |
| `SetRhs(vec)` / `SetRhs(val)` | Overwrite RHS: `rhs = vec` / `rhs[i] = val` |
| `SOR_Solve(x, iter, alphaW, tol)` | Solve `Ax = b`; `x` is input guess / output solution |
| `IsValid()` / `IsFullMatrix()` | Query initialization / readiness state |
| `ColSize()` / `RowSize()` / `NotZeroElemNum()` / `Rhs()` | Query matrix properties |

### SOR_Solve Parameters

| Parameter | Meaning | Valid Range |
|---|---|---|
| `iter` | Max iterations | `[10, 1000]` |
| `alphaW` | Relaxation factor (`1.0` = Gauss-Seidel) | `(0, 2]`, recommended `1.0 ~ 1.5` for SPD matrices |
| `tol` | Convergence tolerance (normalized residual `‖Ax-b‖/‖b‖`) | `(0, 0.1]`, default `1e-6` |

### Error Codes

Returned by all APIs; see the header for the full annotated list.
Key ones: `LSS_OK` (success), `LSS_INI_ERR`, `LSS_VEC_LEN_ERR`, `LSS_MTX_SIZE_ERR`,
`LSS_DIAG_ELEM_ZERO`, `LSS_NAN_ELEM_ERR`, `LSS_SPARSE_ROW_FULL`,
`LSS_SPARSE_ROW_NOT_FULL`, `LSS_SOLVE_SET_ERR`, `LSS_NOT_CONVERGED`, `LSS_DIVERGED`.

## Convergence Notes

- Strict diagonal dominance is a sufficient (not necessary) condition for SOR convergence;
  the solver pre-checks it as a warning only.
- For symmetric positive definite (SPD) matrices, any `alphaW ∈ (0, 2)` guarantees convergence.
- Residuals are monitored every 10 iterations; the solver returns early on convergence
  or divergence, keeping the last iterate in `x`.

## Requirements

- C++17 or later
- No external dependencies

## License

[MIT](LICENSE)
