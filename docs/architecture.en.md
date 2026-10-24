# opencv-m architecture

[中文](architecture.md)

The package `mcpplibs.mwrap.opencv` wraps OpenCV 5.0.0 for C++23 modules.
The wrapper revision is 5.0.0.1. Upstream `cv::` names and type identities remain
unchanged; package/module identity identifies the adaptation layer. This
checkpoint does not claim complete public API or hardware coverage.

## Source ownership

- `upstream/opencv`: retained official source and headers, byte-identical to the
  SHA256-pinned archive in `upstream/sources.lock.toml`. All sixteen main C++
  modules are imported. Bindings, test frameworks and contrib are separate scope.
- `port`: configuration inputs, source catalogs and compatibility headers.
- `generated`: stable configuration/dispatch/resources committed for reproducible
  builds. Feature-specific files belong to the build output directory.
- `src`: public interfaces and explicit adapters for internal-linkage entities.
- `templates`, `tests`, `tools`: executable consumers, behavior checks, import,
  generation and coverage audits.

OpenCV implementation objects have one owner. FFmpeg and ORT come from declared
providers; an additional compat.opencv implementation must not be linked beside
the vendored implementation. Wrapper/SDK layers do not fetch dependencies while
compiling module interfaces.

## Public interfaces and internal adapter units

`src/core.cppm` exports upstream core entities plus the explicit operator/helper
adaptations needed for namespace-level static-inline functions. Values and pure
macro calculations live in `mcpplibs.mwrap.opencv.macros`; raw preprocessing
macros remain available through `<mwrap/opencv/macros.hpp>` before imports.

Each other public module is a generated aggregation unit. For example,
`mcpplibs.mwrap.opencv.stitching` re-exports core, its required module dependencies
and the independent `mcpplibs.mwrap.opencv.detail.stitching` adapter. The internal
unit's GMF includes upstream headers and its export-using list. The `detail`
module family is an implementation detail; upstream `cv::detail` stitching APIs
remain public C++ APIs and retain that namespace.

Keeping textual GMFs and dependency imports in separate units avoids GCC 16's
SVD default-argument conflict when core.hpp and import core meet in one unit.
Interface partitions also triggered a GCC 16 merge ICE in this graph; independent
named adapter units passed the direct-import behavior tests. No compiler/engine
source patch or upstream-header modification is required for this layout.

The root interface exports core/macros and a configuration-specific selection
module. That selection has direct imports, including ORT when requested, so
module scanning and GCC BMI mapper paths see the complete dependency graph.
A module's source existing on disk does not activate its feature.

## Build generation and feature graph

`tools/generate_build.py` derives required module relationships from the pinned
CMake module definitions, generates public aggregation interfaces, the manifest
and per-platform implementation catalogs. Adapter units are maintained separately
and the generator preserves their content. `build.mcpp` selects catalogs and
writes private configuration into its output directory; consumers run no CMake.

Defaults are empty; core is mandatory. `all-modules` selects the sixteen main
module implementations, while codecs, native windows/video and external DNN
SDKs stay explicit. `highgui-native` selects GTK3/Win32/Cocoa; `videoio-native`
selects V4L2/MSMF/AVFoundation. Explicit backend names reject mismatched targets.
`dnn-onnxruntime` implies dnn and the standalone ORT wrapper/official CPU SDK.

The generated manifest declares runtime deployment of fourteen named retained
wrapper/upstream notices plus the embedded-source notice supplement, preserving their source paths.
Static implementation objects cannot rely on a shared-library notice scan.

Private ISA and pthread flags belong only to implementation TUs. Public and
internal wrapper BMIs must remain consumable by baseline consumers. The CI guard
reads resolved compile commands for every interface, including internal units.
Bundled zlib uses its upstream prefix; private codec symbols use hidden Unix
visibility to avoid interposition with the GTK SDK's codecs.

The opt-in `simd` utility exposes native 128-bit intrinsics through
`mcpplibs.mwrap.opencv.core.simd`; [its contract](simd.md) separates the compiled
library baseline from per-TU ISA selection. Vector types remain original SDK
entities. GCC memory-entry forwards preserve unaligned semantics that its
imported inline intrinsic body miscompiled. Native/platform acceptance and the
remaining emulator/ISA families stay explicit in the coverage review.

