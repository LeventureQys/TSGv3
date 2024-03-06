# CMakeList.txt: TSG_Dialog 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
# 设置项目名
set(PROJECT_NAME "DeviceSDK")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
project(DeviceSDK)
# 找到Qt相关的模块包
find_package(Qt5 REQUIRED COMPONENTS Core Widgets SerialPort)
# 将源代码添加到此项目的可执行文件。
include_directories(include)
include_directories(../PublicVar)

set(SOURCES 
 src/DeviceSDK.cpp
 src/MainServiceControll.cpp
)


add_library (DeviceSDK ${SOURCES})
# 链接Qt模块
 target_link_libraries(DeviceSDK Qt5::Core Qt::Widgets Qt::SerialPort)
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET DeviceSDK PROPERTY CXX_STANDARD 20)
endif()
# TODO: 如有需要，请添加测试并安装目标。
