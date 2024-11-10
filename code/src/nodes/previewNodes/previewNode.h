//
// Created by vm on 24.10.11.
//
#pragma once

#include <QLabel>
#include <QPlainTextEdit>
#include "src/nodes/baseNode.h"

class PreviewNode : public BaseNode<std::tuple<BaseDataType>, std::tuple<BaseDataType>> {
public:
    std::tuple<std::shared_ptr<BaseDataType>> compute(std::tuple<std::shared_ptr<BaseDataType>> type1) override;

    QJsonObject onSave() const override;

    bool onLoad(QJsonObject json) override;

    QString caption() const override;

    QString name() const override;

    QWidget *embeddedWidget() override;

private:
    QWidget *base = nullptr;
    QPlainTextEdit *text = nullptr;
};

