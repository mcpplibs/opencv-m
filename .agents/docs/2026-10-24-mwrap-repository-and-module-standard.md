# mwrap 仓库、包与模块设计规范（评审稿）

状态：Draft，等待用户 review。日期：2026-10-24。

本轮只新增设计文档。组织 fork、创建仓库、改名、功能重构、发布和索引迁移均在评审后实施。
配套方案：[OpenCV 与 ORT 实施设计](2026-10-24-opencv-5-full-and-onnxruntime-plan.md)。

## 1. 定位与边界

`mcpplibs` 顶层承载原生 C++ 模块项目；`mcpplibs.mwrap` 承载对已有 C/C++ 头文件库的模块化封装。
`mwrap` = module wrapper，统一替代本类新项目的 `capi`、`cppapi` 分类。既有包按迁移计划逐个处理，不批量改写已发布版本。

共同契约：保留上游公共实体、namespace、类型身份、调用方式与语义，增加命名模块入口。
模块化无法原样携带的内容必须单独列出：宏、内部链接实体、依赖预处理配置的接口和 ABI 配置。
不以“编译成功”代替公共 API 覆盖，也不将已声明的后端枚举当作可用实现。

## 2. 命名规则

| 层次 | 规则 | OpenCV | ONNX Runtime |
|---|---|---|---|
| Git 仓库 | `<upstream-name>-m` | `mcpplibs/opencv-m` | `mcpplibs/onnxruntime-m` |
| package.namespace | `mcpplibs.mwrap` | 同左 | 同左 |
| package.name | 单个原子名称，无 `-m` 后缀 | `opencv` | `onnxruntime` |
| 完整包身份 | namespace + name | `mcpplibs.mwrap.opencv` | `mcpplibs.mwrap.onnxruntime` |
| 根模块 | 同完整包身份 | `mcpplibs.mwrap.opencv` | `mcpplibs.mwrap.onnxruntime` |
| 公共子模块 | 保留上游模块/头文件分组名称 | `mcpplibs.mwrap.opencv.core` | 有实际接口边界再增加 |
| API namespace | 保留上游 | `cv::`、`cv::dnn::` | `Ort::`；C API 保持全局名称 |
| 宏映射模块 | `<root>.macros` | `mcpplibs.mwrap.opencv.macros` | `mcpplibs.mwrap.onnxruntime.macros` |

上游名称中不能用于模块标识符的字符使用稳定、可记录的规范化映射；不因仓库改名改变 API namespace。
不创造 `mcpplibs::mwrap::opencv::Mat` 等替代 API，不主动将 C 全局函数搬入 namespace。
不按 CUDA、GTK、ORT 等实现后端增加公共 API 模块，除非上游本身有可独立消费的接口。

```toml
[package]
namespace = "mcpplibs.mwrap"
name = "opencv"
version = "5.0.0" # 与上游对齐；封装修订依次为 5.0.0.1、5.0.0.2

[package.metadata.mwrap]
upstream-version = "5.0.0"
```

```toml
# 消费端
[dependencies."mcpplibs.mwrap"]
opencv = { version = "5.0.0", features = ["imgproc", "imgcodecs-png", "highgui-native", "dnn-onnxruntime"] }
```

以上 metadata 为本规范自定义记录，mcpp 不会替我们解释 `upstream-version`。
namespace 是多段路径、name 是单段名称，遵守 mcpp 的包身份规则。

## 3. 仓库范式：封装与上游分离

