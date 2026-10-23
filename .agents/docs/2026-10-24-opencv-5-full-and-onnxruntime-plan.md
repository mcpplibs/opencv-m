# OpenCV 5.0.0 全面支持、ORT 与 mwrap 索引迁移设计（评审稿）

状态：Implementation，用户已授权实现、测试、每仓库一个 PR 与 checkpoint commits。日期：2026-10-24。
规范：[mwrap 仓库与模块规范](2026-10-24-mwrap-repository-and-module-standard.md)。
已确认：版本跟随上游，最多追加一个数字封装修订尾号，例如 5.0.0 → 5.0.0.1。

## 1. 本轮交付与后续实施边界

本轮交付两份设计文档，不修改现有产品代码、工具版本、远端仓库或索引。
评审后将 `Sunrisepeak/opencv-m` fork 到 `mcpplibs/opencv-m`，创建 `mcpplibs/onnxruntime-m`，
并在 mcpp-index 新增独立 mwrap 描述符目录和测试目录。
GitHub API 本轮对两个目标仓库返回 404；这仅表示当前查询不可见，不能据此断言组织权限或全局不存在。
实施时检查目标存在性、权限和 fork 状态：存在则复用核验，不覆盖或误建第二个仓库。

### 1.1 本地工作区优先

用户已确认 mcpp、mcpp-index 与 mcpplibs 均有本地目录；实现与联调优先复用这些 checkout。

| 对象 | 本轮实际检查的本地路径 |
|---|---|
| mcpp | /home/speak/workspace/github/mcpp-community/mcpp |
| mcpp-index | /home/speak/workspace/github/mcpp-community/mcpp-index |
| mcpplibs 工作区 | /home/speak/workspace/github/mcpplibs |
| 分层参考 | /home/speak/workspace/github/mcpplibs/openkal-musl |
| provider/loader 辅助检查 | /home/speak/workspace/github/openxlings/xlings、/home/speak/workspace/github/mcpplibs/libxpkg |

本机另有 mcpplibs 下的 mcpp-index checkout 与多个工作树，实施前核对各自分支、未提交修改和目标远端，
选定一个索引工作树实施，不在多个 checkout 间重复迁移。库目录本身不是一个统一 Git 仓库。
源码、文档和调用链优先本地读取；仅在确认已发布版本、官方 SDK 或上游差异时查询远端。
Linux 联调通过本地包依赖与本地索引完成，另做发布归档消费测试，不用远端现成包替代本地变更验收。
用户随后明确要求切换 main 并更新远端，已执行 fetch 与 fast-forward；未修改这些仓库的产品代码。
fork、创建组织仓库和功能重构仍属于设计评审后的实施阶段。

### 1.2 更新远端 main 后的复核基线

为避免功能分支或旧源码误导，以下基线均在 fetch origin main 后核查，HEAD 与 origin/main 一致：

| 仓库 | 最新 main 提交 | 本次使用方式 |
|---|---|---|
| mcpp | 4c80028c147b6827d78a2fe9ca3e0e41faf1b721 | 原目录 main 快进；保留原有未跟踪文件 |
| 选定的 mcpp-index | 18b7b056a4843da948b5fdd6ed3e04a91584dd00 | 原目录从功能分支切 main 并快进；原分支保留 |
| openkal-musl | 7fe6d886643fa4634f0c55bedc415234e3150239 | 原目录 main，远端无新增 |
| libxpkg | 5523780abfedf1e1e06d8b42a077fd76a70f284e | 原目录有未跟踪 lockfile，恢复原分支；在 /tmp/mwrap-design-review-20261024/libxpkg 的干净 main worktree 快进 |
| xlings | 66c5cbf6 | 原目录有未提交改动，保持原状；在 /tmp/mwrap-design-review-20261024/xlings 的干净 main worktree 快进 |

mcpp main 的版本字段已是 2026.10.22.1；本轮 GitHub Releases API latest 仍返回 v2026.10.21.1。
源代码 main 与已确认发布的客户端分开记录：实现 pin 使用可获取、可验证的最新稳定发布，实施前再次查询。
已比较 mcpp 发布 tag 与更新后的 main：描述符发现代码和 features 文档没有差异，本方案相关结论保持成立。
更新后的 libxpkg 0.0.66 在 src/xpkg-loader-boundary.cpp 中仍枚举 pkgs 的一级分类目录，且显式处理包身份。
未更新另一份 mcpplibs/mcpp-index 功能分支 checkout；本轮只选定一个索引工作树作为权威实现对象。

## 2. 已核实的起点

