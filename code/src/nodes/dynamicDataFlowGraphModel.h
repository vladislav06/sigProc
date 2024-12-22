//
// Created by vm on 24.8.11.
//
#pragma once

#include <QtNodes/internal/DataFlowGraphModel.hpp>
#include "QtNodes/GraphicsView"

/**
 * Extension of DataFlowGraphModel which allows connection between nodes if NodeDataType is with the same base
 */
class DynamicDataFlowGraphModel : public QtNodes::DataFlowGraphModel {
Q_OBJECT

public:
    explicit DynamicDataFlowGraphModel(std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry);

    bool connectionPossible(QtNodes::ConnectionId connectionId) const override;


    bool willHaveCycle(QtNodes::ConnectionId connectionId) const;


public slots:

    void onNodeCreation(QtNodes::NodeId nodeId);

    void trigger();

    void onViewClose();

signals:

    void setView(QtNodes::GraphicsView *graphView, DynamicDataFlowGraphModel *graphModel);

    void viewClosed();
};