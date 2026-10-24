# Universal intrinsics

Enable `simd` to import OpenCV's native 128-bit CPU intrinsics with their
original `cv::` vector types and function names:

```toml
[dependencies]
"mcpplibs.mwrap.opencv" = { version = "5.0.0.1", features = ["simd"] }
```

```cpp
import mcpplibs.mwrap.opencv.core.simd;
int main() {
    float input[4]{1, 2, 3, 4}, output[4]{};
    auto sum = cv::v_add(cv::v_load(input), cv::v_setall_f32(2));
    cv::v_store(output, sum);
    return output[0] != 3;
}
```

The root `mcpplibs.mwrap.opencv` import also exposes this surface when `simd`
is selected. It is separate from `all-modules`, which selects sixteen main
algorithm modules. No external SDK, GPU backend or additional ISA flag is added.

The implementation selects the library's compiled baseline: x86_64 SSE2 or
ARM64 NEON, with sixteen-byte vector storage. The imported macros module maps
`CV_SIMD_WIDTH`, `CV_SIMD128`, `CV_SIMD_64F` and related values under
`mcpplibs::mwrap::opencv::macros`. These describe the library module; they do
not change with an importing translation unit's flags. Per-translation-unit
CPU dispatch, AVX2/AVX512 variants and other native architectures still need
separate integration and verification. No empty features
are registered for those implementations.

`simd-emulated` implies `simd` and selects the SDK's software implementation
(`CV_FORCE_SIMD128_CPP`) for this package's public intrinsics. It uses the same
module and `cv::` names, including original `v_reg<T, N>` generic templates.
The package feature union selects one implementation for all consumers; it
does not provide two interchangeable vector ABIs. Textual comparisons must use
the same upstream force macro. The compiled algorithm kernels keep their own
native dispatch configuration.

The emulator adapter includes ordinary core declarations first, then temporarily
maps `CV_INLINE` to `inline` while including the original intrinsics header.
This changes the linkage of namespace function templates needed by module
exports; vector class definitions and implementation bodies remain unchanged.
The original macro is restored afterward. Its SDK reference test uses unmodified
headers without this linkage adaptation. Software mode exports generic memory
templates directly, including on GCC; it does not use native memory forwards.
The pinned SDK's generic `v_reinterpret_as_uint` references a missing
`V_TypeTraits<T>::reinterpret_uint`. Its name remains exported; matching negative
SDK/import probes record the original instantiation failure. Concrete
`v_reinterpret_as_u32` and the other width-specific SDK operations remain usable.

Vector classes and ordinary inline functions refer to original SDK entities.
GCC memory-entry adapters call ordinary producer-compiled forwards: GCC 16
miscompiled an unaligned load through the imported inline intrinsic body.
The adapter retains unaligned input/output support. This introduces a call
boundary for those GCC entries; performance parity has not been measured.
`vx_store` and `vx_store_aligned` keep their compatibility spellings through
explicit forwards. Public aggregation is separate from the header adapter to
avoid GCC's full-header/default-argument merge issue. Full textual SDK header
mixing follows the restrictions in [architecture](architecture.en.md).

`tools/check_simd.py` compares independent original-header/import-only
consumers. It exercises unaligned memory, saturating/wrapping arithmetic,
packing/widening, lane ordering, masks, dot products, rounding, doubles and
interleaved RGB. It also checks every enabled name through direct/root imports.
Pass `--emulated` to check software mode, including eight-lane generic arithmetic,
bitwise operations, double conversion, reductions and rounding shifts.
Native CI covers the declared platforms; pending results remain acceptance
gates. See [coverage](coverage.md) for the current verified scope.
