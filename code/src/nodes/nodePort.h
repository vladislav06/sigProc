//
// Created by vm on 24.31.10.
//

#pragma once

#include "src/utils/concepts.h"


/**
 * NodeDataType wrapper and type eraser for NodePort
 */
struct BaseNodePort {
    QtNodes::NodeDataType type;
    virtual ~BaseNodePort() = default;
};

/**
 * Adds shared_ptr<T> to BaseNodePort
 * @tparam T
 */
template<baseDataType T>
struct NodePort : public BaseNodePort {
    std::shared_ptr<T> data;
};
