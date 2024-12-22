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

    /**
     * Returns type of stored value
     * @return
     */
    virtual QtNodes::NodeDataType valueType() const {
        return {};
    }

    /**
     * Will extract type that is stored in array with this NodeDataType, returns empty type if type is not ArrayDataType
     * @param type type from witch to extract type
     * @return
     */
    static QtNodes::NodeDataType getValueType(QtNodes::NodeDataType arrayType) {
        //arrayType must inherit from BaseArrayDataType
        if (!NodeDataTypeTypeHelpers::inherits(arrayType, DataType::getNodeDataType())) {
            return {};
        }
        auto baseArrayType = DataType::getNodeDataType();
        baseArrayType.id = baseArrayType.id + "_Array";

        auto valueType = NodeDataTypeTypeHelpers::getExtendedPart(arrayType, baseArrayType);
        valueType.name = NodeDataTypeTypeHelpers::getName(valueType);
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


    ~BaseArrayDataType() override = default;
};

/**
 * Holds variable length array of shared_ptr to BaseDataType
 */
template<typename T>
class ArrayDataType;

/**
 * Class for ArrayDataType with type std::shared_ptr<BaseDataType>
 */
class BaseDataTypeArrayDataType : public BaseArrayDataType {
public:
    virtual size_t size() = 0;

    virtual std::shared_ptr<BaseDataType> at(size_t i) = 0;
};

template<SharedPtrToBaseDataType T>
class ArrayDataType<T> : public BaseDataTypeArrayDataType {
public:

//    static constexpr StringType i = (StringType) T::element_type::DataType::id+ "_Array";
    using DataType = NodeDataTypeType<
            "Array_" + T::element_type::DataType::ID,
            "array<" + T::element_type::DataType::NAME + ">",
            BaseArrayDataType>;

    DataType nodeType;


    ArrayDataType()
            : data() {}

    explicit ArrayDataType(std::vector<T> const number)
            : data(number) {}

    ~ArrayDataType() override = default;


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

    std::shared_ptr<BaseDataType> at(size_t i) override {
        if (i >= data.size()) {
            return nullptr;
        }
        return data.at(i);
    }

    QString toString() override {
        QString str = "[";
        for (int i = 0; i < data.size(); i++) {

            if constexpr (SharedPtrToBaseDataType<T>) {
                if (data[i] == nullptr) {
                    continue;
                }
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
    using DataType = NodeDataTypeType<"Array_Double", "array<double>", BaseArrayDataType>;

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
