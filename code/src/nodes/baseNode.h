//
// Created by vm on 24.30.10.
//

#pragma once

#include "src/nodes/dataTypes/concepts.h"

#include <QtNodes/internal/NodeDelegateModel.hpp>
#include "iostream"
#include "src/nodes/dataTypes/arrayData.h"
#include "src/nodes/nodePort.h"
#include "semaphore"
#include "QtConcurrent/QtConcurrent"
#include "QApplication"

/**
 * BaseNode base class with some methods that dont require input and output types
 */
class BaseNodeTypeLessWrapper : public QtNodes::NodeDelegateModel {
Q_OBJECT

public:
    BaseNodeTypeLessWrapper();

    ~BaseNodeTypeLessWrapper() override {
    };

    virtual QFuture<void> prepareToDelete() {
        willBeDeleted = true;
        //handle case when no job was created
        if (runningJob.isCanceled()) {
            uiThreadSemaphore.acquire();
            QPromise<void> p;
            p.finish();
            return p.future();
        }
        return runningJob.then([this]() { uiThreadSemaphore.acquire(); });
    }

    virtual bool isSource() = 0;

    virtual void recalculate() = 0;

    // guards ui thread
    std::binary_semaphore uiThreadSemaphore{1};

signals:

    void callAfterCompute();

public slots:


    void afterComputeSlot() {
        afterCompute();
        uiThreadSemaphore.release();
    };

    /**
     * type of data that is connected will be send to this slot
     */
    virtual void onInputConnectionCreation(QtNodes::ConnectionId connection, QtNodes::NodeDataType type) {};

public:

    /**
     * This method is called in ui thread after compute method has returned
     * update ui elements in this method
     */
    virtual void afterCompute() {};

protected:
    QFuture<void> runningJob;

    bool willBeDeleted = false;
};


/**
 *
 * @tparam InPorts Types of input ports
 * @tparam OutPorts Types of output ports
 * @tparam AdInPort Type of additional input ports
 */
template<tuple InPorts, tuple OutPorts, baseData AdInPort = BaseData>
class BaseNode : public BaseNodeTypeLessWrapper {
public:
    BaseNode() {
    };

    /**
     * Constructor that enables input and output captions
     * @param inputCaptions
     * @param outputCaptions
     */


    ~BaseNode() override {
    }

private:
    //templates

    /**
     * Will convert vector of std::shared_ptr<BaseNodePort> in to tuple of down casted BaseData according to tuple ports
     */
    template<tuple ports, typename T, std::size_t... Indices>
    auto vectorToTupleHelper(const std::vector<T> &v, std::index_sequence<Indices...>) {
        return std::make_tuple(
                (std::dynamic_pointer_cast<NodePort<std::tuple_element_t<Indices, ports>>>(v[Indices])->data)...);
    }

    /**
     * Will convert vector of std::shared_ptr<BaseNodePort> in to tuple of down casted BaseData according to tuple ports
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
    static constexpr void generateNodePort(std::vector<std::shared_ptr<BaseNodePort>> &ports) {
        if constexpr (I < std::tuple_size_v<tup>) {
            using SelectedType = std::tuple_element_t<I, tup>;
            auto port = std::make_shared<NodePort<SelectedType>>();
            using DataType = SelectedType::DataType;
            DataType dataType;
            port->type = dataType.getNodeDataType();
            ports.push_back(port);
            generateNodePort<tup, I + 1>(ports);
        }
    }

    template<tuple ports>
    constexpr std::vector<std::shared_ptr<BaseNodePort>> generateNodePorts() {
        std::vector<std::shared_ptr<BaseNodePort>> nodes;
        generateNodePort<ports>(nodes);
        return nodes;
    }

    //input and output data vectors

    std::vector<std::shared_ptr<BaseNodePort>> inNodePorts = generateNodePorts<InPorts>();
    std::vector<std::shared_ptr<BaseNodePort>> outNodePorts = generateNodePorts<OutPorts>();

    int additionalInPorts = 0;

    std::array<QString, std::tuple_size_v<InPorts>> inputCaptions;
    std::array<QString, std::tuple_size_v<OutPorts>> outputCaptions;

    /**
     * Ignore connection changes when node edits its own connections
     */
    bool dirtyInputConnections = false;

