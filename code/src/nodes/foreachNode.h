//
// Created by vm on 24.4.12.
//
#pragma once

#include "QtNodes/internal/NodeDelegateModel.hpp"
#include "QtNodes/GraphicsView"
#include "QtNodes/DataFlowGraphicsScene"
#include "dynamicDataFlowGraphModel.h"
#include "src/nodes/dataTypes/baseDataType.h"
#include "src/nodes/dataTypes/arrayDataType.h"
#include "baseNode.h"

/**
 * This node is special, it has its own graph model inside
 */
class ForeachNode : public BaseNodeTypeLessWrapper {
Q_OBJECT

private:
    QtNodes::DataFlowGraphicsScene *scene;
    DynamicDataFlowGraphModel *dataFlowGraphModel;

    std::vector<QtNodes::NodeDataType> inputPortTypes = {
            ArrayDataType<std::shared_ptr<BaseDataType>>::DataType::getNodeDataType()};
    std::vector<QtNodes::NodeDataType> outputPortTypes;

    std::vector<std::shared_ptr<BaseDataType>> inputPorts = {nullptr};
    std::vector<std::shared_ptr<BaseDataType>> outputPorts = {nullptr};
public:
    QString caption() const override;

    QString name() const override;

    unsigned int nPorts(QtNodes::PortType portType) const override;

    QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

    void setInData(std::shared_ptr<QtNodes::NodeData> nodeData, const QtNodes::PortIndex portIndex) override;

    std::shared_ptr<QtNodes::NodeData> outData(const QtNodes::PortIndex port) override;

    QWidget *embeddedWidget() override;

    bool isSource() override;

    void recalculate() override;

signals:

    void setView(QtNodes::GraphicsView *graphView);
};


