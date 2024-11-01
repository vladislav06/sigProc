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
        PrintSupport
        DataVisualization
        3DCore
        3DRender
        REQUIRED)

find_package(CGAL COMPONENTS
        Core
        REQUIRED)

find_package(QtNodes)

#settings
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(BUILD_SHARED_LIBS ON)
set(CGAL_DO_NOT_WARN_ABOUT_CMAKE_BUILD_TYPE ON)

#debug options
add_compile_options(-pedantic -fno-omit-frame-pointer -g -O1 -fno-inline)
#add_compile_options(-fsanitize=address)
#add_link_options(-fsanitize=address)

include(src/CMakeLists.txt)

target_compile_definitions(sigProc PUBLIC
        NODE_EDITOR_STATIC)

#link
target_link_libraries(sigProc PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::PrintSupport
        Qt::Charts
        #QXlsx::QXlsx
        Qt::DataVisualization
        Qt::3DCore
        Qt::3DRender
        CGAL::CGAL_Core
        fftw3
        gtest_main
        QtNodes
)

