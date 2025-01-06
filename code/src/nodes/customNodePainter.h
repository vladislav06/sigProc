//
// Created by Vladislavs Agarkovs on 24.29.12.
//
#pragma once

#include "QtNodes/internal/DefaultNodePainter.hpp"


/**
 * This class adds ability to have 2 line node port caption, one line with port caption, second line with port type
 */
class CustomNodePainter : public QtNodes::DefaultNodePainter {
public:
    void paint(QPainter *painter, QtNodes::NodeGraphicsObject &ngo) const override;

    /**
     * Always draws port type, and optionally can draw port caption if not empty
     */
    void drawCustomEntryLabels(QPainter *painter, QtNodes::NodeGraphicsObject &ngo) const;
};