    // guards compute thread until job is done
    std::binary_semaphore computeThreadSemaphore{1};

    //more templates

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
                if (dataType == nullptr && data != nullptr) {
                    std::cout << "Unable to cast data!: " << typeid(data.get()).name() << std::endl;
                }
                std::dynamic_pointer_cast<NodePort<portType>>(ports.at(I))->data = dataType;
            }
            setInNodePortData<tup, I + 1>(ports, data, index);
        } else if (index >= std::tuple_size_v<tup> && index < std::tuple_size_v<tup> + additionalInPorts) {
            using portType = AdInPort;
            auto dataType = std::dynamic_pointer_cast<portType>(data);
            if (dataType == nullptr && data != nullptr) {
                std::cout << "Unable to cast data!: " << typeid(data.get()).name() << std::endl;
            }
            std::dynamic_pointer_cast<NodePort<portType>>(ports.at(index))->data = dataType;
        }

    }

    template<typename OutTuple, std::size_t I = 0>
    void setOutNodePortData(std::vector<std::shared_ptr<BaseNodePort>> &ports,
                            WrappedTupleElements<OutPorts>::type &inTuple) {
        if constexpr (I < std::tuple_size_v<OutTuple>) {
            using portType = std::tuple_element_t<I, OutTuple>::element_type;
            auto newd = std::get<I>(inTuple);
            std::dynamic_pointer_cast<NodePort<portType>>(ports.at(I))->data = newd;
            setOutNodePortData<OutTuple, I + 1>(ports, inTuple);
        }
    }

    template<typename OutTuple, std::size_t I = 0>
    std::shared_ptr<QtNodes::NodeData> getOutNodeData(std::vector<std::shared_ptr<BaseNodePort>> &ports, int index) {

        if constexpr (I < std::tuple_size_v<OutTuple>) {

            using portType = std::tuple_element_t<I, OutTuple>;

            if (I == index) {
                std::shared_ptr<portType> out = std::dynamic_pointer_cast<NodePort<portType>>(ports.at(I))->data;
                auto o = std::dynamic_pointer_cast<QtNodes::NodeData>(out);
                return o;
            } else {
                return getOutNodeData<OutTuple, I + 1>(ports, index);
            }
        }
        return {};
    }

public:
    //overrides

    [[nodiscard]] unsigned int nPorts(QtNodes::PortType portType) const override {
        switch (portType) {
            case QtNodes::PortType::In:
                assert(std::tuple_size_v<InPorts> + additionalInPorts == inNodePorts.size());
                return std::tuple_size_v<InPorts> + additionalInPorts;
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
                if (portIndex < inNodePorts.size())
                    return inNodePorts.at(portIndex)->type;
                else
                    return AdInPort::DataType::getNodeDataType();
            case QtNodes::PortType::Out:
                return outNodePorts.at(portIndex)->type;
            case QtNodes::PortType::None:
                return {};
        }

        return {};
    }

    void setInData(std::shared_ptr<QtNodes::NodeData> nodeData, const QtNodes::PortIndex portIndex) override {
        setInNodePortData<InPorts>(inNodePorts, nodeData, portIndex);
        callCompute();
    }

    bool portCaptionVisible(QtNodes::PortType type, QtNodes::PortIndex index) const override {
        return false;
    }

    QString
    portCaption(QtNodes::PortType portType, QtNodes::PortIndex portIndex) const override {
        switch (portType) {
            case QtNodes::PortType::In:
                if (portIndex >= inputCaptions.size()) {
                    return "";
                }
                return inputCaptions[portIndex];
            case QtNodes::PortType::Out:
                if (portIndex >= outputCaptions.size()) {
                    return "";
                }
                return outputCaptions[portIndex];
            case QtNodes::PortType::None:
                return "";
        }
        return "";
    }


    void callCompute() {
        if (willBeDeleted) {
            return;
        }
        emit computingStarted();
        runningJob = QtConcurrent::run([this](QPromise<void> &promise) {
            computeThreadSemaphore.acquire();
            uiThreadSemaphore.acquire();


            //convert additional params into vector
            std::vector<std::shared_ptr<AdInPort>> adParams;
            for (int i = std::tuple_size_v<InPorts>; i < std::tuple_size_v<InPorts> + additionalInPorts; i++) {
                adParams.push_back(std::dynamic_pointer_cast<NodePort<AdInPort>>(inNodePorts.at(i))->data);
            }
            auto ret = compute(vectorToTuple<InPorts, std::tuple_size_v<InPorts>>(inNodePorts), adParams);

            auto tempOutNodePorts = outNodePorts;
            outNodePorts = generateNodePorts<OutPorts>();
            setOutNodePortData<typename WrappedTupleElements<OutPorts>::type>(outNodePorts, ret);


            for (int i = 0; i < outNodePorts.size(); i++) {
                if (tempOutNodePorts[i] != outNodePorts[i]) {
                    Q_EMIT dataUpdated(i);
                }
            }

            Q_EMIT callAfterCompute();
            computeThreadSemaphore.release();
            emit computingFinished();
        });


    }