```text
<libname>-m/
├── mcpp.toml                  # 封装包身份、features、依赖、targets
├── build.mcpp                 # 按需生成配置、派生 TU 和资源
├── .xlings.json               # 固定经过验证的最新稳定 mcpp
├── upstream/
│   ├── sources.lock.toml      # 来源、tag/commit、归档 SHA256、许可证
│   ├── README.md              # 获取/导入/更新流程，裁剪规则
│   └── <libname>/             # vendored 原始源码（仅需要时存在）
├── patches/
│   ├── series                 # 有序补丁；没有补丁也明确记录
│   └── README.md              # 原因、影响、上游 issue、移除条件
├── port/
│   ├── include/               # 适配头、宏兼容头
│   ├── src/                   # 必需的替换实现和平台适配
│   └── config/                # 配置模板、模块与后端依赖清单
├── src/
│   ├── <libname>.cppm         # 公共根模块，满足 mcpp lib-root 约定
│   ├── macros.cppm
│   └── modules/               # 公共子模块与必要的内部 partition
├── generated/                 # 可复现、确有必要提交的稳定生成物
├── tools/                     # 同步、生成、公共 API 审计工具
├── tests/
│   ├── surface/               # 导出、类型身份、宏等价性
│   ├── behavior/              # 真实功能
│   └── consumers/             # 头文件混用、模块共存、安装后消费
├── examples/
├── docs/
└── .agents/docs/
```

`tests/` 的子目录是本规范布局；须由测试 targets 或受控发现规则接入，不假定目录移动后 mcpp 自动执行全部测试。
构建期、按 feature 变化的文件进入 mcpp 输出目录；不能写回共享 `generated/` 或修改 `upstream/`。
公开头文件通过 include_dirs 暴露；上游内部头与生成配置保持私有，确有消费者需求的配置另行定义契约。

参考 openkal-musl 的实质是三种内容分离：`musl/`（上游）、`port/`（适配）、`musl-generated/`（可复现生成物），
以及修改/排除理由可追溯。这里统一到 `upstream/`，不照搬它的 libc 移植语义。

## 4. 上游实现的三条接入路线

| 路线 | 使用条件 | 要求 |
|---|---|---|
| `compat.<lib>` | 已有可复用的传统库包，配置和 ABI 足够 | 封装仅依赖它，不复制源码/二进制，不重复编译实现 |
| vendor 到 `upstream/<lib>/` | 复杂配置、SIMD、生成代码、平台后端需要本仓库组织 | 固定来源和裁剪清单，源码与 port 分离，所有修改可审计 |
| 预编译 SDK | 上游构建复杂且官方有合适 SDK，如首版 ORT | 平台/ABI/provider/运行库清单、hash 和许可明确；不承诺无产物目标可用 |

`compat` 是传统实现供应者，`mwrap` 是模块接口供应者。一个程序中的同一实现必须只有一个所有者。
不为了形式统一强制新增 `compat.opencv`；当前复杂的 OpenCV 构建可保留源码所有权。
不将 build.mcpp 变成下载器，依赖获取由包/索引/provider 管理；源码预处理通过有输入输出的 action 表达。
若依赖 CMake，在 CI 构建包时或明确声明的构建步骤中使用，必须说明与“消费端无需 CMake”的区别。

## 5. 模块与导出契约

1. 封装层优先使用上游实体的 `export using`；不复制公共类或重新定义同名类型。
2. 每个实体有一个导出归属，根模块 `export import` 当前启用的公共模块。
3. 子模块可单独 import，并能组合 import；不通过偶然的传递头文件获得未声明能力。
4. root 与可选模块闭包由模块清单生成，打包后的公共模块集合与源码消费一致。
5. `static inline` 等确需替换时，记录单独清单；验证语义、ADL、重载与头文件混用，不默默裁剪。
6. 审计范围取上游公共 C/C++ 头文件，Python 绑定标记只能作为生成器输入之一。
7. 内部实现使用 partition 或未公开模块；根模块不能意外把内部接口带入发布闭包。

## 6. 宏的模块化映射

`.macros` 是命名模块，不是预处理器宏的传输通道。
推荐映射 namespace：`mcpplibs::mwrap::<libname>::macros`，保留宏的原始名称。
宏映射是有意新增的 API 面，与保持上游 namespace 的普通实体分开管理。

```cpp
import mcpplibs.mwrap.opencv;
import mcpplibs.mwrap.opencv.macros;
namespace ocvm = mcpplibs::mwrap::opencv::macros;

cv::Mat image(64, 64, ocvm::CV_8UC3);
static_assert(ocvm::CV_MAKETYPE(ocvm::CV_8U, 3) == ocvm::CV_8UC3);
```

