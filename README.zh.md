# opencv-m

通过 `mcpplibs.mwrap.opencv` 使用 OpenCV 5.0.0 C++ API，保留 `cv::` 名称。
候选包版本为 **5.0.0.1**：上游 5.0.0，加封装修订 1。

[English](README.md) · **中文**

本分支是实现 checkpoint。目前已接入全部十六个主模块，并通过 Linux GCC/LLVM 本地测试；GTK3 原生窗口也已通过本地测试，
Cocoa 已通过原生 CI，Win32 尚待结果；公共 API 覆盖审计、更多视频/codec 后端仍在实施；ORT CPU 桥接已通过 Linux 验证。
当前 CI 通过不代表已经完整支持 OpenCV。

## 构建本地 checkout

使用 .xlings.json 和 CI 固定的 mcpp **2026.10.24.1**：

```sh
mcpp test
mcpp test --features imgcodecs-png,imgcodecs-jpeg
mcpp test --features all-modules --strict
mcpp test --features dnn,unifont
python3 tools/test_template.py
```

CI 覆盖 Linux GCC/LLVM、Linux musl 静态、macOS arm64 和 Windows MSVC ABI。
五项均执行 core 与 PNG/JPEG 行为测试；DNN/unifont 组合当前由 Linux GCC 执行。
硬件能力需要各自的运行证据。

## 快捷模板

默认模板自行生成图像、转为灰度并写出 out.png，不需要输入图片。
开发验证会实际执行 mcpp 渲染、检查生成清单，再构建和运行工程。

候选版本发布并加入索引后：

```sh
mcpp new myvision --template mcpplibs.mwrap.opencv@5.0.0.1
cd myvision
mcpp run
```

命名模板 `window` 会显示生成的图像并处理窗口事件：

```sh
mcpp new mywindow --template mcpplibs.mwrap.opencv@5.0.0.1:window
cd mywindow
mcpp run
```

Linux 需要图形会话与 `pkg-config` 命令；GTK3 头文件与库由声明的 SDK 依赖提供。
无桌面的 CI 使用 Xvfb，并通过 `mcpp run -- --smoke-test` 自动关闭窗口。

命名模板 `onnxruntime` 通过 OpenCV 的 ENGINE_ORT 运行生成的 ONNX Add 模型，
并同时导入独立 ORT 模块：

```sh
mcpp new inference --template mcpplibs.mwrap.opencv@5.0.0.1:onnxruntime
cd inference
mcpp run
```

模板显式声明 Linux 共享 SDK 所需的 toolchain-coupled C++ runtime。
桥接测试必须读到 ORT 实际生成的 CPU session profile，原生引擎回退不能通过验收。
启用 dnn-onnxruntime 后，选择聚合模块也会转导出 ORT 模块，使 GCC 的跨包 BMI 路径保持一致；
`cv::` 与 `Ort::` 名称不变。

开发用安装模板 fixture 会预置候选源码；它不能替代尚待执行的正式归档与索引安装验收。

## 依赖与 feature

```toml
[dependencies."mcpplibs.mwrap"]
opencv = { version = "5.0.0.1", features = ["imgcodecs-png", "imgcodecs-jpeg"] }
```

`default = []`，core 始终存在。可选模块按上游依赖关系展开。
依赖边支持通过 `default-features = false` 关闭该边的默认请求。

| Feature | 当前行为 |
|---|---|
| imgproc、flann、geometry | 按需启用对应算法模块 |
| calib、features、objdetect、photo、ptcloud、stereo、stitching、video | 其余上游主仓库 C++ 模块 |
| all-modules | 全部十六个主模块；外部 codec、窗口后端和 SDK 仍显式选择 |
| imgcodecs | 图像 codec API 与通用源码 |
| imgcodecs-png、imgcodecs-jpeg | 启用对应格式，并包含 imgcodecs |
| videoio | 视频 I/O API |
| videoio-ffmpeg | 增加 FFmpeg 实现和 compat.ffmpeg 依赖 |
| videoio-v4l2 | Linux V4L2，其他目标明确拒绝 |
| videoio-native | 按目标选择 Linux V4L2、Windows Media Foundation 或 macOS AVFoundation |
| videoio-msmf / videoio-avfoundation | 显式选择 Windows MSVC ABI / macOS 原生视频后端，拒绝目标不匹配 |
| highgui | 只有 GUI API，当前窗口后端为 NONE |
| highgui-native | Linux x86_64 glibc 选择 GTK3，Windows 选择 Win32，macOS 选择 Cocoa |
| highgui-gtk3、highgui-win32、highgui-cocoa | 显式选择原生后端，目标不匹配时拒绝构建 |
| dnn | OpenCV 原生 DNN 实现，不代表启用 ORT |
| dnn-onnxruntime | 启用 dnn，并通过 mcpplibs.mwrap.onnxruntime 引入官方 ORT 1.25.1 CPU SDK |
| unifont | Unicode/CJK 字体渲染，包含 imgproc |