public:

    std::shared_ptr<QtNodes::NodeData> outData(const QtNodes::PortIndex port) override {
        std::shared_ptr<QtNodes::NodeData> nodeData = getOutNodeData<OutPorts>(outNodePorts, port);
        return nodeData;
    }

    // methods to implement

    /**
     * Must perform calculations based on input tuple, pointers in tuple might be nullptr
     * This method is called in non ui thread, do not update ui in this method.
     * @return newly constructed shred_ptr
     */
    virtual WrappedTupleElements<OutPorts>::type
    compute(WrappedTupleElements<InPorts>::type params, std::vector<std::shared_ptr<AdInPort>> adParams) = 0;


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

    virtual void onInputConnected(int index) {};

    virtual void onInputDisconnected(int index) {};

    //more overrides

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

    //this is needed for nodes that do not have interactive widget and input
    void outputConnectionCreated(const QtNodes::ConnectionId &) override {
//        updated();
    }

    void inputConnectionCreated(const QtNodes::ConnectionId &connection) override {
        if (!dirtyInputConnections) {
            onInputConnected(connection.inPortIndex);
        }
    }

    void inputConnectionDeleted(const QtNodes::ConnectionId &connection) override {
        if (!dirtyInputConnections) {
            onInputDisconnected(connection.inPortIndex);
        }
    }

    //methods to call

    /**
     * Will call compute
     */
    void updated() {
        callCompute();
    }

    void recalculate() override {
        callCompute();
    }

    /**
     * Will add new input port with type AdInPort
     */
    void addInputPort() {
        dirtyInputConnections = true;
        int newPortIndex = additionalInPorts + std::tuple_size_v<InPorts>;
        portsAboutToBeInserted(QtNodes::PortType::In, newPortIndex, newPortIndex);

        //add entry into inNodePorts
        using SelectedType = AdInPort;
        auto port = std::make_shared<NodePort<SelectedType>>();
        using DataType = SelectedType::DataType;
        DataType dataType;
        port->type = dataType.getNodeDataType();
        inNodePorts.push_back(std::move(port));
        additionalInPorts++;

        portsInserted();
        dirtyInputConnections = false;
    }

    /**
     * Will remove input port with type AdInPort, removes only added ports
     * @param modify must modify node widget structure
     */
    void removeInputPort(int index, const std::function<void(void)> &modify) {
        if (additionalInPorts == 0) {
            return;
        }
        if (index < std::tuple_size_v<InPorts>) {
            return;
        }
        dirtyInputConnections = true;


        portsAboutToBeDeleted(QtNodes::PortType::In, index, index);

        inNodePorts.erase(inNodePorts.begin() + index);
        additionalInPorts--;

        modify();
        portsDeleted();


        dirtyInputConnections = false;
    }

    //source node is node with no inputs
    bool isSource() override {
        return std::tuple_size_v<InPorts> == 0;
    }


    void setCaptions(std::array<QString, std::tuple_size_v<InPorts>> input,
                     std::array<QString, std::tuple_size_v<OutPorts>> output) {
        inputCaptions = input;
        outputCaptions = output;
    }
};