| 上游宏种类 | 映射方式 |
|---|---|
| 数值、字符串等对象式宏 | 类型明确的 inline constexpr；先在 GMF 中捕获值，再 undef，避免声明被展开 |
| 可保持语义的计算宏 | constexpr 函数，记录整数提升、求值次数、返回类型等差异 |
| `#if`、token 拼接、字符串化、注解/导出属性 | 兼容头文件保留原始宏 |
| 断言/日志/控制流宏 | 默认保留宏；只有调用位置、求值和控制流契约可证明时才增加具名辅助函数 |

不承诺所有宏都可映射；尤其普通函数不能自动保留 CV_Assert 的表达式字符串化与调用点信息。
兼容头建议 `<mwrap/<libname>/macros.hpp>`，在 import 前包含。测试对象式宏存在时对 qualified 名字的展开冲突；
不能认为加 namespace 就能规避预处理器。文档分别展示纯模块映射用法与原始宏用法，避免同一 TU 直接混用冲突名称。
旧 `cv::CV_*` 常量属于已有封装扩展，迁入新包时转到 `.macros`；旧身份冻结兼容，不继续向上游 namespace 添加宏替身。

## 7. feature、平台和运行时

使用 mcpp 的 implies、forward、feature-deps、cfg(target) 和 build.mcpp action。
feature 表达增加能力，依赖图同一包的请求取并集；不能设计靠依赖路径获得不同 ABI 的相反配置。
仅需启用的外部库放 `[feature-deps.<feature>]`，链接参数由实现供应者携带。
真正互斥的供应者通过明确变体/依赖图约束处理；capability 可以做选择，但不会移除其他已引入包的对象文件，
也不会自动发现 SDK 或保证兼容。同名符号的两份运行库必须在进入最终链接图前避免同时引入。

### 7.1 feature 命名规范

feature 使用小写 kebab-case，优先复用上游模块、后端、依赖的正式名称。

| 类别 | 规则 | 示例 |
|---|---|---|
| 上游模块 | `<module>` | dnn、highgui、videoio、stitching |
| 模块的后端/可选依赖 | `<module>-<backend-or-library>` | highgui-gtk3、videoio-ffmpeg、dnn-onnxruntime |
| 模块的图像格式 | `imgcodecs-<format>` | imgcodecs-jpeg、imgcodecs-tiff |
| 跨模块加速依赖 | 使用正式名称 | opencl、tbb、ipp、lapack |
| contrib 模块 | `contrib-<upstream-module>` | 以固定上游版本的实际模块名为准 |
| 模块全集 | `all-modules` | 仅表达主仓库模块集合，不承诺所有后端/SDK |

不在公共接口混用 gui/highgui、codec/imgcodecs、ort/onnxruntime；正式 feature 使用上游可识别名称。
native 可作为有精确 target 映射表的便利开关，如 highgui-native；显式 backend 名仍保留。
未支持的目标、缺失供应者和互斥后端组合在配置阶段给出明确错误，不静默忽略用户选择。
default 的展开写入版本化清单；不把 desktop/full 等没有唯一含义的名称作为基础能力分类。
已通过 E2E 126/127 核实：当前 pinned mcpp 支持依赖边的 default-features=false，显式 features 仍激活；
不支持的是根项目 --no-default-features CLI。其他依赖边仍可请求默认项。最小默认集合是本规范的产品选择，不是客户端限制。
OpenCV 新包建议 default=[]、core 为必需基础，其余模块与实现显式启用；常用体验由模板列出所需 features。
同名 feature 不得在不同修订中无说明地改成其他执行后端。

### 7.2 索引分类布局

