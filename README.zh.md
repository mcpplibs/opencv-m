# opencv-m

通过 `mcpplibs.mwrap.opencv` 使用 OpenCV 5.0.0 C++ API，保留 `cv::` 名称。
候选包版本为 **5.0.0.1**：上游 5.0.0，加封装修订 1。

[English](README.md) · **中文**

本分支是实现 checkpoint。目前接入八个主模块，GTK3 原生窗口已通过 Linux 本地测试，
Win32/Cocoa 等待原生 CI 验证；其余主模块和 ORT 桥接仍在实施。
当前 CI 通过不代表已经完整支持 OpenCV。

## 构建本地 checkout

使用 .xlings.json 和 CI 固定的 mcpp **2026.10.21.1**：

```sh
mcpp test
mcpp test --features imgcodecs-png,imgcodecs-jpeg
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
| imgcodecs | 图像 codec API 与通用源码 |
| imgcodecs-png、imgcodecs-jpeg | 启用对应格式，并包含 imgcodecs |
| videoio | 视频 I/O API |
| videoio-ffmpeg | 增加 FFmpeg 实现和 compat.ffmpeg 依赖 |
| videoio-v4l2 | Linux V4L2，其他目标明确拒绝 |
| highgui | 只有 GUI API，当前窗口后端为 NONE |
| highgui-native | Linux x86_64 glibc 选择 GTK3，Windows 选择 Win32，macOS 选择 Cocoa |
| highgui-gtk3、highgui-win32、highgui-cocoa | 显式选择原生后端，目标不匹配时拒绝构建 |
| dnn | 现有 OpenCV 原生 DNN 实现，不代表启用 ORT |
| unifont | Unicode/CJK 字体渲染，包含 imgproc |

当前没有 full/all-modules 的完整功能承诺。GTK3 创建窗口、显示图像、处理事件和销毁窗口
已在本地通过，Win32/Cocoa 验证尚未完成；dnn-onnxruntime 仍待实现。
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

## 仓库布局与升级

- upstream/opencv 保存固定上游源码，upstream/sources.lock.toml 记录来源与归档 SHA256。
- port 保存配置输入与兼容头。
- src 保存模块接口和适配，generated 保存稳定生成物。
- build.mcpp 将 feature 相关配置写入自身输出目录。
- tools/generate_build.py 生成 manifest、依赖图与源码清单。
- templates 与 tests 保存可执行消费者和行为验证。

当前上游导入仍只包含八个主模块。API 完整性、新后端和安装归档均是后续发布门槛。
设计与实施证据位于 .agents/docs；共同 mwrap 规范维护在 mcpp-index/docs/mwrap.md。

## 许可证

根 LICENSE 为 Apache-2.0，包元数据与之保持一致。
内置 OpenCV 为 Apache-2.0，各第三方组件保留其自身许可证。