| 项目 | 当前事实 | 重构要求 |
|---|---|---|
| 包身份 | namespace=opencv，name=opencv，version=5.0.0 | 新包 `mcpplibs.mwrap.opencv` |
| 模块 | opencv / opencv.cv / opencv.<mod> | 公共前缀统一为 mcpplibs.mwrap.opencv |
| mcpp | .xlings.json 固定 2026.8.5.4 | 迁移到已核实最新稳定 2026.10.21.1；实施前复核 |
| 源码 | 裁剪导入 8 个主模块 | 补齐主仓库 C++ 模块，单独导入 contrib |
| GUI | config 中 backend=NONE | Win32 / Cocoa / GTK 原生窗口 |
| DNN | 可选 dnn，含 ONNX 导入、ENGINE_ORT 导出 | ORT 未启用，必须接入真实运行库 |
| 宏 | 部分常量被放入 cv::，另有宏兼容头 | 独立 .macros 模块与宏映射 namespace |
| 导出生成 | Python 标记 + curated，跳过部分头/namespace | 完整 C++ API 清单审计，不能静默 prune 成“完整” |
| 构建配置 | 固定 generated/<os> 快照，build.mcpp 注入部分文件 | 由 feature 与 target 生成配置，移除过时语法限制 workaround |
| 索引 | pkgs/o/opencv.opencv.lua；tests/examples/opencv-module* | mwrap 分类与配套 CI/发布发现迁移 |

已对照官方 5.0.0 tag 的 modules 目录；不能用 OpenCV 4 的 calib3d/features2d 等名称直接作为本次模块清单。

## 3. “全面支持”的验收定义

全面支持指：OpenCV 5.0.0 主仓库公共 C++ API、可选后端和模块依赖均有明确覆盖状态，
支持项可以由 feature 选择并在声明的平台验证；不能将所有 SDK 和互斥后端强制放进一个构建。

分别维护四张矩阵，避免以一个 full 开关掩盖缺失：

1. **源码模块矩阵**：模块是否导入、可构建、可单独 import、根模块是否聚合。
2. **API 矩阵**：公共实体、宏、内部链接适配、缺失项与原因。
3. **运行能力矩阵**：窗口、编解码、视频、推理 provider、加速是否实际运行。
4. **目标矩阵**：target triple、工具链、linkage、配置组合及验证状态。

Python/Java/JS/ObjC 绑定、样例工具、ts 测试框架、world 聚合库不作为 mwrap C++ 首轮验收的必需产物。
world 的使用体验由根模块聚合提供，不再重复构建第二套全部实现。
contrib 是独立工作包，未验证前不能宣称 contrib 全支持。
硬件/专有 SDK 项可以阶段性交付，但必须标为 planned / unavailable，而非笼统宣称已全功能。

## 4. 仓库划分与布局

### 4.1 mcpplibs/opencv-m

```text
opencv-m/
├── mcpp.toml / build.mcpp / .xlings.json
├── upstream/
│   ├── sources.lock.toml
│   ├── README.md
│   ├── opencv/                 # 固定官方 5.0.0
│   └── opencv_contrib/         # 后续导入匹配 tag/commit
├── patches/                    # 最小、可重放补丁及原因
├── port/
│   ├── config/                # 模块图、后端配置模板、平台规则
│   ├── include/mwrap/opencv/  # 原始宏兼容头
│   └── src/                   # 必需的替换函数和平台适配
├── src/
│   ├── opencv.cppm            # export module mcpplibs.mwrap.opencv;
│   ├── macros.cppm
│   └── modules/<name>.cppm
├── generated/                 # 稳定生成物；配置相关产物进构建输出目录
├── tools/                     # import、config、exports、coverage
├── tests/{surface,behavior,consumers}/
└── examples/
```

OpenCV 适合保留 vendor 路线：SIMD dispatch、DNN、平台后端、contrib 与生成配置紧密关联。
无需为了分层新增不可独立维护的 compat.opencv。
FFmpeg、codec、TBB、ORT 等复用独立供应者，逐项核实 compat 包可用性。
上游/适配/生成层分开；升级时重新导入上游再重放补丁，生成 diff 和 API 覆盖差异供 review。
裁剪仅限非目标产物，并记录清单；不能在“源码裁剪”阶段删除后续 feature 所需代码。

### 4.2 mcpplibs/onnxruntime-m

独立提供 `mcpplibs.mwrap.onnxruntime`，保留 `Ort::` 和 C API。
首版采用匹配版本的官方 CPU SDK，不将大二进制提交 Git；头文件与 SDK 必须同版本。
包来源、产物地址、SHA256、ABI、最低系统要求、license 和 provider 清单进入供应者记录。

优先评估创建/复用 `compat.onnxruntime` 作为传统 SDK 供应者：

```text
mcpplibs.mwrap.onnxruntime（模块接口）
  └── compat.onnxruntime（头文件、import library、运行库）

mcpplibs.mwrap.opencv --feature dnn-onnxruntime
  └── mcpplibs.mwrap.onnxruntime
       └── compat.onnxruntime
```

OpenCV 的 ORT 实现 TU 使用供应者的原始头与 C API 链接，无须在上游源码里 import 封装模块。
实施时验证 mwrap 包能正确携带供应者所需的公开 include/link/runtime 信息。
若需要 feature 依赖可见性调整，必须在消费者测试中确认，不能只测包内链接。
若平台 SDK 不能以 compat 配方表达，保留独立 SDK 供应者实现，不把下载逻辑塞入模块文件。

