//
// Created by vm on 24.30.10.
//

#pragma once

#include "ui_mainWindow.h"
#include "src/nodes/dataInputNodes/waveformDataInputNode.h"
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeDelegateModelRegistry>

class MainWindow : public QMainWindow, private Ui::mainWindow {
Q_OBJECT

private:
    QtNodes::DataFlowGraphicsScene *scene;
    QtNodes::GraphicsView *view;
    QtNodes::DataFlowGraphModel *dataFlowGraphModel;

public:
    explicit MainWindow(QWidget *parent = nullptr);

    std::shared_ptr<QtNodes::NodeDelegateModelRegistry> nodeRegistry;


    void registerNodes() {
        nodeRegistry->registerModel<WaveformDataInputNode>("Input");
    }
};

