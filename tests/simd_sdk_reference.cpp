#if defined(MCPP_FEATURE_SIMD)
#include <array>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <opencv2/core/simd_intrinsics.hpp>
#include "fixtures/simd_fixture.hpp"
int main() { simd_fixture::emit("sdk"); }
#else
int main() { return 0; }
#endif
