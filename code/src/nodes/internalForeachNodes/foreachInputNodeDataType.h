#include <utility>

//
// Created by vm on 24.9.12.
//
#pragma once

class ForeachInputNodeData : public QtNodes::NodeData {
public:
    ForeachInputNodeData(std::vector<QtNodes::NodeDataType> type, std::vector<std::shared_ptr<BaseData>> data)
            : types(std::move(type)), data(std::move(data)) {};

    ~ForeachInputNodeData() override = default;

    QtNodes::NodeDataType type() const override {
        return QtNodes::NodeDataType{.id="ForeachInputNodeData", .name="ForeachInputNodeData"};
    }

    std::vector<QtNodes::NodeDataType> types;

    std::vector<std::shared_ptr<BaseData>> data;
};