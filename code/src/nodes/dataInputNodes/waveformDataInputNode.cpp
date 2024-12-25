//
// Created by vm on 24.30.10.
//
#include "fileLoader.h"

#include "waveformDataInputNode.h"
#include "src/nodes/dataTypes/arrayData.h"
#include <QtWidgets>

WaveformDataInputNode::WaveformDataInputNode() {

}

QString WaveformDataInputNode::caption() const {
    return "Waveform input node";
}

QString WaveformDataInputNode::name() const {
    return "Input";
}

QWidget *WaveformDataInputNode::embeddedWidget() {
    return nullptr;
}

QJsonObject WaveformDataInputNode::onSave() const {
    return {};
}

bool WaveformDataInputNode::onLoad(QJsonObject json) {

    return true;
}

std::tuple<std::shared_ptr<ArrayData<double>>>
WaveformDataInputNode::compute(std::tuple<std::shared_ptr<FileData>> params,
                               std::vector<std::shared_ptr<BaseData>> adParams) {
    auto file = std::get<0>(params);

    if (file == nullptr) {
        return {};
    }

    //load file in to ArrayData
    auto array = std::make_shared<ArrayData<double>>();

    std::ifstream fin;
    if (!file->inputStream(fin)) {
        return {};
    }
    double ff = 0;

    while (fin >> ff) {
        array->get().push_back(ff);
    }
    return array;
}





