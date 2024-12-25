//
// Created by vm on 24.8.11.
//

#pragma once

#include "baseData.h"
#include "nodeDataType.h"

class ObjectData : public BaseData {
public:
    using DataType = NodeDataType<"Object", "object">;
    DataType nodeType;

    QtNodes::NodeDataType type() const override {
        return nodeType.getNodeDataType();
    }

    ~ObjectData() override = default;

    QString toString() override {


        QJsonDocument json;
        QJsonObject obj;

        for (auto const &[k, v]: fields) {
            if (v == nullptr) {
                obj.insert(k, "nullptr");
            } else {
                obj.insert(k, v->toString());
            }

        }
        json.setObject(obj);

        return json.toJson(QJsonDocument::Indented);
    }

    std::map<QString, std::shared_ptr<BaseData>> fields;
};