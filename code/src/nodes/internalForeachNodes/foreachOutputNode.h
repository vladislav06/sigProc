//
// Created by vm on 24.10.12.
//
#pragma once

#include "src/nodes/baseNode.h"

class ForeachOutputNode : public BaseNodeTypeLessWrapper {
public:
    ~ForeachOutputNode() override = default;

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

    void inputConnectionDeleted(const QtNodes::ConnectionId &) override;


private:
    bool dirtyInputConnections = false;
    std::vector<QtNodes::NodeDataType> inputType{
            BaseData::DataType::getNodeDataType(),
    };

    std::vector<std::shared_ptr<BaseData>> inputData;
};