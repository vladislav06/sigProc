//
// Created by vm on 24.7.12.
//
#pragma once

#include "QtNodes/NodeDelegateModelRegistry"
#include "memory"

/**
 * This function adds  all known nodes to passed NodeDelegateModelRegistry
 * @param nodeRegistry to where add nodes
 */
void registerNodes(std::shared_ptr<QtNodes::NodeDelegateModelRegistry> nodeRegistry);