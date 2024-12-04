//
// Created by vm on 24.4.12.
//

#include "foreachNode.h"

QString ForeachNode::caption() const {
    return "Foreach node";
}

QString ForeachNode::name() const {
    return "Foreach node";
}

unsigned int ForeachNode::nPorts(QtNodes::PortType portType) const {
    switch (portType) {
        case QtNodes::PortType::In:
            return inputPortTypes.size();
        case QtNodes::PortType::Out:
            return outputPortTypes.size();
        case QtNodes::PortType::None:
            return 0;
    }
    return 0;
}

QtNodes::NodeDataType ForeachNode::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const {
    switch (portType) {
        case QtNodes::PortType::In:
            return inputPortTypes.at(portIndex);
        case QtNodes::PortType::Out:
            return outputPortTypes.at(portIndex);
        case QtNodes::PortType::None:
            return {};
    }
    return {};
}

void ForeachNode::setInData(std::shared_ptr<QtNodes::NodeData> nodeData, const QtNodes::PortIndex portIndex) {

}

std::shared_ptr<QtNodes::NodeData> ForeachNode::outData(const QtNodes::PortIndex port) {
    return std::shared_ptr<QtNodes::NodeData>();
}

QWidget *ForeachNode::embeddedWidget() {
    return nullptr;
}

bool ForeachNode::isSource() {
    return false;
}

void ForeachNode::recalculate() {

}
