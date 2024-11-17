//
// Created by vm on 24.8.11.
//
#pragma once

#include <QtNodes/internal/DataFlowGraphModel.hpp>

/**
 * Extension of DataFlowGraphModel which allows connection between nodes if NodeDataType is with the same base
 */
class DynamicDataFlowGraphModel : public QtNodes::DataFlowGraphModel {
public:
    explicit DynamicDataFlowGraphModel(std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry);

    bool connectionPossible(QtNodes::ConnectionId connectionId) const override;


    bool willHaveCycle(QtNodes::ConnectionId connectionId) const;

public slots:

    void onNodeCreation(QtNodes::NodeId nodeId);

    void trigger();
};