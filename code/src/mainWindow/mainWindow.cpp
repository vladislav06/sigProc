//
// Created by vm on 24.30.10.
//

#include <QFileDialog>
#include <QMessageBox>
#include "mainWindow.h"
#include "fstream"
#include "src/globals.h"
#include "QTimer"
#include "QPushButton"
#include "src/nodes/baseNode.h"

MainWindow::MainWindow(QWidget *parent) {
    setupUi(this);
    nodeRegistry = std::make_shared<QtNodes::NodeDelegateModelRegistry>();

    registerNodes(nodeRegistry);

    dataFlowGraphModel = new DynamicDataFlowGraphModel(nodeRegistry);
    scene = new QtNodes::DataFlowGraphicsScene(*dataFlowGraphModel, this);
    scene->setNodePainter(std::make_unique<CustomNodePainter>());
    view = new QtNodes::GraphicsView(scene);
    this->nodeWidget->layout()->addWidget(view);
//    registerNodes<types>();

    connect(this->actionSave, &QAction::triggered, this, &MainWindow::onSave);
    connect(this->actionOpen, &QAction::triggered, this, &MainWindow::onLoad);
    connect(this->actionCalculate, &QAction::triggered, this, &MainWindow::calculate);

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

    connect(dataFlowGraphModel, &DynamicDataFlowGraphModel::setView, this, &MainWindow::changeView);

    connect(this, &MainWindow::calculationEndedSignal, this, &MainWindow::calculationEnded);

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
    //exit from internal editor
    if (backButton != nullptr) {
        backButton->click();
    }

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
        QTimer::singleShot(10, [node, this] {
            dataFlowGraphModel->deleteNode(node);
        });

        QTime dieTime = QTime::currentTime().addMSecs(10);
        while (QTime::currentTime() < dieTime)
            QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
    }
//    assert(dataFlowGraphModel->allNodeIds().size() == 0);

    QTimer::singleShot(10, [doc, this] {
        try {
            dataFlowGraphModel->load(doc.object());
        } catch (std::exception &e) {
            QMessageBox messageBox;
            messageBox.critical(0, "Error", "File is corrupted!");
            messageBox.open();
        }
    });


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

void MainWindow::changeView(QtNodes::GraphicsView *graphView, DynamicDataFlowGraphModel *graphModel) {

    nodeWidget->layout()->removeWidget(view);
    view->hide();
    //create back button
    backButton = new QPushButton("< Back");
    nodeWidget->layout()->addWidget(backButton);
    actionCalculate->setDisabled(true);

    connect(backButton, &QPushButton::clicked, this, [this, graphView, graphModel](bool) {

        nodeWidget->layout()->removeWidget(backButton);
        delete backButton;
        nodeWidget->layout()->removeWidget(graphView);
        delete graphView;
        graphModel->onViewClose();

        nodeWidget->layout()->addWidget(view);
        view->show();
        backButton = nullptr;
        actionCalculate->setDisabled(false);
    });
    nodeWidget->layout()->addWidget(graphView);
}

void MainWindow::calculate(bool checked) {

    //empty graph edge case
    if (dataFlowGraphModel->allNodeIds().empty()) {
        return;
    }
    // disable view
    view->setDisabled(true);

    // tell all nodes to enable calculation mode
    dataFlowGraphModel->setComputing(true);

    progressBar = new QProgressBar;
    progressBar->setMaximum(0);
    progressBar->setMinimum(0);
    progressBar->setValue(0);
    nodeWidget->layout()->addWidget(progressBar);

    //increase and decrease counter when node starts and ends computation
    auto nodes = dataFlowGraphModel->allNodeIds();

    for (auto nodeId: nodes) {

        auto model = dataFlowGraphModel->delegateModel<BaseNodeTypeLessWrapper>(nodeId);

        connect(
                model,
                &QtNodes::NodeDelegateModel::computingStarted, this, [this]() {
                    std::lock_guard<std::mutex> l(progressCounterMutex);
                    progressCounter++;
                }, Qt::DirectConnection);
        connect(
                model,
                &QtNodes::NodeDelegateModel::computingFinished, this, [this]() {
                    std::lock_guard<std::mutex> l(progressCounterMutex);
                    progressCounter--;

                    if (progressCounter == 0) {
                        emit calculationEndedSignal();
                    }
                }, Qt::DirectConnection);
    }


    //disable buttons
    actionCalculate->setDisabled(true);
    actionOpen->setDisabled(true);
    actionSave->setDisabled(true);
    actionSaveAs->setDisabled(true);
    actionAbout->setDisabled(true);
    actionCopy->setDisabled(true);
    actionCut->setDisabled(true);
    actionPaste->setDisabled(true);
    actionNew->setDisabled(true);

    // trigger nodeGraph
    QtConcurrent::run([this](QPromise<void> &promise) {
        dataFlowGraphModel->trigger();

    });
}

void MainWindow::calculationEnded() {
    // tell all nodes to disable calculation mode
    dataFlowGraphModel->setComputing(false);

    //enable buttons
    actionCalculate->setDisabled(false);
    actionOpen->setDisabled(false);
    actionSave->setDisabled(false);
    actionSaveAs->setDisabled(false);
    actionAbout->setDisabled(false);
    actionCopy->setDisabled(false);
    actionCut->setDisabled(false);
    actionPaste->setDisabled(false);
    actionNew->setDisabled(false);
    delete progressBar;
    //enable view back
    view->setDisabled(false);


}

