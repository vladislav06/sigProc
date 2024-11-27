include_directories(./)

target_sources(sigProc PRIVATE
        mathNode.cpp
        fftNode.cpp
)
add_subdirectory(operations)