先覆盖 Linux x86_64 GNU、Windows x86_64 MSVC ABI、macOS arm64。
musl、MinGW 和其他架构需要独立产物与 ABI 验证，不能复用 glibc/MSVC 产物后声称已支持。
GPU/provider 第二阶段加入：记录 ORT、CUDA/cuDNN/TensorRT 等版本约束，防止 CPU/GPU 包携带两份同名运行库。
ORT 有自己的 provider 支持，不等于 OpenCV 的 ORT wrapper 自动暴露了所有配置接口，两条路径分别验证。

## 5. OpenCV 模块与 feature 设计

根模块为 `mcpplibs.mwrap.opencv`，公共子模块遵循 `mcpplibs.mwrap.opencv.<upstream-module>`。
宏映射入口为 `mcpplibs.mwrap.opencv.macros`。

| 分组 | 上游模块 / feature | 当前状态与目标 |
|---|---|---|
| 基础 | core、imgproc、imgcodecs、flann、geometry | 已有基础，审计完整导出与依赖 |
| 交互与视频 | highgui、videoio | 已有接口，补真实窗口/原生采集 |
| 推理 | dnn | 已可选，补 ORT 和后端能力矩阵 |
| 主模块补齐 | calib、features、objdetect、photo、ptcloud、stereo、stitching、video | 当前未导入，按实际依赖闭包补齐 |
| 扩展 | contrib-<name> | 固定匹配来源，逐模块开放 |

算法模块依赖从实际上游 CMake/公共头抽取成显式图，不凭印象手写并长期维护两套。
模块图作为 sources、features、config、根模块和审计报告的共同输入。

### 5.1 命名依据：上游模块、后端与格式

feature 使用小写 kebab-case。模块名直接对应 OpenCV 5，后端/格式使用上游开发者熟悉的名字。
上游 CMake 的 WITH_* / HAVE_* 与 mwrap feature 建立显式映射，但不把 HAVE_* 直接暴露成用户配置。

| 层次 | feature 示例 | 语义与依赖 |
|---|---|---|
| 算法模块 | calib、features、objdetect、photo、ptcloud、stereo、stitching、video、dnn | 启用同名模块及其依赖闭包 |
| GUI 模块 | highgui | 启用 highgui API；单独选择不承诺窗口后端 |
| GUI 后端 | highgui-win32、highgui-cocoa、highgui-gtk3、highgui-qt6、highgui-wayland | implies highgui，启用明确的窗口实现及依赖 |
| GUI 便利开关 | highgui-native | 按目标展开为 win32 / cocoa / gtk3，具体映射版本化 |
| 图像格式 | imgcodecs-jpeg、imgcodecs-png、imgcodecs-tiff、imgcodecs-webp、imgcodecs-openexr、imgcodecs-jpeg2000、imgcodecs-avif、imgcodecs-jpegxl | implies imgcodecs，按图像格式组织；JPEG2000 首选 OpenJPEG 供应者 |
| 视频模块 | videoio | 启用公共视频 I/O 接口及其基础实现，不等于已支持摄像头或所有容器 |
| 视频后端 | videoio-ffmpeg、videoio-gstreamer、videoio-v4l2、videoio-msmf、videoio-dshow、videoio-avfoundation | implies videoio；名字对应上游后端 |
| 原生采集便利开关 | videoio-native | Linux→v4l2，Windows→msmf，macOS→avfoundation；不暗示任意目标可用 |
| DNN 引擎/后端 | dnn-onnxruntime、dnn-openvino、dnn-cuda | implies dnn，分别引入 SDK 和实现；执行引擎仍由上游运行 API 选择 |
| 跨模块加速 | opencl、opengl、tbb、ipp、lapack、eigen | 开启相应构建能力；必须记录影响哪些模块，不能承诺其名称代表全部 API 均加速 |
| 扩展模块 | contrib-<upstream-module> | 启用匹配版本的实际 contrib 模块与依赖 |
| 字体 | unifont | 保留当前精确含义，Unicode 内嵌字体 |

这些是统一命名表和待实施范围，未实现项不提前注册成空 feature，也不标成 supported。
正式名采用 dnn-onnxruntime，文档可将 ONNX Runtime 简称 ORT；不再同时引入 dnn-ort、gui、codec-* 等同义新名称。
当前尚未发布上述旧提案名称，不需要为它们建立永久别名。
GTK 与 Qt 分别用 gtk3、qt6 标明首个支持的主版本；未来主版本变化增加独立名称并记录兼容性。

highgui-native 的 native 表示本包的平台默认实现，不表示 GTK 属于 Linux 内核或所有桌面环境。
它总是增加已记录的默认后端；显式选择 Qt 不会偷偷关掉默认后端。
上游支持多个后端共存时，保留其上游选择机制；不支持共存的组合在配置时拒绝并给出冲突来源。
不能仅凭注册表列出了 GTK/Qt/Win32 就认为当前构建方案支持多后端共存。

