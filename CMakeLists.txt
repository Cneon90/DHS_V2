cmake_minimum_required(VERSION 3.16)
project(DHS)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -Wall -g -Wl,-stack_size,0x01000000 -D_DEBUG")
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g3 -Wall -g -Wl,-stack_size,0x01000000")

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
        "C:/Program Files (x86)/Dev-Cpp/MinGW64/include"
        "C:/Program Files (x86)/Dev-Cpp/MinGW64/x86_64-w64-mingw32/include"
        "C:/Program Files (x86)/Dev-Cpp/MinGW64/lib/gcc/x86_64-w64-mingw32/4.9.2/include"
        "C:/Program Files (x86)/Dev-Cpp/MinGW64/lib/gcc/x86_64-w64-mingw32/4.9.2/include/c++"
)

# ”казываем пути к библиотекам
#link_directories("C:/Program Files (x86)/Dev-Cpp/MinGW64/lib"
#        "C:/Program Files (x86)/Dev-Cpp/MinGW64/x86_64-w64-mingw32/lib")
link_directories("K:/Soft/vcpkg/vcpkg/installed/x64-windows/lib"
        "C:/Program Files (x86)/Dev-Cpp/MinGW64/lib"
        "C:/Program Files (x86)/Dev-Cpp/MinGW64/x86_64-w64-mingw32/lib")

# ƒобавл€ем исполнимый файл
add_executable(DHS ${SOURCES})

# Ћинкуем библиотеки
target_link_libraries(DHS
        ws2_32
        wsock32
        )

# ”казываем дополнительные флаги дл€ компил€ции
#target_compile_options(DHS PRIVATE -Wall -g3)