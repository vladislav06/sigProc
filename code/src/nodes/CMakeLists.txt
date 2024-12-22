include_directories(./)
add_subdirectory(dataTypes)
add_subdirectory(dataInputNodes)
add_subdirectory(dataOutputNodes)
add_subdirectory(mathNodes)
add_subdirectory(objectNodes)
add_subdirectory(previewNodes)
add_subdirectory(fileNodes)
add_subdirectory(internalForeachNodes)
target_sources(sigProc PRIVATE
        dynamicDataFlowGraphModel.cpp
        baseNode.cpp
        foreachNode.cpp
        nodePort.cpp
)