all-modules 选择 16 个主模块实现，不代表完整 API 或后端覆盖。GTK3 创建窗口、显示图像、处理事件和销毁窗口
已在本地通过，Cocoa 和 Win32 窗口已通过原生 CI；ORT CPU 推理和真实 session profile 已通过 Linux GCC/LLVM 与 macOS，
Windows 验证仍待 CI。
只启用 highgui API 调用 imshow 仍会报告缺少窗口后端，需要同时选择后端 feature。

## 模块、API 与宏

```cpp
import mcpplibs.mwrap.opencv;
// 也可以直接导入已启用的模块：
// import mcpplibs.mwrap.opencv.imgproc;
```

根模块聚合 core、macros 和已选择的模块。构建程序在配置独立的输出目录中生成聚合选择模块，
以直接 import 交给扫描器；源码存在不代表消费者已启用该模块。

上游的 `cv::Mat`、`cv::cvtColor` 等名称不变。宏的类型化映射单独放置：

```cpp
namespace macros = mcpplibs::mwrap::opencv::macros;
cv::Mat image(64, 64, macros::CV_8UC3, cv::Scalar(20, 80, 160));
```

需要 CV_Assert 等预处理宏时，在 import **之前**包含 `<mwrap/opencv/macros.hpp>`。
原始宏仍会在限定名称中展开，因此宏形式与类型化映射应在不同使用上下文中选择。


类型化映射包含 OpenCV 5 的全部 13 种标量深度、通道类型族
（如 `CV_16BFC(n)`、`CV_BoolC(n)`、`CV_64UC(n)`）、HAL 值常量和矩阵布局函数。
运行 `python3 tools/generate_macros.py` 重新生成，`--check` 检查生成结果。
声明标记和控制流宏仍通过文本兼容头提供。类型化 `CV_IS_SUBMAT` 使用实际存在的
`CV_SUBMAT_FLAG`，修复上游 5.0.0 对未定义 `CV_MAT_SUBMAT_FLAG` 的引用；原始宏不改动。
测试通过实际 Mat 分配、读写与 ROI 标记验证布局。

需要审计当前封装头可见的 C++ API 时，先运行
`mcpp test --features all-modules --toolchain llvm@23.1.3 --strict`，再运行
`python3 tools/audit_api.py --verify-exports`。报告分别记录签名、所属头、内部链接与导出列表，
并编译独立的纯 import 消费者检查名称查找。未加载的公共头和未列出的声明仍需审阅；
名称可见不等于全部重载或算法行为已覆盖。CI 保存报告与消费者日志供 review。

## 仓库布局与升级

- upstream/opencv 保存固定上游源码，upstream/sources.lock.toml 记录来源与归档 SHA256。
- port 保存配置输入与兼容头。
- src 保存模块接口和适配，generated 保存稳定生成物。
- build.mcpp 将 feature 相关配置写入自身输出目录。
- tools/generate_build.py 生成 manifest、依赖图与源码清单。
- tools/check_upstream.py 对照 SHA256 固定的上游归档验证保留文件字节。
- templates 与 tests 保存可执行消费者和行为验证。

当前已导入全部十六个主仓库 C++ 模块。新增 SIFT 与金字塔光流使用 baseline dispatch，
FAST 保留 x86 AVX2 实现。八个新增模块的 OpenCL 内核尚未接入，配置会明确拒绝启用，
不会把占位头视为 OpenCL 支持。API 完整性、新后端和安装归档仍是后续发布门槛。
设计与实施证据位于 .agents/docs；共同 mwrap 规范维护在 mcpp-index/docs/mwrap.md。

## 许可证

根 LICENSE 为 Apache-2.0，包元数据与之保持一致。
内置 OpenCV 为 Apache-2.0，各第三方组件保留其自身许可证。

命名模板 `video` 使用 FFmpeg 编码并解码 12 帧生成图像，列出原生相机后端；不需要相机硬件。Linux V4L2 测试只验证注册和缺失设备路径，不能证明实体相机采集。

```sh
mcpp new video-demo --template mcpplibs.mwrap.opencv@5.0.0.1:video
cd video-demo
mcpp run
```
