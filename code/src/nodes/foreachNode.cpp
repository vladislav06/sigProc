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
            return outputPortTypes.at(portIndex);
        case QtNodes::PortType::None:
            return {};
    }
    return {};
}

void ForeachNode::setInData(std::shared_ptr<QtNodes::NodeData> nodeData, const QtNodes::PortIndex portIndex) {
    if (nodeData == nullptr) {
        return;
    }
    auto data = std::dynamic_pointer_cast<BaseDataType>(nodeData);
    assert(data != nullptr);
    workSemaphore.acquire();

    inputPorts.at(portIndex) = data;
    //start parallel processing
    //divide input array into even chunks
    auto array = std::dynamic_pointer_cast<BaseDataTypeArrayDataType>(inputPorts[0]);
    assert(array != nullptr);
    int chunkSize = std::ceil((float) array->size() / (float) parallelCount);
    if (chunkSize == 0) {
        return;
    }

    //array that stores data chunks foreach task
    std::vector<std::vector<int>> chunks{};

    std::vector<int> currentChunk;
    // create multiple chunks with chunkSize elements or less
    for (int i = 0; i < array->size(); i++) {
        currentChunk.push_back(i);
        if (i % chunkSize == (chunkSize - 1)) {
            chunks.push_back(currentChunk);
            currentChunk.clear();
        }
    }

    assert(dataFlowGraphModels.size() >= chunks.size());
    threadSemaphore.resize(chunks.size());
    resultsArray.clear();
    for (int i = 0; i < chunks.size(); i++) {
        auto chunk = chunks[i];
        auto &graphModel = dataFlowGraphModels[i];
        threadSemaphore[i].semaphore.acquire();
        //create tasks for each chunk
        ThreadPool::get().queueJob([chunk, &graphModel, array, i, this]() {

            graphModel->counterStarted = 0;
            graphModel->counterFinished = 0;


            auto inputNode = graphModel->dataFlowGraphModel->delegateModel<ForeachInputNode>(inputNodeId);
            auto outputNode = graphModel->dataFlowGraphModel->delegateModel<ForeachOutputNode>(outputNodeId);

            std::vector<std::vector<std::shared_ptr<BaseDataType>>> results;
            for (auto index: chunk) {
                auto types = inputPortTypes;
                auto data = inputPorts;
                types[0] = inputArrayValueType;
                data[0] = array->at(index);
                auto dt = std::make_shared<ForeachInputNodeDataType>(types, data);

                //trigger all input nodes
                auto nodes = graphModel->dataFlowGraphModel->allNodeIds();
                for (auto node: nodes) {
                    auto model = graphModel->dataFlowGraphModel->delegateModel<BaseNodeTypeLessWrapper>(node);
                    if (model->isSource()) {
                        model->recalculate();
                    }
                }

                inputNode->setInData(dt, QtNodes::InvalidPortIndex);
                //wait for answer
                std::mutex m;
                std::unique_lock<std::mutex> lk(m);
                graphModel->workFinished->wait(lk);
                graphModel->counterStarted = 0;
                graphModel->counterFinished = 0;
                auto result = std::dynamic_pointer_cast<ForeachInputNodeDataType>(
                        outputNode->outData(QtNodes::InvalidPortIndex));
                assert(result != nullptr);
                results.push_back(result->data);
            }
            //push results to joint result array
            resultsMutex.lock();
            for (const auto &result: results) {
                resultsArray.push_back(result);
            }
            resultsMutex.unlock();
            threadSemaphore[i].semaphore.release();
        }, this);
    }

    //wait until all work is done;
    for (int i = 0; i < chunks.size(); i++) {
        threadSemaphore[i].semaphore.acquire();
        threadSemaphore[i].semaphore.release();
    }
    //convert resultsArray in to array of ArrayDataType

    //fill outputPorts
    outputPorts.resize(outputPortTypes.size());
    for (int i = 0; i < outputPortTypes.size(); i++) {
        outputPorts[i] = std::make_shared<ArrayDataType<std::shared_ptr<BaseDataType>>>();
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
            emit setView(new QtNodes::GraphicsView(scene), primaryDataFlowGraphModel);
        });
    }
    return base;
}

