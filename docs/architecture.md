# opencv-m 架构

[English](architecture.en.md)

`mcpplibs.mwrap.opencv` 为 OpenCV 5.0.0 提供 C++23 模块入口，封装修订为 5.0.0.1。
上游 `cv::` 名称与类型身份保持一致；包名和模块名表达封装层归属。当前 checkpoint
仍在补齐公共 API 与后端覆盖，不能据此宣称完整支持。

## 源码归属

- `upstream/opencv`：与 `upstream/sources.lock.toml` 中 SHA256 固定的官方归档逐字节一致。
  已导入全部 16 个主仓库 C++ 模块；语言绑定、测试框架与 contrib 单独界定范围。
- `port`：配置输入、源码清单与兼容头。
- `generated`：提交的稳定配置、dispatch 与资源；feature 相关产物进入构建输出目录。
- `src`：公共接口，以及内部链接实体必须使用的显式适配。
- `templates`、`tests`、`tools`：可运行消费者、行为验证、导入、生成与覆盖审计。

OpenCV 实现对象只有一个所有者，不能再链接另一份 compat.opencv。FFmpeg 与 ORT
来自声明的供应者依赖；模块接口编译时不执行隐藏下载。

## 公共接口与内部适配单元

`src/core.cppm` 导出上游 core 实体，并提供 namespace 级 static-inline 运算符与函数
所需的适配。值常量与纯计算宏位于 `mcpplibs.mwrap.opencv.macros`；原始预处理宏
通过 import 前包含 `<mwrap/opencv/macros.hpp>` 使用。

其余公共模块由生成的聚合单元提供。例如 `mcpplibs.mwrap.opencv.stitching`
转导出 core、必需模块依赖与独立的 `mcpplibs.mwrap.opencv.detail.stitching` 适配单元。
内部单元在 GMF 包含上游头文件并使用 export using 导出实体。模块名中的 detail 是
封装实现细节；上游 `cv::detail` 下的拼接 API 则是公共 C++ API，保留原 namespace。

将文本 GMF 与依赖 import 放在不同单元，避免 GCC 16 在 core.hpp 与 import core
位于同一单元时对 SVD 默认参数报冲突。本图使用接口分区还触发了 GCC 16 的合并 ICE；
独立命名的适配单元已通过直接导入行为测试，无需修改编译器、mcpp 或上游头文件。

根接口导出 core、macros 与配置相关的选择模块。选择模块写入直接 import，启用 ORT 时
也记录其模块依赖，保持扫描结果及 GCC BMI mapper 路径一致。源码文件存在不等于 feature 已启用。

## 构建生成与 feature

`tools/generate_build.py` 从固定版本的 CMake 模块声明推导必需依赖，生成公共聚合接口、
manifest 与平台源码清单。内部适配单元单独维护，重新生成时保留内容。
`build.mcpp` 选择清单并在自身输出目录生成私有配置；消费者不运行 CMake。

默认 feature 为空，core 始终存在。all-modules 选择 16 个主模块实现，编解码、原生窗口、
视频后端与外部推理 SDK 分别显式选择。highgui-native 按目标选择 GTK3/Win32/Cocoa，
videoio-native 选择 V4L2/MSMF/AVFoundation。显式后端在目标不匹配时报错。
dnn-onnxruntime 包含 dnn，并通过独立 ORT 封装引入官方 CPU SDK。

ISA 与 pthread 编译模式只用于实现 TU。公共及内部模块 BMI 保持基线可消费，CI 从实际
compile_commands 检查全部接口。内置 zlib 使用上游符号前缀，Unix 下私有 codec 符号
隐藏，避免与 GTK SDK 的共享 codec 发生符号抢占。

## 验证与边界

Python 绑定解析器与 curated 列表是生成输入，不是完整 C++ API 的证明。
`tools/audit_api.py --verify-exports` 使用实际 all-modules Clang 配置记录 namespace/global
声明、含模板的重载签名、内部链接与头文件加载覆盖，并编译根模块及每个公共子模块的独立消费者。
CI 保存报告和消费者日志。

未加载的头、未列入导出列表的实体、内部链接适配、SDK 相关接口与模板/重载行为分别审阅。
类定义可达不代表每个方法都在每种后端配置下运行。纯 import 与文本头混用分别测试；
GCC 上完整 core.hpp 混用仍有声明合并限制。源码模块数量不能代替完整支持验收。

运行测试使用生成的图像、模型与视频。ORT 检查实际 CPU session profile，避免原生回退
冒充通过。GUI 测试实际创建、显示、销毁窗口；Linux 相机测试不声称验证实体设备采集。
发布门槛还包括真实不可变归档消费、SDK 许可通知与程序搬迁后的运行验证。

## 升级步骤

1. 使用 `tools/vendor/import_opencv.sh` 导入固定官方归档。
2. 使用 `tools/check_upstream.py` 验证保留文件的字节。
3. 重新生成构建元数据、导出与宏映射，审阅全部 diff。
4. 重跑 AST/直接导入、行为、后端、归档与原生 CI 门槛。
5. 版本沿用上游，必要时追加一个数字封装修订段。

`.agents/docs` 的验收记录保存 checkpoint 证据与剩余门槛。mcpp 引擎源码只读，
其文档 PR 独立于本库进度。

## 标量适配的语义验证

`saturate_cast` 适配从固定版本 `saturate.hpp` 生成，保留十三种输入重载及上游特化；私有普通模块函数桥接 TU-local `cvRound`。测试在 import 前捕获原始 SDK 函数指针，对 169 组输入/输出类型独立比较，覆盖饱和边界和舍入。`makePtr` 保留上游自定义 deleter 禁用规则，通过正反编译用例验证。泛型 `cv::min/max` 使用上游导入的标准库实体，并保留图像三参数重载的选择行为。

[Coverage matrices / 覆盖矩阵](coverage.md)

## 分发文件

生成清单通过 `runtime.deploy` 显式保留十四份 wrapper/上游许可与版权文件，并保持来源路径，避免组件同名 LICENSE 覆盖。静态实现不依赖共享库扫描发现通知。索引中的实际模板打包测试逐字节检查这些文件，并从独立目录运行搬迁后的程序。

## FileStorage 与共享运行时

序列化适配保留固定 SDK 的 65 份 inline 函数体，内部调用显式限定到适配层，向量代理放入模块私有 namespace。基础函数仍调用原始 SDK，用户自定义序列化仍支持 ADL。枚举保留可选 void* 模板参数，同时避免向模块导出会触发 Clang 崩溃的 dependent enable_if 类型。独立的原始头文件程序和模块程序逐字节比较 XML/YAML/JSON 及 Base64 六种输出。

Linux glibc 项目目标显式选择 toolchain-coupled C++ 运行时，与 ORT 模板一致。实际复现显示加载 SDK 时嵌入另一份静态异常运行时会导致 cv::Exception 捕获终止；改为共享运行时后，同一组测试通过。ORT 模板同时验证推理和异常捕获。

core/imgproc HAL 的公共函数与类型保持 `cv::hal` 名称及上游运行时派发；
`@cond IGNORED` 的旧兼容别名和私有滤波类不加入该接口。instrumentation 导出
原始节点类型与外部接口，保持当前 SDK 未启用采集时的 no-op 行为；整数 setFlags
转发给原始枚举函数。video 补上文档公开的 tracking 扩展接口，并以用户派生特征
类验证上游 TrackerFeatureSet 的真实派发。
