// Intentionally textual-only: no module is imported into the SDK oracle.
#include <opencv2/core.hpp>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include "fixtures/persistence_fixture.hpp"
int main() {
    persistence_fixture::emit("sdk");
}
