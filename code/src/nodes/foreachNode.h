//
// Created by vm on 24.4.12.
//

#pragma once

#include <QPushButton>
#include "QtNodes/internal/NodeDelegateModel.hpp"
#include "QtNodes/GraphicsView"
#include "QtNodes/DataFlowGraphicsScene"
#include "dynamicDataFlowGraphModel.h"
#include "src/nodes/dataTypes/baseData.h"
#include "src/nodes/dataTypes/arrayData.h"
#include "baseNode.h"
#include "src/globals.h"

/**
 * Has one array input and any amount of BaseType inputs.
 * This node will pass each value of input array to dataFlowGraphModel that is stored inside the node.
 * Effectively for(auto T:array<T>);
 */
class ForeachNode : public BaseNodeTypeLessWrapper {
Q_OBJECT

private:

    bool innited = false;
    std::shared_ptr<QtNodes::NodeDelegateModelRegistry> nodeRegistry;

    /**
     * dataFlowGraphModel that is edited by user, after end of edit this dataFlowGraphModel is copied some amount of times,
     * copies are stored in dataFlowGraphModels
     */
    DynamicDataFlowGraphModel *graphModel;

    /// workFinished.notify_all() will be called when DynamicDataFlowGraphModel completes data propagation
//    std::unique_ptr<std::condition_variable> workFinished = std::make_unique<std::condition_variable>();
    std::binary_semaphore workFinished{1};

    int counterStarted = 0;
    int counterFinished = 0;


    std::binary_semaphore workSemaphore{1};


//    /**
//     * Array that currently is being processed
//     */
//    std::shared_ptr<BaseDataArrayData> processedArray;

    QtNodes::DataFlowGraphicsScene *scene;

    // this node accepts any BaseData array as first input and BaseData as other inputs,
    // but actual type of connections must be known to pass it to internal dataFlowGraphModel,
    // so, when connection is created, actual type is saved, and when connection is removed,
    // port is either deleted, or type is reset to BaseData array for first port or BaseData to all other ports
    std::vector<QtNodes::NodeDataType> inputPortTypes = {
            ArrayData<std::shared_ptr<BaseData>>::DataType::getNodeDataType(),
            BaseData::DataType::getNodeDataType()
    };


    /**
     * temporary array for storing output value
     */
    std::vector<std::vector<std::shared_ptr<BaseData>>> resultsArray{};

    std::vector<QtNodes::NodeDataType> outputPortTypes;

    /**
     * Stores type of value that is stored in primary array that is being processed
     */
    QtNodes::NodeDataType inputArrayValueType = BaseData::DataType::getNodeDataType();

    std::vector<std::shared_ptr<BaseData>> inputPorts = {nullptr};
    std::vector<std::shared_ptr<ArrayData<std::shared_ptr<BaseData>>>> outputPorts = {};

    /**
     * ForeachInputNode id that is added to dataFlowGraphModel on creation
     */
    QtNodes::NodeId inputNodeId;

    /**
     * ForeachOutputNode id that is added to dataFlowGraphModel on creation
     */
    QtNodes::NodeId outputNodeId;

    /**
     * Ignore connection changes when node edits its own connections
     * because onInputConnectionCreation and inputConnectionDeleted are called when node deletes its own connections
     */
    bool dirtyInputConnections = false;

    QWidget *base = nullptr;
    QPushButton *button = nullptr;

public:

    ForeachNode() {
    }

    ~ForeachNode() {

    }

    void prepareToDelete(std::function<void(void)> callback) override {
        QPromise<void> p;
        p.finish();
        callback();
    }

    QString caption() const override;

    QString name() const override;

    unsigned int nPorts(QtNodes::PortType portType) const override;

    QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

    void setInData(std::shared_ptr<QtNodes::NodeData> nodeData, const QtNodes::PortIndex portIndex) override;

    std::shared_ptr<QtNodes::NodeData> outData(const QtNodes::PortIndex port) override;

    QWidget *embeddedWidget() override;

    bool isSource() override;

    void recalculate() override;

    void onInputConnectionCreation(QtNodes::ConnectionId connection, QtNodes::NodeDataType type) override;

    void inputConnectionDeleted(const QtNodes::ConnectionId &connection) override;

    QJsonObject save() const override;

    void load(const QJsonObject &json) override;

public slots:

    void viewClosed();

signals:

    void setView(QtNodes::GraphicsView *graphView, DynamicDataFlowGraphModel *graphModel);

private:

    void initDataFlowGraphModel();

    void updateInternalNodeType();

    void updateExternalOutputPorts();
};


