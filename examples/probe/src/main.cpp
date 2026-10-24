// Show what the source-built OpenCV exposes — version, build info, backends.
import std;
import mcpplibs.mwrap.opencv;

int main() {
    std::println("OpenCV {}", cv::getVersionString());
    std::println("SIMD AVX2 available: {}", cv::checkHardwareSupport(CPU_AVX2) != 0);
    std::println("threads: {}", cv::getNumThreads());
    for (auto be : cv::videoio_registry::getBackends())
        std::println("videoio backend: {}", cv::videoio_registry::getBackendName(be));
}
