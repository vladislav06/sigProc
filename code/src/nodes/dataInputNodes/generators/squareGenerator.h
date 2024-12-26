//
// Created by vm on 24.18.11.
//
#pragma once

#include <QSpinBox>
#include <QBoxLayout>
#include <QLabel>
#include "baseGenerator.h"

class SquareGenerator : public BaseGenerator {
public:

    SquareGenerator() {
        base = new QWidget;
        base->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));

        freq = new QDoubleSpinBox();
        amplitude = new QDoubleSpinBox();
        duty = new QDoubleSpinBox();
        offset = new QDoubleSpinBox();
        phase = new QDoubleSpinBox();
        length = new QSpinBox();
        samplingRate = new QSpinBox();

        amplitude->setMinimum(1);
        offset->setRange(-99999,99999),
        duty->setRange(0,1);
        samplingRate->setMinimum(1);

        std::vector<std::pair<QString, std::variant<QSpinBox *, QDoubleSpinBox *>>> pairs = {{"frequency",     freq},
                                                                                             {"amplitude",     amplitude},
                                                                                             {"duty",          duty},
                                                                                             {"offset",        offset},
                                                                                             {"phase",         phase},
                                                                                             {"length",        length},
                                                                                             {"sampling Rate", samplingRate}};
        for (auto pair: pairs) {
            std::visit([pair, this](auto box) {
                box->setMaximum(INT32_MAX);
                QWidget * p = new QWidget();
                p->setLayout(new QBoxLayout(QBoxLayout::LeftToRight));
                p->layout()->addWidget(new QLabel(pair.first));
                p->layout()->addWidget(box);
                base->layout()->addWidget(p);
                connect(box, &QDoubleSpinBox::editingFinished, this, &SquareGenerator::onUpdate);
            }, pair.second);
        }
    }

    QString getName() override {
        return "Square generator";
    }

    QWidget *getWidgets() override {
        return base;
    }

    std::shared_ptr<ArrayData<double>> generate() override {
        int len = length->value() * (double) samplingRate->value();
        double fr = freq->value() / (double) samplingRate->value();
        double ph = phase->value();
        double off = offset->value();
        double am = amplitude->value();
        double dut = 1.0-duty->value();


        auto array = std::make_shared<ArrayData<double>>();
        array->get().resize(len);
        for (int i = 0; i < len; i++) {
            array->get()[i] = am * std::floor(cos(fr * (double) i + ph+dut*M_PI+M_PI) * 0.5 + (cos(dut*M_PI)*0.5+1)) + off;
        }
        return array;
    }

    QJsonObject onSave() override {
        return {
                {"freq",         freq->value()},
                {"amplitude",    amplitude->value()},
                {"duty",         duty->value()},
                {"offset",       offset->value()},
                {"phase",        phase->value()},
                {"length",       length->value()},
                {"samplingRate", samplingRate->value()},
        };
    }

    void onLoad(QJsonObject json) override {
        freq->setValue(json["freq"].toDouble(1));
        amplitude->setValue(json["amplitude"].toDouble(1));
        offset->setValue(json["offset"].toDouble(0));
        phase->setValue(json["phase"].toDouble(0));
        length->setValue(json["length"].toInt(10));
        samplingRate->setValue(json["samplingRate"].toInt(10));
    }

private:
    QWidget *base = nullptr;
    QDoubleSpinBox *freq = nullptr;
    QDoubleSpinBox *amplitude = nullptr;
    QDoubleSpinBox *duty = nullptr;
    QDoubleSpinBox *offset = nullptr;
    QDoubleSpinBox *phase = nullptr;
    QSpinBox *length = nullptr;
    QSpinBox *samplingRate = nullptr;

};