//
// Created by vm on 24.30.10.
//
#pragma once

#include <QtNodes/NodeData>
#include "src/utils/stringType.h"


class BaseDataType;

/**
 * QtNodes::NodeDataType wrapper with templated id and name, support inheritance, by default extends BaseDataType.
 * BaseDataType::ID will be appended to the left of the Id
 * @tparam Id id of this type
 * @tparam Name displayed name
 * @tparam extends what type this type extends, BaseDataType by default
 */
template<StringType Id, StringType Name, typename extends = BaseDataType>
class NodeDataTypeType {
public:
    static constexpr StringType ID = extends::DataType::ID + "_" + Id.value;

    static QtNodes::NodeDataType getNodeDataType() {
        return {QString::fromStdString(ID.value), Name.value};
    }
};

/**
 * QtNodes::NodeDataType wrapper with templated id and name
 * @tparam Id id of this type
 * @tparam Name displayed name
 */
template<StringType Id, StringType Name>
class BaseNodeDataTypeType {
public:
    static constexpr StringType ID = Id;

    static QtNodes::NodeDataType getNodeDataType() {
        return {QString::fromStdString(ID.value), Name.value};
    }
};