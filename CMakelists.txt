cmake_minimum_required(VERSION 3.13)
# Zdefiniuj projekt o nazwie "dummy_project".
project(labubu)  # Nie ma potrzeby określania języka programowania.
 
# Domyślnie korzystaj ze standardu C++17.

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-Wall -Wextra -Werror -Wpedantic -pedantic-errors -Wconversion)

# Zdefiniuj konfigurację budowania "dummy_project", która uwzględnia plik źródłowy `main.cpp`.

include_directories(include)
include_directories(src)
include_directories(test)

set(SOURCE_FILES
        src/factory.cpp 
        src/helpers.cpp
        src/nodes.cpp
        src/package.cpp
        src/reports.cpp
        src/simulation.cpp
        src/storage_types.cpp
        src/types.cpp
        )



add_executable(labubu ${SOURCE_FILES} main.cpp)