void ForeachNode::initDataFlowGraphModel() {
    //init internal node graph
    nodeRegistry = std::make_shared<QtNodes::NodeDelegateModelRegistry>();
    registerNodes(nodeRegistry);

    //register input and output nodes, specific only to this dataFlowGraphModel
    nodeRegistry->registerModel<ForeachInputNode>("Input");
    nodeRegistry->registerModel<ForeachOutputNode>("Output");

    primaryDataFlowGraphModel = new DynamicDataFlowGraphModel(nodeRegistry);
    connect(primaryDataFlowGraphModel, &DynamicDataFlowGraphModel::viewClosed, this, &ForeachNode::viewClosed);

    scene = new QtNodes::DataFlowGraphicsScene(*primaryDataFlowGraphModel, this);

    //add input and output nodes, specific only to this dataFlowGraphModel
    ForeachInputNode foreachInputNode;
    inputNodeId = primaryDataFlowGraphModel->addNode(foreachInputNode.name());
    ForeachOutputNode foreachOutputNode;
    outputNodeId = primaryDataFlowGraphModel->addNode(foreachOutputNode.name());

    auto inputNode = primaryDataFlowGraphModel->delegateModel<ForeachInputNode>(inputNodeId);
    assert(inputNode != nullptr);
    auto outputNode = primaryDataFlowGraphModel->delegateModel<ForeachOutputNode>(outputNodeId);
    assert(outputNode != nullptr);

    // make inputNode and outputNode indestructible by calling undo every time input or output nodes are deleted
    connect(primaryDataFlowGraphModel, &DynamicDataFlowGraphModel::nodeDeleted, [this](QtNodes::NodeId nodeId) {
        if (nodeId == inputNodeId || nodeId == outputNodeId) {
            QTimer::singleShot(0, [this] {
                scene->undoStack().undo();
            });
        }
    });
    updateInternalNodeType();
}

void ForeachNode::copyDataFlowGraphModel() {
    //wait until all work is done

    auto jobs = ThreadPool::get().getJobs(this);
    for (auto &job: jobs) {
        if (job->inProgress == true) {
            std::cout << "waiting" << std::endl;
        }
        while (job->inProgress) {

        }
        ThreadPool::get().deleteJob(job);
    }
    for (int i = 0; i < threadSemaphore.size(); i++) {
        threadSemaphore[i].semaphore.acquire();
    }
    for (int i = 0; i < threadSemaphore.size(); i++) {
        threadSemaphore[i].semaphore.release();
    }


    for (auto &graphModel: dataFlowGraphModels) {
//        for (auto node: graphModel->dataFlowGraphModel->allNodeIds()) {
//            graphModel->dataFlowGraphModel->deleteNode(node);
//        }
        delete graphModel->dataFlowGraphModel;
    }

    dataFlowGraphModels.clear();

    auto json = primaryDataFlowGraphModel->save();
    for (int i = 0; i < parallelCount; i++) {
        std::shared_ptr<GraphModel> graphModel = std::make_shared<GraphModel>(
                new DynamicDataFlowGraphModel(nodeRegistry));
        dataFlowGraphModels.push_back(graphModel);

        //call embeddedWidget foreach created node, or else load will fail
        // count how many times nodes have emitted computingStart() and computingFinished() signals
        // when the same, this means that graphModel have ended its calculations, and the result can be saved
        connect(graphModel->dataFlowGraphModel,
                &DynamicDataFlowGraphModel::nodeCreated,
                this,
                [graphModel, this](QtNodes::NodeId nodeId) {
                    std::cout << "newNode" << std::endl;

                    graphModel->dataFlowGraphModel->delegateModel<BaseNodeTypeLessWrapper>(nodeId)->embeddedWidget();

                    connect(
                            graphModel->dataFlowGraphModel->delegateModel<QtNodes::NodeDelegateModel>(nodeId),
                            &QtNodes::NodeDelegateModel::computingStarted, this, [&graphModel]() {
                                graphModel->counterStarted++;
                                std::cout << "counterStarted++" << std::endl;
                            }, Qt::DirectConnection);

                    connect(
                            graphModel->dataFlowGraphModel->delegateModel<QtNodes::NodeDelegateModel>(nodeId),
                            &QtNodes::NodeDelegateModel::computingFinished, this, [&graphModel]() {
                                graphModel->counterFinished++;
                                std::cout << "counterFinished++" << std::endl;
                                if (graphModel->counterStarted == graphModel->counterFinished) {
                                    graphModel->workFinished->notify_all();
                                }
                            }, Qt::DirectConnection);
                });
        //inputNodeId will be the same for each dataFlowGraphModels, no need to store that
        graphModel->dataFlowGraphModel->load(json);
    }
}

