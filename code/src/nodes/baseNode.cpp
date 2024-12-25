//
// Created by vm on 24.29.11.
//
#include "baseNode.h"

BaseNodeTypeLessWrapper::BaseNodeTypeLessWrapper() {
    connect(this, &BaseNodeTypeLessWrapper::callAfterCompute, this, &BaseNodeTypeLessWrapper::afterComputeSlot, Qt::QueuedConnection);
}
