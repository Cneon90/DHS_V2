cmake_minimum_required(VERSION 3.16)
project(DHS)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)

#set(CMAKE_GENERATOR_PLATFORM "x64")
set(CMAKE_GENERATOR_PLATFORM "x86")
set(CMAKE_EXE_LINKER_FLAGS "-static")
#set(CMAKE_EXE_LINKER_FLAGS "/MT")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MT")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -Wall -g -Wl,-stack_size,0x01000000 -D_DEBUG")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g3 -Wall -g -Wl,-stack_size,0x01000000")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/build/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/build/release)

set(CMAKE_TOOLCHAIN_FILE "K:/Soft/vcpkg/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
set(CMAKE_PREFIX_PATH "K:/Soft/vcpkg/vcpkg/installed/x64-windows")
#set(CMAKE_PREFIX_PATH "K:/Soft/vcpkg/vcpkg/installed/x64-windows/share/indicators")
# ”казываем каталог с исходниками
file(GLOB SOURCES
        *.cpp
        *.c
        extLib/*.cpp
        SgDialog/*.cpp
        SGP2/*.c
        )

# ”казываем пути к заголовочным файлам
include_directories(
        "K:/Soft/vcpkg/vcpkg/installed/x64-windows/include"
        "K:/Soft/vcpkg/vcpkg/installed/x64-windows/lib"
)

# ”казываем пути к библиотекам
link_directories(
        "K:/Soft/vcpkg/vcpkg/installed/x64-windows/lib"
)

find_package(indicators CONFIG REQUIRED)
find_package(Crow CONFIG REQUIRED)


# ƒобавл€ем исполнимый файл
add_executable(DHS ${SOURCES})

# Ћинкуем библиотеки
target_link_libraries(DHS PRIVATE
#        ws2_32
#        wsock32
        ws2_32
        wsock32
        kernel32
        user32
        gdi32
        winspool
        shell32
        ole32
        oleaut32
        uuid
        comdlg32
        advapi32
        indicators::indicators
        PRIVATE Crow::Crow asio::asio
)

# ”казываем дополнительные флаги дл€ компил€ции
#target_compile_options(DHS PRIVATE -Wall -g3)