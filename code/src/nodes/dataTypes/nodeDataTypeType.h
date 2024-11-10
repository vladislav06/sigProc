//
// Created by vm on 24.30.10.
//
#pragma once

#include <QtNodes/NodeData>
#include "src/utils/stringType.h"


class BaseDataType;

/**
 * QtNodes::NodeDataType wrapper with templates and inheritance, inherited id will be appended to the front
 * @tparam Id id of this type
 * @tparam Name displayed name
 * @tparam extends what type this type extends, BaseDataType by default
 */
template<StringType Id, StringType Name, typename extends = BaseDataType>
class NodeDataTypeType {
public:
    inline static std::string id = Id.value;

    QtNodes::NodeDataType getNodeDataType() const {
        std::string s = extends::DataType::id;
        if (s != "") {
            std::string ss = s + "_" + Id.value;
            return QtNodes::NodeDataType{QString::fromStdString(ss), Name.value};
        } else {
            return {Id.value, Name.value};
        }
    }
};