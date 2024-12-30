//
// Created by vm on 24.30.10.
//
#pragma once

#include "ui_mainWindow.h"

#include "src/nodes/dynamicDataFlowGraphModel.h"
#include <QtNodes/DataFlowGraphicsScene>
#include <QtNodes/GraphicsView>
#include <QtNodes/NodeDelegateModelRegistry>
#include <QPushButton>
#include <QProgressBar>
#include "src/nodes/customNodePainter.h"

class MainWindow : public QMainWindow, private Ui::mainWindow {
Q_OBJECT

private:
    QtNodes::DataFlowGraphicsScene *scene;
    QtNodes::GraphicsView *view;
    DynamicDataFlowGraphModel *dataFlowGraphModel;
    std::shared_ptr<QtNodes::NodeDelegateModelRegistry> nodeRegistry;

    QString currentFile;
    bool dirty;
    QPushButton *backButton = nullptr;
    QProgressBar *progressBar = nullptr;


public:
    explicit MainWindow(QWidget *parent = nullptr);

public slots:

    void changeView(QtNodes::GraphicsView *graphView, DynamicDataFlowGraphModel *graphModel);

    void calculate(bool checked);

    void calculationEnded();

signals:

    void calculationEndedSignal();

private:
    void onSave();

    void onLoad();

    void setDirty(bool dirty);

    std::mutex progressCounterMutex;
    int progressCounter = 0;

};

