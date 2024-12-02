//
// Created by vm on 24.30.10.
//

#pragma once

#include "baseDataType.h"
#include "nodeDataTypeType.h"
#include "src/utils/stringType.h"
#include "src/utils/concepts.h"
//template<typename V>
//concept isBaseDataType = requires(V a) {
//    a.toString();
//};

/**
 * Holds variable length array
 */
template<typename T, StringType name = "">
class ArrayDataType : public BaseDataType {
public:
    using DataType = NodeDataTypeType<"Array", name + " array">;
    DataType nodeType;

    ArrayDataType()
            : data() {}

    ArrayDataType(std::vector<T> const number)
            : data(number) {}

    ~ArrayDataType() override {}


    QtNodes::NodeDataType type() const override {
        return nodeType.getNodeDataType();
    }

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

    QString toString() override {
        QString str = "[";
        for (int i = 0; i < data.size(); i++) {

            if constexpr (baseDataType<T>) {
                str += data[i].toString();
            } else {
                str += std::to_string(data[i]);
            }
            if (i != data.size() - 1) {
                str += ", ";
            }
        }
        str += "]";
        return str;
    }

private:

    std::vector<T> data;
};

