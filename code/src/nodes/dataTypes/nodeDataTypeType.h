//
// Created by vm on 24.30.10.
//
#pragma once
//#include "string"
#include <QtNodes/NodeData>
#include "src/utils/stringType.h"


template<StringType Id, StringType Name>
class NodeDataTypeType {
public:
    QtNodes::NodeDataType getNodeDataType() const {
        return {Id.value, Name.value};


    }
};