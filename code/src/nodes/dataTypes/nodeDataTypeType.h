//
// Created by vm on 24.30.10.
//
#pragma once

#include <QtNodes/NodeData>
#include <iostream>
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
    NodeDataTypeType() = default;

    static constexpr StringType ID = extends::DataType::ID + "_" + Id.value;
    static constexpr StringType NAME = Name;

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
    static constexpr StringType NAME = Name;

    static QtNodes::NodeDataType getNodeDataType() {
        return {QString::fromStdString(ID.value), Name.value};
    }
};

class NodeDataTypeTypeHelpers {
public:
    /**
     * Check whether derived contains base at the beginning of type list
     * @param derived
     * @param base
     * @return
     */
    static bool inherits(QtNodes::NodeDataType derived, QtNodes::NodeDataType base) {

        auto derivedTypes = derived.id.split("_");
        auto baseTypes = base.id.split("_");

        //check if output type is fully present in input from the start
        bool contains = false;
        int d = 0;
        int b = 0;
        while (true) {
            if (d >= derivedTypes.size() && b >= baseTypes.size()) {
                contains = true;
                break;
            }
            if (d >= derivedTypes.size()) {
                contains = false;
                break;
            }
            if (b >= baseTypes.size()) {
                contains = true;
                break;
            }
            if (derivedTypes[d] == baseTypes[b]) {
                d++;
                b++;
            } else {
                contains = false;
                break;
            }
        }
        return contains;
    }

    /**
     * Will return part of derived that extends base
     * Example:
     * base:1_2_3
     * derived:1_2_3_4_5_6
     * returns:4_5_6
     * @param derived
     * @param base
     * @return
     */
    static QtNodes::NodeDataType getExtendedPart(QtNodes::NodeDataType derived, QtNodes::NodeDataType base) {
        auto derivedTypes = derived.id.split("_");
        auto baseTypes = base.id.split("_");

        //find last common index
        int d = 0;
        int b = 0;
        while (true) {
            if (d >= derivedTypes.size() && b >= baseTypes.size()) {
                break;
            }
            if (d >= derivedTypes.size()) {
                break;
            }
            if (b >= baseTypes.size()) {
                break;
            }
            if (derivedTypes[d] == baseTypes[b]) {
                d++;
                b++;
            } else {
                break;
            }
        }
        //no common base
        if (d == 0) {
            return {};
        }
        QString result = derivedTypes[d];
        for (int i = d+1; i < derivedTypes.size(); i++) {
            result += "_" + derivedTypes[i];
        }

        return {.id=result, .name=""};
    }

    static QString getName(QtNodes::NodeDataType type){
        return type.id.split("_").back().toLower();
    }
};