### 5.2 默认集合、模块全集与能力边界

不提供语义含混的 desktop/full feature。全面支持是交付目标，通过模块与后端的覆盖矩阵验收。
提供以下可审计集合：

- `default = []`：core 为必需基础，其他模块与外部后端均显式启用。
  这是可预测、低依赖的产品默认选择。已运行 E2E 126/127 确认 pinned mcpp 支持依赖边 default-features=false，
  与根项目没有 --no-default-features CLI 不同。
  imgproc、imgcodecs、flann、geometry、videoio 等作为模块 feature，依赖闭包自动包含 core。
  常用模板替用户列出 imgproc、imgcodecs-jpeg、imgcodecs-png；视频模板再增加 videoio-ffmpeg。
  这是新包身份下有意的最小默认策略，与旧包的默认能力不同，需要迁移表。
- `all-modules`：固定版本主仓库全部公共 C++ 算法/功能模块的集合，包括 dnn、highgui、videoio，排除语言绑定、ts、world。
  它增加公共模块，不隐式增加所有 codec、GUI、GPU SDK 或 contrib。
- 窗口使用显式选择 `highgui-native` 或对应 highgui 后端；根模块仅聚合已经启用的接口。
- 特殊 codec、contrib、硬件后端独立选择，不用“自动全开、不可用就跳过”掩盖能力缺口。

新身份的 default 不再聚合无实际窗口后端的 highgui，避免用户默认拿到 imshow 占位实现。
仅启用 highgui 或 all-modules 而未选择窗口后端时，配置摘要明确输出“highgui API enabled; window backend: NONE”，
文档不把该组合标为窗口支持；使用窗口的例子必须声明 highgui-native 或具体后端。

用户组合示例（已实现并发布相关 feature 后可用）：

```toml
[dependencies."mcpplibs.mwrap"]
opencv = { version = "5.0.0", features = ["imgproc", "imgcodecs-png", "highgui-native", "dnn-onnxruntime", "imgcodecs-tiff"] }
```

最小消费者不选择可选 feature；其他默认集合的包允许通过依赖边 default-features=false 关闭该边默认项。
根项目不提供 --no-default-features CLI。发布案例逐项声明所需模块与后端，其他依赖边仍可要求默认项。
多个依赖请求按 feature 并集合并，禁止基于调用依赖路径生成两套相同包身份的不同公共 ABI。
未支持的目标、缺 SDK、缺 provider 产物、互斥后端在配置阶段明确报错，不静默降级。
所有 feature 的展开清单与能力报告一起版本化，改动进入封装修订说明。

### 5.2.1 ONNX Runtime 包的 feature

onnxruntime-m 默认使用 CPU SDK，保留 CPU provider；首版不提供无实现的 GPU 开关。
后续 provider 采用上游习惯名 cuda、tensorrt、coreml、directml、openvino，并维护平台/SDK/产物矩阵。
启用 provider 需要匹配运行库和实际可用的 API；CPU/GPU 主库互斥变体由供应者处理，不能叠加两份 onnxruntime。
OpenCV dnn-cuda 是 OpenCV 经典 CUDA 后端，与 ORT cuda provider 是两个能力，不能相互隐式启用。
dnn-onnxruntime 首版只承诺 ORT CPU；以后若增加 provider 转发，必须先证明 OpenCV wrapper 暴露了对应选择能力。

### 5.3 DNN 运行示例（目标接口）

```cpp
import mcpplibs.mwrap.opencv.dnn;
auto net = cv::dnn::readNetFromONNX("model.onnx", cv::dnn::ENGINE_ORT);
```

必须真实验证 ORT 被调用：当前上游在未编入 ORT 时可能警告并回退 ENGINE_AUTO。
包测试不能只断言 net 非空；需结合受控日志/测试观测点或 ORT 实际调用证据，区分回退。
另有直接 import onnxruntime 并以 Ort::Session 推理的消费者测试，覆盖独立包价值。
不假设 ENGINE_NEW 已支持 GPU；OpenCV 原生 DNN 与 ORT provider 的能力分别说明。

## 6. 构建与 API 重构

1. 使用最新固定 mcpp，先验证旧功能基线，再删除已无必要的 Windows TU stub 和旧语法 workaround。
2. 生成 cfg 相关 include dirs / sources / flags；可声明的内容留 manifest，生成步骤用 build.mcpp action。
3. 把 target、feature 闭包、ABI 与上游 pin 纳入生成输入和缓存身份，输出只写 mcpp 输出目录。
4. 特别处理 Objective-C++ Cocoa 源码、CUDA、NASM、SIMD dispatch 与宿主代码生成工具。
   标准中列为必验集成点，不假定最新 mcpp 已自动覆盖全部语言/规则。
   若现有规则不足，先做最小验证，再选择声明的工具/rule package 或构建动作，并提出可复现的 mcpp 缺口。
