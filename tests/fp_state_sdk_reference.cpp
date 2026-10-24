#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <opencv2/core.hpp>
#include <opencv2/core/utils/fp_control_utils.hpp>
#if defined(MCPP_FEATURE_VIDEOIO)
#include <opencv2/videoio.hpp>
#endif
#include "fixtures/fp_state_fixture.hpp"
int main() { fp_state_fixture::emit("sdk"); }
