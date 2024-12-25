//
// Created by vm on 24.27.11.
//
#pragma once

#include "src/nodes/baseNode.h"

class FmModulationNode
        : public BaseNode<std::tuple<ArrayData<double>, ArrayData<double>>, std::tuple<ArrayData<double>>> {
public:

    QJsonObject onSave() const override;

    bool onLoad(QJsonObject json) override;

    QString caption() const override;

    QString name() const override;

    QWidget *embeddedWidget() override;

    std::tuple<std::shared_ptr<ArrayData<double>>>
    compute(std::tuple<std::shared_ptr<ArrayData<double>>, std::shared_ptr<ArrayData<double>>> params,
            std::vector<std::shared_ptr<BaseData>> adParams) override;


};


