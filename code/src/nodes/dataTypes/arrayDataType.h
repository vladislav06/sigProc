//
// Created by vm on 24.30.10.
//

#pragma once

#include "baseDataType.h"
#include "nodeDataTypeType.h"

/**
 * Holds variable length array
 */
template<typename T>
class ArrayDataType : public BaseDataType {
public:
    using DataType = NodeDataTypeType<"Array", "array">;
    DataType nodeType;

    ArrayDataType()
            : data() {}

    ArrayDataType(std::vector<T> const number)
            : data(number) {}


    QtNodes::NodeDataType type() const override { return nodeType.getNodeDataType(); }

    /**
     * Returns copy of internal array;
     * @return
     */
    std::vector<T> array() const { return data; }


    /**
     * Returns reference to internal array
     * @return
     */
    std::vector<T> &get() {
        return data;
    }

private:
    std::vector<T> data;
};

