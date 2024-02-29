# CMakeList.txt: TSG_NetWork 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
# 设置项目名
set(PROJECT_NAME "TSG_NetWork")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
project(${PROJECT_NAME})
# 找到Qt相关的模块包
find_package(Qt5 REQUIRED COMPONENTS Core Network)

include_directories(header)

set(SOURCES 
 src/TSG_NetWork.cpp
 src/QtTcpBaseHandler.cpp
 src/QtTcpClientHandler.cpp
 src/QtTcpServerHandler.cpp
)


# 将源代码添加到此项目的可执行文件。
add_library (TSG_NetWork ${SOURCES})
# 链接Qt模块
 target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Network)
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TSG_Dialog PROPERTY CXX_STANDARD 20)
endif()

# TODO: 如有需要，请添加测试并安装目标。
