# 控制所有设备的dll
# 项目特定的逻辑。
# 设置项目名
set(PROJECT_NAME "Devices")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
project(${PROJECT_NAME})
# 找到Qt相关的模块包
find_package(Qt5 REQUIRED COMPONENTS Core SerialPort)
# 将源代码添加到此项目的可执行文件。
include_directories(Header)
include_directories(../PublicVar)

set(SOURCES 
    Src/TSG_MainSerialPort.cpp
)
# 将moc生成的源文件也加入到SOURCES中
list(APPEND SOURCES ${MOC_SOURCES})
add_library (Devices SHARED ${SOURCES})
# 链接Qt模块
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::SerialPort)
if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Devices PROPERTY CXX_STANDARD 20)
endif()

# TODO: 如有需要，请添加测试并安装目标。