void ForeachNode::updateInternalNodeType() {
    //pass data and datatypes to inputNode

    auto types = inputPortTypes;
    // first element in inputPorts is always BaseArrayDataType
    types[0] = inputArrayValueType;
    auto inputNode = primaryDataFlowGraphModel->delegateModel<ForeachInputNode>(inputNodeId);
    assert(inputNode != nullptr);
    std::vector<std::shared_ptr<BaseDataType>> d;
    auto dt = std::make_shared<ForeachInputNodeDataType>(types, d);
    inputNode->setInData(dt, QtNodes::InvalidPortIndex);

    for (auto &graphModel: dataFlowGraphModels) {
        inputNode = graphModel->dataFlowGraphModel->delegateModel<ForeachInputNode>(inputNodeId);
        assert(inputNode != nullptr);
        dt = std::make_shared<ForeachInputNodeDataType>(types, d);
        inputNode->setInData(dt, QtNodes::InvalidPortIndex);
    }

    //listen for data coming from output node
//    auto outputNode = primaryDataFlowGraphModel->delegateModel<ForeachInputNode>(inputNodeId);
//
//    connect(outputNode, &ForeachNode::dataUpdated, this, [](QtNodes::PortIndex) {
//        //do something about it
//    });
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
        inputArrayValueType = BaseArrayDataType::getValueType(type);
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
        inputPortTypes.push_back(BaseDataType::DataType::getNodeDataType());
        portsInserted();

    }
    dirtyInputConnections = false;

}

void ForeachNode::inputConnectionDeleted(const QtNodes::ConnectionId &connection) {
    if (dirtyInputConnections) {
        return;
    }
    dirtyInputConnections = true;
    if (connection.inPortIndex == 0) {
        inputPortTypes[0] = ArrayDataType<std::shared_ptr<BaseDataType>>::DataType::getNodeDataType();
    } else {
//            if (connection.inPortIndex == 1) {
//                //reset port type
//                inputPortTypes[connection.inPortIndex] = BaseDataType::DataType::getNodeDataType();
//            }
        //delete disconnected port
        int index = connection.inPortIndex;
        portsAboutToBeDeleted(QtNodes::PortType::In, index, index);
        inputPortTypes.erase(inputPortTypes.begin() + index);
        inputPorts.erase(inputPorts.begin() + index);
        portsDeleted();
    }
    dirtyInputConnections = false;
}

void ForeachNode::viewClosed() {
    copyDataFlowGraphModel();
    updateExternalOutputPorts();
}

QJsonObject ForeachNode::save() const {
    QJsonObject modelJson = NodeDelegateModel::save();
    modelJson["graph"] = primaryDataFlowGraphModel->save();
    return modelJson;
}

void ForeachNode::load(const QJsonObject &json) {
    primaryDataFlowGraphModel->load(json["graph"].toObject());
    copyDataFlowGraphModel();
    updateExternalOutputPorts();

}

void ForeachNode::updateExternalOutputPorts() {
    auto outputNode = primaryDataFlowGraphModel->delegateModel<ForeachOutputNode>(outputNodeId);
    auto data = std::dynamic_pointer_cast<ForeachInputNodeDataType>(outputNode->outData(QtNodes::InvalidPortIndex));
    assert(data != nullptr);
    data->types;
    //wrap each type with array
    for (auto &type: data->types) {
        type = BaseArrayDataType::wrapWithArray(type);
    }
    outputPortTypes = data->types;
    emit embeddedWidgetSizeUpdated();
}

