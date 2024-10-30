//
// Created by vm on 24.30.10.
//

#include "mainWindow.h"

MainWindow::MainWindow(QWidget *parent) {
    setupUi(this);
    nodeRegistry = std::make_shared<QtNodes::NodeDelegateModelRegistry>();

    registerNodes();

    dataFlowGraphModel = new QtNodes::DataFlowGraphModel(nodeRegistry);
    scene = new QtNodes::DataFlowGraphicsScene(*dataFlowGraphModel, this);
    view = new QtNodes::GraphicsView(scene);
    this->nodeWidget->layout()->addWidget(view);
//    registerNodes<types>();

}
