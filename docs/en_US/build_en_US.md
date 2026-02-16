## Building DiskBox (English)

### 1. Prerequisites
Ensure you have the following:
* A properly functioning 64-bit system running Windows 10 or later, or use a cross-compiler on other platforms;
* Visual Studio 2017 or later, with the corresponding MSVC build toolchain;
* Windows SDK 10.0.26100.0 or later;

### 2. Clone this repository

> [!NOTE]
> You may need to configure SSH manually.
### 3. Download and compile wxWidgets source code
1. Download the source code of wxWidgets 3.3.1 from one of the following links:
- [wxWidgets 3.3.1 (.zip file)](https://github.com/wxWidgets/wxWidgets/releases/download/v3.3.1/wxWidgets-3.3.1.zip)
- [wxWidgets 3.3.1 (.7z file)](https://github.com/wxWidgets/wxWidgets/releases/download/v3.3.1/wxWidgets-3.3.1.7z)
2. Extract the downloaded source code manually.
3. Open `x64 Native Tools Command Prompt for VS 2022` (select according to your VS version), navigate to the `build/msw` directory, and execute the following commands:
- Clean build files (optional)
```
nmake -f makefile.vc clean
```
- Compile Debug static library (x64, MTd)
```
nmake -f makefile.vc BUILD=debug TARGET_CPU=AMD64 SHARED=0 UNICODE=1 USE_RUNTIME_LIBS=0 CPPFLAGS=/utf-8
```
- Compile Release static library (x64, MTd)
```
nmake -f makefile.vc BUILD=release TARGET_CPU=AMD64 SHARED=0 UNICODE=1 USE_RUNTIME_LIBS=0 CPPFLAGS=/utf-8
```
4. Set up dependencies
- If the build is successful, a `vc_x64_lib` folder should be generated in the `lib` directory, with a size of approximately 1.24 GB.
  Copy its contents to `DiskBox/libs/wxWidgets/lib/vc_x64_lib`.
- Copy the contents of the `include` folder from the compressed package to `DiskBox/libs/wxWidgets/lib/include/`.

5. Load the CMake project and execute the `DiskBox` build.