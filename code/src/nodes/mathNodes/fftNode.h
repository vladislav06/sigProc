//
// Created by vm on 24.27.11.
//
#pragma once

#include <QLabel>
#include "src/nodes/dataTypes/arrayData.h"
#include "src/nodes/baseNode.h"

class FftNode
        : public BaseNode<std::tuple<ArrayData<double> >, std::tuple<ArrayData<double>, ArrayData<double>, ArrayData<double>, ArrayData<double>>> {
public:

    QJsonObject onSave() const override;

    bool onLoad(QJsonObject json) override;

    QString caption() const override;

    QString name() const override;

    QWidget *embeddedWidget() override;

    std::tuple<std::shared_ptr<ArrayData<double>>,
            std::shared_ptr<ArrayData<double>>,
            std::shared_ptr<ArrayData<double>>,
            std::shared_ptr<ArrayData<double>>>
    compute(std::tuple<std::shared_ptr<ArrayData<double>>> params,
            std::vector<std::shared_ptr<BaseData>> adParams) override;


private:
    QWidget *base = nullptr;
    QLabel *textIn = nullptr;
    QLabel *textReal = nullptr;
    QLabel *textImag = nullptr;
    QLabel *textAmplitude = nullptr;
    QLabel *textPhase = nullptr;

};

