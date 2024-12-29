//
// Created by vm on 24.29.12.
//
#pragma once

#include "QtNodes/internal/DefaultNodePainter.hpp"


class CustomNodePainter : public QtNodes::DefaultNodePainter {
public:
    void paint(QPainter *painter, QtNodes::NodeGraphicsObject &ngo) const override;

    void drawCustomEntryLabels(QPainter *painter, QtNodes::NodeGraphicsObject &ngo) const;
};