//
// Created by vm on 24.4.12.
//

#include <QBoxLayout>
#include "foreachNode.h"
#include "src/nodes/internalForeachNodes/foreachInputNode.h"
#include "QUndoStack"
#include "QTimer"
#include "src/nodes/internalForeachNodes/foreachInputNodeDataType.h"
#include "src/nodes/internalForeachNodes/foreachOutputNode.h"
#include "customNodePainter.h"

QString ForeachNode::caption() const {
    return "Foreach node";
}

QString ForeachNode::name() const {
    return "Foreach node";
}

unsigned int ForeachNode::nPorts(QtNodes::PortType portType) const {
    switch (portType) {
        case QtNodes::PortType::In:
            return inputPortTypes.size();
        case QtNodes::PortType::Out:
            return outputPortTypes.size();
        case QtNodes::PortType::None:
            return 0;
    }
    return 0;
}

QtNodes::NodeDataType ForeachNode::dataType(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const {
    switch (portType) {
        case QtNodes::PortType::In:
            return inputPortTypes.at(portIndex);
        case QtNodes::PortType::Out:
            if (portIndex >= outputPortTypes.size()) {
                return {};
            }
            return outputPortTypes.at(portIndex);
        case QtNodes::PortType::None:
            return {};
    }
    return {};
}

void ForeachNode::setInData(std::shared_ptr<QtNodes::NodeData> nodeData, const QtNodes::PortIndex portIndex) {
    if (!doComputing) {
        return;
    }

    auto d = std::dynamic_pointer_cast<BaseData>(nodeData);
    inputPorts.at(portIndex) = d;


    if (inputPorts[0] == nullptr) {
        workSemaphore.acquire();
        emit computingStarted();

        outputPorts.resize(outputPortTypes.size());
        for (auto &outputPort: outputPorts) {
            outputPort = nullptr;
        }

        //notify output connections
        for (int i = 0; i < outputPortTypes.size(); i++) {
            emit dataUpdated(i);
        }

        emit computingFinished();
        workSemaphore.release();

        return;
    }

    //start parallel processing
    workSemaphore.acquire();
    emit computingStarted();
    initDataFlowGraphModel();

    auto array = std::dynamic_pointer_cast<BaseDataArrayData>(inputPorts[0]);
    assert(array != nullptr);

    resultsArray.clear();
    for (int i = 0; i < array->size(); i++) {
        auto element = array->at(i);


        auto inputNode = graphModel->delegateModel<ForeachInputNode>(inputNodeId);
        auto outputNode = graphModel->delegateModel<ForeachOutputNode>(outputNodeId);


        //create ForeachInputNodeData from input data, but replace first element with one input array element
        auto types = inputPortTypes;
        auto data = inputPorts;
        types[0] = inputArrayValueType;
        data[0] = element;
        auto dt = std::make_shared<ForeachInputNodeData>(types, data);

        graphModel->setComputing(true);
        //trigger all input nodes
        auto nodes = graphModel->allNodeIds();
        for (auto node: nodes) {
            auto model = graphModel->delegateModel<BaseNodeTypeLessWrapper>(node);
            if (model->isSource()) {
                model->recalculate();
            }
        }
        workFinished.acquire();
        progressCounterMutex.lock();
        progressCounter = 0;
        progressCounterMutex.unlock();
        inputNode->setInData(dt, QtNodes::InvalidPortIndex);

        //wait for answer
        workFinished.acquire();
        workFinished.release();
        progressCounterMutex.lock();
        progressCounter = 0;
        progressCounterMutex.unlock();
        graphModel->setComputing(false);


        auto result = std::dynamic_pointer_cast<ForeachInputNodeData>(
                outputNode->outData(QtNodes::InvalidPortIndex));
        assert(result != nullptr);
        resultsArray.push_back(result->data);
    }
    assert(array->size() == resultsArray.size());
    //convert resultsArray in to array of ArrayData

    //fill outputPorts
    outputPorts.resize(outputPortTypes.size());
    for (int i = 0; i < outputPortTypes.size(); i++) {
        outputPorts[i] = std::make_shared<ArrayData<std::shared_ptr<BaseData>>>();
    }
    for (auto result: resultsArray) {
        for (int i = 0; i < result.size(); i++) {
            outputPorts[i]->get().push_back(result[i]);
        }
    }
    //notify output connections
    for (int i = 0; i < outputPortTypes.size(); i++) {
        emit dataUpdated(i);
    }
    emit computingFinished();
    workSemaphore.release();
}

std::shared_ptr<QtNodes::NodeData> ForeachNode::outData(const QtNodes::PortIndex port) {
    if (port >= outputPorts.size()) {
        return nullptr;
    }

    return outputPorts.at(port);
}

QWidget *ForeachNode::embeddedWidget() {
    if (base == nullptr) {
        //init layout
        base = new QWidget();
        base->setLayout(new QBoxLayout(QBoxLayout::LeftToRight));
        button = new QPushButton("Edit");
        base->layout()->addWidget(button);

        initDataFlowGraphModel();

        connect(button, &QPushButton::clicked, this, [this](bool) {
            updateInternalNodeType();
            emit setView(new QtNodes::GraphicsView(scene), graphModel);
        });
    }
    return base;
}

void ForeachNode::initDataFlowGraphModel() {
    if (innited) {
        return;
    }
    innited = true;
    //init internal node graph
    nodeRegistry = std::make_shared<QtNodes::NodeDelegateModelRegistry>();
    registerNodes(nodeRegistry);

    //register input and output nodes, specific only to this dataFlowGraphModel
    nodeRegistry->registerModel<ForeachInputNode>("Input");
    nodeRegistry->registerModel<ForeachOutputNode>("Output");

    graphModel = new DynamicDataFlowGraphModel(nodeRegistry);
    connect(graphModel, &DynamicDataFlowGraphModel::viewClosed, this, &ForeachNode::viewClosed);

    scene = new QtNodes::DataFlowGraphicsScene(*graphModel, this);
    scene->setNodePainter(std::make_unique<CustomNodePainter>());

    // increase counter each time node starts and finishes computation
    connect(graphModel,
            &DynamicDataFlowGraphModel::nodeCreated,
            this,
            [this](QtNodes::NodeId nodeId) {
                graphModel->delegateModel<BaseNodeTypeLessWrapper>(nodeId)->embeddedWidget();
                connect(
                        graphModel->delegateModel<QtNodes::NodeDelegateModel>(nodeId),
                        &QtNodes::NodeDelegateModel::computingStarted, this, [this]() {
                            progressCounterMutex.lock();
                            progressCounter++;
                            std::cout << "progressCounter ++:" << progressCounter << std::endl;

                            progressCounterMutex.unlock();
                        }, Qt::DirectConnection);
                connect(
                        graphModel->delegateModel<QtNodes::NodeDelegateModel>(nodeId),
                        &QtNodes::NodeDelegateModel::computingFinished, this, [this]() {
                            progressCounterMutex.lock();
                            progressCounter--;
                            std::cout << "progressCounter --:" << progressCounter << std::endl;
                            progressCounterMutex.unlock();
                            if (progressCounter == 0) {
                                workFinished.release();
                            }
                        }, Qt::DirectConnection);
            });


    //add input and output nodes, specific only to this dataFlowGraphModel
    ForeachInputNode foreachInputNode;
    inputNodeId = graphModel->addNode(foreachInputNode.name());
    ForeachOutputNode foreachOutputNode;
    outputNodeId = graphModel->addNode(foreachOutputNode.name());

    auto inputNode = graphModel->delegateModel<ForeachInputNode>(inputNodeId);
    assert(inputNode != nullptr);
    auto outputNode = graphModel->delegateModel<ForeachOutputNode>(outputNodeId);
    assert(outputNode != nullptr);

    // make inputNode and outputNode indestructible by calling undo every time input or output nodes are deleted
    connect(graphModel, &DynamicDataFlowGraphModel::nodeDeleted, [this](QtNodes::NodeId nodeId) {
        if (nodeId == inputNodeId || nodeId == outputNodeId) {
            QTimer::singleShot(0, [this] {
                scene->undoStack().undo();
            });
        }
    });

}


void ForeachNode::updateInternalNodeType() {
    //pass data and datatypes to inputNode

    auto types = inputPortTypes;
    // first element in inputPorts is always BaseArrayData
    types[0] = inputArrayValueType;
    auto inputNode = graphModel->delegateModel<ForeachInputNode>(inputNodeId);
    assert(inputNode != nullptr);
    std::vector<std::shared_ptr<BaseData>> d;
    auto dt = std::make_shared<ForeachInputNodeData>(types, d);
    inputNode->setInData(dt, QtNodes::InvalidPortIndex);
    inputNode->embeddedWidgetSizeUpdated();
}


bool ForeachNode::isSource() {
    return false;
}

void ForeachNode::recalculate() {

}

void ForeachNode::onInputConnectionCreation(QtNodes::ConnectionId connection, QtNodes::NodeDataType type) {
    if (dirtyInputConnections) {
        return;
    }
    dirtyInputConnections = true;
    //save type of main array port
    if (connection.inPortIndex == 0) {
        inputPortTypes[0] = type;
        inputArrayValueType = BaseArrayData::getValueType(type);
    } else {
        //save type of additional data ports
        if (connection.inPortIndex >= inputPortTypes.size()) {
            inputPortTypes.resize(connection.inPortIndex + 1);
        }
        if (connection.inPortIndex >= inputPorts.size()) {
            inputPorts.resize(connection.inPortIndex + 1);
        }
        //save connected port type
        inputPortTypes[connection.inPortIndex] = type;

        //add new input connection with DataBaseType at the end
        int newPortIndex = inputPortTypes.size();
        portsAboutToBeInserted(QtNodes::PortType::In, newPortIndex, newPortIndex);
        inputPortTypes.push_back(BaseData::DataType::getNodeDataType());
        portsInserted();

    }
    dirtyInputConnections = false;

    // explicitly update output node port types
    int portCount = graphModel->delegateModel<BaseNodeTypeLessWrapper>(outputNodeId)->nPorts(QtNodes::PortType::In);

    for (int i = 0; i < portCount; i++) {
        for (auto connection: graphModel->connections(outputNodeId, QtNodes::PortType::In, i)) {

            auto source = graphModel->delegateModel<BaseNodeTypeLessWrapper>(connection.outNodeId);
            auto type = source->dataType(QtNodes::PortType::Out, connection.outPortIndex);
            type = BaseArrayData::wrapWithArray(type);
            outputPortTypes[i] = type;
        }
    }
    emit embeddedWidgetSizeUpdated();
}

void ForeachNode::inputConnectionDeleted(const QtNodes::ConnectionId &connection) {
    if (dirtyInputConnections) {
        return;
    }
    dirtyInputConnections = true;
    if (connection.inPortIndex == 0) {
        inputPortTypes[0] = ArrayData<std::shared_ptr<BaseData>>::DataType::getNodeDataType();
        inputPorts[0] = nullptr;
    } else {
        //delete disconnected port
        int index = connection.inPortIndex;
        portsAboutToBeDeleted(QtNodes::PortType::In, index, index);
        inputPortTypes.erase(inputPortTypes.begin() + index);
        inputPorts.erase(inputPorts.begin() + index);
        portsDeleted();
    }
    dirtyInputConnections = false;

    // explicitly update output node port types
    int portCount = graphModel->delegateModel<BaseNodeTypeLessWrapper>(outputNodeId)->nPorts(QtNodes::PortType::In);

    for (int i = 0; i < portCount; i++) {
        for (auto connection: graphModel->connections(outputNodeId, QtNodes::PortType::In, i)) {

            auto source = graphModel->delegateModel<BaseNodeTypeLessWrapper>(connection.outNodeId);
            auto type = source->dataType(QtNodes::PortType::Out, connection.outPortIndex);
            type = BaseArrayData::wrapWithArray(type);
            outputPortTypes[i] = type;
        }
    }
    emit embeddedWidgetSizeUpdated();
}

void ForeachNode::viewClosed() {
    updateExternalOutputPorts();
}

QJsonObject ForeachNode::save() const {
    QJsonObject modelJson = NodeDelegateModel::save();
    modelJson["graph"] = graphModel->save();
    return modelJson;
}

void ForeachNode::load(const QJsonObject &json) {
    graphModel->load(json["graph"].toObject());

    updateInternalNodeType();
    updateExternalOutputPorts();

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(50ms);


}

void ForeachNode::updateExternalOutputPorts() {
    auto outputNode = graphModel->delegateModel<ForeachOutputNode>(outputNodeId);
    auto data = std::dynamic_pointer_cast<ForeachInputNodeData>(outputNode->outData(QtNodes::InvalidPortIndex));
    assert(data != nullptr);
    //wrap each type with array
    for (auto &type: data->types) {
        type = BaseArrayData::wrapWithArray(type);
    }
    outputPortTypes = data->types;
    emit embeddedWidgetSizeUpdated();
}

