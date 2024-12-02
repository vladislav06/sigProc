cmake_minimum_required(VERSION 3.28)
project(sigProc)

set(CMAKE_CXX_STANDARD 23)

#find qt, QtNodes, CGAL
#fftw and gtest doesn't need to be found
find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        Charts
        REQUIRED)

find_package(CGAL COMPONENTS
        Core
        REQUIRED)

#find_package(QtNodes)

#settings
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(BUILD_SHARED_LIBS ON)
set(CGAL_DO_NOT_WARN_ABOUT_CMAKE_BUILD_TYPE ON)

#debug options
add_compile_options(-pedantic -fno-omit-frame-pointer -g -O0 -fno-inline)
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)


include(src/CMakeLists.txt)
#include(lib/nodeeditor/CMakeLists.txt)
add_subdirectory(lib/nodeeditor)

target_compile_definitions(sigProc PUBLIC
        NODE_EDITOR_STATIC)

target_sources(sigProc PRIVATE resources/resources.qrc)

#link
target_link_libraries(sigProc PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Charts
        #QXlsx::QXlsx
        CGAL::CGAL_Core
        fftw3
        gtest_main
        QtNodes::QtNodes
)