5. 生成 opencv_modules.hpp、cvconfig、highgui config、DNN 配置、dispatch TU 和聚合模块。
6. 编译上游头文件消费者与 module 消费者的相同案例，验证类型/重载/运算符/常量/行为。
7. 将现有自动 prune 流程改为生成显式缺失报告；缺失需理由与回归，不能以静默删除结束。
8. 宏迁到 .macros；对现有 cv::CV_* 提供文档迁移映射，头文件混用测试覆盖 macro 展开冲突。

上游公共 API 完整性与 ABI 保持是验收条件，不强制“零补丁”压倒正确性。
确需补丁时应最小化并记录，与 wrapper 的替换实现分开。

## 7. mcpp-index 的 mwrap 分类

### 7.1 目标布局：Git 与发布保持一致

```text
mcpp-index/
├── pkgs/
│   ├── c/                         # 原有 compat 等包
│   ├── ...
│   └── mwrap/                     # 模块化封装的独立分类，描述符平铺
│       ├── mcpplibs.mwrap.opencv.lua
│       └── mcpplibs.mwrap.onnxruntime.lua
├── tests/
│   ├── examples/                  # 原有非 mwrap 消费者
│   └── mwrap/
│       ├── opencv-base/
│       ├── opencv-highgui/
│       ├── opencv-dnn-onnxruntime/
│       ├── opencv-macros/
│       ├── onnxruntime-cpu/
│       └── legacy-migration/
└── tools/
```

mwrap 独立分类放在 pkgs 下，与现有索引根和扫描深度一致；不再另加 o/ 等首字母层。
分类约定写入 docs/mwrap.md；描述符目录保持纯包文件，避免 README 被旧 lint 当作描述符。
包身份由 descriptor 声明，不从分类目录推断。compat.onnxruntime 仍归传统供应者，不进入 mwrap。

### 7.2 已核查的发现路径与最小适配

核查对象包括 mcpp v2026.10.21.1 的 src/pm/package_fetcher.cppm，以及本地 provider/libxpkg 加载路径。

