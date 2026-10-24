#if defined(MCPP_FEATURE_SIMD)
#include <array>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
// Independent original SDK reference: deliberately no wrapper linkage shim.
#if defined(MCPP_FEATURE_SIMD_EMULATED)
#define CV_FORCE_SIMD128_CPP 1
#endif
#include <opencv2/core/simd_intrinsics.hpp>
#include "fixtures/simd_fixture.hpp"
int main() { simd_fixture::emit("sdk"); }
#else
int main() { return 0; }
#endif
