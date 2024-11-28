//
// Created by vm on 24.28.11.
//
#pragma once

#include <QDoubleSpinBox>
#include <QBoxLayout>
#include <QLabel>
#include "baseGenerator.h"
#include "QJsonObject"

class OffsetGenerator : public BaseGenerator {
public:
    OffsetGenerator() {
        base = new QWidget;
        base->setLayout(new QBoxLayout(QBoxLayout::TopToBottom));

        offset = new QDoubleSpinBox();
        offset->setRange(-9999,9999);
        length = new QSpinBox();
        samplingRate = new QSpinBox();
        samplingRate->setMinimum(1);
        std::vector<std::pair<QString, std::variant<QSpinBox *, QDoubleSpinBox *>>> pairs = {{"offset",        offset},
                                                                                             {"length",        length},
                                                                                             {"sampling Rate", samplingRate}};
        for (auto pair: pairs) {
            std::visit([pair, this](auto box) {
                box->setMaximum(INT32_MAX);
                QWidget *p = new QWidget();
                p->setLayout(new QBoxLayout(QBoxLayout::LeftToRight));
                p->layout()->addWidget(new QLabel(pair.first));
                p->layout()->addWidget(box);
                base->layout()->addWidget(p);
                connect(box, &QDoubleSpinBox::editingFinished, this, &OffsetGenerator::onUpdate);
            }, pair.second);
        }
    }

    QString getName() override {
        return "Offset generator";
    }

    QWidget *getWidgets() override {
        return base;
    }

    std::shared_ptr<ArrayDataType<double>> generate() override {
        int len = length->value() * samplingRate->value();
        double off = offset->value();

        auto array = std::make_shared<ArrayDataType<double>>();
        array->get().resize(len);
        for (int i = 0; i < len; i++) {
            array->get()[i] = off;
        }
        return array;
    }

    QJsonObject onSave() override {
        return {
                {"offset",       offset->value()},
                {"length",       length->value()},
                {"samplingRate", samplingRate->value()},
        };
    }

    void onLoad(QJsonObject json) override {
        offset->setValue(json["offset"].toDouble(0));
        length->setValue(json["length"].toInt(10));
        samplingRate->setValue(json["samplingRate"].toInt(10));
    }

private:
    QWidget *base = nullptr;
    QDoubleSpinBox *offset = nullptr;
    QSpinBox *length = nullptr;
    QSpinBox *samplingRate = nullptr;
};