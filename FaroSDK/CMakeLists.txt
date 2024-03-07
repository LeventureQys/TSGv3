# CMakeList.txt: FaroSDK 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
# 设置项目名
set(PROJECT_NAME "FaroSDK")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
project(DevicFaroSDKeSDK)
# 找到Qt相关的模块包
find_package(Qt5 REQUIRED COMPONENTS Core)
# 将源代码添加到此项目的可执行文件。
include_directories(${CMAKE_SOURCE_DIR}/FaroSDK/include)
include_directories(../PublicVar)
include_directories(${CMAKE_SOURCE_DIR}/FaroSDK/libs)
# 添加FaroSDK目录
include_directories(lib)
set(SOURCES 
 src/FaroSDK.cpp

)


add_library (FaroSDK ${SOURCES})
# 链接Qt模块
 target_link_libraries(FaroSDK Qt5::Core)
 target_link_libraries(FaroSDK FARO.LS.SDK iQOpen)
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET FaroSDK PROPERTY CXX_STANDARD 20)
endif()
# TODO: 如有需要，请添加测试并安装目标。
