//
// Created by vm on 24.30.10.
//
#pragma once

#include <QtNodes/NodeData>
#include "nodeDataType.h"

class BaseData : public QtNodes::NodeData {
public:
    using DataType = BaseNodeDataType<"Base", "any">;
    DataType nodeType;

    virtual QString toString() = 0;

    ~BaseData() override = default;
};


