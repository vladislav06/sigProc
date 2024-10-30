//
// Created by vm on 24.30.10.
//

#pragma once

#include "src/utils/concepts.h"

#include <QtNodes/internal/NodeDelegateModel.hpp>
#include "iostream"
#include "src/nodes/dataTypes/arrayDataType.h"


struct BaseNodePort {
    QtNodes::NodeDataType type;
};

template<baseDataType T>
struct NodePort : public BaseNodePort {
    std::shared_ptr<T> data;
};

struct NoneType : public BaseDataType {
    using DataType = NodeDataTypeType<"None", "None">;
    DataType nodeType;

    QtNodes::NodeDataType type() const override {
        return nodeType.getNodeDataType();
    }

};


/**
 *
 * @tparam InPorts Types of input ports
 * @tparam OutPorts Types of output ports
 */
template<tuple InPorts, tuple OutPorts>
class BaseNode : public QtNodes::NodeDelegateModel {
public:


    template<tuple tup, std::size_t I = 0>
    constexpr void generateNodePort(std::vector<std::unique_ptr<BaseNodePort>> &ports) {
        if constexpr (I < std::tuple_size_v<tup>) {
            using SelectedType = std::tuple_element_t<I, tup>;
            auto port = std::make_unique<NodePort<SelectedType>>();
            using DataType = SelectedType::DataType;
            DataType dataType;
            port->type = dataType.getNodeDataType();
            ports.push_back(std::move(port));
            generateNodePort<tup, I + 1>(ports);
        }
    }

    template<tuple ports>
    constexpr std::vector<std::unique_ptr<BaseNodePort>> generateNodePorts() {
        std::vector<std::unique_ptr<BaseNodePort>> nodes;
        generateNodePort<ports>(nodes);
        return nodes;
    }


    std::vector<std::unique_ptr<BaseNodePort>> inNodePorts = generateNodePorts<InPorts>();
    std::vector<std::unique_ptr<BaseNodePort>> outNodePorts = generateNodePorts<OutPorts>();

    [[nodiscard]] unsigned int nPorts(QtNodes::PortType portType) const override {
        switch (portType) {
            case QtNodes::PortType::In:
                return std::tuple_size_v<InPorts>;
                break;
            case QtNodes::PortType::Out:
                return std::tuple_size_v<OutPorts>;
                break;
            case QtNodes::PortType::None:
                return 0;
                break;
        }
        return 0;
    }

    [[nodiscard]] QtNodes::NodeDataType
    dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override {

        switch (portType) {
            case QtNodes::PortType::In:
                return inNodePorts.at(portIndex)->type;
            case QtNodes::PortType::Out:
                return outNodePorts.at(portIndex)->type;
            case QtNodes::PortType::None:
                return {};
        }

        return {};
    }

    void setInData(std::shared_ptr<QtNodes::NodeData> nodeData, const QtNodes::PortIndex portIndex) override {
    }

    virtual OutPorts compute(InPorts) = 0;

    std::shared_ptr<QtNodes::NodeData> outData(const QtNodes::PortIndex port) override {
        return std::shared_ptr<QtNodes::NodeData>();
    }

};

