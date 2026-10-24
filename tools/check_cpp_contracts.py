#!/usr/bin/env python3
"""Check imported makePtr retains the upstream custom-deleter rejection."""
import argparse
import json
from pathlib import Path
import subprocess
import tempfile

from audit_api import compiler_flags

ROOT = Path(__file__).resolve().parents[1]
parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("--commands", type=Path, default=ROOT / "compile_commands.json")
args = parser.parse_args()
commands = json.loads(args.commands.read_text())
entry = next(item for item in commands if Path(item["file"]).name == "core_cpp_types_test.cpp")
flags = compiler_flags(entry, keep_modules=True)
with tempfile.TemporaryDirectory(prefix="opencv-cpp-contract-") as directory:
    probe = Path(directory) / "contract.cpp"
    def compile_source(source):
        # GCC requires textual standard-library RTTI declarations while
        # instantiating shared_ptr from an imported global-module definition.
        probe.write_text('#include <memory>\n' + source)
        return subprocess.run([entry["arguments"][0], *flags, "-fsyntax-only", str(probe)],
                              cwd=entry["directory"], text=True, stdout=subprocess.PIPE,
                              stderr=subprocess.STDOUT)
    positive = compile_source('import mcpplibs.mwrap.opencv.core;\nint main() { return *cv::makePtr<int>(7); }\n')
    assert positive.returncode == 0, positive.stdout
    negative = compile_source('''import mcpplibs.mwrap.opencv.core;
struct CustomResource {};
namespace cv {
template<> struct DefaultDeleter<CustomResource> {
    void operator()(CustomResource* value) const { delete value; }
};
}
static_assert(cv::has_custom_delete<CustomResource>::value);
int main() { auto pointer = cv::makePtr<CustomResource>(); }
''')
    assert negative.returncode != 0, "makePtr accepted a custom DefaultDeleter"
    assert "Can't use this makePtr with custom DefaultDeleter" in negative.stdout, negative.stdout
print("Imported makePtr accepts ordinary types and rejects custom DefaultDeleter types")
