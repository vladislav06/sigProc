//
// Created by vm on 24.31.10.
//
#pragma once

#include <tuple>
#include "src/nodes/dataTypes/arrayData.h"
#include "src/nodes/baseNode.h"
#include "ui_waveformDataOutputNode.h"
class WaveformDataOutputNode : public BaseNode<std::tuple<ArrayData<double>>, std::tuple<>> {
public:
    WaveformDataOutputNode();

    QString caption() const override;

    QString name() const override;

    QWidget *embeddedWidget() override;

    QJsonObject onSave() const override;

    bool onLoad(QJsonObject json) override;

    std::tuple<> compute(std::tuple<std::shared_ptr<ArrayData<double>>> params,
                         std::vector<std::shared_ptr<BaseData>> adParams) override;

private Q_SLOTS:

    void valueChanged(QString str);

    void onButonPress();

private:
    Ui::waveformDataOutputNode ui;
    QWidget *base = nullptr;

};

