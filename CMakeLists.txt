# CMakeList.txt: RACparse 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 14)
project ("RACparse")

find_package(Threads REQUIRED)

# 将源代码添加到此项目的可执行文件。
add_executable (RACparse "RACparse.cpp" "RACparse.h" "TinyGPS++.cpp" "TinyGPS++.h")
target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
# "serialPort.cpp" "serialPort.h"
install(
	TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION "${CMAKE_INSTALL_NAME_DIR}" COMPONENT bin
)
# TODO: 如有需要，请添加测试并安装目标。
