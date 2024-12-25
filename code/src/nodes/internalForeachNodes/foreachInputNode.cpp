//
// Created by vm on 24.8.12.
//

#include "foreachInputNode.h"
#include "foreachInputNodeDataType.h"

QString ForeachInputNode::caption() const {
    return "In";
}

QString ForeachInputNode::name() const {
    return "Foreach input node";
}

unsigned int ForeachInputNode::nPorts(QtNodes::PortType portType) const {
    switch (portType) {
        case QtNodes::PortType::In:
            return 0;
        case QtNodes::PortType::Out:
            return outputTypes.size();
        case QtNodes::PortType::None:
            return 0;
    }
    return 0;
}

QtNodes::NodeDataType ForeachInputNode::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const {
    switch (portType) {
        case QtNodes::PortType::In:
            return {};
        case QtNodes::PortType::Out:
            if (portIndex >= outputTypes.size()) {
                return {};
            }
            return outputTypes.at(portIndex);
        case QtNodes::PortType::None:
            return {};
    }
    return {};
}


void ForeachInputNode::setInData(std::shared_ptr<QtNodes::NodeData> nodeData, const QtNodes::PortIndex portIndex) {
    if (portIndex == QtNodes::InvalidPortIndex) {
        auto data = std::dynamic_pointer_cast<ForeachInputNodeData>(nodeData);
        assert(data != nullptr);

        if (outputTypes.size() != data->types.size()) {
            //redraw node when output port count changes, without this, geometry is broken
            outputTypes = data->types;
            emit embeddedWidgetSizeUpdated();
        } else {
            outputTypes = data->types;
        }
        outputData = data->data;

        emit computingStarted();
        for (int i = 0; i < outputTypes.size(); i++) {
            emit dataUpdated(i);
        }
        emit computingFinished();
    }
}

std::shared_ptr<QtNodes::NodeData> ForeachInputNode::outData(const QtNodes::PortIndex port) {
    if (port >= outputData.size()) {
        return nullptr;
    }
    return outputData.at(port);
}

QWidget *ForeachInputNode::embeddedWidget() {
    return nullptr;
}

bool ForeachInputNode::isSource() {
    return true;
}

void ForeachInputNode::recalculate() {

}
