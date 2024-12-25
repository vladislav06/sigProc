//
// Created by vm on 24.31.10.
//

#pragma once

#include "src/nodes/dataTypes/concepts.h"


/**
 * NodeDataType wrapper and type eraser for NodePort
 */
struct BaseNodePort {
    QtNodes::NodeDataType type;

    virtual ~BaseNodePort();
};

/**
 * Adds shared_ptr<T> to BaseNodePort
 * @tparam T
 */
template<baseData T>
struct NodePort : public BaseNodePort {
    std::shared_ptr<T> data;

    ~NodePort() override;
};

template<baseData T>
NodePort<T>::~NodePort() = default;
