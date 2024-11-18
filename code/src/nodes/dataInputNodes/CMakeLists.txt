include_directories(./)

add_subdirectory(generators)

target_sources(sigProc PRIVATE
        waveformDataInputNode.cpp
        generatorNode.cpp
)