## API verification and known boundaries

The Python binding parser plus curated export lists are generation inputs, not
an API completeness proof. `tools/audit_api.py --verify-exports` inventories
facade-visible namespace/global declarations, overload signatures (including
function templates), internal linkage and retained-header coverage using the
resolved all-modules Clang configuration. It compiles a root-import consumer and
one consumer for each public module. CI uploads the report and consumer log.

Unloaded headers, unlisted entities, static-inline adaptations, SDK-dependent
surfaces and template/overload behavior remain separately reviewed.
The scalar conversion adapter is generated from the pinned saturate.hpp, keeping
all thirteen input overloads and their upstream specializations; a private
ordinary module function bridges TU-local cvRound. Independent SDK function
pointers captured before imports check all 169 source/destination type pairs.
The makePtr custom-deleter rejection is checked with both successful and rejected
consumer compilations. Generic cv::min/max use the original std entities, with
explicit image overloads to preserve overload selection. Exporting a
class makes its definition reachable but does not prove every method works in
every backend configuration. Module-only use and textual-header mixing have
separate tests; full core.hpp mixing on GCC still has its upstream-declaration
merge limitation. No full-support release can be inferred from source counts.

Runtime tests use generated images/models/videos. ORT checks its actual CPU
session profile to exclude native fallback. GUI tests create/display/destroy a
real window; Linux camera checks do not claim physical device capture. Release
acceptance also requires immutable archive consumers, SDK notices and relocated
application runtime checks, independent of package-internal tests.

## Upgrades

1. Import a pinned official archive with `tools/vendor/import_opencv.sh`.
2. Verify retained bytes with `tools/check_upstream.py`.
3. Regenerate build metadata, exports and macro mappings; review every diff.
4. Re-run AST/direct-import, behavior, backend, archive and native CI gates.
5. Use the upstream version, optionally followed by one numeric wrapper revision.

Persistence adapters preserve all 65 pinned SDK inline bodies, with qualified
internal calls and module-private vector proxies. Existing primitive functions
remain original SDK calls; custom serializers retain ADL. The enum adapter keeps
the SDK's optional void-pointer template argument without exporting a dependent
enable_if type that crashes Clang. Separate textual-header and module consumers
verify identical XML/YAML/JSON and Base64 output on every native CI platform.

Linux glibc project targets explicitly use the toolchain-coupled C++ runtime,
matching the ORT template. The actual SDK process must preserve typed OpenCV
exception catches as well as successful inference. The template tests both.

The readiness ledger under `.agents/docs` records checkpoint evidence and open
gates. mcpp engine source stays read-only; its documentation PR is independent.

[Coverage matrices / 覆盖矩阵](coverage.md)

The core/imgproc HAL exports their public function/class declarations, preserving
upstream `cv::hal` names and runtime dispatch. SDK `@cond IGNORED` compatibility
aliases and private filter implementations are excluded. Instrumentation exports
the external interface and original node types; collection remains disabled as
in the selected SDK configuration. Its integer setFlags overload forwards to the
original enum function. The video adapter additionally includes the documented
tracking extension interfaces, tested with a user-derived feature and original
SDK feature-set dispatch.

All 122 free value operators from the pinned types.hpp now use generated SDK
bodies, replacing the old partial handwritten implementations. This preserves
overload coverage and the SDK's overflow-safe rectangle intersection, including
Jaccard distances near INT_MAX. Fifteen core helpers and image conveniences
retain their upstream bodies with module-safe lookup. rawInOut calls the SDK
member through an instance: OpenCV 5 omitted static on its std::array overload,
making the original free wrapper uncompilable for that argument. Vector behavior
is unchanged. Independent header/member and module consumers compare results.

The ArUco default threshold is captured by value because its SDK declaration has
internal linkage. Module-only access retains its qualified name/value and address
use; naming that value after including the original ArUco header and importing
the module is ambiguous. This limitation is verified separately from API behavior.

Every using-declaration exception requires an explicit reason. The historical
tools/prune_loop.py now builds once and emits a JSON/log report without changing
curated lists or exports; it cannot shrink the API until compilation turns green.
