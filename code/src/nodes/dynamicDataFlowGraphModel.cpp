//
// Created by vm on 24.17.11.
//
#include "dynamicDataFlowGraphModel.h"
#include "baseNode.h"
#include "queue"
#include "QTimer"

DynamicDataFlowGraphModel::DynamicDataFlowGraphModel(std::shared_ptr<QtNodes::NodeDelegateModelRegistry> registry)
        : DataFlowGraphModel(std::move(registry)) {

    connect(this, &QtNodes::DataFlowGraphModel::nodeCreated, this, &DynamicDataFlowGraphModel::onNodeCreation);
}

bool DynamicDataFlowGraphModel::connectionPossible(const QtNodes::ConnectionId connectionId) const {
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

    if (typesOut.contains(typesIn.back())
        && portVacant(QtNodes::PortType::Out) && portVacant(QtNodes::PortType::In)) {
        if (willHaveCycle(connectionId)) {
            return false;
        } else {
            return true;
        }
    } else {
        return false;
    }
}

bool DynamicDataFlowGraphModel::willHaveCycle(const QtNodes::ConnectionId connectionId) const {
    //collect all connections
    auto nodes = allNodeIds();
    std::unordered_set<QtNodes::ConnectionId> connections;

    for (auto node: nodes) {
        auto cons = QtNodes::DataFlowGraphModel::allConnectionIds(node);
        std::for_each(cons.begin(), cons.end(), [&connections](auto const &p) { connections.insert(p); });
    }

    std::vector<std::vector<unsigned int>> adjacencyList;

    unsigned int maxNodeId = 0;
    for (auto connection: connections) {
        if (adjacencyList.size() <= connection.outNodeId) {
            adjacencyList.resize(connection.outNodeId + 1);
        }
        adjacencyList[connection.outNodeId].push_back(connection.inNodeId);
        if (connection.inNodeId > maxNodeId) {
            maxNodeId = connection.inNodeId;
        }
    }

    //add future connection
    if (adjacencyList.size() <= connectionId.outNodeId) {
        adjacencyList.resize(connectionId.outNodeId + 1);
    }
    adjacencyList[connectionId.outNodeId].push_back(connectionId.inNodeId);

    if (connectionId.inNodeId > maxNodeId) {
        maxNodeId = connectionId.inNodeId;
    }
    // adjacencyList must contain all nodeIds
    if (adjacencyList.size() <= maxNodeId) {
        adjacencyList.resize(maxNodeId + 1);
    }


    //algo
    // stores in-degree of each vertex
    std::vector<int> inDegree(adjacencyList.size(), 0);

    // queue to store vertices with 0 in-degree
    std::queue<unsigned int> q;

    int visited = 0; // count of visited vertices

    // Calculate in-degree of each vertex
    for (auto &u: adjacencyList) {
        for (auto v: u) {
            inDegree[v]++;
        }
    }

    // Enqueue vertices with 0 in-degree
    for (int u = 0; u < adjacencyList.size(); u++) {
        if (inDegree[u] == 0) {
            q.push(u);
        }
    }

    // BFS traversal
    while (!q.empty()) {
        unsigned int u = q.front();
        q.pop();
        visited++;

        // Reduce in-degree of adjacent vertices
        for (auto v: adjacencyList[u]) {
            inDegree[v]--;

            // If in-degree becomes 0, enqueue it
            if (inDegree[v] == 0) {
                q.push(v);
            }
        }
    }

    // If not all vertices are visited, cycle
    return visited != adjacencyList.size();
}

void DynamicDataFlowGraphModel::onNodeCreation(const QtNodes::NodeId nodeId) {
    auto model = delegateModel<QtNodes::NodeDelegateModel>(nodeId);

    //register additional signal
    connect(model, &QtNodes::NodeDelegateModel::embeddedWidgetSizeUpdated, this, [this, nodeId]() {
        //update node on next frame because widgets might still not be added
        QTimer::singleShot(0, [this, nodeId] {
            Q_EMIT  this->nodeUpdated(nodeId);
        });
    });
}

void DynamicDataFlowGraphModel::trigger() {
    //will recalculate entire node tree
    auto nodes = allNodeIds();
    for (auto node: nodes) {
        auto model = delegateModel<BaseNodeTypeLessWrapper>(node);
        if (model->isSource()) {
            model->recalculate();
        }
    }
}