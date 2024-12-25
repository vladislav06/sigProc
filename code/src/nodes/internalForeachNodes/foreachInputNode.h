//
// Created by vm on 24.8.12.
//
#pragma once

#include "src/nodes/baseNode.h"

/**
 * This node is data input node inside foreach nodes GraphModel
 */
class ForeachInputNode : public BaseNodeTypeLessWrapper {
public:
    QString caption() const override;

    QString name() const override;

    unsigned int nPorts(QtNodes::PortType portType) const override;

    QtNodes::NodeDataType dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override;

    /**
     * Ignores all data except when portIndex is QtNodes::InvalidPortIndex,
     * then nodeData must be with type of ForeachInputNodeData, and contain output types and output data
     * @param nodeData ForeachInputNodeData containing output data types and data
     * @param portIndex QtNodes::InvalidPortIndex
     */
    void setInData(std::shared_ptr<QtNodes::NodeData> nodeData, const QtNodes::PortIndex portIndex) override;

    std::shared_ptr<QtNodes::NodeData> outData(const QtNodes::PortIndex port) override;

    QWidget *embeddedWidget() override;

    bool isSource() override;

    void recalculate() override;

private:
    std::vector<QtNodes::NodeDataType> outputTypes;
    std::vector<std::shared_ptr<BaseData>> outputData;
};

