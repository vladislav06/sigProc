//
// Created by vm on 24.27.11.
//
#pragma once

#include "src/nodes/baseNode.h"

class FmModulationNode
        : public BaseNode<std::tuple<ArrayDataType<double, "Carrier">, ArrayDataType<double, "Modulator">>, std::tuple<ArrayDataType<double, "Modulated">>> {
public:

    QJsonObject onSave() const override;

    bool onLoad(QJsonObject json) override;

    QString caption() const override;

    QString name() const override;

    QWidget *embeddedWidget() override;

    std::tuple<std::shared_ptr<ArrayDataType<double, "Modulated">>>
    compute(std::tuple<std::shared_ptr<ArrayDataType<double, "Carrier">>, std::shared_ptr<ArrayDataType<double, "Modulator">>> params,
            std::vector<std::shared_ptr<BaseDataType>> adParams) override;


};


