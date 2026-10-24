#!/usr/bin/env python3
"""Regression checks for the C++ inventory (not an API completeness claim)."""
import io
import json
from pathlib import Path
import tempfile
import unittest

from audit_api import ast_declarations, inventory


class InventoryTest(unittest.TestCase):
    def test_streams_translation_unit_children_and_rejects_truncation(self):
        tree = {"kind": "TranslationUnitDecl", "inner": [
            {"kind": "NamespaceDecl", "name": "cv", "inner": [
                {"kind": "CXXRecordDecl", "name": "Mat"}]},
            {"kind": "FunctionDecl", "name": "cvRound"}]}
        capture = io.StringIO()
        self.assertEqual(list(ast_declarations(io.StringIO(json.dumps(tree, indent=2)), capture)), tree["inner"])
        self.assertEqual(json.loads(capture.getvalue()), tree)
        with self.assertRaisesRegex(AssertionError, "Truncated"):
            list(ast_declarations(io.StringIO('    {\n      "kind": "FunctionDecl"\n')))

    def test_file_elision_nested_headers_and_function_bodies(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            a = str(root / "upstream/opencv/modules/core/include/opencv2/core.hpp")
            b = str(root / "upstream/opencv/modules/geometry/include/opencv2/geometry.hpp")
            namespace = {"id": "n", "kind": "NamespaceDecl", "name": "cv",
                "loc": {"offset": 0, "file": a}, "inner": [
                    {"kind": "CXXRecordDecl", "name": "Alpha", "loc": {"offset": 10}},
                    {"kind": "CXXRecordDecl", "name": "Beta", "loc": {"offset": 0, "file": b,
                        "includedFrom": {"file": a}}},
                    {"kind": "CXXRecordDecl", "name": "Gamma", "loc": {"offset": 20}},
                    {"kind": "FunctionDecl", "name": "helper", "loc": {"offset": 40},
                     "inner": [{"kind": "CompoundStmt", "range": {"end": {"offset": 90, "file": a}}}]},
                    {"kind": "CXXRecordDecl", "name": "Tail", "loc": {"offset": 100}}]}
            found = inventory(iter([namespace]), root)
            self.assertEqual(found["cv::Alpha"]["modules"], ["core"])
            self.assertEqual(found["cv::Beta"]["modules"], ["geometry"])
            self.assertEqual(found["cv::Gamma"]["modules"], ["geometry"])
            self.assertEqual(found["cv::Tail"]["modules"], ["core"])

    def test_namespaces_enumerators_overloads_internal_linkage_and_members(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            header = str(root / "upstream/opencv/modules/dnn/include/opencv2/dnn.hpp")
            nested = {"id": "abi", "kind": "NamespaceDecl", "name": "dnn5_v20260605", "inner": [
                {"kind": "CXXRecordDecl", "name": "Net"},
                {"kind": "FunctionDecl", "name": "call", "type": {"qualType": "int (int)"}},
                {"kind": "FunctionDecl", "name": "call", "type": {"qualType": "int (float)"}},
                {"kind": "FunctionTemplateDecl", "name": "helper", "inner": [
                    {"kind": "FunctionDecl", "storageClass": "static", "type": {"qualType": "T (T)"}}]},
                {"kind": "EnumDecl", "name": "Mode", "inner": [
                    {"kind": "EnumConstantDecl", "name": "MODE_CPU"}]},
                {"kind": "EnumDecl", "name": "Scoped", "scopedEnumTag": "class", "inner": [
                    {"kind": "EnumConstantDecl", "name": "HIDDEN"}]},
                {"kind": "FunctionDecl", "name": "forward", "parentDeclContextId": "class-net"}]}
            tree = {"id": "cv", "kind": "NamespaceDecl", "name": "cv", "loc": {"offset": 0, "file": header},
                    "inner": [{"id": "dnn", "kind": "NamespaceDecl", "name": "dnn", "inner": [nested,
                        {"kind": "UsingDirectiveDecl", "nominatedNamespace": {"id": "abi", "name": "dnn5_v20260605"}}]}]}
            found = inventory(iter([tree]), root)
            self.assertEqual(found["cv::dnn::dnn5_v20260605::Net"]["public-name"], "cv::dnn::Net")
            self.assertEqual(found["cv::dnn::dnn5_v20260605::call"]["signatures"], ["int (float)", "int (int)"])
            self.assertTrue(found["cv::dnn::dnn5_v20260605::helper"]["internal-linkage"])
            self.assertEqual(found["cv::dnn::dnn5_v20260605::helper"]["signatures"], ["T (T)"])
            self.assertIn("cv::dnn::dnn5_v20260605::MODE_CPU", found)
            self.assertFalse(any(name.endswith(("HIDDEN", "forward")) for name in found))


if __name__ == "__main__":
    unittest.main()
