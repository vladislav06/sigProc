//
// Created by vm on 24.8.11.
//

#pragma once

#include "baseDataType.h"
#include "nodeDataTypeType.h"

class ObjectDataType : public BaseDataType {
public:
    using DataType = NodeDataTypeType<"Object", "object">;
    DataType nodeType;

    QtNodes::NodeDataType type() const override {
        return nodeType.getNodeDataType();
    }

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

    std::map<QString, std::shared_ptr<BaseDataType>> fields;
};