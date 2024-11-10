//
// Created by vm on 24.8.11.
//
#pragma once

#include <QtNodes/internal/DataFlowGraphModel.hpp>
#include <utility>

/**
 * Extension of DataFlowGraphModel which allows connection between nodes if NodeDataType is with the same base
 */
class DynamicDataFlowGraphModel : public QtNodes::DataFlowGraphModel {
public:
    explicit DynamicDataFlowGraphModel(std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry)
            : DataFlowGraphModel(std::move(registry)) {}

    bool connectionPossible(const QtNodes::ConnectionId connectionId) const override {
        auto getDataType = [&](QtNodes::PortType const portType) {
            return portData(getNodeId(portType, connectionId),
                            portType,
                            getPortIndex(portType, connectionId),
                            QtNodes::PortRole::DataType)
                    .value<QtNodes::NodeDataType>();
        };

        auto portVacant = [&](QtNodes::PortType const portType) {
            QtNodes::NodeId const nodeId = getNodeId(portType, connectionId);
            QtNodes::PortIndex const portIndex = getPortIndex(portType, connectionId);
            auto const connected = connections(nodeId, portType, portIndex);

            auto policy = portData(nodeId, portType, portIndex, QtNodes::PortRole::ConnectionPolicyRole)
                    .value<QtNodes::ConnectionPolicy>();

            return connected.empty() || (policy == QtNodes::ConnectionPolicy::Many);
        };

        auto typeOut = getDataType(QtNodes::PortType::Out);
        auto typeIn = getDataType(QtNodes::PortType::In);


        auto typesOut = typeOut.id.split("_");
        auto typesIn = typeIn.id.split("_");

        //check if out has same type as topmost in

        return typesOut.contains(typesIn.back())
               && portVacant(QtNodes::PortType::Out) && portVacant(QtNodes::PortType::In);
    }
};