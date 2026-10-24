#if defined(MCPP_FEATURE_SIMD)
#include <array>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#if defined(MCPP_FEATURE_SIMD_EMULATED)
#define CV_FORCE_SIMD128_CPP 1
#endif
#include <opencv2/core/simd_intrinsics.hpp>
#include <opencv2/core/hal/intrin_legacy_ops.h>
#include "fixtures/simd_legacy_fixture.hpp"
int main() { simd_legacy_fixture::emit("sdk"); }
#else
int main() { return 0; }
#endif
