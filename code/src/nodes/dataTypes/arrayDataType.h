//
// Created by vm on 24.30.10.
//

#pragma once

#include "baseDataType.h"
#include "nodeDataTypeType.h"
#include "src/utils/stringType.h"
#include "src/utils/concepts.h"
#include "src/utils/stringType.h"

class BaseArrayDataType : public BaseDataType {
public:
    BaseArrayDataType() = default;

    using DataType = NodeDataTypeType<"BaseArray", "Array">;
    DataType nodeType;

    QtNodes::NodeDataType type() const override {
        return nodeType.getNodeDataType();
    }

    ~BaseArrayDataType() override = default;
};

/**
 * Holds variable length array of shared_ptr to BaseDataType
 */
template<typename T>
class ArrayDataType;

template<SharedPtrToBaseDataType T>
class ArrayDataType<T> : public BaseArrayDataType {
public:

//    static constexpr StringType i = (StringType) T::element_type::DataType::id+ "_Array";
    using DataType = NodeDataTypeType<"Array_" + T::element_type::DataType::ID, "array", BaseArrayDataType>;

    DataType nodeType;


    ArrayDataType()
            : data() {}

    explicit ArrayDataType(std::vector<T> const number)
            : data(number) {}

    ~ArrayDataType() override = default;


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
            if constexpr (SharedPtrToBaseDataType<T>) {
                str += data[i]->toString();
            } else if constexpr (baseDataType<T>) {
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

/**
 * Holds variable length array of double
 */
template<>
class ArrayDataType<double> : public BaseArrayDataType {
public:
    using DataType = NodeDataTypeType<"Array_Double", "array", BaseArrayDataType>;

    DataType nodeType;


    ArrayDataType()
            : data() {}

    explicit ArrayDataType(std::vector<double> const number)
            : data(number) {}

    ~ArrayDataType() override = default;


    QtNodes::NodeDataType type() const override {
        return nodeType.getNodeDataType();
    }

    /**
     * Returns copy of internal array;
     * @return
     */
    std::vector<double> array() const { return data; }


    /**
     * Returns reference to internal array
     * @return
     */
    std::vector<double> &get() {
        return data;
    }

    QString toString() override {
        QString str = "[";
        for (int i = 0; i < data.size(); i++) {
            str += std::to_string(data[i]);
            if (i != data.size() - 1) {
                str += ", ";
            }
        }
        str += "]";
        return str;
    }

private:

    std::vector<double> data;
};
