//
// Created by vm on 24.10.11.
//
#pragma once

#include <QLabel>
#include <QPlainTextEdit>
#include "src/nodes/baseNode.h"

class PreviewNode : public BaseNode<std::tuple<BaseData>, std::tuple<BaseData>> {
public:

    QJsonObject onSave() const override;

    bool onLoad(QJsonObject json) override;

    QString caption() const override;

    QString name() const override;

    QWidget *embeddedWidget() override;

    std::tuple<std::shared_ptr<BaseData>>
    compute(std::tuple<std::shared_ptr<BaseData>> params,
            std::vector<std::shared_ptr<BaseData>> adParams) override;

    void afterCompute() override;

private:
    std::tuple<std::shared_ptr<BaseData>> parameters;
    QWidget *base = nullptr;
    QPlainTextEdit *text = nullptr;
};

