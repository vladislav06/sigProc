//
// Created by vm on 24.30.10.
//

#pragma once

#include "ui_mainWindow.h"
#include "src/nodes/dataInputNodes/waveformDataInputNode.h"
#include "src/nodes/dataInputNodes/objectCreateNode.h"

#include "src/nodes/dataOutputNodes/waveformDataOutputNode.h"
#include "src/nodes/objectNodes/setFieldNode.h"
#include "src/nodes/mathNodes/mathNode.h"
#include "src/nodes/dynamicDataFlowGraphModel.h"
#include "src/nodes/previewNodes/previewNode.h"
#include "src/nodes/previewNodes/graphPreviewNode.h"
#include "src/nodes/dataInputNodes/generatorNode.h"
#include "src/nodes/mathNodes/fftNode.h"
#include "src/nodes/mathNodes/fmModulationNode.h"
#include "src/nodes/dataInputNodes/inputFolderNode.h"
#include "src/nodes/dataInputNodes/inputFileNode.h"
#include "src/nodes/fileNodes/getFileNode.h"
#include "src/nodes/foreachNode.h"
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeDelegateModelRegistry>

class MainWindow : public QMainWindow, private Ui::mainWindow {
Q_OBJECT

private:
    QtNodes::DataFlowGraphicsScene *scene;
    QtNodes::GraphicsView *view;
    DynamicDataFlowGraphModel *dataFlowGraphModel;
    QString currentFile;
    bool dirty;

public:
    explicit MainWindow(QWidget *parent = nullptr);

    std::shared_ptr<QtNodes::NodeDelegateModelRegistry> nodeRegistry;


    void registerNodes() {
        nodeRegistry->registerModel<InputFolderNode>("Input");
        nodeRegistry->registerModel<InputFileNode>("Input");
        nodeRegistry->registerModel<GetFileNode>("Input");

        nodeRegistry->registerModel<WaveformDataInputNode>("Input");

        nodeRegistry->registerModel<GeneratorNode>("Input");

        nodeRegistry->registerModel<WaveformDataOutputNode>("Output");

        nodeRegistry->registerModel<MathNode<double>>("Math");
        nodeRegistry->registerModel<FftNode>("Math");
        nodeRegistry->registerModel<FmModulationNode>("Math");

        nodeRegistry->registerModel<ObjectCreateNode>("Object");
        nodeRegistry->registerModel<SetFieldNode>("Object");

        nodeRegistry->registerModel<PreviewNode>("Output");
        nodeRegistry->registerModel<GraphPreviewNode<double>>("Output");

        nodeRegistry->registerModel<ForeachNode>();

    }

public slots:
    void changeView(QtNodes::GraphicsView * graphView);

private:
    void onSave();

    void onLoad();

    void setDirty(bool dirty);

};

