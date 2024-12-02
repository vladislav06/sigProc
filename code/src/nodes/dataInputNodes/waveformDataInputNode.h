//
// Created by vm on 24.30.10.
//
#pragma once

#include <QtWidgets>
#include "src/nodes/baseNode.h"
#include "ui_waveformDataInputNode.h"
#include "src/nodes/dataTypes/fileDataType.h"

class WaveformDataInputNode : public BaseNode<std::tuple<FileDataType>, std::tuple<ArrayDataType<double>>> {
public:


    QString caption() const override;

    QString name() const override;

    QWidget *embeddedWidget() override;

    WaveformDataInputNode();


    QJsonObject onSave() const override;

    bool onLoad(QJsonObject json) override;

    std::tuple<std::shared_ptr<ArrayDataType<double>>>
    compute(std::tuple<std::shared_ptr<FileDataType>> params,
            std::vector<std::shared_ptr<BaseDataType>> adParams) override;
};


