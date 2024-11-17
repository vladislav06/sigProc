//
// Created by vm on 24.30.10.
//

#include "mainWindow.h"
#include "fstream"

MainWindow::MainWindow(QWidget *parent) {
    setupUi(this);
    nodeRegistry = std::make_shared<QtNodes::NodeDelegateModelRegistry>();

    registerNodes();

    dataFlowGraphModel = new DynamicDataFlowGraphModel(nodeRegistry);
    scene = new QtNodes::DataFlowGraphicsScene(*dataFlowGraphModel, this);
    view = new QtNodes::GraphicsView(scene);
    this->nodeWidget->layout()->addWidget(view);
//    registerNodes<types>();

    connect(this->actionSave, &QAction::triggered, this, &MainWindow::onSave);
    connect(this->actionOpen, &QAction::triggered, this, &MainWindow::onLoad);
    connect(this->actionReload, &QAction::triggered, dataFlowGraphModel, &DynamicDataFlowGraphModel::trigger);

}

void MainWindow::onSave(bool) {
    QString name;
    if (currentFile.isEmpty()) {
        name = QFileDialog::getSaveFileName(this, tr("Save File"), QDir::currentPath(),
                                            tr("SigProc file (*.spf)"));
        if (name == "") {
            return;
        }
        currentFile = name;
        setWindowTitle(currentFile);
    } else {
        name = currentFile;
    }


    QFile jsonFile(name);
    jsonFile.open(QFile::WriteOnly);
    if (!jsonFile.isOpen()) {
        return;
    }

    auto json = dataFlowGraphModel->save();
    QJsonDocument doc(json);
    jsonFile.write(doc.toJson());
}

void MainWindow::onLoad(bool) {
    QString name = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath(), tr("SigProc file (*.spf)"));
    if (name == "") {
        return;
    }

    QFile jsonFile(name);
    jsonFile.open(QFile::ReadOnly);
    if (!jsonFile.isOpen()) {
        return;
    }

    auto doc = QJsonDocument().fromJson(jsonFile.readAll());
    dataFlowGraphModel->load(doc.object());
    currentFile = name;
    setWindowTitle(currentFile);
}
