# ======================================================================
#  DesktopAssistant  ―  一站式桌面助手（财务 / 提醒 / 密码 AES‑256）
#
#  依赖：
#    • CMake ≥ 3.15
#    • C++17 编译器（gcc‑10 / clang‑11 / MSVC‑19.28 及以上）
#    • OpenSSL ≥ 1.1.1    （仅用 libcrypto 做 AES / PBKDF2）
#
#  构建步骤（Linux / macOS / WSL）：
#      mkdir -p build && cd build
#      cmake  -DCMAKE_BUILD_TYPE=Release  ..
#      cmake --build .   -j$(nproc)
#
#  Windows（MSVC + vcpkg 提供 OpenSSL）：
#      mkdir build && cd build
#      cmake .. -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake
#      cmake --build . --config Release
#
#  生成的可执行文件：
#      build/DesktopAssistant     (或 DesktopAssistant.exe)
# ======================================================================

cmake_minimum_required(VERSION 3.15)
project(DesktopAssistant LANGUAGES CXX)

# ----------------------------------------------------------------------
#  1. 编译选项
# ----------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 建议的警告级别（可按需调整）
if (MSVC)
    add_compile_options(/W4 /permissive-)
else ()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif ()

# ----------------------------------------------------------------------
#  2. 查找第三方库 — OpenSSL (libcrypto)
# ----------------------------------------------------------------------
find_package(OpenSSL REQUIRED COMPONENTS Crypto)
message(STATUS "OpenSSL version: ${OpenSSL_VERSION}")

# ----------------------------------------------------------------------
#  3. 项目源码
#     ├── include/   头文件
#     └── src/       源文件
# ----------------------------------------------------------------------
# 头文件搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)

# 源文件列表（若新增 .cpp 仅需放到 src/ 即会被自动编译）
file(GLOB SRC_FILES CONFIGURE_DEPENDS
     ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(DesktopAssistant ${SRC_FILES})

# 链接 OpenSSL::Crypto
target_link_libraries(DesktopAssistant PRIVATE OpenSSL::Crypto)

# ----------------------------------------------------------------------
#  4. 可选安装规则
# ----------------------------------------------------------------------
# 运行 'cmake --install build --prefix <dir>' 进行部署
install(TARGETS DesktopAssistant
        RUNTIME DESTINATION bin)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h")

# ----------------------------------------------------------------------
#  5. 额外 IDE 支持
# ----------------------------------------------------------------------
# 生成 compile_commands.json 以便 clang‑d / coc.nvim 等工具自动补全
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

