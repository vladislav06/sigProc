//
// Created by vm on 24.30.10.
//

#pragma once

#include "baseData.h"
#include "nodeDataType.h"
#include "stringType.h"
#include "concepts.h"
#include "stringType.h"

class BaseArrayData : public BaseData {
public:
    BaseArrayData() = default;

    using DataType = NodeDataType<"BaseArray", "Array">;
    DataType nodeType;

    QtNodes::NodeDataType type() const override {
        return nodeType.getNodeDataType();
    }

    /**
     * Returns type of stored value
     * @return
     */
    virtual QtNodes::NodeDataType valueType() const {
        return {};
    }

    /**
     * Will extract type that is stored in array with this NodeDataType, returns empty type if type is not ArrayData
     * @param type type from witch to extract type
     * @return
     */
    static QtNodes::NodeDataType getValueType(QtNodes::NodeDataType arrayType) {
        //arrayType must inherit from BaseArrayData
        if (!NodeDataTypeHelpers::inherits(arrayType, DataType::getNodeDataType())) {
            return {};
        }
        auto baseArrayType = DataType::getNodeDataType();
        baseArrayType.id = baseArrayType.id + "_Array";

        auto valueType = NodeDataTypeHelpers::getExtendedPart(arrayType, baseArrayType);
        valueType.name = NodeDataTypeHelpers::getName(valueType);
        return valueType;
    }

    /**
     * Will wrap type with array type
     * @param type
     * @return
     */
    static QtNodes::NodeDataType wrapWithArray(QtNodes::NodeDataType type) {
        QtNodes::NodeDataType res;
        res.name = "array<" + type.name + ">";
        res.id = DataType::getNodeDataType().id + "_Array_" + type.id;
//        std::cout << "res.name: " << res.name.toStdString() << "res.id: " << res.id.toStdString() << std::endl;
        return res;
    }


    ~BaseArrayData() override = default;
};

/**
 * Holds variable length array of shared_ptr to BaseData
 */
template<typename T>
class ArrayData;

/**
 * Class for ArrayData with type std::shared_ptr<BaseData>
 */
class BaseDataArrayData : public BaseArrayData {
public:
    virtual size_t size() = 0;

    virtual std::shared_ptr<BaseData> at(size_t i) = 0;
};

template<SharedPtrToBaseData T>
class ArrayData<T> : public BaseDataArrayData {
public:

//    static constexpr StringType i = (StringType) T::element_type::DataType::id+ "_Array";
    using DataType = NodeDataType<
            "Array_" + T::element_type::DataType::ID,
            "array<" + T::element_type::DataType::NAME + ">",
            BaseArrayData>;

    DataType nodeType;


    ArrayData()
            : data() {}

    explicit ArrayData(std::vector<T> const number)
            : data(number) {}

    ~ArrayData() override = default;


    QtNodes::NodeDataType type() const override {
        return nodeType.getNodeDataType();
    }

    QtNodes::NodeDataType valueType() const override {
        return T::element_type::DataType::getNodeDataType();
//        return {T::element_type::DataType::ID.value,T::element_type::DataType::NAME.value};
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

    size_t size() override {
        return data.size();
    }

    std::shared_ptr<BaseData> at(size_t i) override {
        if (i >= data.size()) {
            return nullptr;
        }
        return data.at(i);
    }

    QString toString() override {
        QString str = "[";
        for (int i = 0; i < data.size(); i++) {

            if constexpr (SharedPtrToBaseData<T>) {
                if (data[i] == nullptr) {
                    continue;
                }
                str += data[i]->toString();
            } else if constexpr (baseData<T>) {
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
class ArrayData<double> : public BaseArrayData {
public:
    using DataType = NodeDataType<"Array_Double", "array<double>", BaseArrayData>;

    DataType nodeType;


    ArrayData()
            : data() {}

    explicit ArrayData(std::vector<double> const number)
            : data(number) {}

    ~ArrayData() override = default;


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
