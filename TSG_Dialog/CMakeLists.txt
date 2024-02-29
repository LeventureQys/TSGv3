# CMakeList.txt: TSG_Dialog 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
# 设置项目名
set(PROJECT_NAME "TSG_Dialog")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
project(${PROJECT_NAME})
# 找到Qt相关的模块包
find_package(Qt5 REQUIRED COMPONENTS Core)
# 将源代码添加到此项目的可执行文件。
add_library (TSG_Dialog "TSG_Dialog.cpp" "TSG_Dialog.h")
# 链接Qt模块
 target_link_libraries(${PROJECT_NAME} Qt5::Core)
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TSG_Dialog PROPERTY CXX_STANDARD 20)
endif()

# TODO: 如有需要，请添加测试并安装目标。
