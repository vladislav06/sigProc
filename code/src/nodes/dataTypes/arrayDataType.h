//
// Created by vm on 24.30.10.
//

#pragma once

#include "baseDataType.h"
#include "nodeDataTypeType.h"

class ArrayDataType : public BaseDataType {
public:
    using DataType = NodeDataTypeType<"Array", "array">;
    DataType nodeType;

    ArrayDataType()
            : _number(0.0) {}

    ArrayDataType(double const number)
            : _number(number) {}

    QtNodes::NodeDataType type() const override { return nodeType.getNodeDataType(); }

    double number() const { return _number; }

    QString numberAsText() const { return QString::number(_number, 'f'); }

private:
    double _number;

};

