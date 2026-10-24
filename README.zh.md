# opencv-m

OpenCV **5.0.0** 的 C++23 模块封装，保留 `cv::Mat`、`cv::cvtColor` 等上游 API。

[English](README.md)

## 快速开始

使用 mcpp **2026.10.24.1**。以下为候选版本，索引发布后可直接创建项目。

```sh
mcpp new vision --template mcpplibs.mwrap.opencv@5.0.0.1
cd vision
mcpp run
```

默认模板生成图像、转换灰度并写出 PNG，不需要桌面或外部输入。

## 已有项目

```toml
[dependencies]
"mcpplibs.mwrap.opencv" = { version = "5.0.0.1", features = ["imgproc", "imgcodecs-png"] }
```

```cpp
import mcpplibs.mwrap.opencv;
namespace macros = mcpplibs::mwrap::opencv::macros;
int main() {
    cv::Mat image(64, 64, macros::CV_8UC3, cv::Scalar(20, 80, 160));
    return image.empty();
}
```

## 能力边界

core 始终可用。`all-modules` 选择 16 个主模块，编解码与后端分别启用；
模块可构建不代表全部 API 或硬件能力已验证。

- `highgui-native`：Linux x86_64 glibc 的 GTK3、Windows 的 Win32、macOS 的 Cocoa。
- `videoio-native`：V4L2、Media Foundation 或 AVFoundation；`videoio-ffmpeg` 增加 FFmpeg。
- `dnn-onnxruntime`：在 ORT CPU SDK 支持的目标上启用对应推理引擎。
- `simd`：[原生 CPU intrinsics](docs/simd.md)，入口为 `mcpplibs.mwrap.opencv.core.simd`。

另有 `:window`、`:video`、`:onnxruntime` 模板；窗口模板需要桌面会话。
需要预处理宏时，在 import 前包含 `<mwrap/opencv/macros.hpp>`。

## 文档

[用法与 feature](docs/usage.zh.md) · [架构](docs/architecture.md) ·
[示例](examples) · [验收记录](.agents/docs/2026-10-24-readiness-audit.md) · [Apache-2.0](LICENSE)
