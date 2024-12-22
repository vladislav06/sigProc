#include <utility>

//
// Created by vm on 24.9.12.
//
#pragma once

class ForeachInputNodeDataType : public QtNodes::NodeData {
public:
    ForeachInputNodeDataType(std::vector<QtNodes::NodeDataType> type, std::vector<std::shared_ptr<BaseDataType>> data)
            : types(std::move(type)), data(std::move(data)) {};

    ~ForeachInputNodeDataType() override = default;

    QtNodes::NodeDataType type() const override {
        return QtNodes::NodeDataType{.id="ForeachInputNodeDataType", .name="ForeachInputNodeDataType"};
    }

    std::vector<QtNodes::NodeDataType> types;

    std::vector<std::shared_ptr<BaseDataType>> data;
};