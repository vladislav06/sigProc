//
// Created by vm on 24.10.12.
//

#include "foreachOutputNode.h"
#include "foreachInputNodeDataType.h"

QString ForeachOutputNode::caption() const {
    return "Out";
}

QString ForeachOutputNode::name() const {
    return "Foreach output node";
}

unsigned int ForeachOutputNode::nPorts(QtNodes::PortType portType) const {
    switch (portType) {
        case QtNodes::PortType::In:
            return inputType.size();
        case QtNodes::PortType::Out:
            return 0;
        case QtNodes::PortType::None:
            return 0;
    }
    return 0;
}

QtNodes::NodeDataType ForeachOutputNode::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const {
    switch (portType) {
        case QtNodes::PortType::In:
            if (portIndex >= inputType.size()) {
                return {};
            }
            return inputType.at(portIndex);
        case QtNodes::PortType::Out:
            return {};
        case QtNodes::PortType::None:
            return {};
    }
    return {};
}

void ForeachOutputNode::setInData(std::shared_ptr<QtNodes::NodeData> nodeData, const QtNodes::PortIndex portIndex) {
    if (nodeData == nullptr) {
        return;
    }
    auto data = std::dynamic_pointer_cast<BaseData>(nodeData);
    assert(data != nullptr);
    inputData.at(portIndex) = data;
    inputType.at(portIndex) = data->type();
    //notify graphModel
    emit dataUpdated(portIndex);

}

std::shared_ptr<QtNodes::NodeData> ForeachOutputNode::outData(const QtNodes::PortIndex port) {
    if (port == QtNodes::InvalidPortIndex) {
        return std::make_shared<ForeachInputNodeData>(inputType, inputData);
    }
    return inputData.at(port);
}

QWidget *ForeachOutputNode::embeddedWidget() {
    return nullptr;
}

bool ForeachOutputNode::isSource() {
    return false;
}

void ForeachOutputNode::recalculate() {

}

void ForeachOutputNode::onInputConnectionCreation(QtNodes::ConnectionId connection, QtNodes::NodeDataType type) {
    if (dirtyInputConnections) {
        return;
    }
    dirtyInputConnections = true;

    if (connection.inPortIndex >= inputData.size()) {
        inputData.resize(connection.inPortIndex + 1);
    }

    //save type of additional data ports
    if (connection.inPortIndex >= inputType.size()) {
        inputType.resize(connection.inPortIndex + 1);

        //add new input connection with DataBaseType at the end
        int newPortIndex = inputType.size() + 1;
        portsAboutToBeInserted(QtNodes::PortType::In, newPortIndex, newPortIndex);
        inputType.push_back(BaseData::DataType::getNodeDataType());
        portsInserted();
    }

    if (connection.inPortIndex == inputType.size() - 1) {
        //add new input connection with DataBaseType at the end
        int newPortIndex = inputType.size();
        portsAboutToBeInserted(QtNodes::PortType::In, newPortIndex, newPortIndex);
        inputType.push_back(BaseData::DataType::getNodeDataType());
        portsInserted();
    }
    //save connected port type
    inputType[connection.inPortIndex] = type;


    dirtyInputConnections = false;
}

void ForeachOutputNode::inputConnectionDeleted(const QtNodes::ConnectionId &connection) {
    if (dirtyInputConnections) {
        return;
    }
    dirtyInputConnections = true;
    //delete disconnected port
    int index = connection.inPortIndex;
    portsAboutToBeDeleted(QtNodes::PortType::In, index, index);
    inputType.erase(inputType.begin() + index);
    inputData.erase(inputData.begin() + index);
    portsDeleted();

    dirtyInputConnections = false;
}
