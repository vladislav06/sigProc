//
// Created by vm on 24.7.12.
//
#include "globals.h"

#include "src/nodes/objectNodes/setFieldNode.h"
#include "src/nodes/mathNodes/mathNode.h"
#include "src/nodes/previewNodes/previewNode.h"
#include "src/nodes/previewNodes/graphPreviewNode.h"
#include "src/nodes/dataInputNodes/generatorNode.h"
#include "src/nodes/mathNodes/fftNode.h"
#include "src/nodes/mathNodes/fmModulationNode.h"
#include "src/nodes/dataInputNodes/inputFolderNode.h"
#include "src/nodes/dataInputNodes/inputFileNode.h"
#include "src/nodes/fileNodes/getFileNode.h"
#include "src/nodes/foreachNode.h"
#include "src/nodes/dataInputNodes/fileDecoderNode.h"
#include "src/nodes/dataInputNodes/objectCreateNode.h"
#include "src/nodes/dataOutputNodes/fileEncoderNode.h"

void registerNodes(std::shared_ptr<QtNodes::NodeDelegateModelRegistry> nodeRegistry) {
    nodeRegistry->registerModel<InputFolderNode>("Input");
    nodeRegistry->registerModel<InputFileNode>("Input");
    nodeRegistry->registerModel<GetFileNode>("Input");

    nodeRegistry->registerModel<FileDecoderNode>("Input");

    nodeRegistry->registerModel<GeneratorNode>("Input");


    nodeRegistry->registerModel<MathNode<double >>("Math");
    nodeRegistry->registerModel<FftNode>("Math");
    nodeRegistry->registerModel<FmModulationNode>("Math");

    nodeRegistry->registerModel<ObjectCreateNode>("Object");
    nodeRegistry->registerModel<SetFieldNode>("Object");

    nodeRegistry->registerModel<PreviewNode>("Output");
    nodeRegistry->registerModel<GraphPreviewNode<double >>("Output");
    nodeRegistry->registerModel<FileEncoderNode>("Output");


    nodeRegistry->registerModel<ForeachNode>();


}