# 设置项目名
set(PROJECT_NAME "TSG_MainServer")
project(${PROJECT_NAME})

# 找到Qt相关的模块包
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

# 手动列出头文件和源文件

include_directories(header)
include_directories(../TSG_NetWork/header)

set(SOURCES 
 src/TSGv3.0.0.cpp
 src/TSG_MainProcess.cpp   
 src/TSG_NetServer.cpp
 src/TSG_RegeditEditor.cpp
)

# 将源代码添加到此项目的可执行文件。
add_executable(TSG_MainServer ${SOURCES})

# 链接Qt模块
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets)

# 设置C++标准（如果CMake版本大于3.12）
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TSG_MainServer PROPERTY CXX_STANDARD 20)
endif()


# 链接到 TSG_NetWork 库
target_link_libraries(${PROJECT_NAME} TSG_NetWork)