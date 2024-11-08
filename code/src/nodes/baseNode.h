//
// Created by vm on 24.30.10.
//

#pragma once

#include "src/utils/concepts.h"

#include <QtNodes/internal/NodeDelegateModel.hpp>
#include "iostream"
#include "src/nodes/dataTypes/arrayDataType.h"
#include "src/nodes/nodePort.h"

/**
 *
 * @tparam InPorts Types of input ports
 * @tparam OutPorts Types of output ports
 */
template<tuple InPorts, tuple OutPorts>
class BaseNode : public QtNodes::NodeDelegateModel {
private:

    /**
     * Will convert vector of std::shared_ptr<BaseNodePort> in to tuple of down casted BaseDataType according to tuple ports
     */
    template<tuple ports, typename T, std::size_t... Indices>
    auto vectorToTupleHelper(const std::vector<T> &v, std::index_sequence<Indices...>) {
        return std::make_tuple(
                (std::dynamic_pointer_cast<NodePort<std::tuple_element_t<Indices, ports>>>(v[Indices])->data)...);
    }

    /**
     * Will convert vector of std::shared_ptr<BaseNodePort> in to tuple of down casted BaseDataType according to tuple ports
     */
    template<tuple ports, std::size_t N, typename T>
    auto vectorToTuple(const std::vector<T> &v) {
        assert(v.size() >= N);
        return vectorToTupleHelper<ports>(v, std::make_index_sequence<N>());
    }

    template<typename Tuple>
    struct WrappedTupleElements;
    /**
     * Will wrap each tuple element in std::shared_ptr
     */
    template<class... Values>
    struct WrappedTupleElements<std::tuple<Values...>> {
        using type = std::tuple<std::shared_ptr<Values>...>;
    };

    template<tuple tup, std::size_t I = 0>
    constexpr void generateNodePort(std::vector<std::shared_ptr<BaseNodePort>> &ports) {
        if constexpr (I < std::tuple_size_v<tup>) {
            using SelectedType = std::tuple_element_t<I, tup>;
            auto port = std::make_shared<NodePort<SelectedType>>();
            using DataType = SelectedType::DataType;
            DataType dataType;
            port->type = dataType.getNodeDataType();
            ports.push_back(std::move(port));
            generateNodePort<tup, I + 1>(ports);
        }
    }

    template<tuple ports>
    constexpr std::vector<std::shared_ptr<BaseNodePort>> generateNodePorts() {
        std::vector<std::shared_ptr<BaseNodePort>> nodes;
        generateNodePort<ports>(nodes);
        return nodes;
    }

    std::vector<std::shared_ptr<BaseNodePort>> inNodePorts = generateNodePorts<InPorts>();
    std::vector<std::shared_ptr<BaseNodePort>> outNodePorts = generateNodePorts<OutPorts>();

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

    /**
    * Downcasts data to type specified in tuple at index and assigns it to BaseNodePort in ports at index
    * @tparam tup
    * @tparam I
    * @param ports
    * @param data
    */
    template<tuple tup, std::size_t I = 0>
    void setInNodePortData(std::vector<std::shared_ptr<BaseNodePort>> &ports, std::shared_ptr<QtNodes::NodeData> data,
                           int index) {
        if constexpr (I < std::tuple_size_v<tup>) {
            if (I == index) {
                using portType = std::tuple_element_t<I, tup>;
                auto dataType = std::dynamic_pointer_cast<portType>(data);
                std::dynamic_pointer_cast<NodePort<portType>>(ports.at(I))->data = dataType;
            }
            setInNodePortData<tup, I + 1>(ports, data, index);
        }

    }

    template<typename OutTuple, std::size_t I = 0>
    void setOutNodePortData(std::vector<std::shared_ptr<BaseNodePort>> &ports,
                            WrappedTupleElements<OutPorts>::type &inTuple) {
        if constexpr (I < std::tuple_size_v<OutTuple>) {

            using portType = std::tuple_element_t<I, OutTuple>::element_type;
            std::dynamic_pointer_cast<NodePort<portType>>(ports.at(I))->data = std::get<I>(inTuple);


            setOutNodePortData<OutTuple, I + 1>(ports, inTuple);
        }
    }

    template<typename OutTuple, std::size_t I = 0>
    std::shared_ptr<QtNodes::NodeData> getOutNodeData(std::vector<std::shared_ptr<BaseNodePort>> &ports, int index) {
        if constexpr (I < std::tuple_size_v<OutTuple>) {

            using portType = std::tuple_element_t<I, OutTuple>;

            if (I == index) {
                auto nodeData = std::dynamic_pointer_cast<NodePort<portType>>(ports.at(I))->data;
                return nodeData;
            } else {
                getOutNodeData<OutTuple, I + 1>(ports, index);
            }
        }
        return {};
    }

public:


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


    /**
     * Will call compute
     */
    void updated() {
        setInData(nullptr, 0);
    }

    void setInData(std::shared_ptr<QtNodes::NodeData> nodeData, const QtNodes::PortIndex portIndex) override {
        if (nodeData != nullptr) {
            // assign nodeData to specified by portIndex index at inNodePorts
            setInNodePortData<InPorts>(inNodePorts, nodeData, portIndex);
        }

        auto ret = compute(vectorToTuple<InPorts, std::tuple_size_v<InPorts>>(inNodePorts));

        auto tempOutNodePorts = outNodePorts;
        outNodePorts = generateNodePorts<OutPorts>();
        setOutNodePortData<typename WrappedTupleElements<OutPorts>::type>(outNodePorts, ret);

        for (int i = 0; i < outNodePorts.size(); i++) {
            if (tempOutNodePorts[i] != outNodePorts[i]) {
                Q_EMIT dataUpdated(i);
            }
        }
    }

    std::shared_ptr<QtNodes::NodeData> outData(const QtNodes::PortIndex port) override {
        auto nodeData = getOutNodeData<OutPorts>(outNodePorts, port);
        return nodeData;
    }

    /**
     * Must perform calculations based on input tuple, pointers in tuple might be nullptr
     * @return newly constructed shred_ptr
     */
    virtual WrappedTupleElements<OutPorts>::type compute(WrappedTupleElements<InPorts>::type) = 0;

    /**
     * Must return json representation of node data
     * @return
     */
    virtual QJsonObject onSave() const = 0;

    /**
     * Must load node data from json, if there was error while parsing data, false must be returned, true otherwise
     * @return false if load failed, true otherwise
     */
    virtual bool onLoad(QJsonObject json) = 0;


    QJsonObject save() const override {
        QJsonObject modelJson = NodeDelegateModel::save();
        modelJson["data"] = onSave();
        return modelJson;
    }

    void load(QJsonObject const &json) override {
        auto data = json["data"];
        if (data == QJsonValue::Undefined) {
            //error
            return;
        }
        onLoad(data.toObject());
    }
};

