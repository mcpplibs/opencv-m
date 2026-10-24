#!/usr/bin/env python3
"""A failed export diagnostic must never delete or rewrite an API."""
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]


class DiagnosticPolicyTest(unittest.TestCase):
    def test_failure_reports_the_recorded_declaration_without_changing_exports(self):
        with tempfile.TemporaryDirectory(prefix="opencv-export-policy-") as directory:
            root = Path(directory)
            (root / "tools/curated").mkdir(parents=True)
            (root / "src/gen_exports").mkdir(parents=True)
            shutil.copyfile(ROOT / "tools/prune_loop.py", root / "tools/prune_loop.py")
            exports = root / "src/gen_exports/core.inc"
            curated = root / "tools/curated/core.prune.txt"
            exports.write_text("export namespace cv {\nusing cv::Missing;\n}\n")
            curated.write_text("# Reviewed exceptions only.\n")
            before = [hashlib.sha256(path.read_bytes()).hexdigest() for path in (exports, curated)]
            log = root / "failure.log"
            log.write_text("gen_exports/core.inc:2:12: error: no member named 'Missing' in namespace 'cv'\n"
                           "    2 | using cv::Missing;\n")
            report = root / "report.json"
            # No compiler/build tool is available. A replay is read-only even
            # when the export itself is invalid; all files are sandbox fixtures.
            env = dict(os.environ, PATH="")
            result = subprocess.run([sys.executable, str(root / "tools/prune_loop.py"),
                                     "--log", str(log), "--report", str(report)],
                                    env=env, text=True, capture_output=True)
            self.assertEqual(result.returncode, 1, result.stdout + result.stderr)
            self.assertEqual(before, [hashlib.sha256(path.read_bytes()).hexdigest() for path in (exports, curated)])
            recorded = json.loads(report.read_text())
            self.assertEqual(recorded["automatic-api-removals"], 0)
            self.assertEqual(recorded["export-errors"][0]["declaration"], "cv::Missing")
            self.assertEqual(recorded["export-errors"][0]["status"], "requires-review")


if __name__ == "__main__":
    unittest.main()
