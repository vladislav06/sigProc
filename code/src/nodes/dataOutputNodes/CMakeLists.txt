include_directories(./)

add_subdirectory(dataEncoders)

target_sources(sigProc PRIVATE
        fileEncoderNode.cpp
)