描述符统一置于 `mcpp-index/pkgs/mwrap/<full-package-name>.lua`，测试置于 `tests/mwrap/`。
不再增加首字母子目录。Git 源码与发布归档保持同一布局，不生成到 pkgs/m 的转换副本。
已核查 mcpp v2026.10.21.1：身份查找的 fallback 扫描 pkgs 下一级目录，子目录名不要求是单个字母；
因此 pkgs/mwrap 是与现有扫描深度相容的分类，根目录 mwrap/o 则不在该查找路径内。
发布脚本原本复制整个 pkgs，可直接保留新目录；lint、测试选择器、站点和 provider 仍须做集成验证。
即使部分脚本的 pkgs/*/*.lua glob 已能覆盖，也要核查是否存在目录名必须单字母的额外假设。
不因包装层分类而扩大客户端索引协议，也不为保持首字母分桶维护第二份 descriptor。

### 7.3 测试责任

Linux 优先本地执行，CI 重跑发布关键组合；Windows 与 macOS 由 CI 覆盖。
CI 的编译、链接、功能执行和安装后消费分别报告；桌面/设备/GPU 不可用时明确未执行项。
所有平台使用同一测试入口、feature 展开和固定工具版本，不能靠平台专属的临时脚本掩盖配方差异。
设计文档检查与实现验证分别报告；本轮更新规范并不表示上述平台功能已通过运行验证。

配置身份至少覆盖 target triple、工具链/stdlib、linkage、feature 闭包、上游版本和适配补丁。
公共布局、异常/RTTI、运行时、CPU baseline 必须一致；SIMD 编译选项限定在实现 TU，不污染公共 BMI。
宿主工具和目标库分开；不以构建宿主 OS 替代目标平台判断。
预编译库需要测试动态加载的 provider、数据和插件；它们未必出现在普通链接依赖扫描结果中。

### 7.4 每个 -m 库必须提供快捷模板

每个以 -m 命名并发布的模块封装库至少提供一个可运行的默认 mcpp 项目模板，位于 templates/<name>/。
模板通过完整包身份调用，例如 `mcpp new vision --template mcpplibs.mwrap.opencv@5.0.0`。
多个模板使用 `:<template>` 选择，并通过 `mcpp new --list-templates <package>@<version>` 列出。
模板保持纯数据，使用 mcpp 支持的 placeholder，不运行脚本或隐藏下载步骤。
依赖版本取 self.version；当前 self.name 已是完整包选择器，使用带引号的单个 key，不再额外套 namespace 表；生成项目明确列出所需 features。
默认模板不要求摄像头、GPU、桌面或用户另行下载模型；可选高级模板单独提供。
模板必须经过真实 mcpp new、build、run/test 验证，发布归档必须包含 templates/；静态 placeholder 检查不代替运行。

## 8. 版本和发布

**用户已确认的规范：封装版本与上游版本对齐，最多追加一个数字修订尾号。**

| 发布场景 | 上游版本 | mwrap 包版本 |
|---|---|---|
| 首次封装 | 5.0.0 | 5.0.0 |
| 同一上游的第一次封装修订 | 5.0.0 | 5.0.0.1 |
| 同一上游的后续封装修订 | 5.0.0 | 5.0.0.2、5.0.0.3…… |
| 升级到下一个上游版本 | 5.0.1 | 5.0.1，修订计数重新开始 |

不维护独立的 0.x 封装版本线；不使用 5.0.1 表示“仍为 5.0.0 的封装修复”。
修订 N 从 1 开始，不发布 `.0`：mcpp 将缺失数字段当作零，5.0.0 与 5.0.0.0 会排序相等。
同一个上游版本最多增加一个修订段，不能出现 5.0.0.1.1。
不使用 `+build` 承担升级排序，也不把正式封装修订标成 `-mwrap.1` 预发布版本。
metadata 与 sources.lock 仍明确固定上游版本、commit/hash；尾号仅表示封装/配方修订。
对上游本身为多段数字的版本，同样只允许追加一个数字段；上游预发布或非数字版本须显式记录映射规则，不能改变上游版本含义。

已核查 mcpp v2026.10.21.1 的 version_req 实现：支持任意长度数字段，逐段数值比较，
故 5.0.0 < 5.0.0.1 < 5.0.0.2 < 5.0.1 可表达。发布验收还需覆盖 provider、索引、lockfile 与更新链路。
该格式是 mwrap 的版本约定，不称为严格的三段 SemVer。
消费端精确选择已发布字面版本；希望自动接受同一上游的修订时，显式使用 `>=5.0.0, <5.0.1`，
不能把普通 `^5.0.0` 当成只允许封装修订（它可允许后续上游 5.x）。

模块重命名、导出删除、宏映射迁移等破坏性变化必须发布说明和迁移表；尾号不是隐瞒破坏性变化的理由。
本次包身份已切换到 mcpplibs.mwrap，原 opencv.opencv@5.0.0 保留不变；新身份可从 5.0.0 开始。

上游源码、适配层和发布产物分别记录 hash。发布附模块/feature/平台/API 覆盖矩阵及第三方许可证。
二进制包带接口源码和库，不分发依赖特定编译器构建的 BMI。

## 9. 迁移原则

新身份发布前先通过干净消费者验证。旧包/版本的地址与 hash 保持不可变。
`opencv.opencv`、旧 `mcpplibs.capi.*` 可冻结留存并提示替代身份。
如提供旧 import 转发，只做 export import，不重复实现；是否默认携带由兼容测试决定。
同一程序同时引入旧、新实现供应者必须诊断或明确禁止，不能仅靠转发模块掩盖重复链接。

## 10. 实施基线与参考

2026-10-24 通过 GitHub Releases API 核实最新稳定 mcpp 为 `2026.10.21.1`。
实施开始再次核实；若更新，统一变更 .xlings.json、CI、模板和文档并验证，提交中固定确切版本，不跟随浮动 latest。
后续已按用户要求更新本地相关仓库到远端 main；提交、原目录保护与复核结果见配套实施方案 §1.2。
其中 mcpp main 已标记 2026.10.22.1，发布 API 确认版本仍为 2026.10.21.1，两者不混为一谈。

规范定稿后，正式维护位置为 mcpp-index/docs/mwrap.md；本文件保留为设计评审记录。
同步更新索引的 add-mcpp-index-package skill 与 mcpp-usage skill，示例和模板链接正式规范。
技能更新细项见配套方案 P7，规范确认前不提前改动其他仓库的指引。

- [mcpp features / feature-deps](https://github.com/mcpp-community/mcpp/blob/v2026.10.21.1/docs/06-features-and-capabilities.md)
- [mcpp workspace](https://github.com/mcpp-community/mcpp/blob/v2026.10.21.1/docs/07-workspace.md)
- [mcpp 预编译库分发](https://github.com/mcpp-community/mcpp/blob/v2026.10.21.1/docs/12-binary-distribution.md)
- [mcpp 数字段版本比较](https://github.com/mcpp-community/mcpp/blob/v2026.10.21.1/modules/versioning/src/version_req.cppm)
- [mcpp build.mcpp](https://github.com/mcpp-community/mcpp/blob/v2026.10.21.1/docs/30-build-mcpp.md)
- [mcpp 描述符发现与 path 索引](https://github.com/mcpp-community/mcpp/blob/v2026.10.21.1/src/pm/package_fetcher.cppm)
- [openkal-musl](https://github.com/mcpplibs/openkal-musl)：本地核查 musl/、port/、musl-generated/ 和补丁记录。
- 当前仓库实际模块导出与宏实现：src/core.cppm、tools/gen_exports.py、include/mwrap/opencv/macros.hpp。

## 11. 需要 review 的决策

1. 接受完整公共前缀 `mcpplibs.mwrap.<libname>`，不把 `-m` 带入包名与模块名。
2. 接受“compat 优先复用、复杂库 upstream/vendor、ORT 首版预编译 CPU”的实现分层。
3. 接受宏映射独立 namespace 和原始宏兼容头双入口。
4. 已确认：包版本与上游一致，最多追加一个从 1 开始的封装修订尾号；不发布 `.0`。
5. 接受旧身份冻结、新身份切换，兼容入口不形成第二份实现。
6. 索引使用 pkgs/mwrap 平铺描述符与 tests/mwrap 分类，无 staging 路径转换。
7. feature 沿用上游模块/后端名称，Linux 本地验证，Windows/macOS CI 覆盖。
