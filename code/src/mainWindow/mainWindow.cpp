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

    connect(dataFlowGraphModel, &DynamicDataFlowGraphModel::nodePositionUpdated, this, [this]() {
        setDirty(true);
    });
    connect(dataFlowGraphModel, &DynamicDataFlowGraphModel::nodeUpdated, this, [this]() {
        setDirty(true);
    });
    connect(dataFlowGraphModel, &DynamicDataFlowGraphModel::nodeCreated, this, [this]() {
        setDirty(true);
    });
    connect(dataFlowGraphModel, &DynamicDataFlowGraphModel::nodeDeleted, this, [this]() {
        setDirty(true);
    });

}

void MainWindow::onSave() {
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

    setDirty(false);
}

void MainWindow::onLoad() {
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

    //delete all nodes in graph before loading, or else graph will not be loaded correctly
    auto ids = dataFlowGraphModel->allNodeIds();
    for (auto node: ids) {
        dataFlowGraphModel->deleteNode(node);
    }
    assert(dataFlowGraphModel->allNodeIds().size() == 0);


    dataFlowGraphModel->load(doc.object());

    currentFile = name;

    QTimer::singleShot(0, [this] {
        setDirty(true);
        setDirty(false);
    });
}

void MainWindow::setDirty(bool dirty) {
    //change title only on dirty state change
    if (dirty && this->dirty != dirty) {
        QString newTitle = "";
        for (auto c: currentFile) {
            // use undocumented surrogate plane to print italic chars
            char cc = c.toLatin1();
            if (std::isalpha(cc)) {
                if (std::isupper(cc)) {
                    // italic plane 0x1D434 - italic A
                    newTitle.append(QChar::fromUcs4(0x1D434 + (cc - 'A')));
                } else {
                    //handle special cases
                    switch (cc) {
                        //h is not present in this plane
                        case 'h':
                            newTitle.append(QChar::fromUcs4(0x210E));
                            break;
                        default:
                            // italic plane 0x1D44E - italic a
                            newTitle.append(QChar::fromUcs4(0x1D44E + (cc - 'a')));
                    }
                }
            } else {
                newTitle.append(c);
            }
        }
        newTitle.append('*');
        setWindowTitle(newTitle);
    } else if (!dirty && this->dirty != dirty) {
        setWindowTitle(currentFile);
    }
    this->dirty = dirty;
}
