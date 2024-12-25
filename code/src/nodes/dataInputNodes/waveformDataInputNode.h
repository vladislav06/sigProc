//
// Created by vm on 24.30.10.
//
#pragma once

#include <QtWidgets>
#include "src/nodes/baseNode.h"
#include "ui_waveformDataInputNode.h"
#include "src/nodes/dataTypes/fileData.h"

class WaveformDataInputNode : public BaseNode<std::tuple<FileData>, std::tuple<ArrayData<double>>> {
public:


    QString caption() const override;

    QString name() const override;

    QWidget *embeddedWidget() override;

    WaveformDataInputNode();


    QJsonObject onSave() const override;

    bool onLoad(QJsonObject json) override;

    std::tuple<std::shared_ptr<ArrayData<double>>>
    compute(std::tuple<std::shared_ptr<FileData>> params,
            std::vector<std::shared_ptr<BaseData>> adParams) override;
};


