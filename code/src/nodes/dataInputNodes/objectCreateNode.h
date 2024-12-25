//
// Created by vm on 24.8.11.
//
#pragma once

#include "src/nodes/dataTypes/objectData.h"

class ObjectCreateNode : public BaseNode<std::tuple<>, std::tuple<ObjectData>> {
public:

    QJsonObject onSave() const override {
        return QJsonObject();
    }

    bool onLoad(QJsonObject json) override {
        return false;
    }

    QString caption() const override {
        return "Create object";
    }

    QString name() const override {
        return "Create object";
    }

    QWidget *embeddedWidget() override {
        return nullptr;
    }

    std::tuple<std::shared_ptr<ObjectData>>
    compute(std::tuple<> params, std::vector<std::shared_ptr<BaseData>> adParams) override {
        return {std::make_shared<ObjectData>()};
    }

    bool isSource() override {
        return true;
    }
};