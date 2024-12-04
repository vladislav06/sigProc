//
// Created by vm on 24.30.10.
//
#pragma once

#include <QtNodes/NodeData>
#include "nodeDataTypeType.h"

class BaseDataType : public QtNodes::NodeData {
public:
    using DataType = BaseNodeDataTypeType<"Base", "any">;
    DataType nodeType;

    virtual QString toString() = 0;

    ~BaseDataType() override = default;
};


