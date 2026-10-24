# opencv

> 以 C++23 模块使用 OpenCV 5.0.0，保留上游 cv:: API。
> 十六个主模块；原生窗口、编解码、视频和 ORT CPU 按需启用，上游源码与适配分层。

[English](README.md) · **简体中文**

上游仓库：[opencv/opencv](https://github.com/opencv/opencv)

[Releases](https://github.com/mcpplibs/opencv-m/releases) · [mcpp](https://github.com/mcpp-community/mcpp) · [Index](https://github.com/mcpplibs/mcpp-index)

[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg)](https://en.cppreference.com/w/cpp/23)
[![License](https://img.shields.io/badge/license-Apache-2.0-blue.svg)](LICENSE)
[![CI](https://github.com/mcpplibs/opencv-m/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/mcpplibs/opencv-m/actions/workflows/ci.yml)

## 快速开始

```sh
mcpp new vision --template mcpplibs.mwrap.opencv@5.0.0
cd vision
mcpp run
```

```toml
[dependencies]
mcpplibs.mwrap.opencv = { version = "5.0.0", features = ["imgproc", "imgcodecs-png"] }
```

```cpp
import mcpplibs.mwrap.opencv;
namespace macros = mcpplibs::mwrap::opencv::macros;
int main() { return cv::Mat(8, 8, macros::CV_8UC3).empty(); }
```

## 模块

根模块聚合已启用模块，也可单独 import `.core`、`.imgproc`、`.dnn` 等。
见[十六模块与 API 矩阵](docs/coverage.md)。

## 特性开关

core 必需，`default=[]`；按需叠加以下能力。

| Feature | 用途 |
|---|---|
| `all-modules` | 十六个主模块，后端仍显式选择 |
| `highgui-native` | GTK3 / Win32 / Cocoa 窗口 |
| `imgcodecs-png`, `imgcodecs-jpeg`, `imgcodecs-webp` | 按格式选择图像编解码 |
| `videoio-native`, `videoio-ffmpeg` | 平台原生 / FFmpeg 视频 |
| `dnn-onnxruntime` | ORT CPU 推理 |
| `simd`, `simd-emulated` | 原生基线 / 软件向量 |

宏映射位于 `.macros`；文本宏在 import 前包含 `<mwrap/opencv/macros.hpp>`。

## 示例

默认模板自行生成图像；另有 `:window`、`:video`、`:onnxruntime` 模板。
[示例](examples) · [使用说明](docs/usage.zh.md)

## 构建与工具链

已验证 Linux GCC/LLVM、Windows LLVM、macOS ARM64 LLVM；窗口需要桌面会话。
真实摄像头需要设备；GPU/contrib 与非基线 ISA 不在本次发布范围。
[架构](docs/architecture.en.md) · [覆盖与编译器限制](docs/coverage.md)

## 许可证

[Apache-2.0](LICENSE)；保留上游及选中依赖的通知。