- mcpp 先尝试 pkgs/<首字母>/<候选文件名> 快速路径。
- 未命中时按声明身份扫描 pkgs 根及其一级子目录。循环变量虽叫 letterDir，代码不要求目录名是一个字母。
- 因此 pkgs/mwrap/<full-name>.lua 落在既有 fallback 范围内。
- 根目录 mwrap/o/*.lua 不在该扫描范围；pkgs/mwrap/o/*.lua 也多了一层，不能假设递归支持。
- 本地 libxpkg 的索引构建同样枚举 pkgs 下一级目录，目录名不强制首字母；具体 pinned provider 的实际组合仍需端到端验证。
- mcpp-index 发布脚本复制完整 pkgs，因此新分类目录可原样发布，无需路径转换。

**采用结论：取消 staging 路径转换，直接维护和发布 pkgs/mwrap。**
这不要求先修改客户端索引协议；优先在 mcpp-index 内适配目录枚举、测试注册和 CI 选择。
文件系统扫描 fallback 可能比首字母快速路径慢，先测实际规模；只有证据显示明显回归时再考虑通用查找优化。
不为单个 mwrap 分类在 mcpp 引擎中增加硬编码 namespace 分支。

实施前在 Linux 用精确固定的 mcpp/provider 做最小索引 fixture，验收 path、Git 和发布归档三种消费路径：
精确身份解析、版本枚举、搜索、跨 namespace 依赖、lockfile、安装和最终链接均命中当前索引。
旧版客户端的覆盖不是本轮目标；也不以本地源码可扫描代替上述最新版本的运行实测。

### 7.3 必须同步修改的索引入口

- 发布脚本的文件收集、内容 hash、镜像、manifest 与归档。
- lint、包名与保留 namespace 检查、GLOBAL/CN URL 和 sha256 检查。
- workspace members、新 tests/mwrap 项目注册和 index 重定向；适配 run_members.sh、plan_shards 和计时表中旧路径假设。
- changed-files 到测试成员的映射、依赖影响传播、CI shards、耗时记录与缓存键。
- 搜索/站点生成、清单生成和新增包模板。
- 新旧身份迁移测试；旧版本 descriptor 不重新定向到不同内容的归档。

本地索引文档记载过同一路径以多个 namespace 注册产生歧义的限制，需用最新 pinned 版本复核。
测试必须确认 mwrap 与 compat 都来自当前索引的预期 revision；若仍受限制，分别验证配方并增加组合安装测试，
不得用一次成功的远端解析冒充本地变更被验证。

index.toml 的 latest_mcpp、CI 和模板使用统一 pin。min_mcpp 是消费者的兼容下限，
仅在新目录/schema/语义确实要求时提高，不能因为测试使用最新版本就机械抬高全索引下限。

## 8. 测试与发布验收

| 维度 | 必须验证 |
|---|---|
| 基础 | 读写图、图像处理、各新增算法模块真实案例 |
| API | 单模块/聚合/组合 import、模板和 ADL、头文件混用、宏映射等价 |
| GUI | Linux Xvfb 等实际窗口环境、Windows/macOS 桌面 runner 创建窗口、imshow、事件循环与销毁 |
| ORT | 直接 Ort::Session、OpenCV ENGINE_ORT、已知小 ONNX 模型数值结果、无回退证据 |
| 视频/codec | 固定夹具 roundtrip，原生采集与设备相关项标明需设备 runner |
| feature | core-only 默认配置、常用图像配置、all-modules、highgui-native、dnn-onnxruntime、关键 codec/后端组合及配置错误诊断 |
| 二进制 | 干净机器运行、DLL/SO/dylib、动态加载 provider、资源、RPATH/install-name |
| 索引 | path/Git/发布归档目录一致、解析身份与来源、搜索和版本枚举、sha256、旧身份仍可用 |
| 工具链 | Linux GCC/LLVM，macOS arm64 LLVM，Windows MSVC ABI；额外目标按产物和验证开放 |

首次重构保留当前已声明 Linux musl 基础能力；ORT 没有兼容产物时，dnn-onnxruntime 在该目标明确拒绝。
组合空间不做无意义的全排列，采用依赖边覆盖、后端共存、公共 ABI 与跨平台关键组合。
发布报告把 passed、build-only、requires-device、unsupported 分开；缺少硬件时不伪造运行通过。
每项已启用依赖都有明确获取/链接/运行路径；关闭可选 feature 验证不引入其供应者。
许可证、SDK 再分发条件和镜像范围以实际选中依赖为准，不把硬件驱动视为普通可随包复制的库。

### 8.1 Linux 本地测试与其他平台 CI 分工

Linux 本机作为主要迭代环境：先确认实际运行的 mcpp 及 provider 精确版本，再按同一入口执行以下验证。
测试用临时目录/独立构建输出，不修改机器的全局索引或依赖配置来制造成功结果。

| 验证项目 | Linux 本地 | Linux CI | Windows/macOS CI |
|---|---|---|---|
| 索引发现最小 fixture | 必须：pkgs/mwrap 的身份、版本、安装和链接 | 重跑 path/Git/归档 | 原生安装与消费者构建 |
| core-only 与常用图像配置 | GCC/LLVM 编译及行为 | 发布前重跑 | 对应工具链构建及行为 |
| GUI | GTK3 + Xvfb 创建/显示/事件循环/销毁 | 重跑 GTK3 | Win32 / Cocoa；runner 桌面能力需先探测 |
| ORT | 直接 Session 与 OpenCV ORT CPU 数值测试 | 干净环境部署测试 | 各自 SDK 的直接/桥接推理与运行库加载 |
| 模块与宏 | 单独/聚合 import、头文件混用和类型身份 | 完整回归 | 捕获编译器/stdlib 差异 |
| native 摄像头/GPU | 有设备时运行 | 专用 runner | 专用或 self-hosted runner |

Windows/macOS 的本地机器不可用不阻止 Linux 实施；其他平台结果以 CI 为准。
CI 先发布测试产物和报告，失败要有可重现命令、工具版本和日志；修复后重跑相关任务。
普通 hosted runner 无法执行真实摄像头、桌面或 GPU 时，明确标为未执行，并按需增加专用 runner；不能将 build-only 当运行验证。
Linux 本地测试通过不等于其他平台通过；正式支持以该平台规定的验收结果为准。

实现阶段测试入口需要支持上述子集，文档中的布局不是已经存在的新测试命令。
本轮只做设计与源码核查，没有提前宣称这些 OpenCV/ORT 功能运行通过。

## 9. 分阶段交付

### P0：设计评审（当前）

输出本规范和实施方案，确定包身份、宏映射、版本政策、feature 精确展开与 pkgs/mwrap 索引分类。

### P1：组织与工具基线

核验/创建组织 fork，保留 Git 历史；目标 remote 指向 mcpplibs，旧源保留追踪。
新建 onnxruntime-m，应用统一布局；统一固定最新稳定 mcpp，旧功能在新工具下先通过。
不移动旧 tag、不重写旧 release、不把新内容复用旧 hash。

### P2：mwrap 模块和配置骨架

改新身份与公共模块前缀，迁移宏模块和兼容头；源码移到 upstream，生成/适配层分离。
完成配置输入图和 API 覆盖基线；验证消费者与打包模块闭包。

### P3：原生 GUI 与 ORT CPU

先完成每个平台窗口 TU/依赖/生成配置的最小验证，再接 highgui-native 与显式 highgui 后端 feature。
发布可测试 ORT CPU 供应者和 wrapper，接入 OpenCV dnn-onnxruntime，完成两条推理路径。
发布前验证运行时分发，不以链接成功结束。

### P4：主模块、codec 和视频补齐

导入剩余主模块并按依赖拓扑实现；补常用 codec 和原生采集，形成明确 default/all-modules 展开与后端能力矩阵。
对每个新增模块做 API 差异审计与真实行为验证。

### P5：contrib 与加速

按明确需求依次引入 contrib、OpenCL、CUDA、OpenVINO、TBB/IPP 等。
硬件、SDK、provider 和平台不支持项公开；达到矩阵条件后才标为 supported。

### P6：索引迁移与正式发布

收集/发布适配和测试选择器先支持新目录，再迁入新描述符及 tests/mwrap。
包发布、SDK 供应者、镜像与索引 descriptor 按真实产物依次落地。
对旧 capi 等封装采用逐包迁移，不把所有生态包改名绑在 OpenCV 重构里。

### P7：规范定稿与 skill 同步（用户已要求）

review 确定后，在 mcpp-index 的 docs/mwrap.md 发布正式 mwrap 规范，作为生态规范的唯一维护来源。
本仓库 .agents/docs 保留设计决策与历史，README/贡献指南链接正式规范，不长期维护两份各自演进的规范正文。

同步更新实际存在的技能入口：

- mcpp-index/.agents/skills/add-mcpp-index-package/SKILL.md：增加 mwrap 包形态、pkgs/mwrap 与 tests/mwrap 分类，
  修正“目录必须取完整包名首字母”的单一路径要求；补上 upstream[.N]、宏映射、feature 命名、SDK/runtime 和本地/CI 验收。
- mcpp/.agents/skills/mcpp-usage/SKILL.md：补充 mwrap 依赖与导入示例，明确 API namespace 不随模块前缀变化、
  feature 加法语义及默认项限制，链接索引正式规范，不复制完整配方说明。
- 后续 opencv-m/onnxruntime-m 的贡献说明：链接统一规范，记录各自上游同步、补丁、导出审计和 feature 测试方式。

只有实际需要新增通用封装技能时再创建，避免包索引技能、构建工具技能与库贡献说明职责重叠。
修改 skill 时读取适用的 skill-creator 与目标技能说明，再核查正文、示例、索引模板和 lint 规则是否一致。
技能中的迁移建议不能反过来重写已发布的旧包、URL 或 hash。
这些步骤在规范 review 后实施，本轮未提前修改其他仓库的 skill。

## 10. Review 决策表

| 决策 | 建议 |
|---|---|
| 仓库 | fork opencv-m 到 mcpplibs，新建 onnxruntime-m；contrib 初期集成在 opencv-m |
| 公共导入 | mcpplibs.mwrap.<libname>[.<upstream-module>] |
| API | 保持 cv:: / Ort:: / C API；宏替身单列 .macros namespace |
| 上游布局 | upstream 原始源码 + port 适配 + generated 稳定产物 |
| ORT 所有权 | compat SDK 供应者 + onnxruntime-m 模块封装；CPU 优先 |
| 封装版本 | 已确认：5.0.0、5.0.0.1……；升级上游后修订计数重置，不发布 .0 |
| feature 集合 | default=[]、core 为基础；模块/后端显式启用；all-modules 只代表主模块全集 |
| 索引 | pkgs/mwrap 平铺描述符，tests/mwrap 独立测试；源码与发布同布局，无路径转换 |
| mcpp | 当前 2026.10.21.1，实施前复核并统一精确固定 |
| 规范与技能 | 定稿后在 mcpp-index/docs/mwrap.md 维护正式规范，同步索引添加技能与 mcpp 使用技能 |

## 11. 参考与证据

- [OpenCV 5.0.0 modules](https://github.com/opencv/opencv/tree/5.0.0/modules)：官方 tag 清单经 API 核对。
- [OpenCV 5 DNN engines](https://docs.opencv.org/5.0/main_modules/dnn_engine_selection.html)：ORT 与原生引擎的区别。
- [ORT 官方构建](https://onnxruntime.ai/docs/build/inferencing.html)。
- [ORT provider 动态加载](https://onnxruntime.ai/docs/build/eps.html)。
- [mcpp 最新版本核对](https://api.github.com/repos/mcpp-community/mcpp/releases/latest)：本轮返回 v2026.10.21.1。
- [mcpp 2026.10.21.1 文档](https://github.com/mcpp-community/mcpp/tree/v2026.10.21.1/docs)：features、workspace、pack、action。
- 本地 mcpp-index：docs/repository-and-schema.md、tools/publish_mcpp_index.sh、.github/workflows/validate.yml、
  pkgs/o/opencv.opencv.lua、pkgs/m/mcpplibs.capi.lua.lua、index.toml、mcpp.toml。
- 本地 openkal-musl：musl/PATCHES.md、musl-generated/README.md、port/，用于分层参考。

本地检查已按 §1.2 更新并固定到远端 main；实施 PR 前仍须重新 fetch，并记录新的基线，不假定远端保持不变。

## 12. 多视角自我 review 与修正记录

本节为同一作者从不同职责视角自查，不代表额外独立审查或已完成的运行测试。

| 视角 | 找到的问题 | 本次修正 / 验收门槛 |
|---|---|---|
| 包管理维护者 | 根目录 mwrap/o 不在现有 mcpp 扫描范围，先前 staging 增加双布局成本 | 改 pkgs/mwrap 平铺；发布原样复制；不要求改客户端协议 |
| 配方开发者 | 最新代码可走 fallback，不代表 path/Git/provider 搜索全部自动正确 | Linux 最小 fixture 覆盖解析、搜索、安装、lockfile 与链接；其他平台 CI |
| 普通 OpenCV 用户 | gui、codec、ort 与上游名称不一致；full/desktop 无精确边界 | highgui-*、imgcodecs-*、videoio-*、dnn-onnxruntime；移除模糊聚合 |
| 精简构建用户 | 旧 features 文档把根 CLI 与依赖边 opt-out 混淆 | 实现与 E2E 126/127 证明依赖 default-features=false 可用；core-only 保留为产品选择，模板显式列功能 |
| GUI 使用者 | highgui API 或 all-modules 不等于存在实际窗口后端 | 分清模块/后端；摘要输出 NONE；窗口示例必须选具体后端或 highgui-native |
| 后端集成者 | GTK/Qt 共存或 native 覆盖规则不能凭注册表推断 | native 只增加；按上游能力验证共存；冲突在配置阶段拒绝 |
| 推理用户 | DNN 枚举可见、模型成功加载都不能证明用了 ORT | 直接 ORT 和 OpenCV bridge 分别执行，提供无回退证据 |
| 链接/ABI 维护者 | capability 选择不自动从图中移除另一份同符号运行库 | SDK 供应者只能引入一个主库变体；图验证与最终符号/运行测试 |
| 模块/API 维护者 | 使用 Python 标记生成导出并自动 prune 会遗漏 C++ API | 以公共 C++ 头为审计基准，缺失显式记录、禁止静默宣称完整 |
| 宏与混用用户 | namespace 不阻止宏展开，函数替身不等价于任意断言宏 | .macros 与原始宏兼容头分开示例，测试冲突、求值与调用位置 |
| 跨平台维护者 | Linux 通过无法证明 Cocoa TU、Windows DLL、各自 SDK 可用 | Linux 本地 + CI，Windows/macOS 原生 CI；桌面/GPU 缺项公开 |
| 发布维护者 | 只查链接依赖可能漏掉 ORT 动态 provider；新目录可能漏 CI | 显式 runtime 清单、干净环境执行、目录选择器/镜像/站点覆盖 |
| 版本维护者 | .0 与无尾号可能等序，+build 不能实现修订排序 | upstream[.N]、N>=1；不发布 .0；不可变旧版本与旧身份 |

仍需实施实测的事项：最新 bundled provider 的实际目录发现/跨 namespace 组合、Cocoa 的构建动作、
GTK/Qt 后端共存、ORT 的二进制与工具链兼容、根模块打包闭包。
这些被列为先做最小验证的工作，不作为需要用户猜测技术答案的决策题。

本轮 review 的产品决策主要是：接受 pkgs/mwrap 分类、上游风格 feature、最小默认 core-only、
all-modules 的模块全集语义，以及 Linux 本地/其他平台 CI 的验证分工。

## 13. 实施补充：模板与 checkpoint

所有新建/重构的 -m 封装库必须提供快捷模板。OpenCV 默认模板使用自生成图像完成处理与写出；
另提供窗口、视频、DNN/ORT 模板。ORT 默认模板内置小型、可再生成的 ONNX 模型并完成真实 CPU 推理。
验证从 mcpp new 渲染到构建和执行的完整链路，不能只 lint 模板文本。
各仓库使用一个功能分支和一个 PR，保留独立 checkpoint commits，不 squash 或 force-push 丢弃进度。
每次 checkpoint 记录实际已验证范围；最终按计划要求逐项审计，并等待全部相关 CI 通过。
远端权限不足期间继续本地实施，不能把仅本地完成当作 PR/CI 完成。

## 14. Feature opt-out 证据修正

mcpp parser 的 DependencySpec.defaultFeatures、prepare 图的 feature 合并，以及 E2E
126_default_features_opt_out.sh / 127_transitive_default_features_optout.sh 在已发布 2026.10.21.1 上均验证通过。
因此撤回先前“消费者不能关闭默认项”的判断；该判断来自过时且表述混淆的 features 文档。
已在 mcpp PR #836 修正中英功能文档和使用技能，同时修正索引规范。

组织权限已通过现有 Sunrisepeak 授权解决，mcpplibs/opencv-m fork 与 onnxruntime-m 仓库均已创建。
使用单次命令的授权，不改变本机默认 GitHub account。

## 15. mcpp 仓库边界（用户补充）

mcpp 引擎代码只读。已有 PR #836 仅涉及文档与使用 skill，允许保留，但现在不合入。
该 PR 的 review、CI 或合入状态不作为 opencv-m 实施与验证的前置条件。
OpenCV 的模块、feature、平台后端、测试和模板继续在 opencv-m 内推进；
索引分类及供应者适配在 mcpp-index 内处理。若遇到确切客户端缺口，保留最小复现并报告，
不扩大 mcpp 修改范围，也不因此停止不受该缺口影响的 OpenCV 工作。
