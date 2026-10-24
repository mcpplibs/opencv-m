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
CPU dispatch, AVX2/AVX512 variants, other architectures and the software C++
emulator still need separate integration and verification. No empty features
are registered for those implementations.

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
Native CI covers the declared platforms; pending results remain acceptance
gates. See [coverage](coverage.md) for the current verified scope.
