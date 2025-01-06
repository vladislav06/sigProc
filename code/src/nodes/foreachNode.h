//
// Created by Vladislavs Agarkovs on 24.4.12.
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
 * Effectively for(auto T: array<T>);
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
    std::binary_semaphore workFinished{1};


    /// This mutex guards progressCounter
    std::mutex progressCounterMutex;

    /// This counter keeps track of nodes that currently computes something, 0 means that all nodes have ended computation
    int progressCounter = 0;



    /// This semaphore guards computation function (setInData)
    std::binary_semaphore workSemaphore{1};

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

    /**
     * Initializes internal graph model, adds required nodes, and connects required slots
     */
    void initDataFlowGraphModel();

    /**
     * Updates graph models foreachInputNode
     */
    void updateInternalNodeType();

    /**
     * Updates graph models foreachOutputNode
     */
    void updateExternalOutputPorts();
};


