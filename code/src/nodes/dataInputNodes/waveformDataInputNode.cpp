//
// Created by vm on 24.30.10.
//

#include "waveformDataInputNode.h"
#include "src/nodes/dataTypes/arrayDataType.h"


QString WaveformDataInputNode::caption() const {
    return "";
}

QString WaveformDataInputNode::name() const {
    return "";
}

QWidget *WaveformDataInputNode::embeddedWidget() {
    return nullptr;
}

std::tuple<ArrayDataType> WaveformDataInputNode::compute(std::tuple<> ports) {
    return std::tuple<ArrayDataType>();
}


