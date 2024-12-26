include_directories(./)

add_subdirectory(generators)
add_subdirectory(dataDecoders)

target_sources(sigProc PRIVATE
        fileDecoderNode.cpp
        generatorNode.cpp
        inputFolderNode.cpp
        inputFileNode.cpp
)
