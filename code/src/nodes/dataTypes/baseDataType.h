//
// Created by vm on 24.30.10.
//
#pragma once

#include <QtNodes/NodeData>
#include "nodeDataTypeType.h"

class BaseDataType : public QtNodes::NodeData {
public:
    using DataType = NodeDataTypeType<"Base", "any">;
    DataType nodeType;

    virtual QString toString() = 0;
};


