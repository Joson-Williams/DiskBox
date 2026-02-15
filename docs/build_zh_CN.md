## 构建 DiskBox (简体中文)

### 1. 先决条件
确保你具备以下条件：
* 可以正常运行的、Windows 10或更高、 64位系统，或在其它平台上使用交叉编译器；
* Visual Studio 2017 或更高版本、对应的MSVC编译工具链；
* Windows SDK 10.0.26100.0 或更高版本；

### 2. clone 本仓库
```
git clone https://github.com/joson-williams/diskbox.git
```
> [!NOTE]
> 你可能需要自行配置 SSH
### 3. 下载 wxWidgets 源码并编译
1. 在以下链接中任选其一，下载 wxWidgets 3.3.1 的源码：
- [wxWidgets 3.3.1 (.zip file)](https://github.com/wxWidgets/wxWidgets/releases/download/v3.3.1/wxWidgets-3.3.1.zip)
- [wxWidgets 3.3.1 (.7z file)](https://github.com/wxWidgets/wxWidgets/releases/download/v3.3.1/wxWidgets-3.3.1.7z)</br>
2. 自行解压下载到的源码。</br>
3. 打开`x64 Native Tools Command Prompt for VS 2022`（根据你的 VS 版本选择），进入`build/msw`目录，执行以下命令：
- 清理编译文件（可选）
```
nmake -f makefile.vc clean
```
- 编译Debug静态库（x64，MTd）
```
nmake -f makefile.vc BUILD=debug TARGET_CPU=AMD64 SHARED=0 UNICODE=1 USE_RUNTIME_LIBS=0 CPPFLAGS=/utf-8
```
- 编译Release静态库（x64，MTd）
```
nmake -f makefile.vc BUILD=release TARGET_CPU=AMD64 SHARED=0 UNICODE=1 USE_RUNTIME_LIBS=0 CPPFLAGS=/utf-8
```

4. 搭建依赖</br>
- 如果构建正确，应当会在`lib`目录下生成`vc_x64_lib`文件夹，其大小约为1.24 GB
，将其内容复制到`DiskBox/libs/wxWidgets/lib/vc_x64_lib`下。
- 将压缩包里的`include`文件夹内容复制到`DiskBox/libs/wxWidgets/lib/include/`下。

5. 加载 CMake 项目，执行`DiskBox`构建。