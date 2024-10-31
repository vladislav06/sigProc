//
// Created by vm on 24.31.10.
//
#pragma once

#include <tuple>
#include "src/nodes/dataTypes/arrayDataType.h"
#include "src/nodes/baseNode.h"
#include "ui_waveformDataOutputNode.h"
class WaveformDataOutputNode : public BaseNode<std::tuple<ArrayDataType<double>>, std::tuple<>> {
public:
    WaveformDataOutputNode();
    std::tuple<> compute(std::tuple<std::shared_ptr<ArrayDataType<double>>> type1) override;

    QString caption() const override;

    QString name() const override;

    QWidget *embeddedWidget() override;

private Q_SLOTS:

    void valueChanged(QString str);

    void onButonPress();

private:
    Ui::waveformDataOutputNode ui;
    QWidget *base;

};

