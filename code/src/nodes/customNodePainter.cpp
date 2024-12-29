//
// Created by vm on 24.29.12.
//
#include <QJsonDocument>
#include "customNodePainter.h"
#include "QtNodes/internal/AbstractGraphModel.hpp"
#include "QtNodes/internal/NodeGraphicsObject.hpp"
#include "QtNodes/internal/ConnectionGraphicsObject.hpp"
#include "QtNodes/internal/AbstractNodeGeometry.hpp"
#include "QtNodes/internal/BasicGraphicsScene.hpp"
#include "QtNodes/internal/NodeStyle.hpp"
#include "QtNodes/internal/StyleCollection.hpp"
#include "QtNodes/internal/ConnectionIdUtils.hpp"

void CustomNodePainter::paint(QPainter *painter, QtNodes::NodeGraphicsObject &ngo) const {
    drawNodeRect(painter, ngo);

    drawConnectionPoints(painter, ngo);

    drawFilledConnectionPoints(painter, ngo);

    drawNodeCaption(painter, ngo);

    drawCustomEntryLabels(painter, ngo);

    drawResizeRect(painter, ngo);
}

void CustomNodePainter::drawCustomEntryLabels(QPainter *painter, QtNodes::NodeGraphicsObject &ngo) const {
    QtNodes::AbstractGraphModel &model = ngo.graphModel();
    QtNodes::NodeId const nodeId = ngo.nodeId();
    QtNodes::AbstractNodeGeometry &geometry = ngo.nodeScene()->nodeGeometry();

    QJsonDocument json = QJsonDocument::fromVariant(model.nodeData(nodeId, QtNodes::NodeRole::Style));
    QtNodes::NodeStyle nodeStyle(json.object());

    for (QtNodes::PortType portType: {QtNodes::PortType::Out, QtNodes::PortType::In}) {
        unsigned int n = model.nodeData<unsigned int>(nodeId,
                                                      (portType == QtNodes::PortType::Out)
                                                      ? QtNodes::NodeRole::OutPortCount
                                                      : QtNodes::NodeRole::InPortCount);

        for (QtNodes::PortIndex portIndex = 0; portIndex < n; ++portIndex) {
            auto const &connected = model.connections(nodeId, portType, portIndex);

            QPointF p = geometry.portTextPosition(nodeId, portType, portIndex);
            if (connected.empty())
                painter->setPen(nodeStyle.FontColorFaded);
            else
                painter->setPen(nodeStyle.FontColor);

            auto portData = model.portData(nodeId, portType, portIndex, QtNodes::PortRole::DataType);
            auto caption = model.portData<QString>(nodeId, portType, portIndex, QtNodes::PortRole::Caption);
            auto name = portData.value<QtNodes::NodeDataType>().name;

            if (!caption.isEmpty()) {
                auto p0 = p;
                auto p1 = p;
                p0.setY(p0.y() - (painter->fontMetrics().height() / 3.0));
                p1.setY(p1.y() + (painter->fontMetrics().height() / 3.0));
                painter->drawText(p0, caption);
                painter->drawText(p1, name);

            } else {
                painter->drawText(p, name);
            }
        }
    }
}
