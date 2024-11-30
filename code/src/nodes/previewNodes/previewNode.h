//
// Created by vm on 24.10.11.
//
#pragma once

#include <QLabel>
#include <QPlainTextEdit>
#include "src/nodes/baseNode.h"

class PreviewNode : public BaseNode<std::tuple<BaseDataType>, std::tuple<BaseDataType>> {
public:

    QJsonObject onSave() const override;

    bool onLoad(QJsonObject json) override;

    QString caption() const override;

    QString name() const override;

    QWidget *embeddedWidget() override;

    std::tuple<std::shared_ptr<BaseDataType>>
    compute(std::tuple<std::shared_ptr<BaseDataType>> params,
            std::vector<std::shared_ptr<BaseDataType>> adParams) override;

    void afterCompute() override;

private:
    std::tuple<std::shared_ptr<BaseDataType>> parameters;
    QWidget *base = nullptr;
    QPlainTextEdit *text = nullptr;
};

