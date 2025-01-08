//
// Created by Vladislavs Agarkovs on 24.30.10.
//
#pragma once

#include <QtNodes/NodeData>
#include <iostream>
#include "stringType.h"


class BaseData;

/**
 * QtNodes::NodeDataType wrapper with templated id and name, support inheritance, by default extends BaseData.
 * BaseData::ID will be appended to the left of the Id
 * @tparam Id id of this type
 * @tparam Name displayed name
 * @tparam extends what type this type extends, BaseData by default
 */
template<StringType Id, StringType Name, typename extends = BaseData>
class NodeDataType {
public:
    NodeDataType() = default;

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
class BaseNodeDataType {
public:
    static constexpr StringType ID = Id;
    static constexpr StringType NAME = Name;

    static QtNodes::NodeDataType getNodeDataType() {
        return {QString::fromStdString(ID.value), Name.value};
    }
};

class NodeDataTypeHelpers {
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
        for (int i = d + 1; i < derivedTypes.size(); i++) {
            result += "_" + derivedTypes[i];
        }

        return {.id=result, .name=""};
    }


    /**
     * Returns type name from lit of types
     * @param type list of types from which to extract name
     * @return type name
     */
    static QString getName(QStringList types) {
        // go from base until the end or new base
        // if only one base then type name will be last type lowercase
        // if second base found, then second base is template type
        for (int i = 1; i < types.size(); i++) {
            if (types[i] == "Base") {
                //recursively get name
                auto templateName = getName({types.begin() + i, types.end()});
                return types[i - 1].toLower() + "<" + templateName + ">";
            }
        }
        return types.back().toLower();
    }

    /**
     * Returns type name from type id
     * @param type type from which to extract name
     * @return type name
     */
    static QString getName(QtNodes::NodeDataType type) {
        // go from base until the end or new base
        // if only one base then type name will be last type lowercase
        // if second base found, then second base is template type
        auto types = type.id.split("_");
        for (int i = 1; i < types.size(); i++) {
            //case for array
            if(types[i]=="Array"){
                //recursively get name
                auto templateName = getName({types.begin() + i+1, types.end()});
                return types[i].toLower() + "<" + templateName + ">";
            }
        }
        return types.back().toLower();
    }
};