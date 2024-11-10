//
// Created by vm on 24.8.11.
//
#pragma once

#include "src/nodes/dataTypes/objectDataType.h"

class ObjectCreateNode : public BaseNode<std::tuple<>, std::tuple<ObjectDataType>> {
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

    std::tuple<std::shared_ptr<ObjectDataType>> compute(std::tuple<> type1) override {
        return {std::make_shared<ObjectDataType>()};
    }
};